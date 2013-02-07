/*////////////////////////////////////////////////////////////////////////
//
// INTEL CORPORATION PROPRIETARY INFORMATION
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Intel Corporation and may not be copied
// or disclosed except in accordance with the terms of that agreement.
// Copyright (c) 2005-2006 Intel Corporation. All Rights Reserved.
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
// Purpose: Auxiliary functions file.
//
////////////////////////////////////////////////////////////////////////*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "usc.h"
#include "util.h"
#include "vm_sys_info.h"

#include <ipp_w7.h>
#include "ippcore.h"
#include "ipps.h"
#include "ippsc.h"

void SetCommandLineByDefault(CommandLineParams *params)
{
   memset((void*)params,0,sizeof(CommandLineParams));
}
void ReadCommandLine(CommandLineParams *params, int argc, char *argv[])
{
   argc--;
   while(argc-- > 0){ /*process command line */
      argv++;
      if ('-' == (*argv)[0]) {
         if (!strncmp("-r",*argv,2)){ /* check if rate is specified */
            params->bitrate = atol(*argv+2);
            continue;
         }else if (!strncmp("-v",*argv,2)){ /* check vad */
            params->Vad = atol(*argv+2);
            if(params->Vad==0) params->Vad++;
            continue;
         }else if (!strcmp(*argv,"-format")){ /* check format type */
            if(argc-->0){
               strcpy(params->codecName, *++argv);
            }
            continue;
         }else if (!strncmp("-timing",*argv,7)){ /* check format type */
            params->timing = 1;
            if(strlen(*argv)!=strlen("-timing")) {
               /*Time single codec*/
               strcpy(params->TimingCodecName,*argv+7);
            }
            if(argc-->0){
               params->nRepeat = atol(*++argv);
               if(params->nRepeat < 1) params->nRepeat = 1;
            }
            continue;
         }else if (!strcmp(*argv,"-n")){ /* check option report mode */
            params->optionReport = 1;
            continue;
         }else if (!strcmp(*argv,"-list")){ /* check enumerate formats mode */
            params->enumerate = 1;
            return;
         }else if (!strcmp(*argv,"-c")){ /* check if csv output is specified */
            params->puttocsv=1;
            argv++;
            argc--;
            strcpy(params->csvFileName, *argv);
            continue;
         }else if (!strcmp(*argv,"-o")){ /* check if log output is specified */
            params->puttologfile=1;
            argv++;
            argc--;
            strcpy(params->logFileName, *argv);
            continue;
         }else if (!strcmp(*argv,"-alaw")){
            params->nOutputPCMType=ALAW_PCM;
            continue;
         }else if (!strcmp(*argv,"-mulaw")){
            params->nOutputPCMType=MULAW_PCM;
            continue;
         }
      }else{ /* unknown option */
         argc++; /* skip */
         argv--;
         break;
      }
   }

   if(argc-->0){ /* check if input file name is specified */
      argv++;
      strcpy(params->inputFileName, *argv);
   }
   if(!params->timing) {
      if(argc-->0){ /* check if output file name is specified */
         argv++;
         strcpy(params->outputFileName, *argv);
      }
   } else {
      strcpy(params->outputFileName, "Not_Applicable");
   }
   return;
}

void PrintUsage()
{
   printf("Usage :    <options> <inFile> <outFile>\n");
   printf("  options:\n");
   printf("   [-format CodecName] Set codec (for encoder only). \n");
   printf("   [-r<mode>]        Set bitrate mode. \n");
   printf("   [-v[<nVAD>]]      Enables DTX mode number <nVAD>, default VAD mode if omitted.(optional).\n");
   printf("   [-n]              Option reports (optional) .\n");
   printf("   [-c] <csvFile>    Print out performance info line to <csvFile> file (optional).\n");
   printf("   [-o] <logFile>    Print out to <logFile> file (optional).\n");
   printf("   [-list]           Enumerate all stiticly linked codecs(optional). \n");
   printf("   [-alaw]           Output file will be A-Law PCM (optional, only for decoder part).\n");
   printf("   [-mulaw]          Output file will be Mu-Law PCM (optional, only for decoder part).\n");
   printf("For timing: -timing[CodecName] <NRepeat> <options> <inWaveFile>.\n");
   printf("  options:\n");
   printf("   <NRepeat>         How many time repeat input file.\n");
   printf("   [-c] <csvFile>    Print out performance info line to <csvFile> file (optional).\n");
   printf("   [-o] <logFile>    Print out to <logFile> file (optional).\n");
   printf("   [CodecName]       Codec name, if unspecified, all linked codec, corresponding <inWaveFile>.\n");
   printf("                     will be timed.\n");

   return;
}

