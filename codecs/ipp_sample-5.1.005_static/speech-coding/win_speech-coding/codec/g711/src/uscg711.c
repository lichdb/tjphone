/*/////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2004-2006 Intel Corporation. All Rights Reserved.
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
// Purpose: G711 speech codec: USC functions.
//
*/

#include <string.h>
#include <usc.h>
#include "owng711.h"
#include "g711api.h"

static USC_Status GetInfoSize(int *pSize);
static USC_Status GetInfoA(USC_Handle handle, void *pInfo);
static USC_Status GetInfoU(USC_Handle handle, void *pInfo);
static USC_Status NumAlloc(const void *options, int *nbanks);
static USC_Status MemAlloc(const void *options, USC_MemBank *pBanks);
static USC_Status InitA(const void *options, const USC_MemBank *pBanks, USC_Handle *handle);
static USC_Status InitU(const void *options, const USC_MemBank *pBanks, USC_Handle *handle);
static USC_Status ReinitA(const void *modes, USC_Handle handle );
static USC_Status ReinitU(const void *modes, USC_Handle handle );
static USC_Status ControlA(const void *modes, USC_Handle handle );
static USC_Status ControlU(const void *modes, USC_Handle handle );
static USC_Status Encode(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out);
static USC_Status Decode(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out);
static USC_Status GetOutStreamSize(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst);
static USC_Status SetFrameSize(const USC_Option *options, USC_Handle handle, int frameSize);

#define BITSTREAM_SIZE  80
#define G711_NUM_RATES  1
#define G711_SPEECH_FRAME         80
#define G711_BITS_PER_SAMPLE      16
#define G711_SAMPLE_FREQUENCE     8000

typedef struct {
    int   direction;
    int   pf;
    int   vad;
    int   law;        /* 0 - pcm, 1 - aLaw, 2 -muLaw */
} G711_Handle_Header;

/* global usc vector table */

USC_Fxns USC_G711A_Fxns=
{
   {
      USC_Codec,
      GetInfoSize,
      GetInfoA,
      NumAlloc,
      MemAlloc,
      InitA,
      ReinitA,
      ControlA
   },
   Encode,
   Decode,
   GetOutStreamSize,
   SetFrameSize

 };


USC_Fxns USC_G711U_Fxns=
{
   {
      USC_Codec,
      GetInfoSize,
      GetInfoU,
      NumAlloc,
      MemAlloc,
      InitU,
      ReinitU,
      ControlU
   },
   Encode,
   Decode,
   GetOutStreamSize,
   SetFrameSize
};

static __ALIGN32 CONST USC_Rates pTblRates_G711[G711_NUM_RATES]={
    {64000}
};
static USC_Status GetInfoA(USC_Handle handle, void *pInf)
{
    USC_CodecInfo *pInfo=(USC_CodecInfo *)pInf;
    G711_Handle_Header *g711_header;
    pInfo->name = "IPP_G711A";
    pInfo->params.framesize = G711_SPEECH_FRAME*sizeof(short);
    if (handle == NULL) {
        pInfo->params.direction = 0;
        pInfo->params.law = G711_ALAW_CODEC;
        pInfo->params.modes.bitrate = 64000;
        pInfo->params.modes.vad = 1;
        pInfo->params.modes.pf = 1;
    } else {
        g711_header = (G711_Handle_Header*)handle;
        pInfo->params.direction = g711_header->direction;
        pInfo->params.law = g711_header->law;
        pInfo->params.modes.vad = g711_header->vad;
        pInfo->params.modes.pf = g711_header->pf;
    }
    pInfo->maxbitsize = BITSTREAM_SIZE;
    pInfo->pcmType.sample_frequency = G711_SAMPLE_FREQUENCE;
    pInfo->pcmType.bitPerSample = G711_BITS_PER_SAMPLE;
    pInfo->params.modes.truncate = 0;
    pInfo->params.modes.hpf = 0;
    pInfo->nRates = G711_NUM_RATES;
    pInfo->pRateTbl = (const USC_Rates *)&pTblRates_G711;

    return USC_NoError;
}

