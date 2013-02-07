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
//  Purpose: Echo Canceller, subband algorithm
//
*/

#include <stdlib.h>
#include <ipp_w7.h>
#include "ipps.h"
#include "ippsc.h"
#include "ec_api_int.h"
#include "ownec_int.h"
#include "ah_api_int.h"

int ec_isb_setAHFreqShift(void *stat, int shift){
    int status;
    _isbECState *state=(_isbECState *)stat;
    state = IPP_ALIGNED_PTR(state, 16);
    status = ah_SetFreqShift_int(state->ah,shift);
    return status;
}
int ec_isb_setAHhowlPeriod(void *stat, int howlPeriod){
    int status;
    _isbECState *state=(_isbECState *)stat;
    state = IPP_ALIGNED_PTR(state, 16);
    status = ah_SetHowlPeriod_int(state->ah,howlPeriod);
    return status;
}
/* Returns size of frame */
/* init scratch memory buffer */
int ec_isb_InitBuff(_isbECState *state, char *buff) {
    if (buff==NULL) {
        return 1;
    }
    if (state==NULL) {
        return 1;
    }
    state = IPP_ALIGNED_PTR(state, 16);
#if !defined (NO_SCRATCH_MEMORY_USED)
    if(NULL != buff)
       state->Mem.base = buff;
    else
       if (NULL == state->Mem.base)
          return 1;
    state->Mem.CurPtr = state->Mem.base;
    state->Mem.VecPtr = (int *)(state->Mem.base+EC_SCRATCH_MEMORY_SIZE);
#endif
    return 0;
}
int ec_isb_GetInfo(isbECState *state,ECOpcode *isNLP,ECOpcode *isTD,
                   ECOpcode *isAdapt,ECOpcode *isAdaptLite, ECOpcode *isAH)
{
    state = IPP_ALIGNED_PTR(state, 16);
    if(state->mode & 2)
        *isNLP = EC_NLP_ENABLE;
    else
        *isNLP = EC_NLP_DISABLE;

    if(state->mode & 16)
        *isAH = EC_AH_ENABLE1;
    else
        *isAH = EC_AH_DISABLE;

    if(state->mode & 32)
        *isAH = EC_AH_ENABLE2;
    else
        *isAH = EC_AH_DISABLE;

    if(state->mode & 4)
        *isTD = EC_TD_ENABLE;
    else
        *isTD = EC_TD_DISABLE;

    *isAdaptLite = EC_ADAPTATION_DISABLE_LITE;
    if(state->mode & 1){
        *isAdapt = EC_ADAPTATION_ENABLE;
        if(state->mode & 8)
            *isAdaptLite = EC_ADAPTATION_ENABLE_LITE;
    } else
        *isAdapt = EC_ADAPTATION_DISABLE;

    return 0;
}

/* Returns size of frame */
int ec_isb_GetFrameSize(IppPCMFrequency freq, int taptime_ms, int *s)
{
    freq=(IppPCMFrequency)0;taptime_ms=0;
    *s = SB_FRAME_SIZE;
    return 0;
}

int ec_isb_GetSegNum(isbECState *state)
{
    state = IPP_ALIGNED_PTR(state, 16);
    return state->numSegments;
}
int ec_isb_GetSubbandNum(isbECState *state)
{
    state = IPP_ALIGNED_PTR(state, 16);
    return state->numSubbands;
}

