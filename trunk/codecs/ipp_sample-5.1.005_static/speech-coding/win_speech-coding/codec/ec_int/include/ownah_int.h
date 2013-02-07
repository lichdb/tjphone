/* /////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2005 Intel Corporation. All Rights Reserved.
//
//     Intel(R) Integrated Performance Primitives EC Sample
//
//  By downloading and installing this sample, you hereby agree that the
//  accompanying Materials are being provided to you under the terms and
//  conditions of the End User License Agreement for the Intel(R) Integrated
//  Performance Primitives product previously accepted by you. Please refer
//  to the file ipplic.htm located in the root directory of your Intel(R) IPP
//  product installation for more information.
//
//
//  Purpose: anti - howling header file.
//
*/

#ifndef __OWNAH_INT_H__
#define __OWNAH_INT_H__
#include <ipp_w7.h>
#include "ipps.h"
#define ALIGN(s) (((s) + 15) & ~15)
#ifdef __cplusplus
extern "C" {
#endif
//#define _XScale
#define BW_HWL 11
#define BW_DISP_LEN ((((1<<AH_FFT_ORDERx)+2)>>1)/BW_HWL)
#define BW_AMPL_PRECISION 2
#define MAX_HOWL_IDX0 2
#define MAX_HOWL_IDX2 29
#define AH_FFT_ORDER      10
#define AH_FFT_ORDERx     6
#define dFScale           5
#define LOW_BAND          100    //FFT band pass filter margin
#define HIGH_BAND         3300
#define LOW_WIDE_BAND     90
#define HIGH_WIDE_BAND    15600
#define AH_IN_WORK_LEN    (1<<AH_FFT_ORDERx)
typedef struct __ahState_int{
    IppsFFTSpec_R_16s32s *howlSpecFFT;
    IppsFFTSpec_R_16s32s *howlSpecFFTx;
    Ipp16s  *howlHist;
    Ipp8u   *pBuf;              /* work buffer */
    Ipp16s  *sBuf;              /* FFT work buffer */
    Ipp32sc *howlSpectrum;      /* fft out */
    Ipp16s  *upDownBuf;         /* work buffer */
    Ipp32s   pDly2[6*2];
    Ipp32s   pDly2Test[6*2];
    int      howlHistLen;
    int      howlHistCurLen;
    int      prevHistExist;
    int      dF;
    int      howlLowBand;
    int      howlHighBand;

    int      howlPeriod;
    int      howlPeriodInit;
    AHOpcode mode;                 /* current operating mode */
    int      frameSize;
    int      samplingRate;
    int      shift;
    int      hd_mode;
} _ahState_int;

#ifdef __cplusplus
}
#endif

#endif /* __OWNAH_INT_H__ */