static USC_Status GetInfoSize(int *pSize)
{
    *pSize = sizeof(USC_CodecInfo);
    return USC_NoError;
}

static USC_Status GetInfoU(USC_Handle handle, void *pInf)
{
    USC_CodecInfo *pInfo=(USC_CodecInfo *)pInf;
    G711_Handle_Header *g711_header;
    pInfo->name = "IPP_G711U";
    pInfo->params.framesize = G711_SPEECH_FRAME*sizeof(short);
    if (handle == NULL) {
        pInfo->params.direction = 0;
        pInfo->params.law = G711_ALAW_CODEC;
        pInfo->params.modes.bitrate = 64000;
        pInfo->params.modes.vad = 1;
         pInfo->params.modes.pf = 1;
    } else {
        g711_header = (G711_Handle_Header*)handle;
        pInfo->params.direction = g711_header->direction;
        pInfo->params.law = g711_header->law;
        pInfo->params.modes.vad = g711_header->vad;
        pInfo->params.modes.pf = g711_header->pf;
    }
    pInfo->maxbitsize = BITSTREAM_SIZE;
    pInfo->pcmType.sample_frequency = G711_SAMPLE_FREQUENCE;
    pInfo->pcmType.bitPerSample = G711_BITS_PER_SAMPLE;
    pInfo->params.modes.truncate = 0;
    pInfo->params.modes.hpf = 0;
    pInfo->nRates = G711_NUM_RATES;
    pInfo->pRateTbl = (const USC_Rates *)&pTblRates_G711;

    return USC_NoError;
}

static USC_Status NumAlloc(const void *opt, int *nbanks)
{
   const USC_Option *options=(const USC_Option *) opt;
   if(options==NULL) return USC_BadDataPointer;
   if(nbanks==NULL) return USC_BadDataPointer;
   *nbanks = 1;
   return USC_NoError;
}

static USC_Status MemAlloc(const void *opt, USC_MemBank *pBanks)
{
   const USC_Option *options=(const USC_Option *) opt;
   unsigned int nbytes;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;

   pBanks->pMem = NULL;
   pBanks->align = 32;
   pBanks->memType = USC_OBJECT;
   pBanks->memSpaceType = USC_NORMAL;

   if (options->direction == 0) { /* encode only */
      apiG711Encoder_Alloc((int *)&nbytes);
      pBanks->nbytes = nbytes+sizeof(G711_Handle_Header); /* include direction in handle */
   } else if (options->direction == 1) { /* decode only */
      apiG711Decoder_Alloc((int *)&nbytes);
      pBanks->nbytes = nbytes+sizeof(G711_Handle_Header); /* include direction in handle */
   } else {
      return USC_NoOperation;
   }
   return USC_NoError;
}

