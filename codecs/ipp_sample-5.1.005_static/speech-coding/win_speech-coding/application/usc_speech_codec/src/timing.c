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
// Purpose: Codec Timing function.
//
////////////////////////////////////////////////////////////////////////*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ipp_w7.h>
#include "ippcore.h"
#include "ipps.h"

#include "usc.h"

#include "util.h"
#include "loadcodec.h"
#include "wavfile.h"
#include "usccodec.h"
#include "platform.h"

#ifndef USC_MAX_NAME_LEN
   #define USC_MAX_NAME_LEN 64
#endif

int Timing(CommandLineParams *clParams, WaveFileParams *inputWParams, FILE *f_log)
{
   MeasureIt ENCmeasure;
   MeasureIt DECmeasure;
   int numCodecs, lCallResult;
   int lCodecNum, lVAD, nVAD, lBitrate, nRates, lRepeat;
   char codecName[USC_MAX_NAME_LEN];
   USCParams uscENCParams, uscDECParams;
   int duration = 0;
   float spSeconds;

   char *inputBuffer,*outputBuffer,*outputDecodedBuffer;

   inputBuffer = NULL;
   inputBuffer = (char *)ippsMalloc_8u(inputWParams->DataSize);
   if(!inputBuffer) {
      USC_OutputString(f_log, "Cannot allocate %d bytes memory\n",inputWParams->DataSize);
      if(f_log) fclose(f_log);
      return -1;
   }

   /*Read data from the input file to the buffer*/
   WavFileRead(inputWParams, (void *)inputBuffer, inputWParams->DataSize);

   if(clParams->puttocsv) {
      FILE *f_csv = NULL;
      f_csv = OpenCSVFile(clParams->csvFileName);
      if (f_csv == NULL) {
         printf("\nWARNING: File %s could not be open. Ignored.\n", clParams->csvFileName);
      } else {
         OutputSysInfo2CSVFile(f_csv);
         AddCodecTableHeader2CSVFile(f_csv);
         CloseCSVFile(f_csv);
      }
   }

   numCodecs = GetNumLinkedCodecs();
   for(lCodecNum=0;lCodecNum < numCodecs;lCodecNum++) {

      uscENCParams.USC_Fns = Get_ith_StaticLinkedCodecs(lCodecNum, (char *)codecName);
      if(clParams->TimingCodecName[0] != 0) {
         if(strcmp(codecName,clParams->TimingCodecName) != 0) continue;
      }
      uscDECParams.USC_Fns = uscENCParams.USC_Fns;
      lCallResult = USCCodecAllocInfo(&uscENCParams);
      if(lCallResult<0) return -1;
      lCallResult = USCCodecAllocInfo(&uscDECParams);
      if(lCallResult<0) return -1;
      lCallResult = USCCodecGetInfo(&uscENCParams);
      if(lCallResult<0) return -1;
      lCallResult = USCCodecGetInfo(&uscDECParams);
      if(lCallResult<0) return -1;

      if(inputWParams->waveFmt.nBitPerSample != uscENCParams.pInfo->pcmType.bitPerSample) {
         continue;
      }
      if(inputWParams->waveFmt.nSamplesPerSec != uscENCParams.pInfo->pcmType.sample_frequency) {
         continue;
      }
      if(inputWParams->waveFmt.nChannels != 1) {
         continue;
      }


      nVAD = uscENCParams.pInfo->params.modes.vad;
      nRates = uscENCParams.pInfo->nRates;
      for(lVAD=0;lVAD <= nVAD;lVAD++) {
         for(lBitrate=0;lBitrate < nRates;lBitrate++) {
            lCallResult = USCCodecGetInfo(&uscENCParams);
            if(lCallResult<0) {
               USC_OutputString(f_log, "USCCodecGetInfo failed for encoder\n");
               if(f_log) fclose(f_log);
               return -1;
            }
            lCallResult = USCCodecGetInfo(&uscDECParams);
            if(lCallResult<0) {
               USC_OutputString(f_log, "USCCodecGetInfo failed for decoder\n");
               if(f_log) fclose(f_log);
               return -1;
            }

            clParams->Vad = lVAD;
            clParams->bitrate = uscENCParams.pInfo->pRateTbl[lBitrate].bitrate;

            uscENCParams.nChannels = inputWParams->waveFmt.nChannels;
            uscDECParams.nChannels = inputWParams->waveFmt.nChannels;

            /*Set params for encode*/
            SetUSCEncoderParams(&uscENCParams, clParams);
            /*Set params for decode*/
            SetUSCDecoderParams(&uscDECParams, clParams);
            /*Alloc memory for the codec*/
            lCallResult = USCCodecAlloc(&uscENCParams, clParams);
            if(lCallResult<0) {
               USC_OutputString(f_log, "USCCodecAlloc failed for encoder\n");
               if(f_log) fclose(f_log);
               return -1;
            }
            lCallResult = USCCodecAlloc(&uscDECParams, clParams);
            if(lCallResult<0) {
               USC_OutputString(f_log, "USCCodecAlloc failed for decoder\n");
               if(f_log) fclose(f_log);
               return -1;
            }

            duration = 0;
            measure_opendrv(&ENCmeasure, &DECmeasure);
            measure_init(&ENCmeasure);
            measure_init(&DECmeasure);
            for(lRepeat = 0;lRepeat < clParams->nRepeat;lRepeat++) {
               /*Init encoder*/
               lCallResult = USCEncoderInit(&uscENCParams, clParams);
               if(lCallResult<0) {
                  USC_OutputString(f_log, "USCEncoderInit failed\n");
                  if(f_log) fclose(f_log);
                  return -1;
               }
               lCallResult = USCDecoderInit(&uscDECParams, clParams);
               if(lCallResult<0) {
                  USC_OutputString(f_log, "USCDecoderInit failed\n");
                  if(f_log) fclose(f_log);
                  return -1;
               }
               outputBuffer = NULL;
               outputBuffer = (char *)ippsMalloc_8u(uscENCParams.pInfo->params.framesize*inputWParams->waveFmt.nChannels);
               if(!outputBuffer) {
                  USC_OutputString(f_log, "Cannot allocate %d bytes memory\n",uscENCParams.pInfo->params.framesize);
                  if(f_log) fclose(f_log);
                  return -1;
               }
               outputDecodedBuffer = NULL;
               outputDecodedBuffer = (char *)ippsMalloc_8u(uscENCParams.pInfo->params.framesize*inputWParams->waveFmt.nChannels);
               if(!outputDecodedBuffer) {
                  USC_OutputString(f_log, "Cannot allocate %d bytes memory\n",uscENCParams.pInfo->params.framesize);
                  if(f_log) fclose(f_log);
                  return -1;
               }
               /*Allocate memory for the input buffer. Size of input buffer is equal to the size of input file*/
               {
                  int currLen, lLowBound;
                  int infrmLen, FrmDataLen;
                  char *pInputBuffPtr;
                  USC_PCMStream PCMStream;
                  USC_PCMStream DecodedPCMStream;
                  USC_Bitstream Bitstream;

                  pInputBuffPtr = inputBuffer;
                  currLen = inputWParams->DataSize;
                  DecodedPCMStream.pBuffer = outputDecodedBuffer;

                  USCCodecGetTerminationCondition(&uscENCParams, &lLowBound);

                  while(currLen > lLowBound) {
                     /*Do the pre-procession of the frame*/
                     PCMStream.bitrate = uscENCParams.pInfo->params.modes.bitrate;
                     PCMStream.nbytes = uscENCParams.pInfo->params.framesize;
                     PCMStream.pBuffer = pInputBuffPtr;
                     PCMStream.pcmType.bitPerSample = uscENCParams.pInfo->pcmType.bitPerSample;
                     PCMStream.pcmType.sample_frequency = uscENCParams.pInfo->pcmType.sample_frequency;
                     Bitstream.pBuffer = outputBuffer;
                     infrmLen = 0;
                     /*Encode one frame*/
                     measure_start(&ENCmeasure);
                     FrmDataLen = USCCodecEncode(&uscENCParams, &PCMStream,&Bitstream,SINGLE_CHANNEL);
                     measure_pause(&ENCmeasure);
                     if(FrmDataLen < 0) {
                        USC_OutputString(f_log, "USCCodecEncode failed\n");
                        if(f_log) fclose(f_log);
                        return -1;
                     }
                     infrmLen += FrmDataLen;

                     /*Decode one frame*/
                     measure_start(&DECmeasure);
                     FrmDataLen = USCCodecDecode(&uscDECParams, &Bitstream,&DecodedPCMStream,SINGLE_CHANNEL);
                     measure_pause(&DECmeasure);
                     if(FrmDataLen < 0) {
                        USC_OutputString(f_log, "USCCodecDecode failed\n");
                        if(f_log) fclose(f_log);
                        return -1;
                     }
                     /* Move pointer to the next position*/
                     currLen -= infrmLen;
                     pInputBuffPtr += infrmLen;
                     duration += infrmLen;
                  }/*while(currLen > lLowBound)*/
               }
               if(outputBuffer) ippsFree(outputBuffer);
               if(outputDecodedBuffer) ippsFree(outputDecodedBuffer);
            } /*lRepeat*/
            measure_finish(&ENCmeasure);
            measure_finish(&DECmeasure);
            spSeconds = (duration/(float)(uscENCParams.pInfo->pcmType.bitPerSample>>3))/
                        (float)uscENCParams.pInfo->pcmType.sample_frequency;

            USC_OutputString(f_log, "Proceeded %g sec of speech\n",spSeconds);
            if(clParams->puttologfile) {
               measure_output(f_log, &ENCmeasure, spSeconds, 1, clParams->optionReport);
               measure_output(f_log, &DECmeasure, spSeconds, 1, clParams->optionReport);
            } else {
               measure_output(stdout, &ENCmeasure, spSeconds, 1, clParams->optionReport);
               measure_output(stdout, &DECmeasure, spSeconds, 1, clParams->optionReport);
            }

            OutputDuplexString2CSVFile(clParams, uscENCParams.pInfo, spSeconds,
                                      ENCmeasure.speed_in_mhz,DECmeasure.speed_in_mhz);
            /*Free codec memory*/
            USCFree(&uscENCParams);
            USCFree(&uscDECParams);
            lCallResult = USCCodecAllocInfo(&uscENCParams);
            if(lCallResult<0) return -1;
            lCallResult = USCCodecAllocInfo(&uscDECParams);
            if(lCallResult<0) return -1;
            measure_closedrv(&ENCmeasure, &DECmeasure);
         }/*lBitrate*/
      }/*lVAD*/
   }/*lCodecNum*/

   /*Free codec memory*/
   USCFree(&uscENCParams);
   USCFree(&uscDECParams);

   /* Close input file*/
   WavFileClose(inputWParams);
   if(inputBuffer) ippsFree(inputBuffer);

   return 0;
}
