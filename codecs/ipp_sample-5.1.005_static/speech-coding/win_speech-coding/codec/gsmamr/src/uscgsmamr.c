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
// Purpose: GSMAMR speech codec: USC funtions.
//
*/

#include "owngsmamr.h"
#include "gsmamrapi.h"
#include <string.h>
#include "gsmamrusc.h"

#define  GSMAMR_NUM_RATES  8
#define GSMAMR_BITS_PER_SAMPLE 16

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

typedef struct {
    short sid_update_counter;
    TXFrameType prev_ft;
} sid_syncState;

typedef struct {
    int direction;
    int trunc;
    int bitrate;
    int vad;
    int bitrate_old;
    short reset_flag;
    short reset_flag_old;
    sid_syncState sid_state;
} GSMAMR_Handle_Header;

static int sid_sync_init (sid_syncState *st);
static void sid_sync(sid_syncState *st, int mode, TXFrameType *tx_frame_type);
static int is_pcm_frame_homing (short *input_frame);
static short is_bitstream_frame_homing (unsigned char* bitstream, int mode);

/* global usc vector table */
USCFUN USC_Fxns USC_GSMAMR_Fxns=
{
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

static __ALIGN32 CONST int bitsLen[N_MODES]={
   95, 103, 118, 134, 148, 159, 204, 244, 35
};

static __ALIGN32 CONST short LostFrame[GSMAMR_Frame]=
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
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static __ALIGN32 CONST USC_Rates pTblRates_GSMAMR[GSMAMR_NUM_RATES]={
    {12200},
    {10200},
    {7950},
    {7400},
    {6700},
    {5900},
    {5150},
   {4750}
};

static int CheckRate_GSMAMR(int rate_bps)
{
    int rate;

    switch(rate_bps) {
      case 4750:  rate = 0; break;
      case 5150:  rate = 1; break;
      case 5900:  rate = 2; break;
      case 6700:  rate = 3; break;
      case 7400:  rate = 4; break;
      case 7950:  rate = 5; break;
      case 10200: rate = 6; break;
      case 12200: rate = 7; break;
      default: rate = -1; break;
    }
    return rate;
}

static int CheckIdxRate_GSMAMR(int idx_rate)
{
    int rate;

    switch(idx_rate) {
      case 0: rate = 4750; break;
      case 1: rate = 5150; break;
      case 2: rate = 5900; break;
      case 3: rate = 6700; break;
      case 4: rate = 7400; break;
      case 5: rate = 7950; break;
      case 6: rate = 10200; break;
      case 7: rate = 12200; break;
      default: rate = -1; break;
    }
    return rate;
}

static USC_Status GetInfoSize(int *pSize)
{
    *pSize = sizeof(USC_CodecInfo);
    return USC_NoError;
}

static USC_Status GetInfo(USC_Handle handle, USC_CodecInfo *pInfo)
{
    GSMAMR_Handle_Header *gsmamr_header;

    pInfo->name = "IPP_GSMAMR";
    pInfo->params.framesize = GSMAMR_Frame*sizeof(short);
    if (handle == NULL) {
      pInfo->params.modes.bitrate = 4750;
      pInfo->params.modes.truncate = 1;
      pInfo->params.direction = 0;
      pInfo->params.modes.vad = 2;
    } else {
      gsmamr_header = (GSMAMR_Handle_Header*)handle;
      pInfo->params.modes.bitrate = gsmamr_header->bitrate;
      pInfo->params.modes.truncate = gsmamr_header->trunc;
      pInfo->params.direction = gsmamr_header->direction;
      pInfo->params.modes.vad = gsmamr_header->vad;
    }
    pInfo->maxbitsize = BITSTREAM_SIZE;
    pInfo->pcmType.sample_frequency = 8000;
    pInfo->pcmType.bitPerSample = GSMAMR_BITS_PER_SAMPLE;
    pInfo->params.modes.hpf = 0;
    pInfo->params.modes.pf = 0;
    pInfo->params.law = 0;
    pInfo->nRates = GSMAMR_NUM_RATES;
    pInfo->pRateTbl = (const USC_Rates *)&pTblRates_GSMAMR;


    return USC_NoError;
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

    pBanks->pMem = NULL;
    pBanks->align = 32;
    pBanks->memType = USC_OBJECT;
    pBanks->memSpaceType = USC_NORMAL;
   if (options->direction == 0) /* encode only */
    {
        GSMAMREnc_Params enc_params;
        enc_params.codecType = (GSMAMRCodec_Type)0;
        enc_params.mode = options->modes.vad;
        apiGSMAMREncoder_Alloc(&enc_params, &nbytes);
    }
    else if (options->direction == 1) /* decode only */
    {
        GSMAMRDec_Params dec_params;
        dec_params.codecType = (GSMAMRCodec_Type)0;
        dec_params.mode = options->modes.vad;
        apiGSMAMRDecoder_Alloc(&dec_params, &nbytes);
    } else {
        return USC_NoOperation;
    }
    pBanks->nbytes = nbytes + sizeof(GSMAMR_Handle_Header); /* room for USC header */
    return USC_NoError;
}

static USC_Status Init(const USC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle)
{
    GSMAMR_Handle_Header *gsmamr_header;
    int bitrate_idx;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
   if(pBanks->pMem==NULL) return USC_NotInitialized;
   if(pBanks->nbytes<=0) return USC_NotInitialized;
   if(handle==NULL) return USC_InvalidHandler;

    *handle = (USC_Handle*)pBanks->pMem;
    gsmamr_header = (GSMAMR_Handle_Header*)*handle;

    bitrate_idx = CheckRate_GSMAMR(options->modes.bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;
    gsmamr_header->direction = options->direction;
    gsmamr_header->trunc = options->modes.truncate;
    gsmamr_header->vad = options->modes.vad;
    gsmamr_header->bitrate = options->modes.bitrate;
    gsmamr_header->bitrate_old = 0;

    if (options->direction == 0) /* encode only */
    {
        GSMAMREncoder_Obj *EncObj = (GSMAMREncoder_Obj *)((char*)*handle + sizeof(GSMAMR_Handle_Header));
        apiGSMAMREncoder_Init((GSMAMREncoder_Obj*)EncObj, gsmamr_header->vad);
    }
    else if (options->direction == 1) /* decode only */
    {
        GSMAMRDecoder_Obj *DecObj = (GSMAMRDecoder_Obj *)((char*)*handle + sizeof(GSMAMR_Handle_Header));
        apiGSMAMRDecoder_Init((GSMAMRDecoder_Obj*)DecObj, gsmamr_header->vad);
        gsmamr_header->reset_flag = 0;
        gsmamr_header->reset_flag_old = 1;
    } else {
        return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Reinit(const USC_Modes *modes, USC_Handle handle )
{
    GSMAMR_Handle_Header *gsmamr_header;
    int bitrate_idx;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

    gsmamr_header = (GSMAMR_Handle_Header*)handle;

    bitrate_idx = CheckRate_GSMAMR(modes->bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;
    gsmamr_header->vad = modes->vad;
    gsmamr_header->bitrate = modes->bitrate;
    gsmamr_header->bitrate_old = 0;

    if (gsmamr_header->direction == 0) /* encode only */
    {
        GSMAMREncoder_Obj *EncObj = (GSMAMREncoder_Obj *)((char*)handle + sizeof(GSMAMR_Handle_Header));
        gsmamr_header->reset_flag = 0;
        sid_sync_init (&gsmamr_header->sid_state);
        apiGSMAMREncoder_Init((GSMAMREncoder_Obj*)EncObj, modes->vad);
    }
    else if (gsmamr_header->direction == 1) /* decode only */
    {
        GSMAMRDecoder_Obj *DecObj = (GSMAMRDecoder_Obj *)((char*)handle + sizeof(GSMAMR_Handle_Header));
        apiGSMAMRDecoder_Init((GSMAMRDecoder_Obj*)DecObj, modes->vad);
        gsmamr_header->reset_flag = 0;
        gsmamr_header->reset_flag_old = 1;
    } else {
        return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Control(const USC_Modes *modes, USC_Handle handle )
{
   GSMAMR_Handle_Header *gsmamr_header;
   int bitrate_idx;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

   gsmamr_header = (GSMAMR_Handle_Header*)handle;

   bitrate_idx = CheckRate_GSMAMR(modes->bitrate);
   if(bitrate_idx < 0) return USC_UnsupportedBitRate;
   gsmamr_header->vad = modes->vad;
   gsmamr_header->bitrate = modes->bitrate;

   if (gsmamr_header->direction == 0) /* encode only */
   {
      GSMAMREncoder_Obj *EncObj = (GSMAMREncoder_Obj *)((char*)handle + sizeof(GSMAMR_Handle_Header));
      apiGSMAMREncoder_Mode((GSMAMREncoder_Obj*)EncObj, modes->vad);
   }
   else if (gsmamr_header->direction == 1) /* decode only */
   {
      GSMAMRDecoder_Obj *DecObj = (GSMAMRDecoder_Obj *)((char*)handle + sizeof(GSMAMR_Handle_Header));
      apiGSMAMRDecoder_Mode((GSMAMRDecoder_Obj*)DecObj, modes->vad);
   } else {
      return USC_NoOperation;
   }
    return USC_NoError;
}

#define IO_MASK (Ipp16u)0xfff8     /* 13-bit input/output                      */
static __ALIGN32 CONST GSMAMR_Rate_t usc2amr[8]={
    GSMAMR_RATE_4750,
    GSMAMR_RATE_5150,
    GSMAMR_RATE_5900,
    GSMAMR_RATE_6700,
    GSMAMR_RATE_7400,
    GSMAMR_RATE_7950,
    GSMAMR_RATE_10200,
    GSMAMR_RATE_12200
};

static int getBitstreamSize(int rate, int frametype, int *outRate)
{
   int nbytes;
   int usedRate = rate;

   if (frametype != TX_SPEECH_GOOD) {
        usedRate = GSMAMR_RATE_DTX;
   }
    nbytes = ((bitsLen[usedRate] + 7) >> 3);
   //*outRate = usedRate;
   *outRate = rate;

   return nbytes;

}

static USC_Status Encode(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out)
{
    GSMAMR_Handle_Header *gsmamr_header;
    GSMAMREncoder_Obj *EncObj;
    int bitrate_idx, out_WrkRate;
    IPP_ALIGNED_ARRAY(16, unsigned short, work_buf, GSMAMR_Frame);
    int pVad = 0;

    if(in==NULL) return USC_BadDataPointer;
    if(in->nbytes<GSMAMR_Frame*sizeof(short)) return USC_NoOperation;

    gsmamr_header = (GSMAMR_Handle_Header*)handle;

    bitrate_idx = CheckRate_GSMAMR(in->bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;
    gsmamr_header->bitrate = in->bitrate;
    EncObj = (GSMAMREncoder_Obj *)((char*)handle + sizeof(GSMAMR_Handle_Header));

    /* check for homing frame */
    gsmamr_header->reset_flag = (short)is_pcm_frame_homing((short*)in->pBuffer);

    if (gsmamr_header->trunc) {
        /* Delete the LSBs */
        ippsAndC_16u((Ipp16u*)in->pBuffer, IO_MASK, work_buf, GSMAMR_Frame);
    } else {
        ippsCopy_16s((const Ipp16s*)in->pBuffer, (Ipp16s*)work_buf, GSMAMR_Frame);
    }

    if(apiGSMAMREncode(EncObj,(short*)work_buf,usc2amr[bitrate_idx&7],(unsigned char*)out->pBuffer,&pVad) != APIGSMAMR_StsNoErr){
       return USC_NoOperation;
    }

    /* include frame type and mode information in serial bitstream */
    if(!pVad) {
        sid_sync (&gsmamr_header->sid_state, (int)GSMAMR_RATE_DTX, (TXFrameType *)&out->frametype);
    } else {
        sid_sync (&gsmamr_header->sid_state, bitrate_idx, (TXFrameType *)&out->frametype);
    }

    out->nbytes = getBitstreamSize(bitrate_idx, out->frametype, &out_WrkRate);
    out->bitrate = CheckIdxRate_GSMAMR(out_WrkRate);

    in->nbytes = GSMAMR_Frame*sizeof(short);

    if (gsmamr_header->reset_flag != 0) {
        sid_sync_init (&gsmamr_header->sid_state);
        apiGSMAMREncoder_Init(EncObj, EncObj->objPrm.mode);
    }

    return USC_NoError;
}

static RXFrameType tx2rx (TXFrameType tx_type)
{
    switch (tx_type) {
      case TX_SPEECH_GOOD:      return RX_SPEECH_GOOD;
      case TX_SID_FIRST:        return RX_SID_FIRST;
      case TX_SID_UPDATE:       return RX_SID_UPDATE;
      case TX_NO_DATA:          return RX_NO_DATA;
      case TX_SPEECH_DEGRADED:  return RX_SPEECH_DEGRADED;
      case TX_SPEECH_BAD:       return RX_SPEECH_BAD;
      case TX_SID_BAD:          return RX_SID_BAD;
      case TX_ONSET:            return RX_ONSET;
      default:
         return (RXFrameType)(-1);
    }
}

static USC_Status Decode(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out)
{
    GSMAMR_Handle_Header *gsmamr_header;
    int bitrate_idx;
    GSMAMRDecoder_Obj *DecObj;

    if(out==NULL) return USC_BadDataPointer;
    if(handle==NULL) return USC_InvalidHandler;

    gsmamr_header = (GSMAMR_Handle_Header*)handle;
    DecObj = (GSMAMRDecoder_Obj *)((char*)handle + sizeof(GSMAMR_Handle_Header));

    if(in == NULL) {
       /* Lost frame */
      bitrate_idx = CheckRate_GSMAMR(gsmamr_header->bitrate_old);
      if(bitrate_idx < 0) return USC_UnsupportedBitRate;

      gsmamr_header->bitrate = gsmamr_header->bitrate_old;

      if(apiGSMAMRDecode(DecObj,(const unsigned char*)LostFrame,(GSMAMR_Rate_t)bitrate_idx,
           RX_SPEECH_BAD,(short*)out->pBuffer) != APIGSMAMR_StsNoErr){
            return USC_NoOperation;
      }
      if(gsmamr_header->trunc) {
         /* Truncate LSBs */
         ippsAndC_16u_I(IO_MASK, (unsigned short*)out->pBuffer, GSMAMR_Frame);
      }
      gsmamr_header->reset_flag = 0;
      gsmamr_header->reset_flag_old = 1;

      out->nbytes = GSMAMR_Frame*sizeof(short);
    } else {
      bitrate_idx = CheckRate_GSMAMR(in->bitrate);
       if(bitrate_idx < 0) return USC_UnsupportedBitRate;

      gsmamr_header->bitrate = in->bitrate;

      gsmamr_header->bitrate_old = in->bitrate;

       gsmamr_header->reset_flag = is_bitstream_frame_homing((unsigned char *)in->pBuffer, bitrate_idx);

      /* produce encoder homing frame if homed & input=decoder homing frame */
      if ((gsmamr_header->reset_flag != 0) && (gsmamr_header->reset_flag_old != 0))
      {
         ippsSet_16s(EHF_MASK, (short*)out->pBuffer, GSMAMR_Frame);
      }
      else
      {
          if(apiGSMAMRDecode(DecObj,(const unsigned char*)in->pBuffer,(GSMAMR_Rate_t)bitrate_idx,
             tx2rx((TXFrameType)in->frametype),(short*)out->pBuffer) != APIGSMAMR_StsNoErr){
             return USC_NoOperation;
          }
          if (gsmamr_header->trunc) {
             /* Truncate LSBs */
             ippsAndC_16u_I(IO_MASK, (unsigned short*)out->pBuffer, GSMAMR_Frame);
          }
      }
      /* reset decoder if current frame is a homing frame */
      if (gsmamr_header->reset_flag != 0)
      {
          apiGSMAMRDecoder_Init(DecObj, DecObj->objPrm.mode);
      }
      gsmamr_header->reset_flag_old = gsmamr_header->reset_flag;

       out->nbytes = GSMAMR_Frame*sizeof(short);
       {
          int foo;
          in->nbytes = getBitstreamSize(bitrate_idx, in->frametype, &foo);
       }
    }

    out->bitrate = gsmamr_header->bitrate;

   return USC_NoError;
}

static int is_pcm_frame_homing (short *input_frame)
{
    int i, j=0;

    /* check 160 input samples for matching EHF_MASK: defined in e_homing.h */
    for (i = 0; i < GSMAMR_Frame; i++)
    {
        j = input_frame[i] ^ EHF_MASK;

        if (j)
            break;
    }

    return !j;
}

static int sid_sync_init (sid_syncState *state)
{
    state->sid_update_counter = 3;
    state->prev_ft = TX_SPEECH_GOOD;
    return 0;
}

static void sid_sync (sid_syncState *st, GSMAMR_Rate_t mode,
               TXFrameType *tx_frame_type)
{
    if ( mode == GSMAMR_RATE_DTX){
       st->sid_update_counter--;
        if (st->prev_ft == TX_SPEECH_GOOD)
        {
           *tx_frame_type = TX_SID_FIRST;
           st->sid_update_counter = 3;
        }
        else
        {
           if (st->sid_update_counter == 0)
           {
              *tx_frame_type = TX_SID_UPDATE;
              st->sid_update_counter = 8;
           } else {
              *tx_frame_type = TX_NO_DATA;
           }
        }
    }
    else
    {
       st->sid_update_counter = 8 ;
       *tx_frame_type = TX_SPEECH_GOOD;
    }
    st->prev_ft = *tx_frame_type;
    return;
}

/*
  homing decoder frames by modes
*/
static __ALIGN32 CONST unsigned char home_bits_122[]={
   0x08,0x54,0xdb,0x96,
   0xaa,0xad,0x60,0x00,
   0x00,0x00,0x00,0x1b,
   0x58,0x7f,0x66,0x83,
   0x79,0x40,0x90,0x04,
   0x15,0x85,0x4f,0x10,
   0xf6,0xb0,0x24,0x03,
   0xc7,0xea,0x00
};
static __ALIGN32 CONST unsigned char home_bits_102[]={
   0xf8,0x71,0x8b,0xd1,
   0x40,0x00,0x00,0x00,
   0x00,0xda,0xe4,0xc6,
   0x77,0xea,0x2c,0x40,
   0xad,0x6b,0x3d,0x80,
   0x6c,0x17,0xc8,0x55,
   0xc3,0x00
};
static __ALIGN32 CONST unsigned char home_bits_795[]={
   0x61,0x38,0xc5,0xf7,
   0xa0,0x06,0xfa,0x07,
   0x3c,0x08,0x7a,0x5b,
   0x1c,0x69,0xbc,0x41,
   0xca,0x68,0x3c,0x82
};
static __ALIGN32 CONST unsigned char home_bits_74[]={
   0xf8,0x71,0x8b,0xef,
   0x40,0x0d,0xe0,0x36,
   0x20,0x8f,0xc4,0xc1,
   0xba,0x6f,0x01,0xb0,
   0x03,0x78,0x00
};
static __ALIGN32 CONST unsigned char home_bits_67[]={
   0xf8,0x71,0x8b,0xef,
   0x40,0x17,0x01,0xe2,
   0x63,0xe1,0x60,0xb8,
   0xbc,0x07,0xb1,0x8e,
   0x00
};
static __ALIGN32 CONST unsigned char home_bits_59[]={
   0xf8,0x71,0x8b,0xef,
   0x40,0x1e,0xfe,0x01,
   0xcf,0x60,0x7c,0xfb,
   0xf8,0x03,0xdc
};
static __ALIGN32 CONST unsigned char home_bits_515[]={
   0xf8,0x9d,0x38,0xcc,
   0x03,0xdf,0xc0,0x62,
   0xfb,0x7f,0x7f,0x47,
   0xbe
};
static __ALIGN32 CONST unsigned char home_bits_475[]={
   0xf8,0x9d,0x38,0xcc,
   0x03,0x28,0xf7,0x0f,
   0xb1,0x82,0x3d,0x36
};

static __ALIGN32 CONST unsigned char *d_homes[] =
{
   home_bits_475,
   home_bits_515,
   home_bits_59,
   home_bits_67,
   home_bits_74,
   home_bits_795,
   home_bits_102,
   home_bits_122
};

static short is_bitstream_frame_homing (unsigned char* bitstream, int mode)
{
   int i;
   short j=0;
   int bitstremlen = (bitsLen[mode]+7)>>3;
   for (i = 0; i < bitstremlen; i++)
   {
       j = (short)(bitstream[i] ^ d_homes[mode][i]);

       if (j)
           break;
   }
    return (short)(!j);

}

static USC_Status CalsOutStreamSize(const USC_Option *options, int bitrate_idx, int nbytesSrc, int *nbytesDst)
{
   int nBlocks, nSamples, foo;

   if(options->direction==0) { /*Encode: src - PCMStream, dst - bitstream*/
      if(options->modes.vad>2) return USC_UnsupportedVADType;

      nSamples = nbytesSrc / (GSMAMR_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / GSMAMR_Frame;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % GSMAMR_Frame) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }

      *nbytesDst = nBlocks * getBitstreamSize(bitrate_idx, TX_SPEECH_GOOD, &foo);
   } else if(options->direction==1) {/*Decode: src - bitstream, dst - PCMStream*/
      if(options->modes.vad==0) { /*VAD off*/
         nBlocks = nbytesSrc / getBitstreamSize(bitrate_idx, TX_SPEECH_GOOD, &foo);
      } else if((options->modes.vad==1)||(options->modes.vad==2)) { /*VAD on*/
         nBlocks = nbytesSrc / getBitstreamSize(bitrate_idx, TX_SID_FIRST, &foo);
      } else return USC_UnsupportedVADType;

      if (0 == nBlocks) return USC_NoOperation;

      nSamples = nBlocks * GSMAMR_Frame;
      *nbytesDst = nSamples * (GSMAMR_BITS_PER_SAMPLE >> 3);
   } else if(options->direction==2) {/* Both: src - PCMStream, dst - PCMStream*/
      nSamples = nbytesSrc / (GSMAMR_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / GSMAMR_Frame;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % GSMAMR_Frame) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }
      *nbytesDst = nBlocks * GSMAMR_Frame * (GSMAMR_BITS_PER_SAMPLE >> 3);
   } else return USC_NoOperation;

   return USC_NoError;
}
static USC_Status GetOutStreamSize(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst)
{
   int bitrate_idx;

   if(options==NULL) return USC_BadDataPointer;
   if(nbytesDst==NULL) return USC_BadDataPointer;
   if(nbytesSrc <= 0) return USC_NoOperation;

   bitrate_idx = CheckRate_GSMAMR(bitrate);
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
