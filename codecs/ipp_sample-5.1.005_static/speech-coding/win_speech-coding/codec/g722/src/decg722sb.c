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
// Purpose: G.722 speech codec: decode API.
//
*/

#include "owng722sb.h"

static int DecoderObjSize(void){
   int stSize;
   int objSize=sizeof(G722SB_Decoder_Obj);
   ippsSBADPCMDecodeStateSize_G722_16s(&stSize);
   objSize += stSize; /* memory size in bytes */
   return objSize;
}

G722SB_CODECFUN( API_G722SB_Status, apiG722SBDecoder_Alloc, (int *pSize))
{
   *pSize =  DecoderObjSize();
   return API_G722SB_StsNoErr;
}

G722SB_CODECFUN( API_G722SB_Status, apiG722SBDecoder_Init,
              (G722SB_Decoder_Obj* decoderObj, unsigned int mode_qmf))
{

   decoderObj->objPrm.objSize = DecoderObjSize();
   decoderObj->objPrm.mode = mode_qmf;
   decoderObj->objPrm.key = G722_SBAD_DEC_KEY;
   decoderObj->objPrm.rat = 1; /* 64 kbps default */

   ippsZero_16s(decoderObj->qmf_rx_delayx,  SBADPCM_G722_SIZE_QMFDELAY);
   decoderObj->stateDec = (char*)decoderObj + sizeof(G722SB_Decoder_Obj);
   ippsSBADPCMDecodeInit_G722_16s((IppsDecoderState_G722_16s*)decoderObj->stateDec);

   return API_G722SB_StsNoErr;
}


G722SB_CODECFUN(  API_G722SB_Status, apiG722SBDecode,
         (G722SB_Decoder_Obj* decoderObj, int lenBitstream, short mode, char *pSrc, short *pDst ))
{

   int             lenBlk, length;
   int             i;
   IPP_ALIGNED_ARRAY(16, Ipp16s, CodeVec, LEN_BLOCK);
   IPP_ALIGNED_ARRAY(16, Ipp16s, OutVec, LEN_BLOCK);

   if(mode < 1 || mode > 3) mode = (short)decoderObj->objPrm.rat;
   else decoderObj->objPrm.rat = mode;

   if(decoderObj->objPrm.mode == 1) {
      lenBlk = lenBitstream;
      while(lenBlk > 0) {
        if(lenBlk < LEN_BITSTREAM) { length = lenBlk; lenBlk = 0; }
        else { length = LEN_BITSTREAM; lenBlk -= LEN_BITSTREAM; }

        /* Unpack codeword bits:
         - [0-5 bits] - lower-band;
         - [6-7 bits] - upper-band.
        */
        UnpackCodeword(pSrc, length, CodeVec);

        /* ADPCM decode */
        ippsSBADPCMDecode_G722_16s(CodeVec, OutVec, 2*length, mode, (IppsDecoderState_G722_16s*)decoderObj->stateDec);

        /* QMF filter */
        ippsQMFDecode_G722_16s(OutVec, pDst, 2*length, decoderObj->qmf_rx_delayx);

        pSrc += length;
        pDst += 2*length;
      }
   } else {
      lenBlk = lenBitstream;
      while(lenBlk > 0) {
        if(lenBlk < LEN_BITSTREAM) { length = lenBlk; lenBlk = 0; }
        else { length = LEN_BITSTREAM; lenBlk -= LEN_BITSTREAM; }
        /* Unpack codeword bits: [0,5] - lower-band, [6,7] - upper-band */
        UnpackCodeword(pSrc, length, CodeVec);

        /* ADPCM decode */
        ippsSBADPCMDecode_G722_16s(CodeVec, OutVec, 2*length, mode, (IppsDecoderState_G722_16s*)decoderObj->stateDec);

        for(i=0; i < 2*length; i++) pDst[i] = (short)((OutVec[i] << 1) & 0xFFFE);

        pSrc += length;
        pDst += 2*length;
      }
      /*****************************************************/
   }

   return API_G722SB_StsNoErr;

}