static USC_Status InitA(const void *opt, const USC_MemBank *pBanks, USC_Handle *handle)
{
   const USC_Option *options=(const USC_Option *) opt;
   G711_Handle_Header *g711_header;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_BadDataPointer;
   if(pBanks->pMem==NULL) return USC_NotInitialized;
   if(pBanks->nbytes<=0) return USC_NotInitialized;
   if(options->modes.vad > 1) return USC_UnsupportedVADType;

   *handle = (USC_Handle*)pBanks->pMem;
   g711_header = (G711_Handle_Header*)*handle;
   g711_header->direction = options->direction;
   g711_header->law = G711_ALAW_CODEC;
   g711_header->vad = options->modes.vad;
   g711_header->pf = options->modes.pf;

   if (options->direction == 0) { /* encode only */
      G711Encoder_Obj *EncObj = (G711Encoder_Obj *)((char*)*handle + sizeof(G711_Handle_Header));
      apiG711Encoder_Init((G711Encoder_Obj*)EncObj,
            (G711Codec_Type)G711_ALAW_CODEC,(G711Encode_Mode)options->modes.vad);
   } else if (options->direction == 1) { /* decode only */
      G711Decoder_Obj *DecObj = (G711Decoder_Obj *)((char*)*handle + sizeof(G711_Handle_Header));
      apiG711Decoder_Init((G711Decoder_Obj*)DecObj,
            (G711Codec_Type)G711_ALAW_CODEC);
      apiG711Decoder_Mode(DecObj, g711_header->pf);
   } else {
      return USC_NoOperation;
   }
   return USC_NoError;
}
static USC_Status InitU(const void *opt, const USC_MemBank *pBanks, USC_Handle *handle)
{
   const USC_Option *options=(const USC_Option *) opt;
   G711_Handle_Header *g711_header;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_BadDataPointer;
   if(pBanks->pMem==NULL) return USC_NotInitialized;
   if(pBanks->nbytes<=0) return USC_NotInitialized;
   if(options->modes.vad > 1) return USC_UnsupportedVADType;

   *handle = (USC_Handle*)pBanks->pMem;
   g711_header = (G711_Handle_Header*)*handle;
   g711_header->direction = options->direction;
   g711_header->law = G711_MULAW_CODEC;
   g711_header->vad = options->modes.vad;
   g711_header->pf = options->modes.pf;

   if (options->direction == 0) { /* encode only */
      G711Encoder_Obj *EncObj = (G711Encoder_Obj *)((char*)*handle + sizeof(G711_Handle_Header));
      apiG711Encoder_Init((G711Encoder_Obj*)EncObj,
            (G711Codec_Type)G711_MULAW_CODEC,(G711Encode_Mode)options->modes.vad);
   }
   else if (options->direction == 1) { /* decode only */
      G711Decoder_Obj *DecObj = (G711Decoder_Obj *)((char*)*handle + sizeof(G711_Handle_Header));
      apiG711Decoder_Init((G711Decoder_Obj*)DecObj,
            (G711Codec_Type)G711_MULAW_CODEC);
      apiG711Decoder_Mode(DecObj, g711_header->pf);
   } else {
      return USC_NoOperation;
   }
   return USC_NoError;
}
static USC_Status ReinitA(const void *mode, USC_Handle handle )
{
   const USC_Modes *modes=(const USC_Modes *)mode;
   G711_Handle_Header *g711_header;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

   g711_header = (G711_Handle_Header*)handle;
   g711_header->law = G711_ALAW_CODEC;
   g711_header->vad = modes->vad;
   g711_header->pf = modes->pf;

   if (g711_header->direction == 0) { /* encode only */
      G711Encoder_Obj *EncObj = (G711Encoder_Obj *)((char*)handle + sizeof(G711_Handle_Header));
      apiG711Encoder_Init((G711Encoder_Obj*)EncObj, (G711Codec_Type)g711_header->law, (G711Encode_Mode)modes->vad);
   } else if (g711_header->direction == 1) { /* decode only */
      G711Decoder_Obj *DecObj = (G711Decoder_Obj *)((char*)handle + sizeof(G711_Handle_Header));
      apiG711Decoder_Init((G711Decoder_Obj*)DecObj, (G711Codec_Type)g711_header->law);
      apiG711Decoder_Mode(DecObj, modes->pf);
   } else {
      return USC_NoOperation;
   }
   return USC_NoError;
}
static USC_Status ReinitU(const void *mode, USC_Handle handle )
{
   const USC_Modes *modes=(const USC_Modes *)mode;
   G711_Handle_Header *g711_header;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

   g711_header = (G711_Handle_Header*)handle;
   g711_header->law = G711_MULAW_CODEC;
   g711_header->vad = modes->vad;
   g711_header->pf = modes->pf;

   if (g711_header->direction == 0) { /* encode only */
      G711Encoder_Obj *EncObj = (G711Encoder_Obj *)((char*)handle + sizeof(G711_Handle_Header));
      apiG711Encoder_Init((G711Encoder_Obj*)EncObj, (G711Codec_Type)g711_header->law, (G711Encode_Mode)modes->vad);
   } else if (g711_header->direction == 1) { /* decode only */
      G711Decoder_Obj *DecObj = (G711Decoder_Obj *)((char*)handle + sizeof(G711_Handle_Header));
      apiG711Decoder_Init((G711Decoder_Obj*)DecObj, (G711Codec_Type)g711_header->law);
      apiG711Decoder_Mode(DecObj, modes->pf);
   } else {
      return USC_NoOperation;
   }
   return USC_NoError;
}
static USC_Status ControlA(const void *mode, USC_Handle handle )
{
   const USC_Modes *modes=(const USC_Modes *)mode;
   G711_Handle_Header *g711_header;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

   g711_header = (G711_Handle_Header*)handle;

   g711_header->law = G711_ALAW_CODEC;
   g711_header->vad = modes->vad;
   g711_header->pf = modes->pf;

   if(g711_header->direction == 0) {
      G711Encoder_Obj *EncObj = (G711Encoder_Obj *)((char*)handle + sizeof(G711_Handle_Header));
      apiG711Encoder_Mode(EncObj, (G711Encode_Mode)g711_header->vad);
   } else if(g711_header->direction == 1) {
      G711Decoder_Obj *DecObj = (G711Decoder_Obj *)((char*)handle + sizeof(G711_Handle_Header));
      apiG711Decoder_Mode(DecObj, g711_header->pf);
   } else
      return USC_NoOperation;

   return USC_NoError;
}
static USC_Status ControlU(const void *mode, USC_Handle handle )
{
   const USC_Modes *modes=(const USC_Modes *)mode;
   G711_Handle_Header *g711_header;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

   g711_header = (G711_Handle_Header*)handle;

   g711_header->law = G711_MULAW_CODEC;
   g711_header->vad = modes->vad;
   g711_header->pf = modes->pf;

   if(g711_header->direction == 0) {
      G711Encoder_Obj *EncObj = (G711Encoder_Obj *)((char*)handle + sizeof(G711_Handle_Header));
      apiG711Encoder_Mode(EncObj, (G711Encode_Mode)g711_header->vad);
   } else if(g711_header->direction == 1) {
      G711Decoder_Obj *DecObj = (G711Decoder_Obj *)((char*)handle + sizeof(G711_Handle_Header));
      apiG711Decoder_Mode(DecObj, g711_header->pf);
   } else
      return USC_NoOperation;

   return USC_NoError;
}

