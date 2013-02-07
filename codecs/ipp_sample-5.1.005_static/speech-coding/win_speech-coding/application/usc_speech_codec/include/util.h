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
// Purpose: Auxiliary functions header file.
//
////////////////////////////////////////////////////////////////////////*/
#ifndef __UTIL_H__
#define __UTIL_H__

#define LINIAR_PCM        1
#define ALAW_PCM          6
#define MULAW_PCM         7
typedef struct _USCCodec {
   USC_MemBank*        pBanks;
   USC_Handle          hUSCCodec;
}USCCodec;
#define MAX_CHANNELS      2 /*Stereo file*/
#define MAX_FILENAME_LEN      256
typedef struct _USCParams {
   int                 nChannels;
   USC_CodecInfo       *pInfo;
   USC_Fxns            *USC_Fns;
   int                 nbanks;
   USCCodec            uCodec[MAX_CHANNELS];
}USCParams;

typedef struct _CommandLineParams {
   char inputFileName[MAX_FILENAME_LEN];
   char outputFileName[MAX_FILENAME_LEN];
   char logFileName[MAX_FILENAME_LEN];
   char csvFileName[MAX_FILENAME_LEN];
   char codecName[MAX_FILENAME_LEN];
   char TimingCodecName[MAX_FILENAME_LEN];
   int  bitrate;
   int  Vad;
   int  enumerate;
   int  optionReport;
   int  puttocsv;
   int  puttologfile;
   int  timing;
   int  nRepeat;
   int  nOutputPCMType;
}CommandLineParams;

void ReadCommandLine(CommandLineParams *params, int argc, char *argv[]);
void SetCommandLineByDefault(CommandLineParams *params);
void PrintUsage();
FILE *OpenCSVFile(char *csvFileName);
void CloseCSVFile(FILE *f_csv);
int OutputString2CSVFile(CommandLineParams *params, USC_CodecInfo *pInfo, float spSeconds,float speed_in_mhz);
int OutputDuplexString2CSVFile(CommandLineParams *params, USC_CodecInfo *pInfo, float spSeconds,
                               float ENCspeed_in_mhz, float DECspeed_in_mhz);
int OutputSysInfo2CSVFile(FILE *f_csv);
void AddCodecTableHeader2CSVFile(FILE *f_csv);
int Timing();
void USC_OutputString(FILE *flog, char *format,...);
#endif/* __UTIL_H__*/
