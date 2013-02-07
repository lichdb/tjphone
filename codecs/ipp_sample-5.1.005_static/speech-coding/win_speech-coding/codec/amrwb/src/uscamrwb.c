/*/////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2005-2006 Intel Corporation. All Rights Reserved.
//
//     Intel(R) Integrated Performance Primitives
//     USC - Unified Speech Codec interface library
//
// By downloading and installing USC codec, you hereby agree that the
// accompanying Materials are being provided to you under the terms and
// conditions of the End User License Agreement for the Intel(R) Integrated
// Performance Primitives product previously accepted by you. Please refer
// to the file ipplic.htm located in the root directory of your Intel(R) IPP
// product installation for more information.
//
// A speech coding standards promoted by ITU, ETSI, 3GPP and other
// organizations. Implementations of these standards, or the standard enabled
// platforms may require licenses from various entities, including
// Intel Corporation.
//
//
// Purpose: AMRWB speech codec: USC functions.
//
*/

#include "ownamrwb.h"
#include "amrwbapi.h"
#include <string.h>
#include "amrwbusc.h"

#define  AMRWB_NUM_RATES  9
#define  AMRWB_BITS_PER_SAMPLE 16

static USC_Status GetInfoSize(int *pSize);
static USC_Status GetInfo(USC_Handle handle, USC_CodecInfo *pInfo);
static USC_Status NumAlloc(const USC_Option *options, int *nbanks);
static USC_Status MemAlloc(const USC_Option *options, USC_MemBank *pBanks);
static USC_Status Init(const USC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle);
static USC_Status Reinit(const USC_Modes *modes, USC_Handle handle );
static USC_Status Control(const USC_Modes *modes, USC_Handle handle );
static USC_Status Encode(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out);
static USC_Status Decode(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out);
static USC_Status GetOutStreamSize(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst);
static USC_Status SetFrameSize(const USC_Option *options, USC_Handle handle, int frameSize);

typedef struct
{
    short updateSidCount;
    TXFrameType prevFrameType;
} SIDSyncState;

typedef struct {
    int direction;
    int trunc;
    int bitrate;
    int vad;
    int reset_flag;
    int reset_flag_old;
    int bitrate_old;
    int usedRate;
    SIDSyncState sid_state;
    int reserved0; // for future extension
    int reserved1; // for future extension
} AMRWB_Handle_Header;

static int  ownSidSyncInit(SIDSyncState *st);
static void ownSidSync(SIDSyncState *st, int valRate, TXFrameType *pTXFrameType);
static int  ownTestPCMFrameHoming(const short *pSrc);
static int  ownTestBitstreamFrameHoming(short* pPrmsvec, int valRate);

/* global usc vector table */
USCFUN USC_Fxns USC_AMRWB_Fxns={
    {
        USC_Codec,
        GetInfoSize,
        GetInfo,
        NumAlloc,
        MemAlloc,
        Init,
        Reinit,
        Control
    },
    Encode,
    Decode,
    GetOutStreamSize,
    SetFrameSize
};

static USC_Option  params;  /* what is supported  */
static USC_PCMType pcmType; /* codec audio source */

static __ALIGN32 CONST int BitsLenTbl[NUM_OF_MODES]={
   132, 177, 253, 285, 317, 365, 397, 461, 477, 35
};

static __ALIGN32 CONST short LostFrame[AMRWB_Frame]=
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static __ALIGN32 CONST USC_Rates pTblRates_AMRWB[AMRWB_NUM_RATES]={
    {23850},
    {23050},
    {19850},
    {18250},
    {15850},
    {14250},
    {12650},
    {8850},
    {6600}
};

static void ownConvert15(int valRate, const char *pSerialvec, short* pPrmvec)
{
    short i, j, tmp;
    int valNumPrms = BitsLenTbl[valRate];
    int nbit=0;

    j = 0;
    i = 0;

    tmp = (short)(valNumPrms - 15);
    while (tmp > j)
    {
        pPrmvec[i] = amrExtractBits(&pSerialvec,&nbit,15);
        j += 15;
        i++;
    }
    tmp = (short)(valNumPrms - j);
    pPrmvec[i] = amrExtractBits(&pSerialvec,&nbit,tmp);
    pPrmvec[i] <<= (15 - tmp);

    return;
}


