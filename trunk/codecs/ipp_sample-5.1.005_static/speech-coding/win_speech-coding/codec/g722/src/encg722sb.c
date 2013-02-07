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
// Purpose: G.722 speech codec: encode API.
//
*/

#include "owng722sb.h"

static int EncoderObjSize(void){
   int stSize;
   int objSize=sizeof(G722SB_Encoder_Obj);
   ippsSBADPCMEncodeStateSize_G722_16s(&stSize);
   objSize += stSize; /* memory size in bytes */
   return objSize;
}

G722SB_CODECFUN( API_G722SB_Status, apiG722SBEncoder_Alloc, (int *pSize))
{
   *pSize =  EncoderObjSize();
   return API_G722SB_StsNoErr;
}

G722SB_CODECFUN( API_G722SB_Status, apiG722SBEncoder_Init,
              (G722SB_Encoder_Obj* encoderObj, unsigned int mode_qmf))
{

   encoderObj->objPrm.objSize = EncoderObjSize();
   encoderObj->objPrm.mode = mode_qmf;
   encoderObj->objPrm.key = G722_SBAD_ENC_KEY;
   encoderObj->objPrm.rat = 1; /* 64 kbps default */

   ippsZero_16s(encoderObj->qmf_tx_delayx,  SBADPCM_G722_SIZE_QMFDELAY);
   encoderObj->stateEnc = (char*)encoderObj + sizeof(G722SB_Encoder_Obj);
   ippsSBADPCMEncodeInit_G722_16s((IppsEncoderState_G722_16s*)encoderObj->stateEnc);

   return API_G722SB_StsNoErr;
}

G722SB_CODECFUN(  API_G722SB_Status, apiG722SBEncode,
         (G722SB_Encoder_Obj* encoderObj, int lenSamples, short *pSrc, char *pDst ))
{
   int             lenBlk, length;
   int             i, j;
   /* Encoder variables */
   IPP_ALIGNED_ARRAY(16, Ipp16s, QMFVec, LEN_BLOCK);
   IPP_ALIGNED_ARRAY(16, Ipp16s, CodeVec, LEN_BLOCK);

   if(encoderObj->objPrm.mode == 1) {
     lenBlk = (lenSamples & 0xFFFFFFFE);
     while(lenBlk > 0) {
        if(lenBlk < LEN_BLOCK) { length = lenBlk; lenBlk = 0; }
        else { length = LEN_BLOCK; lenBlk -= LEN_BLOCK; }

        /* QMF synthesis */
        ippsQMFEncode_G722_16s(pSrc, QMFVec, length, encoderObj->qmf_tx_delayx);

        /* ADPCM encode */
        ippsSBADPCMEncode_G722_16s(QMFVec, CodeVec, length, (IppsEncoderState_G722_16s*)encoderObj->stateEnc);

        /* Pack codeword bits:
         - [0-5 bits] - lower-band;
         - [6-7 bits] - upper-band.
        */
        PackCodeword(CodeVec, length, pDst);

        pSrc += length;
        pDst += (length/2);
     }
   } else {
     lenBlk = lenSamples;
     while(lenBlk > 0) {
        if(lenBlk < LEN_BITSTREAM) { length = lenBlk; lenBlk = 0; }
        else { length = LEN_BITSTREAM; lenBlk -= LEN_BITSTREAM; }

        for(i=0,j=0;i<length;i++,j+=2) QMFVec[j] = QMFVec[j+1] = (short)(pSrc[i] >> 1);

        /* ADPCM encode */
        ippsSBADPCMEncode_G722_16s(QMFVec, CodeVec, 2*length, (IppsEncoderState_G722_16s*)encoderObj->stateEnc);

        PackCodeword(CodeVec, 2*length, pDst);

        pSrc += length;
        pDst += length;
     }
     /*****************************************************/
   }

   return API_G722SB_StsNoErr;
}



