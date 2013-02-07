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
// Purpose: Plug-ins operation header file.
//
////////////////////////////////////////////////////////////////////////*/
#ifndef __LOADCODEC_H__
#define __LOADCODEC_H__

#ifndef USC_MAX_NAME_LEN
   #define USC_MAX_NAME_LEN 64
#endif

typedef struct _LoadedCodec {
   const char codecName[USC_MAX_NAME_LEN];
   int lFormatTag;
   int lIsVad;
#ifdef USE_PLUGINS
   vm_so_handle handleCodecDLL;
   vm_so_handle handleFormatDLL;
#endif /*USE_PLUGINS*/
   USCParams uscParams;
} LoadedCodec;
#define BY_FORMATTAG 0x1
#define BY_NAME 0x2

/* /////////////////////////////////////////////////////////////////////////////
//  Name:        EnumerateStaticLinkedCodecs
//  Purpose:     Enumerate all static linked codecs by name.
//  Returns:     None.
//  Parameters:
//     f_log       Pointer to the open logfile. If f_log=NULL output will be to the stdout.
*/
void EnumerateStaticLinkedCodecs(FILE *f_log);
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        LoadUSCCodecByName
//  Purpose:     Load USC codec function table by name.
//  Returns:     0 if success, -1 if fails.
//  Parameters:
//     codec     pointer to the input/output codec params structure
//     f_log       Pointer to the open logfile. If f_log=NULL output will be to the stdout.
*/
int LoadUSCCodecByName(LoadedCodec *codec, FILE *f_log);
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        GetUSCCodecParamsByFormat
//  Purpose:     getting codec format description from the format library by name or by format tags.
//  Returns:     0 if success, -1 if fails.
//  Parameters:
//     codec       pointer to the input/output codec params structure
//     lQuery      Flag of search mode
//                 BY_FORMATTAG - search by format tag
//                 BY_NAME      - search by codec name
//     f_log       Pointer to the open logfile. If f_log=NULL output will be to the stdout.
*/
int GetUSCCodecParamsByFormat(LoadedCodec *codec,int lQuery, FILE *f_log);
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        FreeUSCSharedObjects
//  Purpose:     Release all USC shared objects.
//  Returns:     None.
//  Parameters:
//     codec     pointer to the input/output codec params structure
*/
void FreeUSCSharedObjects(LoadedCodec *codec);

int GetNumLinkedCodecs();
void *Get_ith_StaticLinkedCodecs(int index, char *nameCodec);

#endif/* __LOADCODEC_H__*/
