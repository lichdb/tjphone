/*////////////////////////////////////////////////////////////////////////
//
// INTEL CORPORATION PROPRIETARY INFORMATION
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Intel Corporation and may not be copied
// or disclosed except in accordance with the terms of that agreement.
// Copyright (c) 2006 Intel Corporation. All Rights Reserved.
//
//   Intel(R)  Integrated Performance Primitives
//
//     USC speech codec sample
//
// By downloading and installing this sample, you hereby agree that the
// accompanying Materials are being provided to you under the terms and
// conditions of the End User License Agreement for the Intel(R) Integrated
// Performance Primitives product previously accepted by you. Please refer
// to the file ipplic.htm located in the root directory of your Intel(R) IPP
// product installation for more information.
//
// Purpose: USC wrapper functions file.
//
////////////////////////////////////////////////////////////////////////*/
#include <stdio.h>
#include <stdlib.h>
#include "usc.h"
#include "util.h"
#include "usccodec.h"
#include <ipp_w7.h>
#include "ipps.h"

#define FRAME_HEADER_SIZE 6

static int checkBitrate(USCParams *uscPrms, int bitrate)
{
   int i;
   for(i=0;i<uscPrms->pInfo->nRates;i++) {
      if(uscPrms->pInfo->pRateTbl[i].bitrate==bitrate) return 1;
   }
   return 0;
}
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCCodecAllocInfo
//  Purpose:     Allocates memory for the USC_CodecInfo structure.
//  Returns:     0 if success, -1 if fails.
//  Parameters:
//    uscPrms       pointer to the input\output USC codec parametres.
*/
int USCCodecAllocInfo(USCParams *uscPrms)
{
   USC_Status status;
   int size;
   uscPrms->pInfo = NULL;
   status = uscPrms->USC_Fns->std.GetInfoSize(&size);
   if(status!=USC_NoError) return -1;

   uscPrms->pInfo = (USC_CodecInfo*)ippsMalloc_8u(size);

   if(uscPrms->pInfo==NULL) return -1;
   return 0;
}
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCCodecGetInfo
//  Purpose:     Retrive info from USC codec.
//  Returns:     0 if success, -1 if fails.
//  Parameters:
//    uscPrms       pointer to the input USC codec parametres.
*/
int USCCodecGetInfo(USCParams *uscPrms)
{
   USC_Status status;
   status = uscPrms->USC_Fns->std.GetInfo((USC_Handle)NULL, uscPrms->pInfo);
   if(status!=USC_NoError) return -1;
   return 0;
}
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        SetUSCEncoderParams
//  Purpose:     Set encode params.
//  Returns:     None.
//  Parameters:
//    uscPrms       pointer to the input\output USC codec parametres.
//    cmdParams     pointer to the input parametres from command line.
*/
void SetUSCEncoderParams(USCParams *uscPrms, CommandLineParams *cmdParams)
{
   uscPrms->pInfo->params.direction = 0;
   if(cmdParams->Vad > uscPrms->pInfo->params.modes.vad) {
      printf("WARNING: Unsupported VAD type. Ignored\n");
      uscPrms->pInfo->params.modes.vad = 0;
   } else
      uscPrms->pInfo->params.modes.vad = cmdParams->Vad;

   uscPrms->pInfo->params.law = 0;

   if(checkBitrate(uscPrms, cmdParams->bitrate))
      uscPrms->pInfo->params.modes.bitrate = cmdParams->bitrate;
   else
      printf("WARNING: Unsupported rate. Used default: %d\n",uscPrms->pInfo->params.modes.bitrate);

   return;
}

