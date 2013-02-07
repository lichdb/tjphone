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
// Purpose: Speech sample. Main program file.
//
////////////////////////////////////////////////////////////////////////*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ipp_w7.h>
#include "ippcore.h"
#include "ipps.h"
#include "ippsc.h"

#ifdef USE_PLUGINS
#include "vm_shared_object.h"
#endif /*USE_PLUGINS*/

#include "usc.h"

#include "util.h"
#include "loadcodec.h"
#include "wavfile.h"
#include "usccodec.h"
#include "platform.h"

#define LOAD_CODEC_FAIL     1
#define FOPEN_FAIL          2
#define MEMORY_FAIL         3
#define UNKNOWN_FORMAT      4
#define USC_CALL_FAIL       5


#define COPYRIGHT_STRING "Copyright (c) 2005 Intel Corporation. All Rights Reserved."
MeasureIt measure;
int ProcessOneFrameOneChannel(USCParams *uscPrms, char *inputBuffer,
                              char *outputBuffer, int *pSrcLenUsed, int *pDstLenUsed, int channel)
{
   int frmlen, infrmLen, FrmDataLen;
   USC_PCMStream PCMStream;
   USC_Bitstream Bitstream;
   if(uscPrms->pInfo->params.direction==0) {
      /*Do the pre-procession of the frame*/
      infrmLen = USCEncoderPreProcessFrame(uscPrms, inputBuffer,
                                             outputBuffer,&PCMStream,&Bitstream);
      /*Encode one frame*/
      measure_start(&measure);
      FrmDataLen = USCCodecEncode(uscPrms, &PCMStream,&Bitstream,channel);
      measure_pause(&measure);
      if(FrmDataLen < 0) return USC_CALL_FAIL;
      infrmLen += FrmDataLen;
      /*Do the post-procession of the frame*/
      frmlen = USCEncoderPostProcessFrame(uscPrms, inputBuffer,
                                             outputBuffer,&PCMStream,&Bitstream);
      *pSrcLenUsed = infrmLen;
      *pDstLenUsed = frmlen;
   } else {
      /*Do the pre-procession of the frame*/
      infrmLen = USCDecoderPreProcessFrame(uscPrms, inputBuffer,
                                             outputBuffer,&Bitstream,&PCMStream);
      /*Decode one frame*/
      measure_start(&measure);
      FrmDataLen = USCCodecDecode(uscPrms, &Bitstream,&PCMStream,channel);
      measure_pause(&measure);
      if(FrmDataLen < 0) return USC_CALL_FAIL;
      infrmLen += FrmDataLen;
      /*Do the post-procession of the frame*/
      frmlen = USCDecoderPostProcessFrame(uscPrms, inputBuffer,
                                             outputBuffer,&Bitstream,&PCMStream);
      *pSrcLenUsed = infrmLen;
      *pDstLenUsed = frmlen;
   }
   return 0;
}
#define MAX_LEN(a,b) ((a)>(b))? (a):(b)
int ProcessOneChannel(LoadedCodec *codec, WaveFileParams *wfOutputParams, char *inputBuffer,
                              int inputLen, char *outputBuffer, int *pDuration, int *dstLen)
{
   int duration=0, outLen = 0, currLen;
   char *pInputBuffPtr = inputBuffer;
   int frmlen, infrmLen, cvtLen;
   int lLowBound;
   currLen = inputLen;

   USCCodecGetTerminationCondition(&codec->uscParams, &lLowBound);

   while(currLen > lLowBound) {
      ProcessOneFrameOneChannel(&codec->uscParams, pInputBuffPtr,
                              outputBuffer, &infrmLen, &frmlen,SINGLE_CHANNEL);
      /* Write encoded data to the output file*/
      cvtLen = frmlen;
      if((wfOutputParams->waveFmt.nFormatTag==ALAW_PCM)||(wfOutputParams->waveFmt.nFormatTag==MULAW_PCM)) {
         USC_CvtToLaw(&codec->uscParams, wfOutputParams->waveFmt.nFormatTag, outputBuffer, &cvtLen);
      }
      WavFileWrite(wfOutputParams, outputBuffer, cvtLen);
      /* Move pointer to the next position*/
      currLen -= infrmLen;
      pInputBuffPtr += infrmLen;
      duration += MAX_LEN(infrmLen,frmlen);
      outLen += frmlen;
   }

   *pDuration = duration;
   *dstLen = outLen;
   return 0;
}

