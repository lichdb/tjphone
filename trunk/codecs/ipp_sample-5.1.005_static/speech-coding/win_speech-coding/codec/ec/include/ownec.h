/* /////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2005-2006 Intel Corporation. All Rights Reserved.
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
//  Purpose: Echo Canceller header file.
//
*/

#ifndef __OWNEC_H__
#define __OWNEC_H__

#include "ippsc.h"
#include "ec_api.h"
#include "ah_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* common section */

#define ALIGN(s) (((s) + 15) & ~15)
#define ADDWEIGHTED(val, add, coef) ((val) = (val) * (coef) + (1 - (coef)) * (add))

#define ADAPTATION_ENABLED (state->mode & 1)
#define NLP_ENABLED (state->mode & 2)
#define TD_ENABLED (state->mode & 4)
#define AH_ENABLED (state->mode & 8)

/* fullband section */

/* maximum size of frame */
#define FB_FRAME_SIZE_MAX 16
/* size of frame */
#define FB_FRAME_SIZE 8

typedef struct __fbECState{
    IppsFullbandControllerState_EC_32f *controller; /* controller */
    IppsToneDetectState_EC_32f *tdr, *tds;  /* ToneDetect state structures */
    Ipp32f  *firf_coef;        /* fixed filter coeffs */
    Ipp32f  *fira_coef;        /* adaptive filter coeffs */
    Ipp32f  *rin_hist;         /* history buffer */
    ahState *ah;
    Ipp32f  td_coeff,td_thres; /* ToneDisabler coeffs */
    Ipp32f  r_in_pwr;          /* receive-in signal power */
    Ipp32f  s_in_pwr;          /* send-in signal power */
    Ipp32f  err;               /* error of NLMS */
    int     tap;                  /* number of filter coeffs */
    int     tap_m;                /* size of history buffer */
    int     mode;                 /* current operating mode */
    int     hist_buf;             /* history buffer */
    int     pos;                  /* current position in history buffer */
    int     td_mode;              /* mode before activate ToneDisabler */
    int     td_resr;              /* used in ToneDisabler */
    int     td_ress;              /* used in ToneDisabler */
    int     frameSize;            /* size of frame */
} _fbECState;

/* subband section */

#define SB_SUBBANDS 65
#define SB_FRAME_SIZE 64
#define SB_FFT_LEN 128
#define SB_FFT_ORDER 7
#define SB_WIN_LEN 128

typedef struct __sbECState {
    IppsSubbandControllerState_EC_32f *controller; /* subband controller */
    IppsToneDetectState_EC_32f *tdr, *tds; /* ToneDetect state structures */
    IppsFFTSpec_R_32f* spec_fft;   /* FFT structure */
    IppsFFTSpec_C_32fc* spec_fftC; /* FFT structure */
    Ipp8u    *pBuf;                   /* FFT work buffer */
    Ipp32fc **ppRinSubbandHistory; /* receive-in subband history */
    Ipp32fc **ppAdaptiveCoefs;     /* adaptive coeffs */
    Ipp32fc **ppFixedCoefs;        /* fixed coeffs */
    ahState  *ah;
    Ipp32f    pRinHistory[SB_WIN_LEN];   /* receive-in signal history */
    Ipp32f    td_coeff, td_thres;     /* ToneDisabler coeffs */
    Ipp32f    r_in_pwr;               /* receive-in signal power */
    Ipp32f    s_in_pwr;               /* send-in signal power */
    int       FFTSize, frameSize, windowLen;
    int       numSegments;               /* number of filter segments */
    int       numSubbands;               /* number of subbands */
    int       mode;                      /* current operating mode */
    int       td_mode;                   /* mode before activate ToneDisabler */
    int       td_resr, td_ress;          /* used in ToneDisabler */
} _sbECState;

/* subbandfast section */

#if 0

#define SBF_SUBBANDS 17
#define SBF_FRAME_SIZE 24
#define SBF_FFT_LEN 32
#define SBF_FFT_ORDER 5
#define SBF_WIN_LEN 128

#else

#define SBF_SUBBANDS 33
#define SBF_FRAME_SIZE 44
#define SBF_FFT_LEN 64
#define SBF_FFT_ORDER 6
#define SBF_WIN_LEN 256

#endif

typedef struct __sbfECState {
    IppsSubbandControllerState_EC_32f *controller; /* subband controller */
    IppsToneDetectState_EC_32f *tdr, *tds; /* ToneDetect state structures */
    IppsSubbandProcessState_32f *state_syn;
    Ipp8u    *pBuf;                   /* FFT work buffer */
    Ipp32fc **ppRinSubbandHistory; /* receive-in subband history */
    Ipp32fc **ppAdaptiveCoefs;     /* adaptive coeffs */
    Ipp32fc **ppFixedCoefs;        /* fixed coeffs */
    /* receive-in subband analysis structure */
    IppsSubbandProcessState_32f *state_ana_rin;
    /* send-in subband analysis structure */
    IppsSubbandProcessState_32f *state_ana_sin;
    Ipp32f td_coeff, td_thres;     /* ToneDisabler coeffs */
    Ipp32f r_in_pwr;               /* receive-in signal power */
    Ipp32f s_in_pwr;               /* send-in signal power */
    int FFTSize, frameSize, windowLen;
    /* subband synthesis structure */
    int numSegments;               /* number of filter segments */
    int numSubbands;               /* number of subbands */
    int mode;                      /* current operating mode */
    int td_mode;                   /* mode before activate ToneDisabler */
    int td_resr, td_ress;          /* used in ToneDisabler */
    ahState *ah;
} _sbfECState;

#ifdef __cplusplus
}
#endif

#endif /* __OWNEC_H__ */