/* /////////////////////////////////////////////////////////////////////////////
//  Name:        SetUSCDecoderParams
//  Purpose:     Set decode params.
//  Returns:     None.
//  Parameters:
//    uscPrms       pointer to the input\output USC codec parametres.
//    cmdParams     pointer to the input parametres from command line.
*/
void SetUSCDecoderParams(USCParams *uscPrms, CommandLineParams *cmdParams)
{
   uscPrms->pInfo->params.direction = 1;
   uscPrms->pInfo->params.law = 0;
   if(checkBitrate(uscPrms, cmdParams->bitrate))
      uscPrms->pInfo->params.modes.bitrate = cmdParams->bitrate;
   return;
}
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCCodecAlloc
//  Purpose:     Alloc memory for USC codec.
//  Returns:     0 if success, -1 if fails.
//  Parameters:
//    uscPrms       pointer to the input\output USC codec parametres.
//    cmdParams     pointer to the input parametres from command line.
*/
USC_Status USCCodecAlloc(USCParams *uscPrms, CommandLineParams *cmdParams)
{
   int i,j;
   USC_Status status;
   status = uscPrms->USC_Fns->std.NumAlloc(&uscPrms->pInfo->params, &uscPrms->nbanks);
   if(status!=USC_NoError) return -1;

   for(j=0;j<uscPrms->nChannels;j++) {
      uscPrms->uCodec[j].pBanks = (USC_MemBank*)ippsMalloc_8u(sizeof(USC_MemBank)*uscPrms->nbanks);
      if (!uscPrms->uCodec[j].pBanks)
         return -1;
      status = uscPrms->USC_Fns->std.MemAlloc(&uscPrms->pInfo->params, uscPrms->uCodec[j].pBanks);
      if(status!=USC_NoError) return -1;

      for(i=0; i<uscPrms->nbanks;i++){
         uscPrms->uCodec[j].pBanks[i].pMem = (char*)ippsMalloc_8u(uscPrms->uCodec[j].pBanks[i].nbytes);
      }
      for(i=0; i<uscPrms->nbanks;i++){
         if (!uscPrms->uCodec[j].pBanks[i].pMem)
            return -1;
      }
   }

   cmdParams = cmdParams;
   return 0;
}
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCEncoderInit
//  Purpose:     Initialize USC encoder object.
//  Returns:     0 if success, -1 if fails.
//  Parameters:
//    uscPrms       pointer to the input\output USC codec parametres.
//    cmdParams     pointer to the input parametres from command line.
*/
int USCEncoderInit(USCParams *uscPrms, CommandLineParams *cmdParams)
{
   USC_Status status;
   int j;
   for(j=0;j<uscPrms->nChannels;j++) {
      status = uscPrms->USC_Fns->std.Init(&uscPrms->pInfo->params, uscPrms->uCodec[j].pBanks,
                                       &uscPrms->uCodec[j].hUSCCodec);
      if(status!=USC_NoError) return -1;
      /*How to change frame size:
         status = uscPrms->USC_Fns->SetFrameSize(&uscPrms->pInfo.params,uscPrms->uCodec[j].hUSCCodec,160);
         if(status!=USC_NoError) return -1;
         status = uscPrms->USC_Fns->std.GetInfo(uscPrms->uCodec[j].hUSCCodec, &uscPrms->pInfo);
         if(status!=USC_NoError) return -1;
      */
   }
   cmdParams = cmdParams;
   return 0;
}
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCEncoderInit
//  Purpose:     Initialize USC decoder object.
//  Returns:     0 if success, -1 if fails.
//  Parameters:
//    uscPrms       pointer to the input\output USC codec parametres.
//    cmdParams     pointer to the input parametres from command line.
*/
int USCDecoderInit(USCParams *uscPrms, CommandLineParams *cmdParams)
{
   USC_Status status;
   int j;
   for(j=0;j<uscPrms->nChannels;j++) {
      status = uscPrms->USC_Fns->std.Init(&uscPrms->pInfo->params, uscPrms->uCodec[j].pBanks,
                                             &uscPrms->uCodec[j].hUSCCodec);
      if(status!=USC_NoError) return -1;
      /*How to change frame size:
         status = uscPrms->USC_Fns->SetFrameSize(&uscPrms->pInfo.params,uscPrms->uCodec[j].hUSCCodec,160);
         if(status!=USC_NoError) return -1;
         status = uscPrms->USC_Fns->std.GetInfo(uscPrms->uCodec[j].hUSCCodec, &uscPrms->pInfo);
         if(status!=USC_NoError) return -1;
      */
   }
   cmdParams = cmdParams;
   return 0;
}