/* Returns size of buffer for state structure */
int ec_isb_GetSize(IppPCMFrequency freq, int taptime_ms, int *s)
{
    int size = 0, numSegments;
    int pSizeSpec, pSizeInit, pSizeBuf;
    int pSizeInit1, pSizeBuf1, csize;

    size += ALIGN(sizeof(_isbECState));

    ippsFFTGetSize_R_16s32s(SB_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit, &pSizeBuf);

    size += ALIGN(pSizeSpec);

    ippsFFTGetSize_C_32sc(SB_FFT_ORDER, IPP_FFT_DIV_FWD_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit1, &pSizeBuf1);

    size += ALIGN(pSizeSpec);

    csize = pSizeInit;
    if (csize < pSizeInit1)
        csize = pSizeInit1;
    if (csize < pSizeBuf)
        csize = pSizeBuf;
    if (csize < pSizeBuf1)
        csize = pSizeBuf1;

    ippsFFTGetSize_R_32s(SB_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit1, &pSizeBuf1);

    size += ALIGN(pSizeSpec);

    if (csize < pSizeInit1)
        csize = pSizeInit1;

    if (csize < pSizeBuf1)
        csize = pSizeBuf1;

    size += ALIGN(csize);

    if (freq == IPP_PCM_FREQ_8000) {
        numSegments = (taptime_ms * 8 - 1) / SB_FRAME_SIZE + 1;
    } else if (freq == IPP_PCM_FREQ_16000) {
        numSegments = (taptime_ms * 16 - 1) / SB_FRAME_SIZE + 1;
    } else {
        return 1;
    }

    size += ALIGN(3 * numSegments * sizeof(Ipp32sc *));

    size += ALIGN(3 * numSegments * SB_SUBBANDS * sizeof(Ipp32sc));

    ippsSubbandControllerGetSize_EC_16s(SB_SUBBANDS, SB_FRAME_SIZE, numSegments, freq, &csize);

    size += ALIGN(csize);

    ippsToneDetectGetStateSize_EC_16s(freq, &csize);
    size += ALIGN(csize * 2);

    ah_GetSize_int(SB_FRAME_SIZE, &csize);
    size += ALIGN(csize);
    *s = ALIGN(size+15);

    return 0;
}

/* Returns delay introduced to send-path */
int ec_isb_GetSendPathDelay(int *delay)
{
    *delay = 0;
    return 0;
}

/* acquire NLP gain coeff */
int ec_isb_GetNLPGain(_isbECState *state)
{
    state = IPP_ALIGNED_PTR(state, 16);
    return state->sgain;
}

