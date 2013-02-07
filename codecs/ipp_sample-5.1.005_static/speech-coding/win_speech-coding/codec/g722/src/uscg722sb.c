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
// Purpose: G.722 speech codec: USC functions.
//
*/

#include "owng722sb.h"
#include "g722sb.h"
#include "g722sbapi.h"
#include <string.h>
#include <usc.h>

#define  G722SB_NUM_RATES        3
#define  G722SB_BITS_PER_SAMPLE 16

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

#define BITSTREAM_SIZE        160
#define G722SB_SPEECH_FRAME   320

typedef struct {
   int direction;
   int bitrate;
   int hpf;
   int pf;
} G722SB_Handle_Header;

/* global usc vector table */
USCFUN USC_Fxns USC_G722SB_Fxns=
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


static __ALIGN32 CONST short LostFrame[G722SB_SPEECH_FRAME]=
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

static __ALIGN32 CONST USC_Rates pTblRates_G722SB[G722SB_NUM_RATES]={
    {64000},
    {56000},
   {48000}
};

static int CheckRate_G722SB(int rate_bps)
{
   int rate;

    switch(rate_bps) {
      case 64000:  rate = 1; break;
      case 56000:  rate = 2; break;
     case 48000:  rate = 3; break;
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
    G722SB_Handle_Header *g722sb_header;

    pInfo->name = "IPP_G722";
    pInfo->params.framesize = G722SB_SPEECH_FRAME*sizeof(short);
    if (handle == NULL) {
      pInfo->params.modes.bitrate = 64000;
      pInfo->params.direction = 0;
       pInfo->params.modes.hpf = 1;
      pInfo->params.modes.pf = 1;

    } else {
       g722sb_header = (G722SB_Handle_Header*)handle;
       pInfo->params.modes.bitrate = g722sb_header->bitrate;
      pInfo->params.direction = g722sb_header->direction;
       pInfo->params.modes.hpf = g722sb_header->hpf;
      pInfo->params.modes.pf = g722sb_header->pf;
    }

    pInfo->params.modes.vad = 0;
     pInfo->params.modes.truncate = 0;
    //pInfo->maxbitsize = BITSTREAM_SIZE;
    pInfo->maxbitsize = G722SB_SPEECH_FRAME*sizeof(short);
     pInfo->pcmType.sample_frequency = 16000;
    pInfo->pcmType.bitPerSample = 16;
    pInfo->params.law = 0;
    pInfo->nRates = G722SB_NUM_RATES;
     pInfo->pRateTbl = (const USC_Rates *)&pTblRates_G722SB;

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
        apiG722SBEncoder_Alloc(&nbytes);
    }
    else if (options->direction == 1) /* decode only */
    {
        apiG722SBDecoder_Alloc(&nbytes);
    } else {
        return USC_NoOperation;
    }
    pBanks->nbytes = nbytes+sizeof(G722SB_Handle_Header); /* include direction in handle */
    return USC_NoError;
}

