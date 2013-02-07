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
// Purpose: G.729/A/B/D/E speech codec: USC functions.
//
*/
#include "owng729.h"
#include "g729api.h"
#include <string.h>
#include "g729usc.h"

#define  G729I_NUM_RATES  3
#define  G729A_NUM_RATES  1

static USC_Status GetInfoSize(int *pSize);
static USC_Status GetInfo_G729I(USC_Handle handle, USC_CodecInfo *pInfo);
static USC_Status GetInfo_G729A(USC_Handle handle, USC_CodecInfo *pInfo);
static USC_Status NumAlloc(const USC_Option *options, int *nbanks);
static USC_Status MemAlloc_G729I(const USC_Option *options, USC_MemBank *pBanks);
static USC_Status MemAlloc_G729A(const USC_Option *options, USC_MemBank *pBanks);
static USC_Status Init_G729I(const USC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle);
static USC_Status Init_G729A(const USC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle);
static USC_Status Reinit_G729I(const USC_Modes *modes, USC_Handle handle );
static USC_Status Reinit_G729A(const USC_Modes *modes, USC_Handle handle );
static USC_Status Control_G729I(const USC_Modes *modes, USC_Handle handle );
static USC_Status Control_G729A(const USC_Modes *modes, USC_Handle handle );
static USC_Status Encode_G729I(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out);
static USC_Status Encode_G729A(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out);
static USC_Status Decode_G729I(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out);
static USC_Status Decode_G729A(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out);
static USC_Status GetOutStreamSizeI(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst);
static USC_Status GetOutStreamSizeA(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst);
static USC_Status SetFrameSize(const USC_Option *options, USC_Handle handle, int frameSize);

#define BITSTREAM_SIZE       15
#define G729_SPEECH_FRAME    80
#define G729_BITS_PER_SAMPLE 16
#define G729_SID_FRAMESIZE    2
#define G729_NUM_UNTR_FRAMES  2

typedef struct {
    int direction;
    int bitrate;
    int vad;
    int reserved; // for future extension
} G729_Handle_Header;

/* global usc vector table */
USCFUN USC_Fxns USC_G729I_Fxns=
{
    {
        USC_Codec,
        GetInfoSize,
        GetInfo_G729I,
        NumAlloc,
        MemAlloc_G729I,
        Init_G729I,
        Reinit_G729I,
        Control_G729I
    },
    Encode_G729I,
    Decode_G729I,
    GetOutStreamSizeI,
    SetFrameSize

};

USCFUN USC_Fxns USC_G729A_Fxns=
{
    {
        USC_Codec,
        GetInfoSize,
        GetInfo_G729A,
        NumAlloc,
        MemAlloc_G729A,
        Init_G729A,
        Reinit_G729A,
        Control_G729A
    },
    Encode_G729A,
    Decode_G729A,
    GetOutStreamSizeA,
    SetFrameSize

};

static USC_Option  params;  /* what is supported  */
static USC_PCMType pcmType; /* codec audio source */


static __ALIGN32 CONST short LostFrame[G729_SPEECH_FRAME]=
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static __ALIGN32 CONST USC_Rates pTblRates_G729I[G729I_NUM_RATES]={
    {11800},
    {8000},
    {6400}
};

static __ALIGN32 CONST USC_Rates pTblRates_G729A[G729A_NUM_RATES]={
    {8000}
};

static int CheckRate_G729I(int rate_bps)
{
    int rate;

    switch(rate_bps) {
      case 8000:  rate = 0; break;
      case 6400:  rate = 2; break;
      case 11800: rate = 3; break;
      default: rate = -1; break;
    }
    return rate;
}

static int CheckRate_G729A(int rate_bps)
{
    int rate;

    switch(rate_bps) {
      case 8000:  rate = 1; break;
      default: rate = -1; break;
    }
    return rate;
}

static USC_Status GetInfoSize(int *pSize)
{
    *pSize = sizeof(USC_CodecInfo);
    return USC_NoError;
}

static USC_Status GetInfo_G729I(USC_Handle handle, USC_CodecInfo *pInfo)
{

    G729_Handle_Header *g729_header;

    pInfo->name = "IPP_G729I";
    pInfo->params.framesize = G729_SPEECH_FRAME*sizeof(short);
    if (handle == NULL) {
       pInfo->params.modes.bitrate = 8000;
       pInfo->params.direction = 0;
       pInfo->params.modes.vad = 1;
    } else {
       g729_header = (G729_Handle_Header*)handle;
       pInfo->params.modes.bitrate = g729_header->bitrate;
       pInfo->params.direction = g729_header->direction;
       pInfo->params.modes.vad = g729_header->vad;
    }
    pInfo->params.modes.truncate = 0;
    pInfo->maxbitsize = BITSTREAM_SIZE;
    pInfo->pcmType.sample_frequency = 8000;
    pInfo->pcmType.bitPerSample = G729_BITS_PER_SAMPLE;
    pInfo->params.modes.hpf = 0;
    pInfo->params.modes.pf = 0;
    pInfo->params.law = 0;
    pInfo->nRates = G729I_NUM_RATES;
    pInfo->pRateTbl = (const USC_Rates *)&pTblRates_G729I;

   return USC_NoError;
}