static USC_Status GetInfoSize(int *pSize)
{
    *pSize = sizeof(USC_CodecInfo);
    return USC_NoError;
}
static USC_Status GetInfo(USC_Handle handle, USC_CodecInfo *pInfo)
{
    AMRWB_Handle_Header *amrwb_header;

    pInfo->name = "IPP_AMRWB";
    pInfo->params.framesize = AMRWB_Frame*sizeof(short);
    if (handle == NULL) {
        pInfo->params.modes.bitrate = 6600;
        pInfo->params.modes.truncate = 1;
        pInfo->params.direction = 0;
        pInfo->params.modes.vad = 1;
    } else {
      amrwb_header = (AMRWB_Handle_Header*)handle;
      pInfo->params.modes.bitrate = amrwb_header->bitrate;
      pInfo->params.modes.truncate = amrwb_header->trunc;
      pInfo->params.direction = amrwb_header->direction;
      pInfo->params.modes.vad = amrwb_header->vad;
    }
    pInfo->maxbitsize = BITSTREAM_SIZE;
    pInfo->pcmType.sample_frequency = 16000;
    pInfo->pcmType.bitPerSample = AMRWB_BITS_PER_SAMPLE;
    pInfo->params.modes.hpf = 0;
    pInfo->params.modes.pf = 0;
    pInfo->params.law = 0;
    pInfo->nRates = AMRWB_NUM_RATES;
    pInfo->pRateTbl = (const USC_Rates *)&pTblRates_AMRWB;

    return USC_NoError;
}

static int CheckRate_AMRWB(int rate_bps)
{
    int rate;

    switch(rate_bps) {
      case 6600:  rate = 0; break;
      case 8850:  rate = 1; break;
      case 12650: rate = 2; break;
      case 14250: rate = 3; break;
      case 15850: rate = 4; break;
      case 18250: rate = 5; break;
      case 19850: rate = 6; break;
      case 23050: rate = 7; break;
      case 23850: rate = 8; break;
      default: rate = -1; break;
    }
    return rate;
}

static int CheckIdxRate_AMRWB(int idx_rate)
{
    int rate;

    switch(idx_rate) {
      case 0: rate = 6600; break;
      case 1: rate = 8850; break;
      case 2: rate = 12650; break;
      case 3: rate = 14250; break;
      case 4: rate = 15850; break;
      case 5: rate = 18250; break;
      case 6: rate = 19850; break;
      case 7: rate = 23050; break;
      case 8: rate = 23850; break;
      default: rate = -1; break;
    }
    return rate;
}


static USC_Status NumAlloc(const USC_Option *options, int *nbanks)
{
   if(options==NULL) return USC_BadDataPointer;
   if(nbanks==NULL) return USC_BadDataPointer;

   *nbanks = 1;
   return USC_NoError;
}

static USC_Status MemAlloc(const USC_Option *options, USC_MemBank *pBanks)
{
    unsigned int nbytes;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
   if(options->modes.vad > 1) return USC_UnsupportedVADType;

    pBanks->pMem = NULL;
    pBanks->align = 32;
    pBanks->memType = USC_OBJECT;
    pBanks->memSpaceType = USC_NORMAL;

    if (options->direction == 0) /* encode only */
    {
        AMRWBEnc_Params enc_params;
        enc_params.codecType = (AMRWBCodec_Type)0;
        enc_params.mode = options->modes.vad;
        apiAMRWBEncoder_Alloc(&enc_params, &nbytes);
    }
    else if (options->direction == 1) /* decode only */
    {
        AMRWBDec_Params dec_params;
        dec_params.codecType = (AMRWBCodec_Type)0;
        dec_params.mode = options->modes.vad;
        apiAMRWBDecoder_Alloc(&dec_params, &nbytes);
    } else {
        return USC_NoOperation;
    }
    pBanks->nbytes = nbytes + sizeof(AMRWB_Handle_Header); /* include header in handle */
    return USC_NoError;
}