#if defined( _WIN32_WCE )
#define WINCE_CMDLINE_SIZE 512
#define WINCE_EXENAME_SIZE 128
#define WINCE_NCMD_PARAMS   16
int parseCmndLine( char* exename, const char* cmndline, char* line, int linelen, char** argv, int argvlen ) {
   int i;
   char* token;
   char* seps = " ,";                     /* argement separators */
   int argc = 1;                          /* number of parameters */
   for (i=0; i<argvlen; i++) argv[i] = NULL;
   argv[0] = exename;                     /* the first standard argument */
   memset( line, 0, linelen );
   strncpy( line, cmndline, linelen-1 );
   token = strtok( line, seps );          /* the first true argument */
   while( token != NULL && argc <= argvlen ) {
      argv[argc++] = token;
      token = strtok( NULL, seps );
   }
   return argc;
}

int WINAPI WinMain( HINSTANCE hinst, HINSTANCE xxx, LPWSTR lpCmdLine, int yyy )
{
   char line[WINCE_CMDLINE_SIZE];                     /* to copy command line */
   char* argvv[WINCE_NCMD_PARAMS];
   char** argv=argvv;

   wchar_t wexename[WINCE_EXENAME_SIZE];
   char exename[WINCE_EXENAME_SIZE];
   char cmdline[WINCE_CMDLINE_SIZE];

   /* simulate argc and argv parameters */
   int argc;
#else /*Other OS*/
int main(int argc, char *argv[])
{
#endif /*_WIN32_WCE*/
   int lCallResult, encode;
   int PCMType = -1;
   LoadedCodec codec;
   WaveFileParams wfInputParams;
   WaveFileParams wfOutputParams;
   CommandLineParams clParams;
   char *inputBuffer=NULL;
   char *outputBuffer=NULL;
   int currLen, duration;
   const  IppLibraryVersion *verIppSC;
   float spSeconds;
   FILE *f_log=NULL;

#if defined( _WIN32_WCE )

   GetModuleFileName( hinst, wexename, WINCE_EXENAME_SIZE );
   sprintf( exename, "%ls", wexename );
   sprintf( cmdline, "%ls", lpCmdLine );
   argc = parseCmndLine( exename, cmdline, line, WINCE_CMDLINE_SIZE, argv, WINCE_NCMD_PARAMS );

#endif
   /*
      Dynamic re-linkage from PX to target IPP library,
      need only if codec was linked with ippmerged lib
   */

   ippStaticInit();

#ifdef USE_PLUGINS
   codec.handleCodecDLL = NULL;
   codec.handleFormatDLL = NULL;
#endif /*USE_PLUGINS*/

   SetCommandLineByDefault(&clParams);
   /*Get params from comman line.*/
   ReadCommandLine(&clParams, argc, argv);
   if(clParams.puttologfile) {
      f_log=fopen(clParams.logFileName,"a");
      if(f_log==NULL) {
         printf("Cannot open %s log file for writing\n",clParams.logFileName);
         printf("Log file ignored.\n");
         clParams.puttologfile = 0;
      }
   }

   if(clParams.optionReport) {
      verIppSC = ippscGetLibVersion();
      USC_OutputString(f_log, "%s\n",COPYRIGHT_STRING);
      USC_OutputString(f_log, "Intel Unified Speech Codec interface based console player\n");
      USC_OutputString(f_log, "The Intel(R) IPPSC library used:  %d.%d.%d Build %d, name %s\n",
             verIppSC->major,verIppSC->minor,verIppSC->majorBuild,verIppSC->build,verIppSC->Name);
   }

   if(clParams.enumerate) {
      EnumerateStaticLinkedCodecs(f_log);
      if(f_log) fclose(f_log);
      return 0;
   }

   if((!clParams.inputFileName[0]) ||(!clParams.outputFileName[0])) {
      PrintUsage();
      return 0;
   }
   /*Open inpuit file and read header*/
   InitWavFile(&wfInputParams);
   lCallResult = OpenWavFile(&wfInputParams, clParams.inputFileName, FILE_READ);
   if(lCallResult==-1) {
      USC_OutputString(f_log, "Cannot open %s file for reading\n",clParams.inputFileName);
      if(f_log) fclose(f_log);
      return FOPEN_FAIL;
   }
   /*Read WAVE file header*/
   lCallResult = WavFileReadHeader(&wfInputParams);
   if(lCallResult<0) {
      if(lCallResult==-1) USC_OutputString(f_log, "Couldn't read from the %s file\n",clParams.inputFileName);
      if(lCallResult==-2) USC_OutputString(f_log, "File %s isn't in RIFF format\n",clParams.inputFileName);
      if(lCallResult==-3) USC_OutputString(f_log, "File %s is in RIFF format but not in a WAVE foramt\n",
                                       clParams.inputFileName);;
      WavFileClose(&wfInputParams);
      if(f_log) fclose(f_log);
      return FOPEN_FAIL;
   }
   if(clParams.timing) {
      lCallResult = Timing(&clParams, &wfInputParams, f_log);
      return lCallResult;
   }
   /*If input file in PCM format than encode else decode*/
   if((wfInputParams.waveFmt.nFormatTag==LINIAR_PCM)||
            (wfInputParams.waveFmt.nFormatTag==ALAW_PCM/*A-law*/)||
            (wfInputParams.waveFmt.nFormatTag==MULAW_PCM/*mu-law*/)) {
      encode = 1;
      PCMType = wfInputParams.waveFmt.nFormatTag;
      strcpy((char*)codec.codecName, clParams.codecName);
      codec.lIsVad = (clParams.Vad>0);
   } else {
      encode = 0;
      codec.lFormatTag = wfInputParams.waveFmt.nFormatTag;
      clParams.bitrate = wfInputParams.bitrate;
   }

   if(encode) {
      /*Load codec by name from command line*/
      lCallResult = LoadUSCCodecByName(&codec,f_log);
      if(lCallResult<0) {
         USC_OutputString(f_log, "Cannot find %s codec.\n",codec.codecName);
         if(f_log) fclose(f_log);
         return LOAD_CODEC_FAIL;
      }
      /*Get USC codec params*/
      lCallResult = USCCodecAllocInfo(&codec.uscParams);
      if(lCallResult<0) return USC_CALL_FAIL;
      lCallResult = USCCodecGetInfo(&codec.uscParams);
      if(lCallResult<0) return USC_CALL_FAIL;
      /*Get its supported format details*/
      lCallResult = GetUSCCodecParamsByFormat(&codec,BY_NAME,f_log);
      if(lCallResult<0) {
         USC_OutputString(f_log, "Cannot find %s codec format description.\n",codec.codecName);
         if(f_log) fclose(f_log);
         return LOAD_CODEC_FAIL;
      }
   } else {
      /*Get its supported format details*/
      lCallResult = GetUSCCodecParamsByFormat(&codec,BY_FORMATTAG,f_log);
      if(lCallResult<0) {
         USC_OutputString(f_log, "Cannot find format description.\n");
         if(f_log) fclose(f_log);
         return LOAD_CODEC_FAIL;
      }
      /*Load codec by name from format description*/
      lCallResult = LoadUSCCodecByName(&codec,f_log);
      if(lCallResult<0) {
         USC_OutputString(f_log, "Cannot find %s codec.\n",codec.codecName);
         if(f_log) fclose(f_log);
         return LOAD_CODEC_FAIL;
      }
      /*Get USC codec params*/
      lCallResult = USCCodecAllocInfo(&codec.uscParams);
      if(lCallResult<0) return USC_CALL_FAIL;
      lCallResult = USCCodecGetInfo(&codec.uscParams);
      if(lCallResult<0) return USC_CALL_FAIL;
   }
   /* Check sample_frequence, bitspersample and number of channels*/
   if(encode) {
      if(PCMType==LINIAR_PCM) {
         if(wfInputParams.waveFmt.nBitPerSample != codec.uscParams.pInfo->pcmType.bitPerSample) {
            USC_OutputString(f_log, "Bits per sample must be %d for the %s codec\n",
                  codec.uscParams.pInfo->pcmType.bitPerSample, codec.uscParams.pInfo->name);
            if(f_log) fclose(f_log);
            return UNKNOWN_FORMAT;
         }
      } else {
         if(wfInputParams.waveFmt.nBitPerSample != 8) {
            USC_OutputString(f_log, "Bits per sample must be 8 for the A-Law/Mu-Law input file\n");
            if(f_log) fclose(f_log);
            return UNKNOWN_FORMAT;
         }
      }
      if(wfInputParams.waveFmt.nSamplesPerSec != codec.uscParams.pInfo->pcmType.sample_frequency) {
         USC_OutputString(f_log, "Must be %d samples per seconds for the %s codec\n",
               codec.uscParams.pInfo->pcmType.sample_frequency,
               codec.uscParams.pInfo->name);
         if(f_log) fclose(f_log);
         return UNKNOWN_FORMAT;
      }
      if(wfInputParams.waveFmt.nChannels != 1) {
         USC_OutputString(f_log, "Input file must be mono\n");
         if(f_log) fclose(f_log);
         return UNKNOWN_FORMAT;
      }
   }
   /*Initialize Wave File params*/
   InitWavFile(&wfOutputParams);
   if (!encode) {
      /*Output file is pcm*/
      wfOutputParams.waveFmt.nFormatTag = LINIAR_PCM;
      wfOutputParams.waveFmt.nBitPerSample = (short)codec.uscParams.pInfo->pcmType.bitPerSample;
      if(clParams.nOutputPCMType==ALAW_PCM) {
         wfOutputParams.waveFmt.nFormatTag = ALAW_PCM;
         wfOutputParams.waveFmt.nBitPerSample = 8;
      } else if(clParams.nOutputPCMType==MULAW_PCM) {
         wfOutputParams.waveFmt.nFormatTag = MULAW_PCM;
         wfOutputParams.waveFmt.nBitPerSample = 8;
      }
   } else {
      wfOutputParams.waveFmt.nFormatTag = (short)codec.lFormatTag;
      wfOutputParams.waveFmt.nBitPerSample = 0;
   }
   /*Open wave file and read its header*/
   lCallResult=OpenWavFile(&wfOutputParams, clParams.outputFileName, FILE_WRITE);
   if(lCallResult==-1) {
      USC_OutputString(f_log, "Cannot open %s file for writing\n",clParams.outputFileName);
      if(f_log) fclose(f_log);
      return FOPEN_FAIL;
   }

   if(encode) {
      /*Set params for encode*/
      SetUSCEncoderParams(&codec.uscParams, &clParams);
   } else {
      /*Set params for decode*/
      SetUSCDecoderParams(&codec.uscParams, &clParams);
   }
   /*Alloc memory for the codec*/
   codec.uscParams.nChannels = wfInputParams.waveFmt.nChannels;
   lCallResult = USCCodecAlloc(&codec.uscParams, &clParams);
   if(lCallResult<0) return USC_CALL_FAIL;
   if(encode) {
      /*Init encoder*/
      lCallResult = USCEncoderInit(&codec.uscParams, &clParams);
   } else {
      /*Init decoder*/
      lCallResult = USCDecoderInit(&codec.uscParams, &clParams);
   }
   if(lCallResult<0) return USC_CALL_FAIL;
   /*Allocate memory for the input buffer. Size of input buffer is equal to the size of input file*/
   inputBuffer = (char *)ippsMalloc_8u(wfInputParams.DataSize);
   if(!inputBuffer) {
      USC_OutputString(f_log, "Cannot allocate %d bytes memory\n",wfInputParams.DataSize);
      if(f_log) fclose(f_log);
      return MEMORY_FAIL;
   }

   /*Allocate memory for the output buffer. Size of output buffer is equal to the size of 1 frame*/
   outputBuffer = (char *)ippsMalloc_8u(codec.uscParams.pInfo->params.framesize*wfInputParams.waveFmt.nChannels);
   if(!outputBuffer) {
      USC_OutputString(f_log, "Cannot allocate %d bytes memory\n",codec.uscParams.pInfo->params.framesize);
      if(f_log) fclose(f_log);
      return MEMORY_FAIL;
   }

   /*Read data from the input file to the buffer*/
   WavFileRead(&wfInputParams, (void *)inputBuffer, wfInputParams.DataSize);
   if((encode)&&(PCMType > LINIAR_PCM)) {
      lCallResult = USC_CvtToLiniar(&codec.uscParams, PCMType, &inputBuffer, (int*)&wfInputParams.DataSize);
      if(lCallResult<0) return MEMORY_FAIL;
   }

   /*Set pointer to the start of the data*/

   measure_init(&measure);
   ProcessOneChannel(&codec, &wfOutputParams, inputBuffer, wfInputParams.DataSize,
                        outputBuffer, &duration, &currLen);
   measure_finish(&measure);
   spSeconds = (duration/(float)(codec.uscParams.pInfo->pcmType.bitPerSample>>3))/
                        (float)codec.uscParams.pInfo->pcmType.sample_frequency;
   USC_OutputString(f_log, "Proceeded %g sec of speech\n",spSeconds);
   if(clParams.puttologfile) {
      measure_output(f_log, &measure, spSeconds, 1, clParams.optionReport);
   } else {
      measure_output(stdout, &measure, spSeconds, 1, clParams.optionReport);
   }

   OutputString2CSVFile(&clParams, codec.uscParams.pInfo, spSeconds,measure.speed_in_mhz);

   /* Fill output file params*/
   wfOutputParams.waveFmt.nChannels = 1;
   wfOutputParams.waveFmt.nSamplesPerSec = codec.uscParams.pInfo->pcmType.sample_frequency;

   if (!encode) {
      /*Output file is pcm*/
      wfOutputParams.waveFmt.nBlockAlign = wfOutputParams.waveFmt.nChannels*(wfOutputParams.waveFmt.nBitPerSample>>3);
   } else {
      int nBlockAlign;
      wfOutputParams.bitrate = codec.uscParams.pInfo->params.modes.bitrate;
      /* Calc nBlockAlign of the current codec*/
      USCCodecGetSize(&codec.uscParams, codec.uscParams.pInfo->params.framesize, &nBlockAlign);
      wfOutputParams.waveFmt.nBlockAlign = (short)((nBlockAlign+USCGetFrameHeaderSize())*wfOutputParams.waveFmt.nChannels);
   }

   /* Close input file*/
   WavFileClose(&wfInputParams);
   /* Write file header*/
   WavFileWriteHeader(&wfOutputParams);
   /* Close output file*/
   WavFileClose(&wfOutputParams);
   /*Free resources*/
   if(inputBuffer) ippsFree(inputBuffer);
   if(outputBuffer) ippsFree(outputBuffer);
   /*Free codec memory*/
   USCFree(&codec.uscParams);
   /* Close plug-ins*/
   FreeUSCSharedObjects(&codec);
   if(f_log) fclose(f_log);
   return 0;
}