/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCEncoderPreProcessFrame
//  Purpose:     Pre-process frame to encode.
//  Returns:     lenght of written data to the output stream.
//  Parameters:
//    uscPrms   pointer to the input USC codec parametres.
//    pSrc      pointer to the input PCM data.
//    pDst      pointer to the output bitstream data.
//    in        pointer to the output USC_PCMStream structure for the frame.
//    out       pointer to the output USC_Bitstream structure for the frame.
*/
int USCEncoderPreProcessFrame(USCParams *uscPrms, char *pSrc, char *pDst,USC_PCMStream *in,USC_Bitstream *out)
{
   in->bitrate = uscPrms->pInfo->params.modes.bitrate;
   in->nbytes = uscPrms->pInfo->params.framesize;
   in->pBuffer = pSrc;
   in->pcmType.bitPerSample = uscPrms->pInfo->pcmType.bitPerSample;
   in->pcmType.sample_frequency = uscPrms->pInfo->pcmType.sample_frequency;
   out->pBuffer = pDst+FRAME_HEADER_SIZE;
   return 0;
}

/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCEncoderPostProcessFrame
//  Purpose:     Post-process of encoded frame.
//  Returns:     lenght of written data to the output stream.
//  Parameters:
//    uscPrms   pointer to the input USC codec parametres.
//    pSrc      pointer to the input PCM data.
//    pDst      pointer to the output bitstream data.
//    in        pointer to the output USC_PCMStream structure for the frame.
//    out       pointer to the output USC_Bitstream structure for the frame.
*/
int USCEncoderPostProcessFrame(USCParams *uscPrms, char *pSrc, char *pDst,USC_PCMStream *in,USC_Bitstream *out)
{
   unsigned char tmp;
   tmp = (unsigned char)(((unsigned int)out->bitrate)>>16);
   pDst[0] = (char)(tmp&0xFF);
   tmp = (unsigned char)(((unsigned int)out->bitrate)>>8);
   pDst[1] = (char)(tmp&0xFF);
   pDst[2] = (char)(out->bitrate&0xFF);
   pDst[3] = (char)out->frametype;
   tmp = (unsigned char)(((unsigned short)out->nbytes)>>8);
   pDst[4] = (char)(tmp&0xFF);
   pDst[5] = (char)(out->nbytes&0xFF);

   in = in;
   pSrc = pSrc;
   uscPrms = uscPrms;

   return out->nbytes+FRAME_HEADER_SIZE;
}

