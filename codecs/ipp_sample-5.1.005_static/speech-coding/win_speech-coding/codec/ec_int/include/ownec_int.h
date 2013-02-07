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
//  Purpose: Echo Canceller header file.
//
*/

#ifndef __OWNEC_H__
#define __OWNEC_H__

#include <ipp_w7.h>
#include "ipps.h"
#include "ippsc.h"
#include "ec_api_int.h"
#include "scratchmem.h"

#if defined(__ICC) || defined( __ICL ) || defined ( __ECL )
    #define __INLINE static __inline
#elif defined( __GNUC__ )
    #define __INLINE static __inline__
#else
    #define __INLINE static
#endif

#include "scratchmem.h"

#ifdef __cplusplus
extern "C" {
#endif

/* common section */

#define COEF_SF 14
#define COEF_ONE (1 << COEF_SF)

#define F_SF 20

#define ALIGN(s) (((s) + 15) & ~15)
#define ADDWEIGHTED_INT(val, add, coef) ((val) = SCALE(((val) * (coef) + (COEF_ONE - coef) * (add)), COEF_SF))

#define SAT_32S_16S(v)((v) > 32767 ? 32767 : ((v) < -32768 ? -32768 : (v)))
#define SCALE(v, f) (((v) + (1<<((f)-1))) >> (f))

#define ADAPTATION_ENABLED (state->mode & 1)
#define NLP_ENABLED (state->mode & 2)
#define TD_ENABLED (state->mode & 4)

/* fullband section */

/* maximum size of frame */
#define FB_FRAME_SIZE_MAX 16
/* size of frame */
#define FB_FRAME_SIZE 8

typedef struct __ifbECState {
#if !defined (NO_SCRATCH_MEMORY_USED)
    ScratchMem_Obj Mem;
#endif
    Ipp64s r_in_pwr;          /* receive-in signal power */
    Ipp64s s_in_pwr;          /* send-in signal power */
    IppsFullbandControllerState_EC_16s *controller; /* controller */
    IppsToneDetectState_EC_16s *tdr, *tds;  /* ToneDetect state structures */
    Ipp16s *firf_coef;        /* fixed filter coeffs */
    Ipp16s *fira_coef;        /* adaptive filter coeffs */
    Ipp16s *rin_hist;         /* history buffer */
    ahState_int *ah;
    Ipp32s td_coeff, td_thres;/* ToneDisabler coeffs */
    int tap;                  /* filter coeffs number*/
    int tap_m;                /* history buffer size*/
    int mode;                 /* current operating mode */
    int hist_buf;             /* history buffer */
    int pos;                  /* current position in history buffer */
    int td_mode;              /* mode before activate ToneDisabler */
    int td_resr;              /* used in ToneDisabler */
    int td_ress;              /* used in ToneDisabler */
    int frameSize;            /* frame size */
    Ipp16s err;               /* NLMS error*/
    Ipp16s sgain;             /* NLP gain */
} _ifbECState;

/* subband section */

#define SB_SUBBANDS 65
#define SB_FRAME_SIZE 64
#define SB_FFT_LEN (SB_FRAME_SIZE<<1)
#define SB_FFT_ORDER 7
#define SB_WIN_LEN (SB_FRAME_SIZE<<1)

typedef struct __isbECState {
#if !defined (NO_SCRATCH_MEMORY_USED)
    ScratchMem_Obj Mem;
#endif
    Ipp64s r_in_pwr;                /* receive-in signal power */
    Ipp64s s_in_pwr;                /* send-in signal power */
    IppsSubbandControllerState_EC_16s *controller; /* subband controller */
    IppsToneDetectState_EC_16s *tdr, *tds; /* ToneDetect state structures */
    IppsFFTSpec_R_16s32s* spec_fft; /* FFT structure */
    IppsFFTSpec_R_32s* spec_fft32;  /* FFT structure */
    IppsFFTSpec_C_32sc* spec_fftC;  /* FFT structure */
    Ipp32sc **ppRinSubbandHistory;  /* receive-in subband history */
    Ipp32sc **ppAdaptiveCoefs;      /* adaptive coeffs */
    Ipp32sc **ppFixedCoefs;         /* fixed coeffs */
    ahState_int *ah;
    Ipp8u    *pBuf;                   /* FFT work buffer */
    Ipp32s td_coeff, td_thres;        /* ToneDisabler coeffs */
    int    FFTSize, frameSize, windowLen;
    int    numSegments;               /* filter segments number */
    int    numSubbands;               /* subbands number */
    int    mode;                      /* current operating mode */
    int    td_mode;                   /* mode before activate ToneDisabler */
    int    td_resr, td_ress;          /* used in ToneDisabler */
    int    constrainSubbandNum;
    Ipp16s pRinHistory[SB_WIN_LEN];   /* receive-in signal history */
    Ipp16s sgain;                     /* NLP gain */
} _isbECState;

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

typedef struct __isbfECState {
#if !defined (NO_SCRATCH_MEMORY_USED)
    ScratchMem_Obj Mem;
#endif
    Ipp64s r_in_pwr;                /* receive-in signal power */
    Ipp64s s_in_pwr;                /* send-in signal power */
    /* receive-in subband analysis structure */
    IppsSubbandProcessState_16s *state_ana_rin;
    /* send-in subband analysis structure */
    IppsSubbandProcessState_16s *state_ana_sin;
    /* subband synthesis structure */
    IppsSubbandProcessState_16s *state_syn;
    Ipp32sc **ppRinSubbandHistory; /* receive-in subband history */
    Ipp32sc **ppAdaptiveCoefs;     /* adaptive coeffs */
    Ipp32sc **ppFixedCoefs;        /* fixed coeffs */
    IppsSubbandControllerState_EC_16s *controller; /* subband controller */
    IppsToneDetectState_EC_16s *tdr, *tds; /* ToneDetect state structures */
    ahState_int *ah;
    Ipp8u *pBuf;                   /* FFT work buffer */
    Ipp32s td_coeff, td_thres;     /* ToneDisabler coeffs */
    int FFTSize, frameSize, windowLen;
    int numSegments;               /* filter segments number */
    int numSubbands;               /* subbands number */
    int mode;                      /* current operating mode */
    int td_mode;                   /* mode before activate ToneDisabler */
    int td_resr, td_ress;          /* used in ToneDisabler */
    Ipp16s sgain;                  /* NLP gain */
} _isbfECState;

#ifdef __cplusplus
}
#endif

#endif /* __OWNEC_H__ */
