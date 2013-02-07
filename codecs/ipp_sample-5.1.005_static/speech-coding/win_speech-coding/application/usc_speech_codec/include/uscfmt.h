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
// Purpose: Format library API definition header files.
//
////////////////////////////////////////////////////////////////////////*/
#ifndef __USCFMT_H__
#define __USCFMT_H__

#ifdef USE_PLUGINS
   #if defined( _MSC_VER ) || defined( __ICL ) || defined ( __ECL )
      #define USCFMTINFO  __declspec(dllexport)
   #else
      #define USCFMTINFO  extern
   #endif
#else
   #define USCFMTINFO  extern
#endif /*USE_PLUGINS*/
#define USC_MAKE_FORMAT_DET_VERSION(major,minor) (((major&0xFF)<<8)|\
                                                         (minor&0xFF))

#define USC_MAKE_FORMAT_DET(vad,hpf,pf,truncate,law) (((law&0x3)<<5)|\
                                                         ((truncate&0x1)<<4)|\
                                                         ((pf&0x1)<<3)|\
                                                         ((hpf&0x1)<<2)|\
                                                         (vad&0x3))
#define GET_VAD_FROM_FORMAT_DET(fmtDet) (fmtDet&0x3)
#define GET_HPF_FROM_FORMAT_DET(fmtDet) ((fmtDet>>2)&0x1)
#define GET_PF_FROM_FORMAT_DET(fmtDet) ((fmtDet>>3)&0x1)
#define GET_TRUNC_FROM_FORMAT_DET(fmtDet) ((fmtDet>>4)&0x1)
#define GET_LAW_FROM_FORMAT_DET(fmtDet) ((fmtDet>>4)&0x3)

/* /////////////////////////////////////////////////////////////////////////////
//  Name:        GetFormatsInfoVersion
//  Purpose:     getting version of the format library.
//  Returns:     number of of avialable formats.
//  Parameters:
//
*/
USCFMTINFO int GetFormatsInfoVersion();

typedef struct {
   /* /////////////////////////////////////////////////////////////////////////////
   //  Name:        GetNumFormats
   //  Purpose:     getting number of of avialable formats.
   //  Returns:     number of of avialable formats.
   //  Parameters:
   //
   */
   int (*GetNumFormats)(void);

   /* /////////////////////////////////////////////////////////////////////////////
   //  Name:        GetNameByFormatTag
   //  Purpose:     getting codec name by format tag.
   //  Returns:     1 if success, 0 if fails.
   //  Parameters:
   //    lFormatTag        input format tag.
   //    pDstCodecName     pointer to the output codec name.
   */
   int (*GetNameByFormatTag)(int lFormatTag, char *pDstCodecName);

   /* /////////////////////////////////////////////////////////////////////////////
   //  Name:        GetFormatDetailsByFormatTag
   //  Purpose:     getting format details in USC_MAKE_FORMAT_DET form by format tag.
   //  Returns:     1 if success, 0 if fails.
   //  Parameters:
   //    lFormatTag        input format tag.
   //    pFormatDetails    pointer to the output format details in USC_MAKE_FORMAT_DET form.
   */
   int (*GetFormatDetailsByFormatTag)(int lFormatTag, int *pFormatDetails);

   /* /////////////////////////////////////////////////////////////////////////////
   //  Name:        GetFormatTagByNameAndByDetails
   //  Purpose:     getting format tag by codec name and format details in USC_MAKE_FORMAT_DET.
   //  Returns:     1 if success, 0 if fails.
   //  Parameters:
   //    pSrcCodecName     pointer to the input codec name.
   //    fmtDetails        format details in USC_MAKE_FORMAT_DET form.
   //    lFormatTag        pointer to the output format tag.
   */
   int (*GetFormatTagByNameAndByDetails)(const char *pSrcCodecName, int fmtDetails, int *lFormatTag);
} usc_fmt_info_Fxns;

#endif /*__USCFMT_H__*/
