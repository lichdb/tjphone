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
// Purpose: G711 speech codec: API header file.
//
*/

#if !defined( __G711IAPI_H__ )
#define __G711IAPI_H__
#ifdef __cplusplus
extern "C" {
#endif

typedef enum _G711Codec_Type{
   G711_ALAW_CODEC=1,
   G711_MULAW_CODEC=2
}G711Codec_Type;

typedef enum _G711Encode_Mode{
   G711Encode_VAD_Enabled=1,
   G711Encode_VAD_Disabled=0
}G711Encode_Mode;

typedef enum _G711Decode_Mode{
   G711Decode_PLC_Enabled=1,
   G711Decode_PLC_Disabled=0
}G711Decode_Mode;

typedef enum{
    APIG711_StsBadCodecType     =   -5,
    APIG711_StsNotInitialized   =   -4,
    APIG711_StsBadArgErr        =   -3,
    APIG711_StsDeactivated      =   -2,
    APIG711_StsErr              =   -1,
    APIG711_StsNoErr            =    0
}APIG711_Status;

typedef enum{
    G711_Bad_Frame           = -1,
    G711_Voice_Frame         = 3,
    G711_SID_Frame           = 1,
    G711_Untransmitted_Frame = 0
}G711_FrameType;

struct _G711Encoder_Obj;
struct _G711Decoder_Obj;
typedef struct _G711Encoder_Obj G711Encoder_Obj;
typedef struct _G711Decoder_Obj G711Decoder_Obj;

#if defined( _WIN32 ) || defined ( _WIN64 )
  #define __STDCALL  __stdcall
  #define __CDECL    __cdecl
#else
  #define __STDCALL
  #define __CDECL
#endif

#define   G711_CODECAPI(type,name,arg)                extern type name arg;

/*
                   Functions declarations
*/
G711_CODECAPI( APIG711_Status, apiG711Encoder_Alloc,
         (int *pCodecSize))
G711_CODECAPI( APIG711_Status, apiG711Decoder_Alloc,
         (int *pCodecSize))
G711_CODECAPI( APIG711_Status, apiG711Codec_ScratchMemoryAlloc,
         (int *pCodecSize))
G711_CODECAPI( APIG711_Status, apiG711Encoder_Init,
         (G711Encoder_Obj* encoderObj, G711Codec_Type codecType, G711Encode_Mode mode))
G711_CODECAPI( APIG711_Status, apiG711Decoder_Init,
         (G711Decoder_Obj* decoderObj, G711Codec_Type codecType))
G711_CODECAPI( APIG711_Status, apiG711Encoder_InitBuff,
         (G711Encoder_Obj* encoderObj, char *buff))
G711_CODECAPI( APIG711_Status, apiG711Decoder_InitBuff,
         (G711Decoder_Obj* decoderObj, char *buff))
G711_CODECAPI( APIG711_Status, apiG711Encode,
         (G711Encoder_Obj* encoderObj,const short* src, unsigned char* dst, int *frametype))
G711_CODECAPI( APIG711_Status, apiG711Decode,
         (G711Decoder_Obj* decoderObj,const unsigned char* src, int frametype, short* dst))
G711_CODECAPI( APIG711_Status, apiG711Encoder_Mode,
               (G711Encoder_Obj* encoderObj, G711Encode_Mode mode))
G711_CODECAPI( APIG711_Status, apiG711Decoder_Mode,
               (G711Decoder_Obj* decoderObj, int mode))

#ifdef __cplusplus
}
#endif


#endif /* __CODECAPI_H__ */