static USC_Status GetInfo_G729A(USC_Handle handle, USC_CodecInfo *pInfo)
{

    G729_Handle_Header *g729_header;

    pInfo->name = "IPP_G729A";
    pInfo->params.framesize = G729_SPEECH_FRAME*sizeof(short);
    if (handle == NULL) {
       pInfo->params.modes.bitrate = 8000;
       pInfo->params.direction = 0;
       pInfo->params.modes.vad = 1;
    } else {
       g729_header = (G729_Handle_Header*)handle;
       pInfo->params.modes.bitrate = g729_header->bitrate;
       pInfo->params.direction = g729_header->direction;
       pInfo->params.modes.vad = g729_header->vad;
    }
    pInfo->params.modes.truncate = 0;
    pInfo->maxbitsize = BITSTREAM_SIZE;
    pInfo->pcmType.sample_frequency = 8000;
    pInfo->pcmType.bitPerSample = G729_BITS_PER_SAMPLE;
    pInfo->params.modes.hpf = 0;
    pInfo->params.modes.pf = 0;
    pInfo->params.law = 0;
    pInfo->nRates = G729A_NUM_RATES;
    pInfo->pRateTbl = (const USC_Rates *)&pTblRates_G729A;

   return USC_NoError;
}

static USC_Status NumAlloc(const USC_Option *options, int *nbanks)
{
   if(options==NULL) return USC_BadDataPointer;
   if(nbanks==NULL) return USC_BadDataPointer;
   *nbanks = 1;
   return USC_NoError;
}