static USC_Status Init(const USC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle)
{
    int bitrate_idx;
    G722SB_Handle_Header *g722sb_header;
    *handle = (USC_Handle*)pBanks->pMem;
    g722sb_header = (G722SB_Handle_Header*)*handle;


    bitrate_idx = CheckRate_G722SB(options->modes.bitrate);
     if(bitrate_idx < 0) return USC_UnsupportedBitRate;
    g722sb_header->hpf = options->modes.hpf;
    g722sb_header->pf = options->modes.pf;
     g722sb_header->bitrate = options->modes.bitrate;
    g722sb_header->direction = options->direction;

    if (options->direction == 0) /* encode only */
    {
      G722SB_Encoder_Obj *EncObj = (G722SB_Encoder_Obj *)((char*)*handle + sizeof(G722SB_Handle_Header));
      apiG722SBEncoder_Init(EncObj, g722sb_header->hpf);
    }
    else if (options->direction == 1) /* decode only */
    {
      G722SB_Decoder_Obj *DecObj = (G722SB_Decoder_Obj *)((char*)*handle + sizeof(G722SB_Handle_Header));
      apiG722SBDecoder_Init(DecObj, g722sb_header->pf);

    } else {
      return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Reinit(const USC_Modes *modes, USC_Handle handle )
{
    int bitrate_idx;
    G722SB_Handle_Header *g722sb_header;
    g722sb_header = (G722SB_Handle_Header*)handle;

    bitrate_idx = CheckRate_G722SB(modes->bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;
    g722sb_header->hpf = modes->hpf;
    g722sb_header->pf = modes->pf;
     g722sb_header->bitrate = modes->bitrate;

    if (g722sb_header->direction == 0) /* encode only */
    {
      G722SB_Encoder_Obj *EncObj = (G722SB_Encoder_Obj *)((char*)handle + sizeof(G722SB_Handle_Header));
      apiG722SBEncoder_Init(EncObj, g722sb_header->hpf);
    }
    else if (g722sb_header->direction == 1) /* decode only */
    {
      G722SB_Decoder_Obj *DecObj = (G722SB_Decoder_Obj *)((char*)handle + sizeof(G722SB_Handle_Header));
      apiG722SBDecoder_Init(DecObj, g722sb_header->pf);
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

static int getBitstreamSize(const USC_PCMStream *in, int hpf)
{
   int size, nSamples;

   nSamples = in->nbytes/sizeof(short);
   if(hpf) {
     size = nSamples/2;
   } else {
     size = nSamples;
   }

   return size;

}

static int getPCMStreamSize(const USC_Bitstream *in, int pf)
{
   int size;

   if(pf) {
     size = in->nbytes*sizeof(short)*2;
   } else {
     size = in->nbytes*sizeof(short);
   }

   return size;

}

static USC_Status Encode(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out)
{
    int bitrate_idx, lenSamples, i;
    char pOutBuff[2];
    short *in_buff, *out_buff;
    G722SB_Handle_Header *g722sb_header;
    G722SB_Encoder_Obj *EncObj;
    g722sb_header = (G722SB_Handle_Header*)handle;

    bitrate_idx = CheckRate_G722SB(in->bitrate);
     if(bitrate_idx < 0) return USC_UnsupportedBitRate;
    if(in->nbytes <= 0) return USC_NoOperation;

    g722sb_header->bitrate = in->bitrate;
    EncObj = (G722SB_Encoder_Obj *)((char*)handle + sizeof(G722SB_Handle_Header));

    if(g722sb_header->hpf) {
      if(in->nbytes < G722SB_SPEECH_FRAME*sizeof(short)) lenSamples = in->nbytes/sizeof(short);
      else lenSamples = G722SB_SPEECH_FRAME;

      if(apiG722SBEncode(EncObj, lenSamples, (short*)in->pBuffer, out->pBuffer) != API_G722SB_StsNoErr){
        return USC_NoOperation;
      }
      in->nbytes = lenSamples*sizeof(short);
      out->nbytes = getBitstreamSize(in, g722sb_header->hpf);
    } else {

      lenSamples = in->nbytes/sizeof(short);

      in_buff = (short*)in->pBuffer;
      out_buff = (short*)out->pBuffer;
      for(i=0; i<lenSamples; i++) {
        if((in_buff[i] & 1) == 1) {
          apiG722SBEncoder_Init(EncObj, g722sb_header->hpf);
          out_buff[i] = 1;
        } else {
          if(apiG722SBEncode(EncObj, 1, (short*)&in_buff[i], (char *)&pOutBuff[0]) != API_G722SB_StsNoErr){
            return USC_NoOperation;
          }
          out_buff[i] = (short)((short)(pOutBuff[0] << 8) & 0xFF00);
        }
      }
      out->nbytes = in->nbytes;
    }

     out->frametype = 0;
     out->bitrate = in->bitrate;

    return USC_NoError;
}

static USC_Status Decode(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out)
{
    G722SB_Handle_Header *g722sb_header;
    G722SB_Decoder_Obj *DecObj;
     int bitrate_idx, length, i, j;
    char bit[2];
    short *in_buff, *out_buff;
    g722sb_header = (G722SB_Handle_Header*)handle;
    DecObj = (G722SB_Decoder_Obj *)((char*)handle + sizeof(G722SB_Handle_Header));

    if(in == NULL) {
       /* Lost frame */
      bitrate_idx = CheckRate_G722SB(g722sb_header->bitrate);
       if(bitrate_idx < 0) return USC_UnsupportedBitRate;

      if(g722sb_header->pf) {
        if(apiG722SBDecode(DecObj, (int)BITSTREAM_SIZE, (short)bitrate_idx, (char *)LostFrame, (short*)out->pBuffer) != API_G722SB_StsNoErr){
          return USC_NoOperation;
        }
        out->nbytes = G722SB_SPEECH_FRAME*sizeof(short);
      } else {
        length = BITSTREAM_SIZE;
        in_buff = (short*)LostFrame;
        out_buff = (short*)out->pBuffer;
        for(i=0,j=0; i<length; i++) {
          bit[0] = (char)((in_buff[i] >> 8) & 0x00FF);
          if(apiG722SBDecode(DecObj, 1, (short)bitrate_idx, (char *)&bit[0], (short*)&out_buff[j]) != API_G722SB_StsNoErr){
            return USC_NoOperation;
          }
          j+=2;
        }
        out->nbytes = j*sizeof(short);
      }

    } else {
      bitrate_idx = CheckRate_G722SB(in->bitrate);
       if(bitrate_idx < 0) return USC_UnsupportedBitRate;
      if(in->nbytes <= 0) return USC_NoOperation;
       g722sb_header->bitrate = in->bitrate;

      if(g722sb_header->pf) {
        if(in->nbytes < BITSTREAM_SIZE) length = in->nbytes;
        else length = BITSTREAM_SIZE;

        if(apiG722SBDecode(DecObj, length, (short)bitrate_idx, (char *)in->pBuffer, (short*)out->pBuffer) != API_G722SB_StsNoErr){
          return USC_NoOperation;
        }
        in->nbytes = length;
        out->nbytes = getPCMStreamSize(in, g722sb_header->pf);
      } else {
        length = in->nbytes/sizeof(short);
        in_buff = (short*)in->pBuffer;
        out_buff = (short*)out->pBuffer;
        for(i=0,j=0; i<length; i++) {
          if((in_buff[i] & 1) == 1) {
            apiG722SBDecoder_Init(DecObj, g722sb_header->pf);
            out_buff[j++] = 1;
            out_buff[j++] = 1;
          } else {
            bit[0] = (char)((in_buff[i] >> 8) & 0x00FF);
            if(apiG722SBDecode(DecObj, 1, (short)bitrate_idx, (char *)&bit[0], (short*)&out_buff[j]) != API_G722SB_StsNoErr){
              return USC_NoOperation;
            }
            j+=2;
          }
        }
        out->nbytes = j*sizeof(short);
      }
      //in->nbytes = BITSTREAM_SIZE;
    }

    return USC_NoError;
}

static USC_Status GetOutStreamSize(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst)
{

   int bitrate_idx;
   int nBlocks, nSamples;

   if(options==NULL) return USC_BadDataPointer;
   if(nbytesDst==NULL) return USC_BadDataPointer;
   if(nbytesSrc <= 0) return USC_NoOperation;

   bitrate_idx = CheckRate_G722SB(bitrate);
   if(bitrate_idx < 0) return USC_UnsupportedBitRate;

   if(options->modes.vad>0) return USC_UnsupportedVADType;

   if(options->direction==0) { /*Encode: src - PCMStream, dst - bitstream*/

      nSamples = nbytesSrc / (G722SB_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / G722SB_SPEECH_FRAME;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % G722SB_SPEECH_FRAME) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }
      *nbytesDst = nBlocks * BITSTREAM_SIZE;

   } else if(options->direction==1) {/*Decode: src - bitstream, dst - PCMStream*/
      nBlocks = nbytesSrc / BITSTREAM_SIZE;

      if (0 == nBlocks) return USC_NoOperation;

      nSamples = nBlocks * G722SB_SPEECH_FRAME;
      *nbytesDst = nSamples * (G722SB_BITS_PER_SAMPLE >> 3);

   } else if(options->direction==2) {/* Both: src - PCMStream, dst - PCMStream*/
      nSamples = nbytesSrc / (G722SB_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / G722SB_SPEECH_FRAME;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % G722SB_SPEECH_FRAME) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }
      *nbytesDst = nBlocks * G722SB_SPEECH_FRAME * (G722SB_BITS_PER_SAMPLE >> 3);
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
