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
// Purpose: Auxiliary functions header file.
//
////////////////////////////////////////////////////////////////////////*/
#ifndef __UTIL_H__
#define __UTIL_H__

#include "usc_ec.h"
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_FILENAME_LEN      256
#define MAX_LEN_STRING       1024

typedef struct _USC_EC_Params {
   USC_EC_Fxns        *pUSC_EC_Fxns;
   USC_EC_Info         pInfo;
   int                 nChannels;
   int                 nBanks;
   USC_MemBank        *pBanks;
   USC_Handle          objEC;
}USC_EC_Params;

typedef struct _CommandLineParams {
   char rinFileName[MAX_FILENAME_LEN];
   char sinFileName[MAX_FILENAME_LEN];
   char soutFileName[MAX_FILENAME_LEN];
   char logFileName[MAX_FILENAME_LEN];
   char csvFileName[MAX_FILENAME_LEN];
   char ECName[MAX_FILENAME_LEN];
   int  freq;
   int  alg;
   int  adapt;
   int  tail;
   int  fdelay;
   int  ah_mode;
   int  hd_mode;
   int  hd_period;
   int  freq_shift;
   int  nRepeat;
   int  puttocsv;
   int  puttolog;
   int  enumerate;
   int  printSysInfo;
}CommandLineParams;


void SetCommandLineByDefault(CommandLineParams *params);
int OutputInfoString(int flConsole, FILE *fptrLog, const char *string);
int ReadCommandLine(CommandLineParams *params, int argc, char *argv[]);
void PrintUsage(const char * pAppName, FILE *fptrLog);
int sysInfoToCSV(FILE *f_csv);
#ifdef __cplusplus
}
#endif
#endif/* __UTIL_H__*/