static USC_Status Init(const USC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle)
{
    AMRWB_Handle_Header *amrwb_header;
    int bitrate_idx;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
   if(pBanks->pMem==NULL) return USC_NotInitialized;
   if(pBanks->nbytes<=0) return USC_NotInitialized;
   if(handle==NULL) return USC_InvalidHandler;

    *handle = (USC_Handle*)pBanks->pMem;
    amrwb_header = (AMRWB_Handle_Header*)*handle;

    bitrate_idx = CheckRate_AMRWB(options->modes.bitrate);
     if(bitrate_idx < 0) return USC_UnsupportedBitRate;
    amrwb_header->direction = options->direction;
    amrwb_header->trunc = options->modes.truncate;
    amrwb_header->vad = options->modes.vad;
     amrwb_header->bitrate = options->modes.bitrate;
     amrwb_header->bitrate_old = 0;

    if (options->direction == 0) /* encode only */
    {
        AMRWBEncoder_Obj *EncObj = (AMRWBEncoder_Obj *)((char*)*handle + sizeof(AMRWB_Handle_Header));
        apiAMRWBEncoder_Init((AMRWBEncoder_Obj*)EncObj, amrwb_header->vad);
    }
    else if (options->direction == 1) /* decode only */
    {
        AMRWBDecoder_Obj *DecObj = (AMRWBDecoder_Obj *)((char*)*handle + sizeof(AMRWB_Handle_Header));
        apiAMRWBDecoder_Init((AMRWBDecoder_Obj*)DecObj);
          amrwb_header->usedRate = 6600;
        amrwb_header->reset_flag = 0;
        amrwb_header->reset_flag_old = 1;

    } else {
        return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Reinit(const USC_Modes *modes, USC_Handle handle )
{
    AMRWB_Handle_Header *amrwb_header;
    int bitrate_idx;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

    amrwb_header = (AMRWB_Handle_Header*)handle;

    bitrate_idx = CheckRate_AMRWB(modes->bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;
    amrwb_header->vad = modes->vad;
    amrwb_header->bitrate = modes->bitrate;
    amrwb_header->bitrate_old = 0;

    if (amrwb_header->direction == 0) /* encode only */
    {
        AMRWBEncoder_Obj *EncObj = (AMRWBEncoder_Obj *)((char*)handle + sizeof(AMRWB_Handle_Header));
        amrwb_header->reset_flag = 0;
        ownSidSyncInit(&amrwb_header->sid_state);
        apiAMRWBEncoder_Init((AMRWBEncoder_Obj*)EncObj, modes->vad);
    }
    else if (amrwb_header->direction == 1) /* decode only */
    {
        AMRWBDecoder_Obj *DecObj = (AMRWBDecoder_Obj *)((char*)handle + sizeof(AMRWB_Handle_Header));
        apiAMRWBDecoder_Init((AMRWBDecoder_Obj*)DecObj);
          amrwb_header->usedRate = 6600;
        amrwb_header->reset_flag = 0;
        amrwb_header->reset_flag_old = 1;
    } else {
        return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Control(const USC_Modes *modes, USC_Handle handle )
{
   AMRWB_Handle_Header *amrwb_header;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

   amrwb_header = (AMRWB_Handle_Header*)handle;

   amrwb_header->trunc = modes->truncate;
   amrwb_header->vad = modes->vad;
   amrwb_header->bitrate = modes->bitrate;

   if (amrwb_header->direction == 0) { /* encode only */
      AMRWBEncoder_Obj *EncObj;
      EncObj = (AMRWBEncoder_Obj *)((char*)handle + sizeof(AMRWB_Handle_Header));
      apiAMRWBEncoder_Mode(EncObj, amrwb_header->vad);
   }

   return USC_NoError;
}

static __ALIGN32 CONST AMRWB_Rate_t usc2amrwb[9]={
    AMRWB_RATE_6600,
    AMRWB_RATE_8850,
    AMRWB_RATE_12650,
    AMRWB_RATE_14250,
    AMRWB_RATE_15850,
    AMRWB_RATE_18250,
    AMRWB_RATE_19850,
    AMRWB_RATE_23050,
    AMRWB_RATE_23850
};

static int getBitstreamSize(int rate, int frametype, int *outRate)
{
    int nbytes;
    int usedRate = rate;

    if (frametype != TX_SPEECH) {
        usedRate = AMRWB_RATE_DTX;
    }

    nbytes = ((BitsLenTbl[usedRate] + 7) / 8);
    *outRate = rate;

    return nbytes;

}

static USC_Status Encode(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out)
{
    AMRWB_Handle_Header *amrwb_header;
    int bitrate_idx;
    AMRWBEncoder_Obj *EncObj;
    unsigned short work_buf[AMRWB_Frame];
    int WrkRate, out_WrkRate;

    if(in==NULL) return USC_BadDataPointer;
    if(in->nbytes<AMRWB_Frame*sizeof(short)) return USC_NoOperation;

    amrwb_header = (AMRWB_Handle_Header*)handle;

    if(amrwb_header == NULL) return USC_InvalidHandler;
     bitrate_idx = CheckRate_AMRWB(in->bitrate);
     if(bitrate_idx < 0) return USC_UnsupportedBitRate;
     amrwb_header->bitrate = in->bitrate;

    EncObj = (AMRWBEncoder_Obj *)((char*)handle + sizeof(AMRWB_Handle_Header));

    /* check for homing frame */
    amrwb_header->reset_flag = ownTestPCMFrameHoming((short*)in->pBuffer);

    if (amrwb_header->trunc) {
       /* Delete the LSBs */
       ippsAndC_16u((Ipp16u*)in->pBuffer, (Ipp16u)IO_MASK, work_buf, AMRWB_Frame);
//       int i;
//       short *pPCM = (short *)in->pBuffer;
//       for (i = 0; i < AMRWB_Frame; i ++) work_buf[i] = (IO_MASK & pPCM[i]);
    } else {
       ippsCopy_16s((const Ipp16s*)in->pBuffer, (Ipp16s*)work_buf, AMRWB_Frame);
    }

    WrkRate = usc2amrwb[bitrate_idx];
    if(apiAMRWBEncode(EncObj,(const unsigned short*)work_buf,(unsigned char*)out->pBuffer,(AMRWB_Rate_t *)&WrkRate,EncObj->iDtx) != APIAMRWB_StsNoErr){
       return USC_NoOperation;
    }
    /* include frame type and mode information in serial bitstream */
    ownSidSync(&amrwb_header->sid_state, WrkRate, (TXFrameType *)&out->frametype);

     out->nbytes = getBitstreamSize(bitrate_idx, out->frametype, &out_WrkRate);
     out->bitrate = CheckIdxRate_AMRWB(out_WrkRate);

    if (amrwb_header->reset_flag != 0) {
        ownSidSyncInit(&amrwb_header->sid_state);
        apiAMRWBEncoder_Init(EncObj, EncObj->iDtx);
    }

    in->nbytes = AMRWB_Frame*sizeof(short);
    return USC_NoError;
}

static RXFrameType ownTX2RX (TXFrameType valTXType)
{
    switch (valTXType) {
      case TX_SPEECH:                    return RX_SPEECH_GOOD;
      case TX_SID_FIRST:                 return RX_SID_FIRST;
      case TX_SID_UPDATE:                return RX_SID_UPDATE;
      case TX_NO_DATA:                   return RX_NO_DATA;
      case TX_SPEECH_PROBABLY_DEGRADED:  return RX_SPEECH_PROBABLY_DEGRADED;
      case TX_SPEECH_LOST:               return RX_SPEECH_LOST;
      case TX_SPEECH_BAD:                return RX_SPEECH_BAD;
      case TX_SID_BAD:                   return RX_SID_BAD;
      default:
        return (RXFrameType)(-1);
    }
}

static USC_Status Decode(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out)
{
    AMRWB_Handle_Header *amrwb_header;
    AMRWBDecoder_Obj *DecObj;
    int bitrate_idx;
    int mode;
    RXFrameType rx_type;
    short prm[56];

   if(out==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

    amrwb_header = (AMRWB_Handle_Header*)handle;
    DecObj = (AMRWBDecoder_Obj *)((char*)handle + sizeof(AMRWB_Handle_Header));

    if(in == NULL) {
       bitrate_idx = CheckRate_AMRWB(amrwb_header->bitrate_old);
       if(bitrate_idx < 0) return USC_UnsupportedBitRate;

       amrwb_header->bitrate = amrwb_header->bitrate_old;
       out->bitrate = amrwb_header->bitrate_old;
       rx_type = RX_SPEECH_LOST;

       if(apiAMRWBDecode(DecObj,(const unsigned char*)LostFrame, usc2amrwb[bitrate_idx],rx_type,(unsigned short*)out->pBuffer) != APIAMRWB_StsNoErr){
            return USC_NoOperation;
      }
      if(amrwb_header->trunc) {
          ippsAndC_16u_I((Ipp16u)IO_MASK, (unsigned short*)out->pBuffer, AMRWB_Frame);
//          int i;
//          short *pPCM = (short *)out->pBuffer;
//          for (i = 0; i < AMRWB_Frame; i ++) pPCM[i] &= IO_MASK;
      }
      amrwb_header->reset_flag = 0;
      amrwb_header->reset_flag_old = 1;

      out->nbytes = AMRWB_Frame*sizeof(short);

    } else {
       if(in->nbytes<=0) return USC_NoOperation;
       bitrate_idx = CheckRate_AMRWB(in->bitrate);
       if(bitrate_idx < 0) return USC_UnsupportedBitRate;
       amrwb_header->bitrate = in->bitrate;
      out->bitrate = in->bitrate;
      rx_type = ownTX2RX((TXFrameType)in->frametype);

      if ((rx_type == RX_SID_BAD) || (rx_type == RX_SID_UPDATE) || (rx_type == RX_NO_DATA)) {
         mode = AMRWB_RATE_DTX;
      } else {
           mode = bitrate_idx;
      }
      if ((rx_type == RX_SPEECH_LOST)) {
         out->bitrate = amrwb_header->usedRate;
         amrwb_header->reset_flag = 0;
      } else {
         amrwb_header->usedRate = out->bitrate;
         /* if homed: check if this frame is another homing frame */
         ownConvert15(mode,in->pBuffer, prm);
         amrwb_header->reset_flag = ownTestBitstreamFrameHoming(prm, mode);
      }

      /* produce encoder homing frame if homed & input=decoder homing frame */
      if ((amrwb_header->reset_flag != 0) && (amrwb_header->reset_flag_old != 0))
      {
         ippsSet_16s(EHF_MASK, (short*)out->pBuffer, AMRWB_Frame);
      }
      else
      {
         if(apiAMRWBDecode(DecObj,(const unsigned char*)in->pBuffer,usc2amrwb[bitrate_idx],rx_type,(unsigned short*)out->pBuffer) != APIAMRWB_StsNoErr){
            return USC_NoOperation;
         }
         if (amrwb_header->trunc) {
            /* Truncate LSBs */
            ippsAndC_16u_I((Ipp16u)IO_MASK, (unsigned short*)out->pBuffer, AMRWB_Frame);
//            int i;
//            short *pPCM = (short *)out->pBuffer;
//            for (i = 0; i < AMRWB_Frame; i ++) pPCM[i] &= IO_MASK;
         }
      }
      /* reset decoder if current frame is a homing frame */
      if (amrwb_header->reset_flag != 0)
      {
         apiAMRWBDecoder_Init((AMRWBDecoder_Obj*)DecObj);
           amrwb_header->usedRate = 6600;
      }
      amrwb_header->reset_flag_old = amrwb_header->reset_flag;

       out->nbytes = AMRWB_Frame*sizeof(short);
       {
          int foo;
          in->nbytes = getBitstreamSize(bitrate_idx, in->frametype, &foo);
       }
    }

   return USC_NoError;
}

static int ownSidSyncInit(SIDSyncState *st)
{
    st->updateSidCount = 3;
    st->prevFrameType = TX_SPEECH;
    return 0;
}

static void ownSidSync(SIDSyncState *st, int valRate, TXFrameType *pTXFrameType)
{
    if ( valRate == AMRWB_RATE_DTX)
    {
        st->updateSidCount--;
        if (st->prevFrameType == TX_SPEECH)
        {
           *pTXFrameType = TX_SID_FIRST;
           st->updateSidCount = 3;
        }
        else
        {
           if (st->updateSidCount == 0)
           {
              *pTXFrameType = TX_SID_UPDATE;
              st->updateSidCount = 8;
           } else {
              *pTXFrameType = TX_NO_DATA;
           }
        }
    }
    else
    {
       st->updateSidCount = 8 ;
       *pTXFrameType = TX_SPEECH;
    }
    st->prevFrameType = *pTXFrameType;
}

static int ownTestPCMFrameHoming(const short *pSrc)
{
    int i, fl = 0;

    for (i = 0; i < AMRWB_Frame; i++)
    {
        fl = pSrc[i] ^ EHF_MASK;
        if (fl) break;
    }

    return (!fl);
}

#define PRML 15
#define NUMPRM6600 NUMBITS6600/PRML + 1
#define NUMPRM8850 NUMBITS8850/PRML + 1
#define NUMPRM12650 NUMBITS12650/PRML + 1
#define NUMPRM14250 NUMBITS14250/PRML + 1
#define NUMPRM15850 NUMBITS15850/PRML + 1
#define NUMPRM18250 NUMBITS18250/PRML + 1
#define NUMPRM19850 NUMBITS19850/PRML + 1
#define NUMPRM23050 NUMBITS23050/PRML + 1
#define NUMPRM23850 NUMBITS23850/PRML + 1

static __ALIGN32 CONST short DecHomFrm6600Tbl[NUMPRM6600] =
{
   3168, 29954, 29213, 16121, 64,
  13440, 30624, 16430, 19008
};

static __ALIGN32 CONST short DecHomFrm8850Tbl[NUMPRM8850] =
{
   3168, 31665, 9943,  9123, 15599,  4358,
  20248, 2048, 17040, 27787, 16816, 13888
};

static __ALIGN32 CONST short DecHomFrm12650Tbl[NUMPRM12650] =
{
  3168, 31665,  9943,  9128,  3647,
  8129, 30930, 27926, 18880, 12319,
   496,  1042,  4061, 20446, 25629,
 28069, 13948
};

static __ALIGN32 CONST short DecHomFrm14250Tbl[NUMPRM14250] =
{
    3168, 31665,  9943,  9131, 24815,
     655, 26616, 26764,  7238, 19136,
    6144,    88,  4158, 25733, 30567,
    30494,  221, 20321, 17823
};

static __ALIGN32 CONST short DecHomFrm15850Tbl[NUMPRM15850] =
{
    3168, 31665,  9943,  9131, 24815,
     700,  3824,  7271, 26400,  9528,
    6594, 26112,   108,  2068, 12867,
   16317, 23035, 24632,  7528,  1752,
    6759, 24576
};

static __ALIGN32 CONST short DecHomFrm18250Tbl[NUMPRM18250] =
{
     3168, 31665,  9943,  9135, 14787,
    14423, 30477, 24927, 25345, 30154,
      916,  5728, 18978,  2048,   528,
    16449,  2436,  3581, 23527, 29479,
     8237, 16810, 27091, 19052,     0
};

static __ALIGN32 CONST short DecHomFrm19850Tbl[NUMPRM19850] =
{
     3168, 31665,  9943,  9129,  8637, 31807,
    24646,   736, 28643,  2977,  2566, 25564,
    12930, 13960,  2048,   834,  3270,  4100,
    26920, 16237, 31227, 17667, 15059, 20589,
    30249, 29123,     0
};

static __ALIGN32 CONST short DecHomFrm23050Tbl[NUMPRM23050] =
{
     3168, 31665,  9943,  9132, 16748,  3202,  28179,
    16317, 30590, 15857, 19960,  8818, 21711,  21538,
     4260, 16690, 20224,  3666,  4194,  9497,  16320,
    15388,  5755, 31551, 14080, 3574,  15932,     50,
    23392, 26053, 31216
};

static __ALIGN32 CONST short DecHomFrm23850Tbl[NUMPRM23850] =
{
     3168, 31665,  9943,  9134, 24776,  5857, 18475,
    28535, 29662, 14321, 16725,  4396, 29353, 10003,
    17068, 20504,   720,     0,  8465, 12581, 28863,
    24774,  9709, 26043,  7941, 27649, 13965, 15236,
    18026, 22047, 16681,  3968
};

static __ALIGN32 CONST short *DecHomFrmTbl[] =
{
  DecHomFrm6600Tbl,
  DecHomFrm8850Tbl,
  DecHomFrm12650Tbl,
  DecHomFrm14250Tbl,
  DecHomFrm15850Tbl,
  DecHomFrm18250Tbl,
  DecHomFrm19850Tbl,
  DecHomFrm23050Tbl,
  DecHomFrm23850Tbl,
  DecHomFrm23850Tbl
};

/* check if the parameters matches the parameters of the corresponding decoder homing frame */
static int ownTestBitstreamFrameHoming (short* pPrmsvec, int valRate)
{
    int i, valFlag;
    short tmp, valSht;
    int valNumBytes = (BitsLenTbl[valRate] + 14) / 15;
    valSht  = (short)(valNumBytes * 15 - BitsLenTbl[valRate]);

    valFlag = 0;
    if (valRate != AMRWB_RATE_DTX)
    {
        if (valRate != AMRWB_RATE_23850)
        {
            for (i = 0; i < valNumBytes-1; i++)
            {
                valFlag = (short)(pPrmsvec[i] ^ DecHomFrmTbl[valRate][i]);
                if (valFlag) break;
            }
        } else
        {
            valSht = 0;
            for (i = 0; i < valNumBytes-1; i++)
            {
                switch (i)
                {
                case 10:
                    tmp = (short)(pPrmsvec[i] & 0x61FF);
                    break;
                case 17:
                    tmp = (short)(pPrmsvec[i] & 0xE0FF);
                    break;
                case 24:
                    tmp = (short)(pPrmsvec[i] & 0x7F0F);
                    break;
                default:
                    tmp = pPrmsvec[i];
                    break;
                }
                valFlag = (short) (tmp ^ DecHomFrmTbl[valRate][i]);
                if (valFlag) break;
            }
        }
        tmp = 0x7fff;
        tmp >>= valSht;
        tmp <<= valSht;
        tmp = (short) (DecHomFrmTbl[valRate][i] & tmp);
        tmp = (short) (pPrmsvec[i] ^ tmp);
        valFlag = (short) (valFlag | tmp);
    }
    else
    {
        valFlag = 1;
    }
    return (!valFlag);
}

static USC_Status CalsOutStreamSize(const USC_Option *options, int bitrate_idx, int nbytesSrc, int *nbytesDst)
{
   int nBlocks, nSamples, foo;

   if(options->direction==0) { /*Encode: src - PCMStream, dst - bitstream*/
      if(options->modes.vad>1) return USC_UnsupportedVADType;

      nSamples = nbytesSrc / (AMRWB_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / AMRWB_Frame;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % AMRWB_Frame) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }

      *nbytesDst = nBlocks * getBitstreamSize(bitrate_idx, TX_SPEECH, &foo);
   } else if(options->direction==1) {/*Decode: src - bitstream, dst - PCMStream*/
      if(options->modes.vad==0) { /*VAD off*/
         nBlocks = nbytesSrc / getBitstreamSize(bitrate_idx, TX_SPEECH, &foo);
      } else if(options->modes.vad==1) { /*VAD on*/
         nBlocks = nbytesSrc / getBitstreamSize(bitrate_idx, TX_SID_FIRST, &foo);
      } else return USC_UnsupportedVADType;

      if (0 == nBlocks) return USC_NoOperation;

      nSamples = nBlocks * AMRWB_Frame;
      *nbytesDst = nSamples * (AMRWB_BITS_PER_SAMPLE >> 3);
   } else if(options->direction==2) {/* Both: src - PCMStream, dst - PCMStream*/
      nSamples = nbytesSrc / (AMRWB_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / AMRWB_Frame;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % AMRWB_Frame) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }
      *nbytesDst = nBlocks * AMRWB_Frame * (AMRWB_BITS_PER_SAMPLE >> 3);
   } else return USC_NoOperation;

   return USC_NoError;
}
static USC_Status GetOutStreamSize(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst)
{
   int bitrate_idx;

   if(options==NULL) return USC_BadDataPointer;
   if(nbytesDst==NULL) return USC_BadDataPointer;
   if(nbytesSrc <= 0) return USC_NoOperation;

   bitrate_idx = CheckRate_AMRWB(bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;

   return CalsOutStreamSize(options, bitrate_idx, nbytesSrc, nbytesDst);
}

USC_Status SetFrameSize(const USC_Option *options, USC_Handle handle, int frameSize)
{
   if(options==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;
   frameSize = frameSize;
   return USC_NoError;
}
