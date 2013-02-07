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
// Purpose: Plug-ins operation functions.
//
////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usc_cdcs.h"
#ifdef USE_PLUGINS
   #include "vm_shared_object.h"
   #include <io.h>
#endif /*USE_PLUGINS*/
#include "usc.h"
#include "util.h"
#include "loadcodec.h"
#include "uscfmt.h"
#ifndef USE_PLUGINS
extern usc_fmt_info_Fxns USC_FORMAT_INFO_FXNS;
#endif

#define MAXLENSTR       256

#ifdef USE_PLUGINS
   #if defined (WIN32) || defined (WINCE)
      #define DLL_FIND_BY_EXTENSION_MASK "*.dll"
   #elif defined (ARM) /* LINUX32 ARM, LINUX32*/
      #define DLL_FIND_BY_EXTENSION_MASK "*.so"
   #endif
#endif /*USE_PLUGINS*/

typedef struct _StaticCodecs {
   char codecName[USC_MAX_NAME_LEN];
   void *funcPtr;
}StaticCodecs;

StaticCodecs LinkedCodecs[] = {
#if (defined _USC_G723 || defined _USC_ALL)
   "IPP_G723.1",&USC_G723_Fxns,
#endif
#if (defined _USC_G726 || defined _USC_ALL)
   "IPP_G726",&USC_G726_Fxns,
#endif
#if (defined _USC_AMRWB || defined _USC_ALL)
   "IPP_AMRWB",&USC_AMRWB_Fxns,
#endif
#if (defined _USC_GSMAMR || defined _USC_ALL)
   "IPP_GSMAMR",&USC_GSMAMR_Fxns,
#endif
#if (defined _USC_GSMFR || defined _USC_ALL)
   "IPP_GSMFR",&USC_GSMFR_Fxns,
#endif
#if (defined _USC_G728 || defined _USC_ALL)
   "IPP_G728",&USC_G728_Fxns,
#endif
#if (defined _USC_G729 || defined _USC_ALL)
   "IPP_G729I",&USC_G729I_Fxns,
   "IPP_G729A",&USC_G729A_Fxns,
#endif
#if (defined _USC_G722 || defined _USC_ALL)
   "IPP_G722.1",&USC_G722_Fxns,
#endif
#if (defined _USC_G729FP || defined _USC_ALL)
   "IPP_G729A_FP",&USC_G729AFP_Fxns,
   "IPP_G729I_FP",&USC_G729IFP_Fxns,
#endif
#if (defined _USC_G722SB || defined _USC_ALL)
   "IPP_G722",&USC_G722SB_Fxns,
#endif
#if (defined _USC_G711 || defined _USC_ALL)
   "IPP_G711A",&USC_G711A_Fxns,
   "IPP_G711U",&USC_G711U_Fxns,
#endif
   "END",NULL
};

/* /////////////////////////////////////////////////////////////////////////////
//  Name:        EnumerateStaticLinkedCodecs
//  Purpose:     Enumerate all static linked codecs by name.
//  Returns:     None.
//  Parameters:
*/
void EnumerateStaticLinkedCodecs(FILE *f_log)
{
   int i = 0;
   printf("Following codecs are linked:\n");
   while(strcmp(LinkedCodecs[i].codecName,"END")!=0) {
      USC_OutputString(f_log, "           %s\n",LinkedCodecs[i].codecName);
      i++;
   }

   return;
}

void *TryToFindStatic(const char *nameCodec)
{
   void *pAddr = NULL;
   int i=0;
   while(strcmp(LinkedCodecs[i].codecName,"END")!=0) {
      if(strcmp(nameCodec, LinkedCodecs[i].codecName) == 0) {
         pAddr = LinkedCodecs[i].funcPtr;
      }
      i++;
   }
   return pAddr;
}

int GetNumLinkedCodecs()
{
   int i = 0;
   while(strcmp(LinkedCodecs[i].codecName,"END")!=0) {
      i++;
   }

   return i;
}

void *Get_ith_StaticLinkedCodecs(int index, char *nameCodec)
{
   strcpy(nameCodec,LinkedCodecs[index].codecName);
   return LinkedCodecs[index].funcPtr;
}
#ifdef USE_PLUGINS
void CodecName2TableName(const char *nameCodec, char *codecTblName)
{
   strcpy(codecTblName, "USC_");
   strcat(codecTblName, nameCodec);
   strcat(codecTblName, "_Fxns");
}
#endif /*USE_PLUGINS*/