/* Initializes state structure */
int ec_isb_Init(_isbECState *state, IppPCMFrequency freq, int taptime_ms)
{
    int i, numSegments;
    int pSizeSpec, pSizeInit, pSizeBuf;
    int pSizeInit1, pSizeBuf1, csize;
    Ipp8u *ptr, *ptr0, *ptr1, *ptr2, *buf;
    state = IPP_ALIGNED_PTR(state, 16);
    ptr = (Ipp8u *)state;

    ptr += ALIGN(sizeof(_isbECState));


    ippsFFTGetSize_R_16s32s(SB_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit, &pSizeBuf);

    ptr0 = ptr;
    ptr += ALIGN(pSizeSpec);

    ippsFFTGetSize_C_32sc(SB_FFT_ORDER, IPP_FFT_DIV_FWD_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit1, &pSizeBuf1);

    ptr1 = ptr;
    ptr += ALIGN(pSizeSpec);

    csize = pSizeInit;
    if (csize < pSizeInit1)
        csize = pSizeInit1;
    if (csize < pSizeBuf)
        csize = pSizeBuf;
    if (csize < pSizeBuf1)
        csize = pSizeBuf1;

    ippsFFTGetSize_R_32s(SB_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit1, &pSizeBuf1);

    ptr2 = ptr;
    ptr += ALIGN(pSizeSpec);

    if (csize < pSizeInit1)
        csize = pSizeInit1;

    if (csize < pSizeBuf1)
        csize = pSizeBuf1;

    ippsFFTInit_R_16s32s(&(state->spec_fft), SB_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        ptr0, ptr);
    ippsFFTInit_R_32s(&(state->spec_fft32), SB_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        ptr1, ptr);
    ippsFFTInit_C_32sc(&(state->spec_fftC), SB_FFT_ORDER, IPP_FFT_DIV_FWD_BY_N, ippAlgHintAccurate,
        ptr2, ptr);

    /* Work buffer for FFT */
    state->pBuf = ptr;

    ptr += ALIGN(csize);

    state->FFTSize = 128;
    state->numSubbands = 65;
    state->frameSize = 64;
    state->windowLen = SB_WIN_LEN;

    if (freq == IPP_PCM_FREQ_8000) {
        numSegments = (taptime_ms * 8 - 1) / state->frameSize + 1;
    } else if (freq == IPP_PCM_FREQ_16000) {
        numSegments = (taptime_ms * 16 - 1) / state->frameSize + 1;
    } else {
        return 1;
    }

    state->numSegments = numSegments;

    /* receive-in subband history */
    state->ppRinSubbandHistory = (Ipp32sc **)ptr;
    ptr += ALIGN(3 * numSegments * sizeof(Ipp32sc *));
    /* adaptive coeffs */
    state->ppAdaptiveCoefs = state->ppRinSubbandHistory + numSegments;
    /* fixed coeffs */
    state->ppFixedCoefs = state->ppRinSubbandHistory + numSegments * 2;

    buf = ptr;
    ptr += ALIGN(3 * numSegments * SB_SUBBANDS * sizeof(Ipp32sc));

    /* Zero coeffs buffer and history buffer */
    ippsZero_32sc((Ipp32sc *)buf, 3 * numSegments * SB_SUBBANDS);

    /* Initialize receive-in subband history array */
    for (i = 0; i < numSegments; i++) {
        (state->ppRinSubbandHistory)[i] = (Ipp32sc *)buf + i * (state->numSubbands);
    }
    buf += numSegments * (state->numSubbands) * sizeof(Ipp32sc);

    /* Initialize adaptive coeffs array */
    for (i = 0; i < numSegments; i++) {
        (state->ppAdaptiveCoefs)[i] = (Ipp32sc *)buf + i * (state->numSubbands);
    }
    buf += numSegments * (state->numSubbands) * sizeof(Ipp32sc);

    /* Initialize fixed coeffs array */
    for (i = 0; i < numSegments; i++) {
        (state->ppFixedCoefs)[i] = (Ipp32sc *)buf + i * (state->numSubbands);
    }

    /* Initialize subband controller */
    state->controller = (IppsSubbandControllerState_EC_16s *)ptr;
    if (ippsSubbandControllerInit_EC_16s(state->controller, state->numSubbands, state->frameSize, numSegments, freq) != ippStsOk)
        return 1;

    ippsSubbandControllerGetSize_EC_16s(SB_SUBBANDS, SB_FRAME_SIZE, numSegments, freq, &csize);

    ptr += ALIGN(csize);
    state->ah = (ahState_int *)ptr;
    ah_Init_int(state->ah, freq, SB_FRAME_SIZE);
    ah_GetSize_int(SB_FRAME_SIZE, &csize);
    ptr += ALIGN(csize);

    /* Initialize tone detection */
    ippsToneDetectGetStateSize_EC_16s(freq, &csize);
    state->tdr = (IppsToneDetectState_EC_16s *)ptr;
    ptr += csize;
    state->tds = (IppsToneDetectState_EC_16s *)ptr;
    ippsToneDetectInit_EC_16s(state->tdr, freq);
    ippsToneDetectInit_EC_16s(state->tds, freq);

    /* zero receive-in history buffer */
    for (i = 0; i < state->windowLen; i++)
        state->pRinHistory[i] = 0;

    /* enable adaptation */
    state->mode = 1;

    state->r_in_pwr = 0;
    state->s_in_pwr = 0;
    state->td_coeff = COEF_ONE - COEF_ONE * 4 * state->frameSize / freq;
    state->td_thres = 57344;
    state->td_resr = state->td_ress = 0;
    state->sgain = IPP_MAX_16S;
    state->constrainSubbandNum=0;
    return 0;
}

