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
//     USC Echo Canceller sample
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
#include "util.h"
#include <ipp_w7.h>
#include "ipps.h"
#include "ippsc.h"
#include "vm_sys_info.h"

int OutputInfoString(int flConsole, FILE *fptrLog, const char *string)
{
   int count;

   if(fptrLog == NULL) count = printf("%s", string);
   else {
     if(flConsole == 0) count = printf("%s", string);
     count = fprintf(fptrLog, "%s", string);
   }
   return count;
}

void SetCommandLineByDefault(CommandLineParams *params)
{
   memset((void*)params,0,sizeof(CommandLineParams));
   strcpy(params->ECName, "IPP_EC");
   params->alg = 2;
   params->freq = 8000;
   params->tail = 16;
   params->ah_mode = 0;
   params->nRepeat = 1;
   params->puttolog = 0;
   params->puttocsv = 0;
   params->printSysInfo=0;
   params->hd_mode=1;
   params->hd_period=-1;
   params->freq_shift=0;
   params->adapt=1;
}
int ReadCommandLine(CommandLineParams *params, int argc, char *argv[])
{
   int usage = 0;


   argc--;
   while(argc-- > 0){
      argv++;
      if ('-' == (*argv)[0]) {
         if (!strncmp("-f",*argv,2)){
            if(argc-->0){
               int f = atoi(*++argv);
               if (f == 8)
                  params->freq = 8000;
               else if (f == 16) {
                  params->freq = 16000;
               } else {
                  OutputInfoString(1, NULL,"The sample frequency must be 8 or 16 kHz\n");
                  usage = 1;
               }
            }
            continue;
         } else if(!strcmp("-type",*argv)) {
            if(argc-->0){
              argv ++;
              strcpy(params->ECName, *argv);
            }
            continue;
         } else if(!strcmp("-a",*argv)) {
            if(argc-->0){
                argv ++;
                if (!strcmp("fb",*argv) || !strcmp("fullband",*argv)) {
                    params->alg = 1;
                } else if (!strcmp("sb",*argv) || !strcmp("subband",*argv)) {
                    params->alg = 2;
                } else if (!strcmp("sbf",*argv) || !strcmp("subbandfast",*argv)) {
                    params->alg = 3;
                } else {
                  OutputInfoString(1, NULL,"The algorithm specifier must be fb (fullband), sb (subband) or sbf (subband fast)\n");
                  usage = 1;
               }
            }
            continue;
         } else if(!strcmp("-ad",*argv)) {
             if(argc-->0){
                 argv ++;
                 if (!strcmp("full",*argv)) {
                     params->adapt = 1;
                 } else if (!strcmp("lite",*argv)) {
                     params->adapt = 2;
                 } else {
                     OutputInfoString(1, NULL,"The adaptation specifier must be full or lite.\n");
                     usage = 1;
                 }
             }
             continue;
         }else if(!strcmp("-l",*argv)) {/* set the echo tail length */
            if(argc-->0){
               int eplen = atoi(*++argv);
               if (eplen >= 1 && eplen <= 200)
                  params->tail = eplen;
               else {
                  OutputInfoString(1, NULL,"The echo tail length must be in the range [1, 200] ms for subband algorithm, [1, 16] for fullband algorithm\n");
                  usage = 1;
               }
            }
            continue;
         }else if (!strcmp(*argv,"-sys")){/* set the option to print the system information */
            params->printSysInfo=1;
            continue;
         }else if (!strcmp(*argv,"-delay")){
             if(argc-->0){
                 params->fdelay = atoi(*++argv);
                 if(params->fdelay < 0) params->fdelay=0;
             }
         }else if (!strcmp(*argv,"-ah")){/* set the anti-howling device on and set the howling detector device mode*/
             if(argc-->0){
                 int hdmode = atoi(*++argv);
                 if(hdmode==1)
                     params->ah_mode=1;
                 else
                     if(hdmode==2)
                         params->ah_mode=2;
                     else{
                         OutputInfoString(1, NULL,"The anti-howler mode must be set to 1 or 2\n");
                         usage = 1;
                     }
             }
             continue;
         }else if (!strcmp(*argv,"-sh")){/* Set the frequency shift number.
                                         Set the anti-howling device on.*/
             if(argc-->0){
                 params->freq_shift = atoi(*++argv);
             }
             continue;
         }else if (!strcmp(*argv,"-hp")){/* set the number of
                                         frames to be filtered after the howling has
                                         been detected.*/
             if(argc-->0){
                 params->hd_period = atoi(*++argv);
             }
             continue;
         }else if (!strcmp(*argv,"-s")){/* set the number of repeats */
            if(argc-->0){
               int rep = atoi(*++argv);
               if(0 == rep) rep=1;
               if(rep > params->nRepeat) params->nRepeat=rep;
            }
            continue;
         }else if(!strcmp("-o",*argv)) {/* the log file is specified */
            if(argc-->0){
              argv++;
              strcpy(params->logFileName, *argv);
              params->puttolog = 1;
            }
            continue;
         }else if (!strcmp(*argv,"-list")){ /* Print out the modes list */
            params->enumerate = 1;
            usage=1;
            return usage;
         } else if (!strcmp(*argv,"-c")){ /* the "csv" output is specified */
             if(argc-->0){
                 params->puttocsv=1;
                 argv++;
                 strcpy(params->csvFileName, *argv);
             }else{
                 OutputInfoString(1, NULL,"Unknown csv file name.\n");
                 usage=1;
             }
             continue;
         }
      }else{
         argc++;
         argv--;
         break;
      }
   }


   if ((params->alg == 1) && (params->tail > 16)) {
       OutputInfoString(1, NULL,"The echo tail length for fullband algorithm must be in the range of [1, 16] ms\n");
       usage = 1;
   }
   if(argc-->0){
      argv++;
      strcpy(params->rinFileName, *argv);
   }else{
      OutputInfoString(1, NULL,"Unknown r-in input file.\n");
      usage=1;
   }
   if(argc-->0){
      argv++;
      strcpy(params->sinFileName, *argv);
   }else{
      OutputInfoString(1, NULL,"Unknown s-in input file.\n");
      usage=1;
   }
   if(argc-->0){
      argv++;
      strcpy(params->soutFileName, *argv);
   }else{
      OutputInfoString(1, NULL,"Unknown output file.\n");
      usage=1;
   }

   return usage;
}

