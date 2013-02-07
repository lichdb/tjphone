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
// Purpose: G.728 speech codec: USC functions.
//
*/
#include "owng728.h"
#include <string.h>
#include <usc.h>
#include "g728api.h"
#include <ipp_w7.h>
#include <ipps.h>

#define G728_VEC_PER_FRAME  4
#define G728_VECTOR         5
#define G728_FRAME          (G728_VEC_PER_FRAME * G728_VECTOR)
#define BITSTREAM_SIZE_G728 (G728_VECTOR)
#define G728_NUM_RATES      3
#define G728_NUM_FRAMES_PER_PACKET 4
#define G728_BITS_PER_SAMPLE 16
#define G728_SAMPLE_FREQ     8000

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
    int pst;
    int G728Type;
} G728_Handle_Header;

/* global usc vector table */
USCFUN USC_Fxns USC_G728_Fxns=
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

static __ALIGN32 CONST USC_Rates pTblRates_G728[G728_NUM_RATES]={
    {16000},
    {12800},
    {9600}
};

static int CheckRate_G728(int rate_bps)
{
   int rate;

    switch(rate_bps) {
      case 16000: rate = 0; break;
      case 12800: rate = 1; break;
      case 9600:  rate = 2; break;
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
   G728_Handle_Header *g728_header;

   if(pInfo==NULL) return USC_BadDataPointer;

   pInfo->name = "IPP_G728";
   pInfo->params.framesize = G728_NUM_FRAMES_PER_PACKET*G728_FRAME*sizeof(short);
   if (handle == NULL) {
      pInfo->params.modes.bitrate = 16000;
      pInfo->params.direction = 0;
      pInfo->params.modes.pf = 1;
   } else {
      g728_header = (G728_Handle_Header*)handle;
      pInfo->params.modes.bitrate = g728_header->bitrate;
      pInfo->params.direction = g728_header->direction;
      pInfo->params.modes.pf = g728_header->pst;
   }
   pInfo->params.modes.truncate = 0;
   pInfo->params.modes.vad = 0;
   pInfo->maxbitsize = G728_NUM_FRAMES_PER_PACKET*BITSTREAM_SIZE_G728*sizeof(short);
   pInfo->pcmType.sample_frequency = G728_SAMPLE_FREQ;
   pInfo->pcmType.bitPerSample = G728_BITS_PER_SAMPLE;
   pInfo->params.modes.hpf = 0;
   pInfo->params.law = 0;
   pInfo->nRates = G728_NUM_RATES;
   pInfo->pRateTbl = (const USC_Rates *)&pTblRates_G728;

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
      apiG728Encoder_Alloc(&nbytes);
   } else if (options->direction == 1) /* decode only */
   {
      apiG728Decoder_Alloc(&nbytes);
   } else {
      return USC_NoOperation;
   }
   pBanks->nbytes = nbytes + sizeof(G728_Handle_Header); /* room for USC header */
   return USC_NoError;

}

G728_Rate usc2apiRate[3] = {
   G728_Rate_16000,
   G728_Rate_12800,
   G728_Rate_9600
};