/* Do operation or set mode */
int ec_isb_ModeOp(_isbECState *state, ECOpcode op)
{
    int i, j;

    state = IPP_ALIGNED_PTR(state, 16);
    switch (op) {
    case (EC_COEFFS_ZERO):      /* Zero coeffs of filters */
        {
            for (i = 0; i < state->numSegments; i++)
                for (j = 0; j < state->numSubbands; j++)
                    state->ppAdaptiveCoefs[i][j].re = state->ppAdaptiveCoefs[i][j].im =
                        state->ppFixedCoefs[i][j].re = state->ppFixedCoefs[i][j].im = 0;
        }
        break;
    case(EC_ADAPTATION_ENABLE): /* Enable adaptation */
        if (!(state->mode & 1))
            ippsSubbandControllerReset_EC_16s(state->controller);
        state->mode |= 1;
        break;
    case(EC_ADAPTATION_ENABLE_LITE): /* Enable adaptation */
        if (!(state->mode & 1))
            ippsSubbandControllerReset_EC_16s(state->controller);
        state->mode |= 9; /* Enable adaptation + no constrain */
        break;
    case(EC_ADAPTATION_DISABLE): /* Disable adaptation */
        state->mode &= ~1;
        break;
    case(EC_ADAPTATION_DISABLE_LITE): /* Disable adaptation */
        state->mode &= ~8;
        break;
    case(EC_NLP_ENABLE):    /* Enable NLP */
        state->mode |= 2;
        break;
    case(EC_NLP_DISABLE):   /* Disable NLP */
        state->mode &= ~2;
        break;
    case(EC_TD_ENABLE):     /* Enable ToneDisabler */
        state->mode |= 4;
        break;
    case(EC_TD_DISABLE):    /* Disable ToneDisabler */
        state->mode &= ~4;
        break;
    case(EC_AH_ENABLE1):     /* Enable howling control */
        ah_SetMode_int(state->ah,AH_ENABLE1);// set anti-howling on
        state->mode |= 16;
        break;
    case(EC_AH_ENABLE2):     /* Enable howling control */
        ah_SetMode_int(state->ah,AH_ENABLE2);// set anti-howling on
        state->mode |= 32;
        break;
    case(EC_AH_DISABLE):    /* Disable howling control */
        ah_SetMode_int(state->ah,AH_DISABLE);// set anti-howling off
        state->mode &= ~16;
        state->mode &= ~32;
        break;
    default:
        break;
    }

    return 0;
}