void PrintUsage(const char * pAppName, FILE *fptrLog)
{
   char pString[MAX_LEN_STRING];

   sprintf(pString, "Usage: %s [options] <receive-in 16-bit PCM file> <send-in 16-bit PCM file> <send-out 16-bit PCM file>\n", pAppName);
   OutputInfoString(1, fptrLog, (const char*)pString);
   OutputInfoString(1, fptrLog,"   [-type EC type ] Set the type of Echo Canceller [EC_INT or EC_FP].\n");
   OutputInfoString(1, fptrLog,"   [-f <frequency>] Set the sampling frequency, kHz: 8 or 16. Default 8.\n");
   OutputInfoString(1, fptrLog,"   [-a <algorithm>] Algorithm specifier: fb (fullband), sb (subband) or sbf (subband fast). \n");
   OutputInfoString(1, fptrLog,"                    Default subband.\n");
   OutputInfoString(1, fptrLog,"   [-ad <algorithm>] Adaptation specifier: full or lite. Default full.\n");
   OutputInfoString(1, fptrLog,"   [-hp <num>]      Set the number of frames to be filtered after the howling has been detected.\n");
   OutputInfoString(1, fptrLog,"   [-ah <mode>]     Set the anti-howling device on. Mode=[1,2] defines the howling detector\n");
   OutputInfoString(1, fptrLog,"                    device algorithm. 1-spectra-based,2- energy-based.\n");
   OutputInfoString(1, fptrLog,"   [-sh <num>]      Set the frequency shift number.\n");
   OutputInfoString(1, fptrLog,"   [-l <num>]       Set the echo tail length, ms: in the range [1, 200] for subband algorithm.\n");
   OutputInfoString(1, fptrLog,"                    [1, 16] for fullband algorithm. Default 16.\n");
   OutputInfoString(1, fptrLog,"   [-delay <val>]   Set the delay in receive-in PCM file,  integer number of milliseconds. \n");
   OutputInfoString(1, fptrLog,"                    Leading end of receive-in PCM will be cut off prior to echo cancel. Default:no delay.\n");
   OutputInfoString(1, fptrLog,"   [-s <num>]       Set the number of times file to be processed (optional).\n");
   OutputInfoString(1, fptrLog,"   [-c <csv filename>] Set the csv file name for the performance info to be print to (optional).\n");
   OutputInfoString(1, fptrLog,"   [-o <log filename>] Set the log file name for the functional parameters to be print to (optional).\n");
   OutputInfoString(1, fptrLog,"   [-list] Print out the modes list(optional).\n");
   OutputInfoString(1, fptrLog,"   [-sys] Print out system information.\n");

   return;
}
int sysInfoToCSV(FILE *f_csv)
{
    char infoBuffer[256];
    const  IppLibraryVersion *verIppSC, *verIppSP;

    if(f_csv == NULL) return -1;

#ifndef OSX32
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
#endif

    verIppSC = ippscGetLibVersion();
    verIppSP = ippsGetLibVersion();

    fprintf(f_csv,"IPPS library,%s,%s,%s\n",
        verIppSP->Name,verIppSP->Version,verIppSP->BuildDate);

    fprintf(f_csv,"IPPSC library,%s,%s,%s\n",
        verIppSC->Name,verIppSC->Version,verIppSC->BuildDate);
    fprintf(f_csv,"EC TYPE,alg type,adaptation,bits,sampling rate,rin file,sin file,speech in sec,speed in mHz\n");
return 0;
}