static int GetBitstreamSize_G711(int frametype)
{
   if(frametype==G711_Voice_Frame) return BITSTREAM_SIZE;
   else return 2;
}
static USC_Status Encode(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out)
{
   G711_Handle_Header *g711_header;
   G711Encoder_Obj *EncObj;

   if(in==NULL) return USC_BadDataPointer;
   if(out==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

   if(in->pcmType.bitPerSample!=G711_BITS_PER_SAMPLE) return USC_UnsupportedPCMType;
   if(in->pcmType.sample_frequency!=G711_SAMPLE_FREQUENCE) return USC_UnsupportedPCMType;
   if(in->bitrate != pTblRates_G711[0].bitrate) return USC_UnsupportedBitRate;

   if(in->nbytes < G711_SPEECH_FRAME*sizeof(short)) {
      in->nbytes = 0;
      return USC_NoOperation;
   }

   g711_header = (G711_Handle_Header*)handle;
   EncObj = (G711Encoder_Obj *)((char*)handle + sizeof(G711_Handle_Header));

   if(apiG711Encode(EncObj,(const short*)in->pBuffer,(unsigned char *)out->pBuffer,
       &out->frametype) != APIG711_StsNoErr){
      return USC_NoOperation;
   }
   out->bitrate = in->bitrate;
   out->nbytes = GetBitstreamSize_G711(out->frametype);
   in->nbytes = G711_SPEECH_FRAME*sizeof(short);
   return USC_NoError;
}
static __ALIGN32 CONST unsigned char LostFrame_G711[BITSTREAM_SIZE] = {
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0,
   0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0,
   0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0,
   0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0,
   0
};

static USC_Status Decode(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out)
{
   G711_Handle_Header *g711_header;
   G711Decoder_Obj *DecObj;

   if(out==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

   g711_header = (G711_Handle_Header*)handle;
   DecObj = (G711Decoder_Obj *)((char*)handle + sizeof(G711_Handle_Header));

   if(in==NULL) {
      if(apiG711Decode(DecObj,LostFrame_G711,G711_Bad_Frame,
                       (short*)out->pBuffer) != APIG711_StsNoErr){
         return USC_NoOperation;
      }
      out->bitrate = pTblRates_G711[0].bitrate;
   } else {
      if(apiG711Decode(DecObj,(const unsigned char*)in->pBuffer,in->frametype,
                              (short*)out->pBuffer) != APIG711_StsNoErr){
         return USC_NoOperation;
      }
      out->bitrate = in->bitrate;
      in->nbytes = GetBitstreamSize_G711(in->frametype);
   }
   out->pcmType.bitPerSample = G711_BITS_PER_SAMPLE;
   out->pcmType.sample_frequency = G711_SAMPLE_FREQUENCE;
   out->nbytes = G711_SPEECH_FRAME*sizeof(short);
   return USC_NoError;
}

static USC_Status CalsOutStreamSize(const void *opt, int nbytesSrc, int *nbytesDst)
{
   const USC_Option *options=(const USC_Option *) opt;
   int nBlocks, nSamples;

   if(options->direction==0) { /*Encode: src - PCMStream, dst - bitstream*/
      if(options->modes.vad>1) return USC_UnsupportedVADType;

      nSamples = nbytesSrc / (G711_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / G711_SPEECH_FRAME;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % G711_SPEECH_FRAME) {
         nBlocks++;
      }

      *nbytesDst = nBlocks * GetBitstreamSize_G711(G711_Voice_Frame);
   } else if(options->direction==1) {/*Decode: src - bitstream, dst - PCMStream*/
      if(options->modes.vad==0) { /*VAD off*/
         nBlocks = nbytesSrc / GetBitstreamSize_G711(G711_Voice_Frame);
      } else if(options->modes.vad==1) { /*VAD on*/
         nBlocks = nbytesSrc / GetBitstreamSize_G711(G711_SID_Frame);
      } else return USC_UnsupportedVADType;

      if (0 == nBlocks) return USC_NoOperation;

      nSamples = nBlocks * G711_SPEECH_FRAME;
      *nbytesDst = nSamples * (G711_BITS_PER_SAMPLE >> 3);
   } else if(options->direction==2) {/* Both: src - PCMStream, dst - PCMStream*/
      nSamples = nbytesSrc / (G711_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / G711_SPEECH_FRAME;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % G711_SPEECH_FRAME) {
         nBlocks++;
      }
      *nbytesDst = nBlocks * G711_SPEECH_FRAME * (G711_BITS_PER_SAMPLE >> 3);
   } else return USC_NoOperation;

   return USC_NoError;
}
static USC_Status GetOutStreamSize(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst)
{
   if(options==NULL) return USC_BadDataPointer;
   if(nbytesDst==NULL) return USC_BadDataPointer;
   if(nbytesSrc <= 0) return USC_NoOperation;

    if(bitrate != pTblRates_G711[0].bitrate) return USC_UnsupportedBitRate;

   return CalsOutStreamSize(options, nbytesSrc, nbytesDst);
}

USC_Status SetFrameSize(const USC_Option *options, USC_Handle handle, int frameSize)
{
   if(options==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;
   if(frameSize==0) return USC_NoOperation;

   return USC_NoError;
}