/* Tone Disabler */
static int ec_isb_ToneDisabler(_isbECState *state, Ipp16s *r_in, Ipp16s *s_in)
{
    int resr, ress;
    int i;
    Ipp64s r_in_pwr = 0, s_in_pwr = 0;

    state = IPP_ALIGNED_PTR(state, 16);
    /* Detect 2100 Hz with periodical phase reversal */
    ippsToneDetect_EC_16s(r_in, state->frameSize, &resr, state->tdr);
    ippsToneDetect_EC_16s(s_in, state->frameSize, &ress, state->tds);

    /* Update receive-in signal and send-in signal powers */
    for (i = 0; i < state->frameSize; i++) {
        r_in_pwr += r_in[i] * r_in[i];
        s_in_pwr += s_in[i] * s_in[i];
    }

    ADDWEIGHTED_INT(state->r_in_pwr, r_in_pwr, state->td_coeff);
    ADDWEIGHTED_INT(state->s_in_pwr, s_in_pwr, state->td_coeff);

    if (state->td_ress || state->td_resr) {
       if ((!state->td_resr || state->r_in_pwr < state->td_thres) &&
            (!state->td_ress || state->s_in_pwr < state->td_thres))
        {
            /* Restore previous mode if 250+-100ms signal of<=36dbm0*/
            state->td_resr = state->td_ress = 0;
            if (state->td_mode & 1)
                ec_isb_ModeOp(state, EC_ADAPTATION_ENABLE);
            if (state->td_mode & 2)
                ec_isb_ModeOp(state, EC_NLP_ENABLE);
        }
    } else if (resr || ress) {
        /* Zero coeffs, disable adaptation and NLP */
        if (resr) state->td_resr = 1;
        if (ress) state->td_ress = 1;

        state->td_mode = state->mode;
        ec_isb_ModeOp(state, EC_COEFFS_ZERO);
        ec_isb_ModeOp(state, EC_ADAPTATION_DISABLE);
        ec_isb_ModeOp(state, EC_NLP_DISABLE);
    }

    return 0;
}
static void isb_ownConstrainCoeff(_isbECState *state, int subbandNum){
   int k,s;
   LOCAL_ALIGN_ARRAY(32,Ipp32s,bsrc,SB_SUBBANDS * 4,state);
   LOCAL_ALIGN_ARRAY(32,Ipp32s,bsrc1,SB_SUBBANDS * 4,state);
   LOCAL_ALIGN_ARRAY(32,Ipp32s,bdst,SB_FFT_LEN,state);

   state = IPP_ALIGNED_PTR(state, 16);
   ippsFFTInv_CCSToR_32s_Sfs((Ipp32s *)(state->ppAdaptiveCoefs[subbandNum]), bdst, state->spec_fft32, 0, state->pBuf);

   ippsZero_32s(bsrc1,SB_SUBBANDS * 4);
   for (k = 0; k < SB_FRAME_SIZE; k++) {
      bsrc1[2 * k] = SB_FFT_LEN * bdst[k];
   }

   ippsFFTFwd_CToC_32sc_Sfs((const Ipp32sc *)bsrc1, (Ipp32sc *)bsrc, state->spec_fftC, 0, state->pBuf);

   for (s = 0; s < SB_SUBBANDS; s++) {
      state->ppAdaptiveCoefs[subbandNum][s].re = bsrc[2 * s];
      state->ppAdaptiveCoefs[subbandNum][s].im = bsrc[2 * s + 1];
   }
   LOCAL_ALIGN_ARRAY_FREE(32,Ipp32s,bdst,SB_FFT_LEN,state);
   LOCAL_ALIGN_ARRAY_FREE(32,Ipp32s,bsrc1,SB_SUBBANDS * 4,state);
   LOCAL_ALIGN_ARRAY_FREE(32,Ipp32s,bsrc,SB_SUBBANDS * 4,state);

}
/* Process one frame */
int ec_isb_ProcessFrame(_isbECState *state, Ipp16s *rin, Ipp16s *sin, Ipp16s *sout)
{
    int i;
    LOCAL_ALIGN_ARRAY(32,Ipp32sc,fira_err,SB_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY(32,Ipp32sc,firf_err,SB_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY(32,Ipp32sc,fira_out,SB_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY(32,Ipp32sc,firf_out,SB_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY(32,Ipp32s_EC_Sfs,pStepSize,SB_SUBBANDS*2,state);
    LOCAL_ALIGN_ARRAY(32,Ipp16s,pTimeFiltOut,SB_FFT_LEN,state);
    LOCAL_ALIGN_ARRAY(32,Ipp16s,pTimeError,SB_FFT_LEN,state);
    Ipp32sc* pSegment; /* Pointer to segment of input history */
    int iSegment;
    int numSegments;
    Ipp16s *pRinHistory;
    int windowLen;
    int frameSize;
    state = IPP_ALIGNED_PTR(state, 16);
    numSegments = state->numSegments;
    pRinHistory = state->pRinHistory;
    windowLen = state->windowLen;
    frameSize = state->frameSize;

    /* Tone Disabler */
    if (TD_ENABLED)
        ec_isb_ToneDisabler(state, rin, sin);

    /* update receive-in subband history buffer */
    pSegment = state->ppRinSubbandHistory[state->numSegments-1];
    for(iSegment = state->numSegments-1; iSegment > 0; iSegment--)
    {
      state->ppRinSubbandHistory[iSegment] = state->ppRinSubbandHistory[iSegment-1];
    }
    state->ppRinSubbandHistory[0] = pSegment;

    /* update receive-in history buffer */
    for (i = 0; i < windowLen - frameSize; i++)
        pRinHistory[i] = pRinHistory[i + frameSize];

    for (i = 0; i < frameSize; i++)
        pRinHistory[i + windowLen - frameSize] = rin[i];

    /* Apply FFT to receive-in signal */
    ippsFFTFwd_RToCCS_16s32s_Sfs(pRinHistory, (Ipp32s *)(state->ppRinSubbandHistory[0]), state->spec_fft, 0, state->pBuf);

    /* Do filtering with fixed coeffs */
    ippsFIRSubband_EC_32sc_Sfs(state->ppRinSubbandHistory, state->ppFixedCoefs, firf_out,
        numSegments, state->numSubbands, F_SF);

    /* Get fixed filter output in time domain: apply inverse FFT */
    ippsFFTInv_CCSToR_32s16s_Sfs((Ipp32s *)firf_out, pTimeFiltOut, state->spec_fft, 0, state->pBuf);

    /* First half of error is set to zero. */
    for (i = 0; i < SB_FRAME_SIZE; i++)
        pTimeError[i] = 0;

    /* Subtract microphone signal from fixed filter output. */
    for (i = 0; i < SB_FRAME_SIZE; i++) {
        pTimeError[SB_FRAME_SIZE + i] = SAT_32S_16S(sin[i] - pTimeFiltOut[SB_FRAME_SIZE + i]);
        sout[i] = pTimeError[SB_FRAME_SIZE + i];
    }

    if (ADAPTATION_ENABLED) // Adaptation Enabled
    {
        /* Get fixed filter error in frequency domain */
        ippsFFTFwd_RToCCS_16s32s_Sfs(pTimeError, (Ipp32s *)firf_err, state->spec_fft, 0, state->pBuf);

        /* Do filtering with adaptive coeffs */
        ippsFIRSubband_EC_32sc_Sfs(state->ppRinSubbandHistory, state->ppAdaptiveCoefs, fira_out,
            numSegments, state->numSubbands, F_SF);
        /* Get adaptive filter output in time domain */
        ippsFFTInv_CCSToR_32s16s_Sfs((Ipp32s *)fira_out, pTimeFiltOut, state->spec_fft, 0, state->pBuf);

        /* Subtract microphone signal from adaptive filter output. */
        for (i = 0; i < SB_FRAME_SIZE; i++) {
            pTimeError[SB_FRAME_SIZE + i] = SAT_32S_16S(sin[i] - pTimeFiltOut[SB_FRAME_SIZE + i]);
        }

        ippsFFTFwd_RToCCS_16s32s_Sfs(pTimeError, (Ipp32s *)fira_err, state->spec_fft, 0, state->pBuf);

        /* Update subband controller */
        ippsSubbandControllerUpdate_EC_16s(rin, sin, (const Ipp32sc **)(state->ppRinSubbandHistory),
            0, pStepSize, state->controller);

        /* Update adaptive coeffs */
        ippsFIRSubbandCoeffUpdate_EC_32sc_I(pStepSize, (const Ipp32sc **)(state->ppRinSubbandHistory), fira_err,
            state->ppAdaptiveCoefs, numSegments, state->numSubbands, F_SF);


        if(state->mode & 8){ /* lite */
            /* Constrain coeffs each 10th frame. */
           if(!(state->constrainSubbandNum)) {
               for (i = 0; i < numSegments; i++) {
                  isb_ownConstrainCoeff(state,i);
               }
           }
           state->constrainSubbandNum++;
           if(state->constrainSubbandNum >= 10) state->constrainSubbandNum=0;
        }else{
            /* Constrain coeffs. */
            for (i = 0; i < numSegments; i++) {
                  isb_ownConstrainCoeff(state,i);
            }
        }

        /* Apply subband controller */
         ippsSubbandController_EC_16s(fira_err, firf_err, state->ppAdaptiveCoefs, state->ppFixedCoefs,
            &state->sgain,state->controller);
        /* Apply NLP coeff */
       ah_ProcessFrame_int(state->ah,sout,sout);/*operates before NLP*/
       if (NLP_ENABLED)
            for (i = 0; i < frameSize; i++)
                sout[i] = (Ipp16s)SCALE((Ipp32s)sout[i] * state->sgain, 15);
    }
    if (!ADAPTATION_ENABLED)
        ah_ProcessFrame_int(state->ah,sout,sout);

    LOCAL_ALIGN_ARRAY_FREE(32,Ipp16s,pTimeError,SB_FFT_LEN,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp16s,pTimeFiltOut,SB_FFT_LEN,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32s_EC_Sfs,pStepSize,SB_SUBBANDS*2,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32sc,firf_out,SB_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32sc,fira_out,SB_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32sc,firf_err,SB_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32sc,fira_err,SB_SUBBANDS,state);


    return 0;
}