int OutputString2CSVFile(CommandLineParams *params, USC_CodecInfo *pInfo, float spSeconds,float speed_in_mhz)
{
   if (params->puttocsv) { /* open the csv file if any */
      FILE *f_csv=NULL;
      char* pFileName;

      if ( (f_csv = fopen(params->csvFileName, "a")) == NULL) {
         printf("\nWARNING: File %s could not be open. Ignored.\n", params->csvFileName);
         return -1;
      }
      if ((pFileName = strrchr( params->inputFileName, '/')) != NULL) {
        pFileName += 1;
      } else if ((pFileName = strrchr( params->inputFileName, '\\')) != NULL) {
        pFileName += 1;
      } else
        pFileName = params->inputFileName;
      fprintf(f_csv,"%s,%s,%s,%d,%s,%4.2f,%4.2f,%d\n",pInfo->name,
         (pInfo->params.direction)? "decode":"encode",
         (pInfo->params.modes.vad == 1)?"VAD1":(pInfo->params.modes.vad == 2)?"VAD2":"    ",
         pInfo->params.modes.bitrate,pFileName,spSeconds,speed_in_mhz,1);
      fclose(f_csv);
   }
   return 0;
}

FILE *OpenCSVFile(char *csvFileName)
{
   FILE *f_csv=NULL;
   f_csv = fopen(csvFileName, "a");
   return f_csv;
}
void CloseCSVFile(FILE *f_csv)
{
   if(f_csv) {
      fclose(f_csv);
   }
}
int OutputDuplexString2CSVFile(CommandLineParams *params, USC_CodecInfo *pInfo, float spSeconds,
                               float ENCspeed_in_mhz, float DECspeed_in_mhz)
{
   if (params->puttocsv) { /* open the csv file if any */
      FILE *f_csv=NULL;
      char* pFileName;

      f_csv = OpenCSVFile(params->csvFileName);

      if (f_csv == NULL) {
         printf("\nWARNING: File %s could not be open. Ignored.\n", params->csvFileName);
         return -1;
      }
      if ((pFileName = strrchr( params->inputFileName, '/')) != NULL) {
        pFileName += 1;
      } else if ((pFileName = strrchr( params->inputFileName, '\\')) != NULL) {
        pFileName += 1;
      } else
        pFileName = params->inputFileName;
      fprintf(f_csv,"%s,%s,%d,%s,%4.2f,%4.2f,%4.2f,%d\n",pInfo->name,
         (pInfo->params.modes.vad == 1)?"VAD1":(pInfo->params.modes.vad == 2)?"VAD2":"    ",
         pInfo->params.modes.bitrate,pFileName,spSeconds,ENCspeed_in_mhz,DECspeed_in_mhz,1);
      CloseCSVFile(f_csv);
   }
   return 0;
}

int OutputSysInfo2CSVFile(FILE *f_csv)
{
   char infoBuffer[256];
   const  IppLibraryVersion *verIppSC, *verIppSP;

   if(f_csv == NULL) return -1;

   infoBuffer[0] = '\0';
   vm_sys_info_get_date((char*)infoBuffer,DDMMYY);
   vm_string_fprintf(f_csv,VM_STRING("Date,%s"),infoBuffer);

   infoBuffer[0] = '\0';
   vm_sys_info_get_time((char*)infoBuffer,HHMMSS);
   vm_string_fprintf(f_csv,VM_STRING(",Time,%s\n"),infoBuffer);

   infoBuffer[0] = '\0';
   vm_sys_info_get_cpu_name((char*)infoBuffer);
   vm_string_fprintf(f_csv,VM_STRING("CPU,%s\n"),infoBuffer);

   infoBuffer[0] = '\0';
   vm_sys_info_get_os_name((char*)infoBuffer);
   vm_string_fprintf(f_csv,VM_STRING("OS,%s\n"),infoBuffer);

   infoBuffer[0] = '\0';
   vm_sys_info_get_computer_name((char*)infoBuffer);
   vm_string_fprintf(f_csv,VM_STRING("Computer name,%s\n"),infoBuffer);

   verIppSC = ippscGetLibVersion();
   verIppSP = ippsGetLibVersion();

   fprintf(f_csv,"IPPS library,%s,%s,%s\n",
      verIppSP->Name,verIppSP->Version,verIppSP->BuildDate);

   fprintf(f_csv,"IPPSC library,%s,%s,%s\n",
      verIppSC->Name,verIppSC->Version,verIppSC->BuildDate);
   return 0;
}

void AddCodecTableHeader2CSVFile(FILE *f_csv)
{
   if(f_csv==NULL) return;
   fprintf(f_csv,"Codec,Voice Activity Detection,Bitrate(bps),Audio file,Duration(sec),Encode(MHz),Decode(MHz),nChannels\n");
   return;
}
#define MAX_LINE_LEN 1024

void USC_OutputString(FILE *flog, char *format,...)
{
   char  line[MAX_LINE_LEN];
   va_list  args;

   va_start(args, format);
   vsprintf(line, format, args);
   va_end(args);

   if(flog) {
      fprintf(flog,"%s", line);
   } else {
      printf("%s", line);
   }

   return;
}