static USC_Status MemAlloc_G729I(const USC_Option *options, USC_MemBank *pBanks)
{
    unsigned int nbytes;
    int bitrate_idx;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;

    pBanks->pMem = NULL;
    pBanks->align = 32;
    pBanks->memType = USC_OBJECT;
    pBanks->memSpaceType = USC_NORMAL;

    bitrate_idx = CheckRate_G729I(options->modes.bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;

    if (options->direction == 0) /* encode only */
    {
        apiG729Encoder_Alloc(G729I_CODEC, (int*)&nbytes);
    }
    else if (options->direction == 1) /* decode only */
    {
        apiG729Decoder_Alloc(G729I_CODEC, (int*)&nbytes);
    } else {
        return USC_NoOperation;
    }
    pBanks->nbytes = nbytes + sizeof(G729_Handle_Header); /* include header in handle */
    return USC_NoError;
}


static USC_Status MemAlloc_G729A(const USC_Option *options, USC_MemBank *pBanks)
{
    unsigned int nbytes;
    int bitrate_idx;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;

    pBanks->pMem = NULL;

    bitrate_idx = CheckRate_G729A(options->modes.bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;

    if (options->direction == 0) /* encode only */
    {
        apiG729Encoder_Alloc((G729Codec_Type)bitrate_idx, (int*)&nbytes);
    }
    else if (options->direction == 1) /* decode only */
    {
        apiG729Decoder_Alloc((G729Codec_Type)bitrate_idx, (int*)&nbytes);
    } else {
        return USC_NoOperation;
    }
    pBanks->nbytes = nbytes + sizeof(G729_Handle_Header); /* include header in handle */
    return USC_NoError;
}

static USC_Status Init_G729I(const USC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle)
{
    G729_Handle_Header *g729_header;
    int bitrate_idx;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
   if(pBanks->pMem==NULL) return USC_NotInitialized;
   if(pBanks->nbytes<=0) return USC_NotInitialized;
   if(handle==NULL) return USC_InvalidHandler;

    *handle = (USC_Handle*)pBanks->pMem;
    g729_header = (G729_Handle_Header*)*handle;

    bitrate_idx = CheckRate_G729I(options->modes.bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;

    g729_header->vad = options->modes.vad;
    g729_header->bitrate = options->modes.bitrate;
    g729_header->direction = options->direction;

    if (options->direction == 0) /* encode only */
    {
        G729Encoder_Obj *EncObj = (G729Encoder_Obj *)((char*)*handle + sizeof(G729_Handle_Header));
        apiG729Encoder_Init((G729Encoder_Obj*)EncObj,
            (G729Codec_Type)G729I_CODEC,(G729Encode_Mode)g729_header->vad);
    }
    else if (options->direction == 1) /* decode only */
    {
        G729Decoder_Obj *DecObj = (G729Decoder_Obj *)((char*)*handle + sizeof(G729_Handle_Header));
        apiG729Decoder_Init((G729Decoder_Obj*)DecObj, (G729Codec_Type)G729I_CODEC);
    } else {
        return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Init_G729A(const USC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle)
{
    G729_Handle_Header *g729_header;
    int bitrate_idx;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
   if(pBanks->pMem==NULL) return USC_NotInitialized;
   if(pBanks->nbytes<=0) return USC_NotInitialized;
   if(handle==NULL) return USC_InvalidHandler;

    *handle = (USC_Handle*)pBanks->pMem;
    g729_header = (G729_Handle_Header*)*handle;

    bitrate_idx = CheckRate_G729A(options->modes.bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;

    g729_header->vad = options->modes.vad;
    g729_header->bitrate = options->modes.bitrate;
    g729_header->direction = options->direction;

    if (options->direction == 0) /* encode only */
    {
        G729Encoder_Obj *EncObj = (G729Encoder_Obj *)((char*)*handle + sizeof(G729_Handle_Header));
        apiG729Encoder_Init((G729Encoder_Obj*)EncObj,
            (G729Codec_Type)bitrate_idx,(G729Encode_Mode)g729_header->vad);
    }
    else if (options->direction == 1) /* decode only */
    {
        G729Decoder_Obj *DecObj = (G729Decoder_Obj *)((char*)*handle + sizeof(G729_Handle_Header));
        apiG729Decoder_Init((G729Decoder_Obj*)DecObj, (G729Codec_Type)bitrate_idx);
    } else {
        return USC_NoOperation;
    }
    return USC_NoError;
}


static USC_Status Reinit_G729I(const USC_Modes *modes, USC_Handle handle )
{
    G729_Handle_Header *g729_header;
    int bitrate_idx;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

    g729_header = (G729_Handle_Header*)handle;

    bitrate_idx = CheckRate_G729I(modes->bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;

    g729_header->vad = modes->vad;
    g729_header->bitrate = modes->bitrate;

    if (g729_header->direction == 0) /* encode only */
    {
        G729Encoder_Obj *EncObj = (G729Encoder_Obj *)((char*)handle + sizeof(G729_Handle_Header));
        apiG729Encoder_Init((G729Encoder_Obj*)EncObj, G729I_CODEC, (G729Encode_Mode)modes->vad);
    }
    else if (g729_header->direction == 1) /* decode only */
    {
        G729Decoder_Obj *DecObj = (G729Decoder_Obj *)((char*)handle + sizeof(G729_Handle_Header));
        apiG729Decoder_Init((G729Decoder_Obj*)DecObj, G729I_CODEC);
    } else {
        return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Reinit_G729A(const USC_Modes *modes, USC_Handle handle )
{
    G729_Handle_Header *g729_header;
    int bitrate_idx;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

    g729_header = (G729_Handle_Header*)handle;

    bitrate_idx = CheckRate_G729A(modes->bitrate);
     if(bitrate_idx < 0) return USC_UnsupportedBitRate;

     g729_header->vad = modes->vad;
     g729_header->bitrate = modes->bitrate;

    if (g729_header->direction == 0) /* encode only */
    {
        G729Encoder_Obj *EncObj = (G729Encoder_Obj *)((char*)handle + sizeof(G729_Handle_Header));
        apiG729Encoder_Init((G729Encoder_Obj*)EncObj, (G729Codec_Type)bitrate_idx, (G729Encode_Mode)modes->vad);
    }
    else if (g729_header->direction == 1) /* decode only */
    {
        G729Decoder_Obj *DecObj = (G729Decoder_Obj *)((char*)handle + sizeof(G729_Handle_Header));
        apiG729Decoder_Init((G729Decoder_Obj*)DecObj, (G729Codec_Type)bitrate_idx);
    } else {
        return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Control_G729I(const USC_Modes *modes, USC_Handle handle )
{
   G729_Handle_Header *g729_header;
   int bitrate_idx;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

   g729_header = (G729_Handle_Header*)handle;

   bitrate_idx = CheckRate_G729I(modes->bitrate);
   if(bitrate_idx < 0) return USC_UnsupportedBitRate;

   g729_header->vad = modes->vad;
   g729_header->bitrate = modes->bitrate;

   if (g729_header->direction == 0) /* encode only */
   {
        G729Encoder_Obj *EncObj = (G729Encoder_Obj *)((char*)handle + sizeof(G729_Handle_Header));
        apiG729Encoder_Mode((G729Encoder_Obj*)EncObj, (G729Encode_Mode)modes->vad);
   }
   return USC_NoError;
}

static USC_Status Control_G729A(const USC_Modes *modes, USC_Handle handle )
{
   G729_Handle_Header *g729_header;
   int bitrate_idx;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

   g729_header = (G729_Handle_Header*)handle;

   bitrate_idx = CheckRate_G729A(modes->bitrate);
   if(bitrate_idx < 0) return USC_UnsupportedBitRate;

   g729_header->vad = modes->vad;
   g729_header->bitrate = modes->bitrate;

   if (g729_header->direction == 0) /* encode only */
   {
        G729Encoder_Obj *EncObj = (G729Encoder_Obj *)((char*)handle + sizeof(G729_Handle_Header));
        apiG729Encoder_Mode((G729Encoder_Obj*)EncObj, (G729Encode_Mode)modes->vad);
   }
   return USC_NoError;
}

static int getBitstreamSize(int frametype)
{

    switch (frametype) {
      case 0: return 0;
      case 1: return 2;
      case 2: return 8;
      case 3: return 10;
      case 4: return 15;
      default: return 0;
    }
}

static USC_Status Encode_G729I(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out)
{
    G729_Handle_Header *g729_header;
    G729Encoder_Obj *EncObj;
    int bitrate_idx;

    if(in==NULL) return USC_BadDataPointer;
    if(in->nbytes<G729_SPEECH_FRAME*sizeof(short)) return USC_BadDataPointer;

    bitrate_idx = CheckRate_G729I(in->bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;

    g729_header = (G729_Handle_Header*)handle;
    g729_header->bitrate = in->bitrate;
    EncObj = (G729Encoder_Obj *)((char*)handle + sizeof(G729_Handle_Header));

    if(apiG729Encode(EncObj,(const short*)in->pBuffer,(unsigned char*)out->pBuffer,(G729Codec_Type)bitrate_idx,&out->frametype) != APIG729_StsNoErr){
       return USC_NoOperation;
    }
     out->nbytes = getBitstreamSize(out->frametype);
     out->bitrate = in->bitrate;

     in->nbytes = G729_SPEECH_FRAME*sizeof(short);

    return USC_NoError;
}

static USC_Status Encode_G729A(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out)
{
    G729_Handle_Header *g729_header;
    G729Encoder_Obj *EncObj;
    int bitrate_idx;

    if(in==NULL) return USC_BadDataPointer;
    if(in->nbytes<G729_SPEECH_FRAME*sizeof(short)) return USC_BadDataPointer;

    bitrate_idx = CheckRate_G729A(in->bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;

    g729_header = (G729_Handle_Header*)handle;
    g729_header->bitrate = in->bitrate;
    EncObj = (G729Encoder_Obj *)((char*)handle + sizeof(G729_Handle_Header));

    if(apiG729Encode(EncObj,(const short*)in->pBuffer,(unsigned char*)out->pBuffer,(G729Codec_Type)bitrate_idx,&out->frametype) != APIG729_StsNoErr){
       return USC_NoOperation;
    }
    out->nbytes = getBitstreamSize(out->frametype);
    out->bitrate = in->bitrate;

    in->nbytes = G729_SPEECH_FRAME*sizeof(short);

    return USC_NoError;
}


static USC_Status Decode_G729I(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out)
{
    G729_Handle_Header *g729_header;
    G729Decoder_Obj *DecObj;
    int bitrate_idx;

   if(out==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

    g729_header = (G729_Handle_Header*)handle;
    DecObj = (G729Decoder_Obj *)((char*)handle + sizeof(G729_Handle_Header));

    if(in == NULL) {
       /* Lost frame */
       if(apiG729Decode(DecObj,(const unsigned char*)LostFrame,(-1),(short*)out->pBuffer) != APIG729_StsNoErr){
         return USC_NoOperation;
      }
      out->bitrate = g729_header->bitrate;
    } else {
      bitrate_idx = CheckRate_G729I(in->bitrate);
      if(bitrate_idx < 0) return USC_UnsupportedBitRate;

       g729_header->bitrate = in->bitrate;
      if(apiG729Decode(DecObj,(const unsigned char*)in->pBuffer,in->frametype,(short*)out->pBuffer) != APIG729_StsNoErr){
         return USC_NoOperation;
      }
      in->nbytes = getBitstreamSize(in->frametype);
      in->bitrate = g729_header->bitrate;
    }
    out->nbytes = G729_SPEECH_FRAME*sizeof(short);

    return USC_NoError;
}


static USC_Status Decode_G729A(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out)
{
    G729_Handle_Header *g729_header;
    G729Decoder_Obj *DecObj;
    int bitrate_idx;

   if(out==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

    g729_header = (G729_Handle_Header*)handle;
    DecObj = (G729Decoder_Obj *)((char*)handle + sizeof(G729_Handle_Header));

    if(in == NULL) {
      /* Lost frame */
      if(apiG729Decode(DecObj,(const unsigned char*)LostFrame,(-1),(short*)out->pBuffer) != APIG729_StsNoErr){
        return USC_NoOperation;
      }
      out->bitrate = g729_header->bitrate;
    } else {
      bitrate_idx = CheckRate_G729A(in->bitrate);
      if(bitrate_idx < 0) return USC_UnsupportedBitRate;
      g729_header->bitrate = in->bitrate;
      if(apiG729Decode(DecObj,(const unsigned char*)in->pBuffer,in->frametype,(short*)out->pBuffer) != APIG729_StsNoErr){
        return USC_NoOperation;
      }
      in->nbytes = getBitstreamSize(in->frametype);
      out->bitrate = in->bitrate;
    }
    out->nbytes = G729_SPEECH_FRAME*sizeof(short);

    return USC_NoError;
}

static __ALIGN32 CONST int pFrameSize_G729I[G729I_NUM_RATES]={
    15,
    10,
   8
};
static int RateToIndex_G729I(int rate_bps)
{
    int idx;

    switch(rate_bps) {
     case 11800:  idx = 0; break;
      case 8000:  idx = 1; break;
      case 6400: idx = 2; break;
      default: idx = -1; break;
    }
    return idx;
}

static int RateToIndex_G729A(int rate_bps)
{
    int idx;

    switch(rate_bps) {
     case 8000:  idx = 1; break;
      default: idx = -1; break;
    }
    return idx;
}

static USC_Status CalsOutStreamSize(const USC_Option *options, int bitrate_idx, int nbytesSrc, int *nbytesDst)
{
   int nBlocks, nSamples;

   if(options->direction==0) { /*Encode: src - PCMStream, dst - bitstream*/
      if(options->modes.vad>1) return USC_UnsupportedVADType;

      nSamples = nbytesSrc / (G729_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / G729_SPEECH_FRAME;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % G729_SPEECH_FRAME) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }

      *nbytesDst = nBlocks * pFrameSize_G729I[bitrate_idx];
   } else if(options->direction==1) {/*Decode: src - bitstream, dst - PCMStream*/
      if(options->modes.vad==0) { /*VAD off*/
         nBlocks = nbytesSrc / pFrameSize_G729I[bitrate_idx];
      } else if(options->modes.vad==1) { /*VAD on*/
         nBlocks = nbytesSrc / G729_SID_FRAMESIZE;
         nBlocks += nBlocks*G729_NUM_UNTR_FRAMES; /*Len of Untr frame  in bitstream==0, there are 2 untr frames after SID frame*/
      } else return USC_UnsupportedVADType;

      if (0 == nBlocks) return USC_NoOperation;

      nSamples = nBlocks * G729_SPEECH_FRAME;
      *nbytesDst = nSamples * (G729_BITS_PER_SAMPLE >> 3);
   } else if(options->direction==2) {/* Both: src - PCMStream, dst - PCMStream*/
      nSamples = nbytesSrc / (G729_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / G729_SPEECH_FRAME;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % G729_SPEECH_FRAME) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }
      *nbytesDst = nBlocks * G729_SPEECH_FRAME * (G729_BITS_PER_SAMPLE >> 3);
   } else return USC_NoOperation;

   return USC_NoError;
}
static USC_Status GetOutStreamSizeI(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst)
{
   int bitrate_idx;

   if(options==NULL) return USC_BadDataPointer;
   if(nbytesDst==NULL) return USC_BadDataPointer;
   if(nbytesSrc <= 0) return USC_NoOperation;

   bitrate_idx = RateToIndex_G729I(bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;

   return CalsOutStreamSize(options, bitrate_idx, nbytesSrc, nbytesDst);
}

static USC_Status GetOutStreamSizeA(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst)
{
   int bitrate_idx;

   if(options==NULL) return USC_BadDataPointer;
   if(nbytesDst==NULL) return USC_BadDataPointer;
   if(nbytesSrc <= 0) return USC_NoOperation;

   bitrate_idx = RateToIndex_G729A(bitrate);
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
