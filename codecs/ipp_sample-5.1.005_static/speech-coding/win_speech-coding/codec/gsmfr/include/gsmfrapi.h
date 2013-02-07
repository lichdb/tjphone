/*/////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2004-2006 Intel Corporation. All Rights Reserved.
//
//     Intel(R) Integrated Performance Primitives
//     USC - Unified Speech Codec interface library
//
// By downloading and installing USC codec, you hereby agree that the
// accompanying Materials are being provided to you under the terms and
// conditions of the End User License Agreement for the Intel(R) Integrated
// Performance Primitives product previously accepted by you. Please refer
// to the file ipplic.htm located in the root directory of your Intel(R) IPP
// product installation for more information.
//
// A speech coding standards promoted by ITU, ETSI, 3GPP and other
// organizations. Implementations of these standards, or the standard enabled
// platforms may require licenses from various entities, including
// Intel Corporation.
//
//
// Purpose: GSM FR 06.10: API header file.
//
*/

#ifndef __GSMFR_API_H__
#define __GSMFR_API_H__
#include <ipp_w7.h>
#include "ippsc.h"
#include "gsmfr_types.h"
#ifdef __cplusplus  /*  GSM FR 06.10 */
extern "C" { /*  GSM FR 06.10 */
#endif /*  GSM FR 06.10 */

typedef enum{
    APIGSMFR_StsBadCodecType     =   -5,
    APIGSMFR_StsNotInitialized   =   -4,
    APIGSMFR_StsBadArgErr        =   -3,
    APIGSMFR_StsDeactivated      =   -2,
    APIGSMFR_StsErr              =   -1,
    APIGSMFR_StsNoErr            =    0
}APIGSMFR_Status;

struct _GSMFREncoder_Obj;
struct _GSMFRDecoder_Obj;
typedef struct _GSMFREncoder_Obj GSMFREncoder_Obj;
typedef struct _GSMFRDecoder_Obj GSMFRDecoder_Obj;

#if defined( _WIN32 ) || defined ( _WIN64 )
  #define __STDCALL  __stdcall
  #define __CDECL    __cdecl
#else
  #define __STDCALL
  #define __CDECL
#endif

#define   GSMFR_CODECAPI(type,name,arg)                extern type name arg;

/* /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Functions declarations
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////// */
GSMFR_CODECAPI( APIGSMFR_Status, apiGSMFREncoder_Alloc,
         ( int *pCodecSize))
GSMFR_CODECAPI( APIGSMFR_Status, apiGSMFRDecoder_Alloc,
         ( int *pCodecSize))
GSMFR_CODECAPI( APIGSMFR_Status, apiGSMFREncoder_Init,
         (GSMFREncoder_Obj* encoderObj,
         GSMFR_Params_t *params ))
GSMFR_CODECAPI( APIGSMFR_Status, apiGSMFRDecoder_Init,
         (GSMFRDecoder_Obj *decoderObj,
         GSMFR_Params_t *params ))
GSMFR_CODECAPI( APIGSMFR_Status, apiGSMFREncoder_Mode,
         (GSMFREncoder_Obj* encoderObj,
         GSMFR_Params_t *params ))
GSMFR_CODECAPI( APIGSMFR_Status, apiGSMFREncode,
         (GSMFREncoder_Obj *encoderObj,
         const short *src,
         unsigned char *dst,
         int *vad,
         int *hangover ))
GSMFR_CODECAPI( APIGSMFR_Status, apiGSMFRDecode,
         (GSMFRDecoder_Obj* decoderObj,
         const unsigned char *src,
         short *dst))
GSMFR_CODECAPI( APIGSMFR_Status, apiGSMFRDecoder_GetSize,
         (GSMFRDecoder_Obj* decoderObj, unsigned int *pCodecSize))
GSMFR_CODECAPI( APIGSMFR_Status, apiGSMFREncoder_GetSize,
         (GSMFREncoder_Obj* encoderObj, unsigned int *pCodecSize))
#ifdef __cplusplus
}
#endif

#endif /* __GSMFR_API_H__ */
