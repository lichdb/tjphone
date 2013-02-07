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
// Purpose: G711 speech codec: encoder API.
//
*/

#include <stdio.h>
#include <stdlib.h>
#include <ipp_w7.h>
#include <ippsc.h>
#include <ipps.h>
#include "owng711.h"
#include "g729api.h"

static int EncoderObjSize(void) {
    int objSize=sizeof(G711Encoder_Obj);
    int g729_size;
    apiG729Encoder_Alloc(G729A_CODEC, &g729_size);
    objSize += g729_size;
    return objSize;
}

G711_CODECFUN( APIG711_Status, apiG711Codec_ScratchMemoryAlloc,(int *pCodecSize)) {
    if (NULL==pCodecSize)
        return APIG711_StsBadArgErr;
#if !defined (NO_SCRATCH_MEMORY_USED)
    *pCodecSize = G711_ENCODER_SCRATCH_MEMORY_SIZE;
#endif
    return APIG711_StsNoErr;
}

G711_CODECFUN( APIG711_Status, apiG711Encoder_Alloc,
               (int *pCodecSize)) {
   if (NULL==pCodecSize)
      return APIG711_StsBadArgErr;
   *pCodecSize = EncoderObjSize();
   return APIG711_StsNoErr;
}

G711_CODECFUN( APIG711_Status, apiG711Encoder_Init,
               (G711Encoder_Obj* encoderObj, G711Codec_Type codecType, G711Encode_Mode mode)) {
    if (NULL==encoderObj)
        return APIG711_StsBadArgErr;

    if ((codecType != G711_ALAW_CODEC)&&(codecType != G711_MULAW_CODEC)) {
        return APIG711_StsBadCodecType;
    }
    if (G711Encode_VAD_Enabled != mode && G711Encode_VAD_Disabled != mode) {
        return APIG711_StsBadArgErr;
    }

    ippsZero_16s((short*)encoderObj, sizeof(*encoderObj)>>1);

    encoderObj->objPrm.objSize = EncoderObjSize();
    encoderObj->objPrm.mode = mode;
    encoderObj->objPrm.key = ENC_KEY;
    encoderObj->objPrm.codecType=codecType;

    encoderObj->codecType = codecType;

    encoderObj->g729_obj = (char*)encoderObj + sizeof(G711Encoder_Obj);

    apiG729Encoder_Init(encoderObj->g729_obj, G729A_CODEC, G729Encode_VAD_Enabled);

    return APIG711_StsNoErr;
}

G711_CODECFUN( APIG711_Status, apiG711Encoder_Mode,
               (G711Encoder_Obj* encoderObj, G711Encode_Mode mode))
{
   if (NULL==encoderObj)
        return APIG711_StsBadArgErr;
   encoderObj->objPrm.mode = mode;
   return APIG711_StsNoErr;
}
G711_CODECFUN( APIG711_Status, apiG711Encoder_InitBuff,
               (G711Encoder_Obj* encoderObj, char *buff)) {
    if (NULL==encoderObj || NULL==buff)
        return APIG711_StsBadArgErr;
#if !defined (NO_SCRATCH_MEMORY_USED)
    encoderObj->Mem.base = buff;
    encoderObj->Mem.CurPtr = encoderObj->Mem.base;
    encoderObj->Mem.VecPtr = (int *)(encoderObj->Mem.base+G711_ENCODER_SCRATCH_MEMORY_SIZE);
#endif
    return APIG711_StsNoErr;
}

G711_CODECFUN( APIG711_Status, apiG711Encode,
               (G711Encoder_Obj* encoderObj, const short *src, unsigned char* dst, int *frametype)) {
    short *inputHistory = encoderObj->inputHistory;
    short anau[4];

    if (encoderObj->objPrm.mode == G711Encode_VAD_Enabled) {
        APIG729_Status status;
        status = apiG729EncodeVAD(encoderObj->g729_obj,src,anau,G729A_CODEC,frametype);
        if (status != APIG729_StsNoErr) {
            return APIG711_StsErr;
        }
        if (*frametype == G711_Voice_Frame) {
            if (encoderObj->codecType == G711_ALAW_CODEC) {
                ippsLinToALaw_16s8u(inputHistory, dst, LP_SUBFRAME_DIM);
                ippsLinToALaw_16s8u(src, dst + LP_SUBFRAME_DIM, LP_SUBFRAME_DIM);
            } else { /* G711_MULAW_CODEC */
                ippsLinToMuLaw_16s8u(inputHistory, dst, LP_SUBFRAME_DIM);
                ippsLinToMuLaw_16s8u(src, dst + LP_SUBFRAME_DIM, LP_SUBFRAME_DIM);
            }
        } else {
            dst[0] = (unsigned char)(((anau[0] & 1) << 7) | ((anau[1] & 31) << 2) | ((anau[2] & 15)>>2));
            dst[1] = (unsigned char)(((anau[2] & 3) << 6) | ((anau[3] & 31) << 1));
        }
        ippsCopy_16s(src + LP_SUBFRAME_DIM, inputHistory, LP_SUBFRAME_DIM);
    } else {
        *frametype = G711_Voice_Frame;
        if (encoderObj->codecType == G711_ALAW_CODEC) {
            ippsLinToALaw_16s8u(src, dst, LP_FRAME_DIM);
        } else { /* G711_MULAW_CODEC */
            ippsLinToMuLaw_16s8u(src, dst, LP_FRAME_DIM);
        }
    }
    return APIG711_StsNoErr;
}