static USC_Status Init(const USC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle)
{
   G728_Handle_Header *g728_header;
   int bitrate_idx;

   if(handle==NULL) return USC_BadDataPointer;
   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
   if(pBanks->pMem==NULL) return USC_BadDataPointer;

   *handle = (USC_Handle*)pBanks->pMem;
   g728_header = (G728_Handle_Header*)*handle;

   bitrate_idx = CheckRate_G728(options->modes.bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;
   g728_header->direction = options->direction;
   g728_header->bitrate = options->modes.bitrate;
   g728_header->pst=options->modes.pf;

   if (options->direction == 0) /* encode only */
   {
      G728Encoder_Obj *EncObj = (G728Encoder_Obj *)((char*)*handle + sizeof(G728_Handle_Header));
      apiG728Encoder_Init(EncObj, usc2apiRate[bitrate_idx]);
      g728_header->G728Type=G728_Pure;
   } else if (options->direction == 1) /* decode only */
   {
      G728Decoder_Obj *DecObj = (G728Decoder_Obj *)((char*)*handle + sizeof(G728_Handle_Header));
      g728_header->G728Type=G728_Annex_I;
      apiG728Decoder_Init(DecObj, (G728_Type)g728_header->G728Type, usc2apiRate[bitrate_idx], g728_header->pst);
   } else {
      return USC_NoOperation;
   }
   return USC_NoError;
}

static USC_Status Reinit(const USC_Modes *modes, USC_Handle handle )
{
   G728_Handle_Header *g728_header;
   int bitrate_idx;

   if(handle==NULL) return USC_BadDataPointer;
   if(modes==NULL) return USC_BadDataPointer;

   g728_header = (G728_Handle_Header*)handle;

   bitrate_idx = CheckRate_G728(modes->bitrate);
   if(bitrate_idx < 0) return USC_UnsupportedBitRate;
   g728_header->bitrate = modes->bitrate;

   if (g728_header->direction == 0) /* encode only */
   {
      G728Encoder_Obj *EncObj = (G728Encoder_Obj *)((char*)handle + sizeof(G728_Handle_Header));
      g728_header->G728Type = G728_Pure;
      apiG728Encoder_Init(EncObj, usc2apiRate[bitrate_idx]);
   }
   else if (g728_header->direction == 1) /* decode only */
   {
      G728Decoder_Obj *DecObj = (G728Decoder_Obj *)((char*)handle + sizeof(G728_Handle_Header));
      g728_header->G728Type = G728_Annex_I;
      g728_header->pst = modes->pf;
      apiG728Decoder_Init(DecObj, (G728_Type)g728_header->G728Type, usc2apiRate[bitrate_idx], g728_header->pst);
   } else {
      return USC_NoOperation;
   }
   return USC_NoError;
}

static USC_Status Control(const USC_Modes *modes, USC_Handle handle )
{
   if(handle==NULL) return USC_BadDataPointer;
   if(modes==NULL) return USC_BadDataPointer;

   return Reinit(modes, handle);
}

int outFrameSize(int bitRate)
{
   int size=0;
   if(bitRate==0) {
      size=5*sizeof(char);
   } else if(bitRate==1) {
      size=4*sizeof(char);
   } else {
      size=3*sizeof(char);
   }
   return size;
}

static USC_Status Encode(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out)
{
   G728_Handle_Header *g728_header;
   G728Encoder_Obj *EncObj;
   int i, bitrate_idx, inputLen;
   IPP_ALIGNED_ARRAY(16, short, work_buf, G728_NUM_FRAMES_PER_PACKET*G728_FRAME);

   if(handle==NULL) return USC_BadDataPointer;
   if(out==NULL) return USC_BadDataPointer;
   if(in==NULL) return USC_BadDataPointer;
   if(in->pcmType.sample_frequency != G728_SAMPLE_FREQ) return USC_UnsupportedPCMType;
   if(in->pcmType.bitPerSample != G728_BITS_PER_SAMPLE) return USC_UnsupportedPCMType;

   g728_header = (G728_Handle_Header*)handle;
   EncObj = (G728Encoder_Obj *)((char*)handle + sizeof(G728_Handle_Header));

   inputLen = in->nbytes;
   if(in->nbytes >= G728_NUM_FRAMES_PER_PACKET*G728_FRAME*sizeof(short))
      inputLen = G728_NUM_FRAMES_PER_PACKET*G728_FRAME*sizeof(short);
   else
      inputLen = (in->nbytes/(G728_VECTOR*sizeof(short)))*(G728_VECTOR*sizeof(short));
   if(inputLen <= 0) return USC_NoOperation;

   in->nbytes = inputLen;

   if(g728_header->bitrate != in->bitrate) {
      bitrate_idx = CheckRate_G728(in->bitrate);
       if(bitrate_idx < 0) return USC_UnsupportedBitRate;
      apiG728Encoder_Init(EncObj, usc2apiRate[bitrate_idx]);
      g728_header->bitrate = in->bitrate;
   } else {
      bitrate_idx = CheckRate_G728(g728_header->bitrate);
       if(bitrate_idx < 0) return USC_UnsupportedBitRate;
   }

   /* scale down to 14-bit uniform PCM*/
   ippsRShiftC_16s((Ipp16s*)in->pBuffer, 1, (Ipp16s*)work_buf, in->nbytes/sizeof(short));

   for(i=0;i<G728_NUM_FRAMES_PER_PACKET;i++) {
      if(apiG728Encode(EncObj, work_buf+i*G728_FRAME,(unsigned char*)(out->pBuffer+i*outFrameSize(bitrate_idx))) != APIG728_StsNoErr){
         return USC_NoOperation;
      }
   }

   out->nbytes = G728_NUM_FRAMES_PER_PACKET*outFrameSize(bitrate_idx);
   out->bitrate = in->bitrate;
   out->frametype = 0;

   return USC_NoError;
}

static __ALIGN32 CONST unsigned char LostFrame[6] = {
   1, 0, 0, 0, 0, 0
};

static USC_Status Decode(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out)
{
   int i, j, sbrfLen, bitrate_idx;
   G728_Handle_Header *g728_header;
   G728Decoder_Obj *DecObj;
   char packet[6];

   if(handle==NULL) return USC_BadDataPointer;
   if(out==NULL) return USC_BadDataPointer;

   g728_header = (G728_Handle_Header*)handle;
   DecObj = (G728Decoder_Obj *)((char*)handle + sizeof(G728_Handle_Header));

   if(in==NULL) {
      for(i=0;i<G728_NUM_FRAMES_PER_PACKET;i++) {
         if(apiG728Decode(DecObj,LostFrame, (short*)out->pBuffer+i*20)!=APIG728_StsNoErr){
            return USC_NoOperation;
         }
      }
      out->bitrate = g728_header->bitrate;
   } else {
      bitrate_idx = CheckRate_G728(in->bitrate);
      if(bitrate_idx < 0) return USC_UnsupportedBitRate;
      if(g728_header->bitrate != in->bitrate) {
        apiG728Decoder_Init(DecObj, (G728_Type)g728_header->G728Type, usc2apiRate[bitrate_idx], g728_header->pst);
        g728_header->bitrate = in->bitrate;
      }

      sbrfLen = outFrameSize(bitrate_idx);
      if(in->nbytes < sbrfLen*G728_NUM_FRAMES_PER_PACKET) return USC_NoOperation;

      for(i=0;i<G728_NUM_FRAMES_PER_PACKET;i++) {
         packet[0] = (char)in->frametype;
         for(j=0;j<sbrfLen;j++)
            packet[j+1] = in->pBuffer[i*sbrfLen + j];
         if(apiG728Decode(DecObj,(unsigned char*)packet, (short*)out->pBuffer+i*20)!=APIG728_StsNoErr){
            return USC_NoOperation;
         }
      }
      in->nbytes = sbrfLen*G728_NUM_FRAMES_PER_PACKET;
      out->bitrate = in->bitrate;
   }

   out->nbytes = G728_NUM_FRAMES_PER_PACKET*G728_FRAME*sizeof(short);
   out->pcmType.sample_frequency = G728_SAMPLE_FREQ;
   out->pcmType.bitPerSample = G728_BITS_PER_SAMPLE;

   return USC_NoError;
}

static USC_Status CalsOutStreamSize(const USC_Option *options, int bitrate_idx, int nbytesSrc, int *nbytesDst)
{
   int nBlocks, nSamples;

   if(options->direction==0) { /*Encode: src - PCMStream, dst - bitstream*/
      if(options->modes.vad>1) return USC_UnsupportedVADType;

      nSamples = nbytesSrc / (G728_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / (G728_NUM_FRAMES_PER_PACKET*G728_FRAME);

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % (G728_NUM_FRAMES_PER_PACKET*G728_FRAME)) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }

      *nbytesDst = nBlocks * outFrameSize(bitrate_idx)*G728_NUM_FRAMES_PER_PACKET;
   } else if(options->direction==1) {/*Decode: src - bitstream, dst - PCMStream*/
      if(options->modes.vad!=0) USC_UnsupportedVADType;

      nBlocks = nbytesSrc / (outFrameSize(bitrate_idx)*G728_NUM_FRAMES_PER_PACKET);
      if (0 == nBlocks) return USC_NoOperation;

      nSamples = nBlocks * (G728_NUM_FRAMES_PER_PACKET*G728_FRAME);
      *nbytesDst = nSamples * (G728_BITS_PER_SAMPLE >> 3);
   } else if(options->direction==2) {/* Both: src - PCMStream, dst - PCMStream*/
      nSamples = nbytesSrc / (G728_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / (G728_NUM_FRAMES_PER_PACKET*G728_FRAME);

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % (G728_NUM_FRAMES_PER_PACKET*G728_FRAME)) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }
      *nbytesDst = nBlocks * (G728_NUM_FRAMES_PER_PACKET*G728_FRAME) * (G728_BITS_PER_SAMPLE >> 3);
   } else return USC_NoOperation;

   return USC_NoError;
}
static USC_Status GetOutStreamSize(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst)
{
   int bitrate_idx;

   if(options==NULL) return USC_BadDataPointer;
   if(nbytesDst==NULL) return USC_BadDataPointer;
   if(nbytesSrc <= 0) return USC_NoOperation;

   bitrate_idx = CheckRate_G728(bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;

   return CalsOutStreamSize(options, bitrate_idx, nbytesSrc, nbytesDst);
}

USC_Status SetFrameSize(const USC_Option *options, USC_Handle handle, int frameSize)
{
   if(options==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

   frameSize = frameSize;/*To avoid warning*/

   return USC_NoError;
}

