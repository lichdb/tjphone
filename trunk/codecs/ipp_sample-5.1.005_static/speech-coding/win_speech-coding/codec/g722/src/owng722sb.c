/*/////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2005-2006 Intel Corporation. All Rights Reserved.
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
// Purpose: G.722 speech codec: auxiliaries.
//
*/

#include "owng722sb.h"


void UnpackCodeword(const char *pSrc, int len, short *pDst)
{
   int i, j;
   for(i = 0, j = 0; i < len; i++) {
     pDst[j++] = (short)((char)pSrc[i] & 0x3F); /* 6 LSB */
     pDst[j++] = (short)(((char)pSrc[i] >> 6) & 0x03);/* 2 MSB */
   }
}

void PackCodeword(const short *pSrc, int len, char *pDst)
{
    int i, j;
    for(i=0,j=0; i<len; i+=2) pDst[j++] = (char)(((pSrc[i+1] << 6) + pSrc[i]) & 0xFF);
}




