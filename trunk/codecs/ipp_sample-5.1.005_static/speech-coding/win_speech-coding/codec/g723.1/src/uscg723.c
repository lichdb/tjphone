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
// Purpose: G.723.1 speech codec: USC functions.
//
*/

#include "owng723.h"
#include "g723api.h"
#include <string.h>
#include <usc.h>

#define  G723_NUM_RATES  2

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

#define BITSTREAM_SIZE      24
#define G723_SPEECH_FRAME   240
#define G723_BITS_PER_SAMPLE   16
#define G723_UNTR_FRAMESIZE 1

typedef struct {
    int direction;
    int bitrate;
    int vad;
    int hpf;
    int pf;
    int reserved0; // for future extension
    int reserved1; // for future extension
    int reserved2; // for future extension
} G723_Handle_Header;

/* global usc vector table */
USCFUN USC_Fxns USC_G723_Fxns=
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


static __ALIGN32 CONST short LostFrame[G723_SPEECH_FRAME]=
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
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static __ALIGN32 CONST USC_Rates pTblRates_G723[G723_NUM_RATES]={
    {6300},
    {5300}
};

static int CheckRate_G723(int rate_bps)
{
    int rate;

    switch(rate_bps) {
      case 6300:  rate = 0; break;
      case 5300:  rate = 1; break;
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
    G723_Handle_Header *g723_header;

    pInfo->name = "IPP_G723.1";
    pInfo->params.framesize = G723_SPEECH_FRAME*sizeof(short);
    if (handle == NULL) {
      pInfo->params.modes.bitrate = 6300;
      pInfo->params.direction = 0;
       pInfo->params.modes.vad = 1;
       pInfo->params.modes.hpf = 1;
       pInfo->params.modes.pf = 1;
    } else {
       g723_header = (G723_Handle_Header*)handle;
       pInfo->params.modes.bitrate = g723_header->bitrate;
      pInfo->params.direction = g723_header->direction;
      pInfo->params.modes.vad = g723_header->vad;
       pInfo->params.modes.hpf = g723_header->hpf;
       pInfo->params.modes.pf = g723_header->pf;
    }
     pInfo->params.modes.truncate = 0;
    pInfo->maxbitsize = BITSTREAM_SIZE*sizeof(short);
     pInfo->pcmType.sample_frequency = 8000;
    pInfo->pcmType.bitPerSample = G723_BITS_PER_SAMPLE;
    pInfo->params.law = 0;
    pInfo->nRates = G723_NUM_RATES;
     pInfo->pRateTbl = (const USC_Rates *)&pTblRates_G723;

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
   int nbytes;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
    pBanks->pMem = NULL;
    pBanks->align = 32;
    pBanks->memType = USC_OBJECT;
    pBanks->memSpaceType = USC_NORMAL;

    if (options->direction == 0) /* encode only */
    {
        apiG723Encoder_Alloc(&nbytes);
        pBanks->nbytes = nbytes+sizeof(G723_Handle_Header); /* include direction in handle */
    }
    else if (options->direction == 1) /* decode only */
    {
        apiG723Decoder_Alloc(&nbytes);
        pBanks->nbytes = nbytes+sizeof(G723_Handle_Header); /* include direction in handle */
    } else {
        return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Init(const USC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle)
{
   int mode=0;
   int bitrate_idx;
   G723_Handle_Header *g723_header;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
   if(pBanks->pMem==NULL) return USC_NotInitialized;
   if(pBanks->nbytes<=0) return USC_NotInitialized;
   if(handle==NULL) return USC_InvalidHandler;

    *handle = (USC_Handle*)pBanks->pMem;
    g723_header = (G723_Handle_Header*)*handle;

     bitrate_idx = CheckRate_G723(options->modes.bitrate);
     if(bitrate_idx < 0) return USC_UnsupportedBitRate;
     g723_header->hpf = options->modes.hpf;
     g723_header->pf = options->modes.pf;
     g723_header->vad = options->modes.vad;
     g723_header->bitrate = options->modes.bitrate;
    g723_header->direction = options->direction;

    if (options->direction == 0) /* encode only */
    {
      G723Encoder_Obj *EncObj = (G723Encoder_Obj *)((char*)*handle + sizeof(G723_Handle_Header));
        if(options->modes.hpf) mode |= 2;
        if(options->modes.vad) mode |= 1;
        apiG723Encoder_Init(EncObj, mode);
    }
    else if (options->direction == 1) /* decode only */
    {
      G723Decoder_Obj *DecObj = (G723Decoder_Obj *)((char*)*handle + sizeof(G723_Handle_Header));
        if(options->modes.pf) mode = 1;
        apiG723Decoder_Init(DecObj, mode);
    } else {
      return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Reinit(const USC_Modes *modes, USC_Handle handle )
{
     int mode=0;
     int bitrate_idx;
    G723_Handle_Header *g723_header;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

    g723_header = (G723_Handle_Header*)handle;

     bitrate_idx = CheckRate_G723(modes->bitrate);
     if(bitrate_idx < 0) return USC_UnsupportedBitRate;
     g723_header->hpf = modes->hpf;
     g723_header->pf = modes->pf;
     g723_header->vad = modes->vad;
     g723_header->bitrate = modes->bitrate;

    if (g723_header->direction == 0) /* encode only */
    {
      G723Encoder_Obj *EncObj = (G723Encoder_Obj *)((char*)handle + sizeof(G723_Handle_Header));
        if(modes->hpf) mode |= 2;
        if(modes->vad) mode |= 1;
        apiG723Encoder_Init(EncObj, mode);
    }
    else if (g723_header->direction == 1) /* decode only */
    {
      G723Decoder_Obj *DecObj = (G723Decoder_Obj *)((char*)handle + sizeof(G723_Handle_Header));
        if(modes->pf) mode = 1;
        apiG723Decoder_Init(DecObj, mode);
    } else {
        return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Control(const USC_Modes *modes, USC_Handle handle )
{
   int mode=0;
   G723_Handle_Header *g723_header;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

   g723_header = (G723_Handle_Header*)handle;

   g723_header->hpf = modes->hpf;
   g723_header->pf = modes->pf;
   g723_header->vad = modes->vad;
   g723_header->bitrate = modes->bitrate;

   if (g723_header->direction == 0) { /* encode only */
      G723Encoder_Obj *EncObj = (G723Encoder_Obj *)((char*)handle + sizeof(G723_Handle_Header));
      if(modes->hpf) mode |= 2;
      if(modes->vad) mode |= 1;
      apiG723Encoder_ControlMode(EncObj, mode);
   } else if (g723_header->direction == 1) {/* decode only */
      G723Decoder_Obj *DecObj = (G723Decoder_Obj *)((char*)handle + sizeof(G723_Handle_Header));
      if(modes->pf) mode = 1;
      apiG723Decoder_ControlMode(DecObj, mode);
   }
   return USC_NoError;
}

static int getBitstreamSize(char *pBitstream)
{
    short  Info,size ;

    Info = (short)(pBitstream[0] & (char)0x3) ;
    /* Check frame type and rate informations */
     size=24;
    switch (Info) {
        case 0x0002 : {   /* SID frame */
            size=4;
            break;
        }
        case 0x0003 : {  /* untransmitted silence frame */
            size=1;
            break;
        }
        case 0x0001 : {   /* active frame, low rate */
            size=20;
            break;
        }
        default : {  /* active frame, high rate */
            break;
        }
    }

    return size;
}

static USC_Status Encode(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out)
{
    G723_Handle_Header *g723_header;
     int bitrate_idx;
    G723Encoder_Obj *EncObj;

    if(in==NULL) return USC_BadDataPointer;
    if(in->nbytes<G723_SPEECH_FRAME*sizeof(short)) return USC_NoOperation;

    g723_header = (G723_Handle_Header*)handle;

     bitrate_idx = CheckRate_G723(in->bitrate);
     if(bitrate_idx < 0) return USC_UnsupportedBitRate;
     g723_header->bitrate = in->bitrate;
    EncObj = (G723Encoder_Obj *)((char*)handle + sizeof(G723_Handle_Header));

    if(apiG723Encode(EncObj,(const short*)in->pBuffer, (short)bitrate_idx, out->pBuffer) != APIG723_StsNoErr){
       return USC_NoOperation;
    }
     out->frametype = 0;
     out->nbytes = getBitstreamSize(out->pBuffer);
     out->bitrate = in->bitrate;

     in->nbytes = G723_SPEECH_FRAME*sizeof(short);

    return USC_NoError;
}

static USC_Status Decode(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out)
{
    G723_Handle_Header *g723_header;
    G723Decoder_Obj *DecObj;
     int bitrate_idx;

    if(out==NULL) return USC_BadDataPointer;
    if(handle==NULL) return USC_InvalidHandler;

    g723_header = (G723_Handle_Header*)handle;
    DecObj = (G723Decoder_Obj *)((char*)handle + sizeof(G723_Handle_Header));

    if(in == NULL) {
       /* Lost frame */
       if(apiG723Decode(DecObj,(const char*)LostFrame,1,(short*)out->pBuffer) != APIG723_StsNoErr){
        return USC_NoOperation;
      }
      out->bitrate = g723_header->bitrate;
    } else {
       bitrate_idx = CheckRate_G723(in->bitrate);
       if(bitrate_idx < 0) return USC_UnsupportedBitRate;
       g723_header->bitrate = in->bitrate;
       in->nbytes = getBitstreamSize(in->pBuffer);
       if(apiG723Decode(DecObj,(const char*)in->pBuffer, (short)in->frametype, (short*)out->pBuffer) != APIG723_StsNoErr){
        return USC_NoOperation;
      }
      out->bitrate = in->bitrate;
    }
    out->nbytes = G723_SPEECH_FRAME*sizeof(short);

    return USC_NoError;
}

static __ALIGN32 CONST int pFrameSize_G723[G723_NUM_RATES]={
    24,
    20
};

static USC_Status GetOutStreamSize(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst)
{
   int bitrate_idx;
   int nBlocks, nSamples;

   if(options==NULL) return USC_BadDataPointer;
   if(nbytesDst==NULL) return USC_BadDataPointer;
   if(nbytesSrc <= 0) return USC_NoOperation;

   bitrate_idx = CheckRate_G723(bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;

   if(options->direction==0) { /*Encode: src - PCMStream, dst - bitstream*/
      if(options->modes.vad>1) return USC_UnsupportedVADType;

      nSamples = nbytesSrc / (G723_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / G723_SPEECH_FRAME;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % G723_SPEECH_FRAME) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }

      *nbytesDst = nBlocks * pFrameSize_G723[bitrate_idx];
   } else if(options->direction==1) {/*Decode: src - bitstream, dst - PCMStream*/
      if(options->modes.vad==0) { /*VAD off*/
         nBlocks = nbytesSrc / pFrameSize_G723[bitrate_idx];
      } else if(options->modes.vad==1) { /*VAD on*/
         nBlocks = nbytesSrc / G723_UNTR_FRAMESIZE;
      } else return USC_UnsupportedVADType;

      if (0 == nBlocks) return USC_NoOperation;

      nSamples = nBlocks * G723_SPEECH_FRAME;
      *nbytesDst = nSamples * (G723_BITS_PER_SAMPLE >> 3);
   } else if(options->direction==2) {/* Both: src - PCMStream, dst - PCMStream*/
      nSamples = nbytesSrc / (G723_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / G723_SPEECH_FRAME;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % G723_SPEECH_FRAME) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }
      *nbytesDst = nBlocks * G723_SPEECH_FRAME * (G723_BITS_PER_SAMPLE >> 3);
   } else return USC_NoOperation;
   return USC_NoError;
}

USC_Status SetFrameSize(const USC_Option *options, USC_Handle handle, int frameSize)
{
   if(options==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;
   frameSize = frameSize;

   return USC_NoError;
}
