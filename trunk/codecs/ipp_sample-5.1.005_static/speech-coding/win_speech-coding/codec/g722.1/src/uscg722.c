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
// Purpose: G.722.1 speech codec: USC functions.
//
*/
#include <string.h>
#include <usc.h>
#include "g722.h"
#include "g722api.h"
#include "owng722.h"

#define  G722_1_NUM_RATES  3
#define  G722_BITS_PER_SAMPLE 16

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
    int direction;
    int bitrate;
    int reserved0; // for future extension
    int reserved1; // for future extension
} G722_Handle_Header;

/* global usc vector table */
USCFUN USC_Fxns USC_G722_Fxns=
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

static __ALIGN32 CONST USC_Rates pTblRates_G722_1[G722_1_NUM_RATES]={
    {16000},
    {24000},
    {32000}
};

static USC_Status GetInfoSize(int *pSize)
{
    *pSize = sizeof(USC_CodecInfo);
    return USC_NoError;
}

static USC_Status GetInfo(USC_Handle handle, USC_CodecInfo *pInfo)
{
    G722_Handle_Header *g722_header;

    pInfo->name = "IPP_G722.1";
    pInfo->params.framesize = G722_FRAMESIZE*sizeof(short);
    if (handle == NULL) {
      pInfo->params.modes.bitrate = 24000;
      pInfo->params.direction = 0;
    } else {
      g722_header = (G722_Handle_Header*)handle;
      pInfo->params.modes.bitrate = g722_header->bitrate;
      pInfo->params.direction = g722_header->direction;
    }
    pInfo->params.modes.vad = 0;
    pInfo->params.modes.truncate = 0;
    pInfo->maxbitsize = (G722_MAX_BITS_PER_FRAME/16)*sizeof(short);
    pInfo->pcmType.sample_frequency = 16000;
    pInfo->pcmType.bitPerSample = G722_BITS_PER_SAMPLE;
    pInfo->params.modes.hpf = 0;
    pInfo->params.modes.pf = 0;
    pInfo->params.law = 0;
    pInfo->nRates = G722_1_NUM_RATES;
    pInfo->pRateTbl = (const USC_Rates *)&pTblRates_G722_1;


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
        apiG722Encoder_Alloc(&nbytes);
        pBanks->nbytes = nbytes + sizeof(G722_Handle_Header); /* room for USC header */
    }
    else if (options->direction == 1) /* decode only */
    {
        apiG722Decoder_Alloc(&nbytes);
        pBanks->nbytes = nbytes + sizeof(G722_Handle_Header); /* room for USC header */
    } else {
        return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Init(const USC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle)
{
    G722_Handle_Header *g722_header;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
   if(pBanks->pMem==NULL) return USC_NotInitialized;
   if(pBanks->nbytes<=0) return USC_NotInitialized;
   if(handle==NULL) return USC_InvalidHandler;

    *handle = (USC_Handle*)pBanks->pMem;
    g722_header = (G722_Handle_Header*)*handle;
    g722_header->direction = options->direction;
    g722_header->bitrate = options->modes.bitrate;

    if (options->direction == 0) /* encode only */
    {
        G722Encoder_Obj *EncObj = (G722Encoder_Obj *)((char*)*handle + sizeof(G722_Handle_Header));
        apiG722Encoder_Init(EncObj);
    }
    else if (options->direction == 1) /* decode only */
    {
        G722Decoder_Obj *DecObj = (G722Decoder_Obj *)((char*)*handle + sizeof(G722_Handle_Header));
        apiG722Decoder_Init(DecObj);
    } else {
        return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Reinit(const USC_Modes *modes, USC_Handle handle )
{
    G722_Handle_Header *g722_header;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

    g722_header = (G722_Handle_Header*)handle;
    g722_header->bitrate = modes->bitrate;

    if (g722_header->direction == 0) /* encode only */
    {
        G722Encoder_Obj *EncObj = (G722Encoder_Obj *)((char*)handle + sizeof(G722_Handle_Header));
        apiG722Encoder_Init(EncObj);
    }
    else if (g722_header->direction == 1) /* decode only */
    {
        G722Decoder_Obj *DecObj = (G722Decoder_Obj *)((char*)handle + sizeof(G722_Handle_Header));
        apiG722Decoder_Init(DecObj);
    } else {
        return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Control(const USC_Modes *modes, USC_Handle handle )
{
   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;
   return USC_NoError;
}

static USC_Status Encode(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out)
{
    G722_Handle_Header *g722_header;
    G722Encoder_Obj *EncObj;

    if(handle==NULL) return USC_InvalidHandler;
    if(in==NULL) return USC_BadDataPointer;
    if(out==NULL) return USC_BadDataPointer;
    if(in->nbytes<G722_FRAMESIZE*sizeof(short)) return USC_NoOperation;

    g722_header = (G722_Handle_Header*)handle;
    g722_header->bitrate = in->bitrate;
    EncObj = (G722Encoder_Obj *)((char*)handle + sizeof(G722_Handle_Header));
    if(apiG722Encode(EncObj, in->bitrate/50, (short*)in->pBuffer,(short*)out->pBuffer) != APIG722_StsNoErr){
       return USC_NoOperation;
    }

    out->frametype = 0;
    out->nbytes = (in->bitrate/50/8);
    out->bitrate = in->bitrate;

    in->nbytes = G722_FRAMESIZE*sizeof(short);

    return USC_NoError;
}

static __ALIGN32 CONST short LostFrame[G722_MAX_BITS_PER_FRAME/16]=
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static USC_Status Decode(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out)
{
   G722_Handle_Header *g722_header;
   G722Decoder_Obj *DecObj;

   if(handle==NULL) return USC_InvalidHandler;
   if(out==NULL) return USC_BadDataPointer;

   g722_header = (G722_Handle_Header*)handle;
   DecObj = (G722Decoder_Obj *)((char*)handle + sizeof(G722_Handle_Header));
   if(in==NULL) {
      if(apiG722Decode(DecObj,1, g722_header->bitrate/50,(short*)LostFrame, (short*)out->pBuffer)!=APIG722_StsNoErr){
         return USC_NoOperation;
      }
      out->bitrate = g722_header->bitrate;
   } else {
      if(in->pBuffer==NULL) return USC_BadDataPointer;
      if(in->nbytes<=0) return USC_NoOperation;

      g722_header->bitrate = in->bitrate;
      if(apiG722Decode(DecObj, (short)(in->frametype), in->bitrate/50,(short*)in->pBuffer,
         (short*)out->pBuffer)!=APIG722_StsNoErr){
         return USC_NoOperation;
      }
      out->bitrate = in->bitrate;
      in->nbytes = (in->bitrate/50/8);
   }
   out->nbytes = G722_FRAMESIZE*sizeof(short);
   ippsAndC_16u_I(0xfffc, (Ipp16u*)out->pBuffer, G722_FRAMESIZE);
   return USC_NoError;
}

static int CheckRate_G7221(int rate_bps)
{
    int rate;

    switch(rate_bps) {
      case 24000:  rate = 0; break;
      case 32000:  rate = 1; break;
      default: rate = -1; break;
    }
    return rate;
}

static USC_Status CalsOutStreamSize(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst)
{
   int nBlocks, nSamples;

   if(options->modes.vad>0) return USC_UnsupportedVADType;

   if(options->direction==0) { /*Encode: src - PCMStream, dst - bitstream*/
      nSamples = nbytesSrc / (G722_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / G722_FRAMESIZE;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % G722_FRAMESIZE) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }

      *nbytesDst = nBlocks * (bitrate/50/8);
   } else if(options->direction==1) {/*Decode: src - bitstream, dst - PCMStream*/
      nBlocks = nbytesSrc / (bitrate/50/8);

      if (0 == nBlocks) return USC_NoOperation;

      nSamples = nBlocks * G722_FRAMESIZE;
      *nbytesDst = nSamples * (G722_BITS_PER_SAMPLE >> 3);
   } else if(options->direction==2) {/* Both: src - PCMStream, dst - PCMStream*/
      nSamples = nbytesSrc / (G722_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / G722_FRAMESIZE;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % G722_FRAMESIZE) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }
      *nbytesDst = nBlocks * G722_FRAMESIZE * (G722_BITS_PER_SAMPLE >> 3);
   } else return USC_NoOperation;

   return USC_NoError;
}
static USC_Status GetOutStreamSize(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst)
{
   int bitrate_idx;

   if(options==NULL) return USC_BadDataPointer;
   if(nbytesDst==NULL) return USC_BadDataPointer;
   if(nbytesSrc <= 0) return USC_NoOperation;

   bitrate_idx = CheckRate_G7221(bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;

   return CalsOutStreamSize(options, bitrate, nbytesSrc, nbytesDst);
}

USC_Status SetFrameSize(const USC_Option *options, USC_Handle handle, int frameSize)
{
   if(options==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;
   frameSize = frameSize;

   return USC_NoError;
}