/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCDecoderPreProcessFrame
//  Purpose:     Pre-process frame to decode.
//  Returns:     lenght of processed data from the input stream.
//  Parameters:
//    uscPrms   pointer to the input USC codec parametres.
//    pSrc      pointer to the input bitstream data.
//    pDst      pointer to the output PCM data.
//    in        pointer to the output USC_Bitstream structure for the frame.
//    out       pointer to the output USC_PCMStream structure for the frame.
*/
int USCDecoderPreProcessFrame(USCParams *uscPrms, char *pSrc, char *pDst,USC_Bitstream *in,USC_PCMStream *out)
{
   in->bitrate = (int)(((unsigned char)(pSrc[0])<<16)|((unsigned char)(pSrc[1])<<8)|(unsigned char)pSrc[2]);
   in->frametype = (int)((signed char)pSrc[3]);
   in->nbytes = (int)(((unsigned char)(pSrc[4])<<8)|(unsigned char)pSrc[5]);
   in->pBuffer = pSrc+FRAME_HEADER_SIZE;
   out->pBuffer = pDst;

   uscPrms = uscPrms;

   return FRAME_HEADER_SIZE;
}
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCDecoderPostProcessFrame
//  Purpose:     Post-process of decoded frame.
//  Returns:     lenght of processed data from the input stream.
//  Parameters:
//    uscPrms   pointer to the input USC codec parametres.
//    pSrc      pointer to the input bitstream data.
//    pDst      pointer to the output PCM data.
//    in        pointer to the output USC_Bitstream structure for the frame.
//    out       pointer to the output USC_PCMStream structure for the frame.
*/
int USCDecoderPostProcessFrame(USCParams *uscPrms, char *pSrc, char *pDst,USC_Bitstream *in,USC_PCMStream *out)
{
   uscPrms = uscPrms;
   pSrc = pSrc;
   pDst = pDst;
   in = in;
   return out->nbytes;
}
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCCodecEncode
//  Purpose:     Encode pre-processed frame.
//  Returns:     lenght of processed data from the input stream.
//  Parameters:
//    uscPrms   pointer to the input USC codec parametres.
//    in        pointer to the input USC_Bitstream structure for the frame.
//    out       pointer to the output USC_PCMStream structure for the frame.
*/
int USCCodecEncode(USCParams *uscPrms, USC_PCMStream *in,USC_Bitstream *out, int channel)
{
   USC_Status status;
   status = uscPrms->USC_Fns->Encode (uscPrms->uCodec[channel].hUSCCodec, in, out);
   if(status!=USC_NoError) return -1;

   return in->nbytes;
}
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCCodecDecode
//  Purpose:     Decode pre-processed frame.
//  Returns:     lenght of processed data from the input stream.
//  Parameters:
//    uscPrms   pointer to the input USC codec parametres.
//    in        pointer to the input USC_PCMStream structure for the frame.
//    out       pointer to the output USC_Bitstream structure for the frame.
*/
int USCCodecDecode(USCParams *uscPrms, USC_Bitstream *in,USC_PCMStream *out, int channel)
{
   USC_Status status;
   status = uscPrms->USC_Fns->Decode (uscPrms->uCodec[channel].hUSCCodec, in, out);
   if(status!=USC_NoError) return -1;

   return in->nbytes;
}
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCCodecGetSize
//  Purpose:     Get output stream size.
//  Returns:     0 if success, -1 if fails.
//  Parameters:
//    uscPrms   pointer to the input USC codec parametres.
//    lenSrc    length of the input stream.
//    lenDst    pointer to the length of the output stream.
*/
int USCCodecGetSize(USCParams *uscPrms, int lenSrc, int *lenDst)
{
   USC_Status status;
   *lenDst = 0;
   status = uscPrms->USC_Fns->GetOutStreamSize(&uscPrms->pInfo->params, uscPrms->pInfo->params.modes.bitrate, lenSrc, lenDst);
   if(status!=USC_NoError) return -1;
   return 0;
}
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCCodecCorrectSize
//  Purpose:     Correct output stream size to include frame headers.
//  Returns:     0 if success.
//  Parameters:
//    uscPrms          pointer to the input USC codec parametres.
//    lenSrc           length of the input stream.
//    pSrcDstOutLen    pointer to the corrected length of the output stream.
*/
int USCCodecCorrectSize(USCParams *uscPrms, int lenSrc, int *pSrcDstOutLen)
{
   int nFrames;
   if(uscPrms->pInfo->params.direction==0) {
      nFrames = lenSrc/uscPrms->pInfo->params.framesize;
      /* Add block to hold the last compressed frame*/
      if (lenSrc % uscPrms->pInfo->params.framesize != 0) {
         nFrames++;
      }
      *pSrcDstOutLen += nFrames*FRAME_HEADER_SIZE;
   }

   return 0;
}
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCCodecCorrectSize
//  Purpose:     Get termination condition to organize encode\decode loop.
//  Returns:     0 if success.
//  Parameters:
//    uscPrms          pointer to the input USC codec parametres.
//    pLowBound        pointer to the low bound of the data to process.
//    pSrcDstOutLen    pointer to the corrected length of the output stream.
*/
int USCCodecGetTerminationCondition(USCParams *uscPrms, int *pLowBound)
{
   if(uscPrms->pInfo->params.direction==0) {
      /* In encode mode than we only operate under the whole frame*/
      *pLowBound = uscPrms->pInfo->params.framesize-1;
   } else {
      /* In decode mode we estimate the whole encoded frame presents in bitstream*/
      *pLowBound = 0;
   }

   return 0;
}
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCFree
//  Purpose:     Release resources of the USC codec.
//  Returns:     0 if success.
//  Parameters:
//    uscPrms   pointer to the input USC codec parametres.
*/
int USCFree(USCParams *uscPrms)
{
   int i,j;
   for(j=0;j<uscPrms->nChannels;j++) {
      for(i=0; i<uscPrms->nbanks;i++){
         ippsFree(uscPrms->uCodec[j].pBanks[i].pMem);
         uscPrms->uCodec[j].pBanks[i].pMem = NULL;
      }
      ippsFree(uscPrms->uCodec[j].pBanks);
      uscPrms->uCodec[j].pBanks = NULL;
      uscPrms->nbanks = 0;
   }
   if(uscPrms->pInfo) {
      ippsFree(uscPrms->pInfo);
      uscPrms->pInfo = NULL;
   }

   return 0;
}
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        USCGetFrameHeaderSize
//  Purpose:     Retrieve frame header size.
//  Returns:     frame header size.
//  Parameters:
*/
int USCGetFrameHeaderSize()
{
   return FRAME_HEADER_SIZE;
}