/* /////////////////////////////////////////////////////////////////////////////
//  Name:        LoadUSCCodecByName
//  Purpose:     Load USC codec function table by name.
//  Returns:     0 if success, -1 if fails.
//  Parameters:
//     codec     pointer to the input/output codec params structure
*/
int LoadUSCCodecByName(LoadedCodec *codec,FILE *f_log)
{
#ifdef USE_PLUGINS
   char codecTblName[MAXLENSTR];
   vm_finddata_t fileCtx;
   long searchFileHandle;

   codec->handleCodecDLL=NULL;
#endif /*USE_PLUGINS*/
   codec->uscParams.USC_Fns = NULL;

   codec->uscParams.USC_Fns = (USC_Fxns*)TryToFindStatic(codec->codecName);
   if(codec->uscParams.USC_Fns) {
      USC_OutputString(f_log, " Staticly loaded codec: %s\n",codec->codecName);
      return 0;
   }
#ifdef USE_PLUGINS
   if( (searchFileHandle = vm_string_findfirst( DLL_FIND_BY_EXTENSION_MASK, &fileCtx )) == -1L )
       printf("No DLL files in current directory!\n");
   else {
      CodecName2TableName(codec->codecName, codecTblName);
      codec->handleCodecDLL = vm_so_load(fileCtx.name);

      if(codec->handleCodecDLL) {
         codec->uscParams.USC_Fns = (USC_Fxns*)vm_so_get_addr(codec->handleCodecDLL, codecTblName);
         if(codec->uscParams.USC_Fns) {
            printf(" Loaded %s library with codec %s\n",fileCtx.name,codec->codecName);
            return 0;
         }
         vm_so_free(codec->handleCodecDLL);
      }
      while( vm_string_findnext( searchFileHandle, &fileCtx ) == 0 ) {
         codec->handleCodecDLL=NULL;
         codec->handleCodecDLL = vm_so_load(fileCtx.name);
         if(codec->handleCodecDLL) {
            codec->uscParams.USC_Fns = (USC_Fxns*)vm_so_get_addr(codec->handleCodecDLL, codecTblName);
            if(codec->uscParams.USC_Fns) {
               printf(" Loaded %s library with codec %s\n",fileCtx.name,codec->codecName);
               return 0;
            }
            vm_so_free(codec->handleCodecDLL);
         }
      }
      _findclose(searchFileHandle);
   }
   printf(" Cannot find %s codec\n",codec->codecName);
#endif /*USE_PLUGINS*/
   return -1;
}
#ifndef USE_PLUGINS
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        GetUSCCodecParamsByFormat
//  Purpose:     getting codec format description from the format library by name or by format tags.
//  Returns:     0 if success, -1 if fails.
//  Parameters:
//     codec       pointer to the input/output codec params structure
//     lQuery      Flag of search mode
//                 BY_FORMATTAG - search by format tag
//                 BY_NAME      - search by codec name
*/
int GetUSCCodecParamsByFormat(LoadedCodec *codec, int lQuery,FILE *f_log)
{
   int CallResult;
   int version;

   version = GetFormatsInfoVersion();
   version = (version>>8)&0xFF;
   /* Check version of the format library. We estimate that ig greater or equal than 1.0*/
   if(version >= 1) {
      if(lQuery&BY_FORMATTAG) {
         CallResult = USC_FORMAT_INFO_FXNS.GetNameByFormatTag(codec->lFormatTag, (char *)codec->codecName);
         if(CallResult==1) {
            USC_FORMAT_INFO_FXNS.GetFormatDetailsByFormatTag(codec->lFormatTag, &codec->lIsVad);
            codec->lIsVad = GET_VAD_FROM_FORMAT_DET(codec->lIsVad);
            USC_OutputString(f_log, " Loaded %s format description\n", codec->codecName);
            return 0;
         }
      } else if(lQuery&BY_NAME) {
         int det = USC_MAKE_FORMAT_DET(codec->lIsVad,codec->uscParams.pInfo->params.modes.hpf,
                     codec->uscParams.pInfo->params.modes.pf,codec->uscParams.pInfo->params.modes.truncate,0);
         CallResult = USC_FORMAT_INFO_FXNS.GetFormatTagByNameAndByDetails(codec->codecName, det, &codec->lFormatTag);
         if(CallResult==1) {
            USC_OutputString(f_log, " Loaded %s format description\n", codec->codecName);
            return 0;
         }
      }
   }
   return -1;
}
#else
typedef int (*FormatVersion)(void);
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        GetUSCCodecParamsByFormat
//  Purpose:     getting codec format description from the format library by name or by format tags.
//  Returns:     0 if success, -1 if fails.
//  Parameters:
//     codec       pointer to the input/output codec params structure
//     lQuery      Flag of search mode
//                 BY_FORMATTAG - search by format tag
//                 BY_NAME      - search by codec name
*/
int GetUSCCodecParamsByFormat(LoadedCodec *codec, int lQuery,FILE *f_log)
{
   FormatVersion pFormatVer = NULL;
   void  *pFormatFnxs = NULL;
   vm_finddata_t fileCtx;
   long searchFileHandle;
   int CallResult;

   usc_fmt_info_Fxns *fmtFxns;

   codec->handleFormatDLL=NULL;

   if( (searchFileHandle = vm_string_findfirst( DLL_FIND_BY_EXTENSION_MASK, &fileCtx )) == -1L )
       printf("No DLL files in current directory!\n");
   else {
      codec->handleFormatDLL = vm_so_load(fileCtx.name);

      if(codec->handleFormatDLL) {
         pFormatVer = (void*)vm_so_get_addr(codec->handleFormatDLL, "GetFormatsInfoVersion");
         if(pFormatVer) {
            int version;
            version = pFormatVer();
            version = (version>>8)&0xFF;
            if(version >= 1) {/*Its supported*/
               pFormatFnxs = (void*)vm_so_get_addr(codec->handleFormatDLL, "USC_FORMAT_INFO_FXNS");
               if(pFormatFnxs) {
                  fmtFxns = (usc_fmt_info_Fxns *)pFormatFnxs;
                  if(lQuery&BY_FORMATTAG) {
                     CallResult = fmtFxns->GetNameByFormatTag(codec->lFormatTag, codec->codecName);
                     if(CallResult==1) {
                        fmtFxns->GetFormatDetailsByFormatTag(codec->lFormatTag, &codec->lIsVad);
                        codec->lIsVad = GET_VAD_FROM_FORMAT_DET(codec->lIsVad);
                        printf(" Loaded %s format description from %s\n",
                           codec->codecName, fileCtx.name);
                        return 0;
                     }
                  } else if(lQuery&BY_NAME) {
                     int det = USC_MAKE_FORMAT_DET(codec->lIsVad,codec->uscParams.pInfo.params.modes.hpf,
                        codec->uscParams.pInfo.params.modes.pf,codec->uscParams.pInfo.params.modes.truncate,
                        0);
                     CallResult = fmtFxns->GetFormatTagByNameAndByDetails(codec->codecName, det,
                        &codec->lFormatTag);
                     if(CallResult==1) {
                        printf(" Loaded %s format description from %s\n",
                           codec->codecName, fileCtx.name);
                        return 0;
                     }
                  }
               }
            }
         }
         vm_so_free(codec->handleFormatDLL);
      }
      while( vm_string_findnext( searchFileHandle, &fileCtx ) == 0 ) {
         codec->handleFormatDLL=NULL;
         codec->handleFormatDLL = vm_so_load(fileCtx.name);
         if(codec->handleFormatDLL) {
            pFormatVer = (void*)vm_so_get_addr(codec->handleFormatDLL, "USC_FORMATS");
            if(pFormatVer) {
            int version;
            version = pFormatVer();
            version = (version>>8)&0x8;
            if(version >= 1) {/*Its supported*/
               pFormatFnxs = (void*)vm_so_get_addr(codec->handleFormatDLL, "USC_FORMAT_INFO_FXNS");
               if(pFormatFnxs) {
                  fmtFxns = (usc_fmt_info_Fxns *)pFormatFnxs;
                  if(lQuery&BY_FORMATTAG) {
                     CallResult = fmtFxns->GetNameByFormatTag(codec->lFormatTag, codec->codecName);
                     if(CallResult==1) {
                        fmtFxns->GetFormatDetailsByFormatTag(codec->lFormatTag, &codec->lIsVad);
                        codec->lIsVad = GET_VAD_FROM_FORMAT_DET(codec->lIsVad);
                        printf(" Loaded %s format description from %s\n",
                           codec->codecName, fileCtx.name);
                        return 0;
                     }
                  } else if(lQuery&BY_NAME) {
                     int det = USC_MAKE_FORMAT_DET(codec->lIsVad,codec->uscParams.pInfo.params.modes.hpf,
                        codec->uscParams.pInfo.params.modes.pf,codec->uscParams.pInfo.params.modes.truncate,
                        0);
                     CallResult = fmtFxns->GetFormatTagByNameAndByDetails(codec->codecName, det, &codec->lFormatTag);
                     if(CallResult==1) {
                        printf(" Loaded %s format description from %s\n",
                           codec->codecName, fileCtx.name);
                        return 0;
                     }
                  }
               }
            }
         }
            vm_so_free(codec->handleFormatDLL);
         }
      }
      _findclose(searchFileHandle);
   }
   printf(" Cannot find format description for %s codec\n",codec->codecName);
   return -1;
}
#endif /*USE_PLUGINS*/
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        FreeUSCSharedObjects
//  Purpose:     Release all USC shared objects.
//  Returns:     None.
//  Parameters:
//     codec     pointer to the input/output codec params structure
*/
void FreeUSCSharedObjects(LoadedCodec *codec)
{
#ifdef USE_PLUGINS
   vm_so_free(codec->handleCodecDLL);
   codec->handleCodecDLL = NULL;
   vm_so_free(codec->handleFormatDLL);
   codec->handleFormatDLL = NULL;
#else
   codec = codec;/*Just to avoid varning*/
#endif /*USE_PLUGINS*/
   return;
}