int USC_CvtToLiniar(USCParams *uscPrms, int PCMType, char **inputBuffer, int *pLen)
{
   if (PCMType > LINIAR_PCM) {
      unsigned char *cvt_buff = NULL;
      int lCvtLen = *pLen << 1;

      cvt_buff=ippsMalloc_8u(lCvtLen+uscPrms->pInfo->params.framesize);
      if(cvt_buff==NULL){
         return -1;
      }
      /* unpack to linear PCM if compound input */
      if (PCMType==ALAW_PCM) { /* A-Law input */
         ippsALawToLin_8u16s((unsigned char*)*inputBuffer, (short*)cvt_buff, *pLen);
      } else if (PCMType==MULAW_PCM){    /* Mu-Law input */
         ippsMuLawToLin_8u16s((unsigned char*)*inputBuffer, (short*)cvt_buff, *pLen);
      } else {
         return -1;
      }
      ippsFree(*inputBuffer);
      *inputBuffer = (char*)cvt_buff;
      *pLen = lCvtLen;
   }

   return 0;
}

int USC_CvtToLaw(USCParams *uscPrms, int PCMType, char *inputBuffer, int *pLen)
{
   if (PCMType > LINIAR_PCM) {
      int lCvtLen = *pLen >> 1;
      if (PCMType==ALAW_PCM) { /* A-Law output */
         ippsLinToALaw_16s8u((const short*) inputBuffer, (Ipp8u*) inputBuffer, lCvtLen);
      } else if (PCMType==MULAW_PCM) {   /* Mu-Law output */
         short *tmpBuffer=NULL;
         tmpBuffer = ippsMalloc_16s(lCvtLen);
         if(tmpBuffer==NULL) return -1;
         ippsCopy_16s((short*)inputBuffer,tmpBuffer,lCvtLen);
         ippsLinToMuLaw_16s8u(tmpBuffer, (Ipp8u*) inputBuffer, lCvtLen);
         ippsFree(tmpBuffer);
      } else {
         return -1;
      }
      *pLen = lCvtLen;
   }
   uscPrms = uscPrms;
   return 0;
}
