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
//  Purpose: Echo Canceller, sub band algorithm
//
*/
#include <ipp_w7.h>
#include "ipps.h"
#include "ippsc.h"
#include "ec_api.h"
#include "ownec.h"
#include "ah_api.h"

int ec_sb_setAHFreqShift(void *stat, int shift){
    int status;
    _sbECState *state=(_sbECState *)stat;
    state = IPP_ALIGNED_PTR(state, 16);
    status = ah_SetFreqShift(state->ah,shift);
    return status;
}
int ec_sb_setAHhowlPeriod(void *stat, int howlPeriod){
    int status;
    _sbECState *state=(_sbECState *)stat;
    state = IPP_ALIGNED_PTR(state, 16);
    status = ah_SetHowlPeriod(state->ah,howlPeriod);
    return status;
}
int ec_sb_GetInfo(sbECState *state,ECOpcode *isNLP,ECOpcode *isTD,
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
    } else
        *isAdapt = EC_ADAPTATION_DISABLE;

    return 0;
}
/* Returns size of frame */
int ec_sb_GetFrameSize(int *s)
{
    *s = SB_FRAME_SIZE;
    return 0;
}

/* Returns size of buffer for state structure */
int ec_sb_GetSize(IppPCMFrequency freq, int taptime_ms, int *s)
{
    int size = 0, numSegments;
    int pSizeSpec, pSizeInit, pSizeBuf;
    int pSizeInit1, pSizeBuf1, csize;

    size += ALIGN(sizeof(_sbECState));

    ippsFFTGetSize_R_32f(SB_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit, &pSizeBuf);

    size += ALIGN(pSizeSpec);

    ippsFFTGetSize_C_32fc(SB_FFT_ORDER, IPP_FFT_DIV_FWD_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit1, &pSizeBuf1);

    size += ALIGN(pSizeSpec);

    csize = pSizeInit;
    if (csize < pSizeInit1)
        csize = pSizeInit1;
    if (csize < pSizeBuf)
        csize = pSizeBuf;
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

    size += ALIGN(3 * numSegments * sizeof(Ipp32fc *));

    size += ALIGN(3 * numSegments * SB_SUBBANDS * sizeof(Ipp32fc));

    ippsSubbandControllerGetSize_EC_32f(SB_SUBBANDS, SB_FRAME_SIZE, numSegments, freq, &csize);

    size += ALIGN(csize);

    ippsToneDetectGetStateSize_EC_32f(freq, &csize);
    size += ALIGN(csize * 2);

    ah_GetSize(SB_FRAME_SIZE, &csize);
    size += ALIGN(csize);
    *s = ALIGN(size+15);

    return 0;
}

/* Returns delay introduced to send-path */
int ec_sb_GetSendPathDelay(int *delay)
{
    *delay = 0;
    return 0;
}

/* Initializes state structure */
int ec_sb_Init(void *stat, IppPCMFrequency freq, int taptime_ms)
{
    _sbECState *state=(_sbECState *)stat;
    int i, numSegments;
    int pSizeSpec, pSizeInit, pSizeBuf;
    int pSizeInit1, pSizeBuf1, csize;
    Ipp8u  *ptr, *ptr0, *ptr1, *buf;
    state = IPP_ALIGNED_PTR(state, 16);
    ptr = (Ipp8u *)state;

    ptr += ALIGN(sizeof(_sbECState));

    /* Initialize FFTs */
    ippsFFTGetSize_R_32f(SB_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit, &pSizeBuf);
    ptr0 = ptr;
    ptr += ALIGN(pSizeSpec);

    ippsFFTGetSize_C_32fc(SB_FFT_ORDER, IPP_FFT_DIV_FWD_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit1, &pSizeBuf1);
    ptr1 = ptr;
    ptr += ALIGN(pSizeSpec);

    ippsFFTInit_R_32f(&(state->spec_fft), SB_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        ptr0, ptr);
    ippsFFTInit_C_32fc(&(state->spec_fftC), SB_FFT_ORDER, IPP_FFT_DIV_FWD_BY_N, ippAlgHintAccurate,
        ptr1, ptr);

    /* Work buffer for FFT */
    state->pBuf = ptr;

    csize = pSizeInit;
    if (csize < pSizeInit1)
        csize = pSizeInit1;
    if (csize < pSizeBuf)
        csize = pSizeBuf;
    if (csize < pSizeBuf1)
        csize = pSizeBuf1;

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
    state->ppRinSubbandHistory = (Ipp32fc **)ptr;
    ptr += ALIGN(3 * numSegments * sizeof(Ipp32fc *));
    /* adaptive coeffs */
    state->ppAdaptiveCoefs = state->ppRinSubbandHistory + numSegments;
    /* fixed coeffs */
    state->ppFixedCoefs = state->ppRinSubbandHistory + numSegments * 2;

    buf = ptr;
    ptr += ALIGN(3 * numSegments * SB_SUBBANDS * sizeof(Ipp32fc));

    /* Zero coeffs buffer and history buffer */
    ippsZero_32fc((Ipp32fc *)buf, 3 * numSegments * SB_SUBBANDS);

    /* Initialize receive-in subband history array */
    for (i = 0; i < numSegments; i++) {
        (state->ppRinSubbandHistory)[i] = (Ipp32fc *)buf + i * (state->numSubbands);
    }
    buf += numSegments * (state->numSubbands) * sizeof(Ipp32fc);

    /* Initialize adaptive coeffs array */
    for (i = 0; i < numSegments; i++) {
        (state->ppAdaptiveCoefs)[i] = (Ipp32fc *)buf + i * (state->numSubbands);
    }
    buf += numSegments * (state->numSubbands) * sizeof(Ipp32fc);

    /* Initialize fixed coeffs array */
    for (i = 0; i < numSegments; i++) {
        (state->ppFixedCoefs)[i] = (Ipp32fc *)buf + i * (state->numSubbands);
    }

    /* Initialize subband controller */
    state->controller = (IppsSubbandControllerState_EC_32f *)ptr;
    if (ippsSubbandControllerInit_EC_32f(state->controller, state->numSubbands, state->frameSize, numSegments, freq) != ippStsOk)
        return 1;

    ippsSubbandControllerGetSize_EC_32f(SB_SUBBANDS, SB_FRAME_SIZE, numSegments, freq, &csize);

    ptr += ALIGN(csize);
    state->ah = (ahState *)ptr;
    ah_Init(state->ah, freq, SB_FRAME_SIZE);
    ah_GetSize(SB_FRAME_SIZE, &csize);
    ptr += ALIGN(csize);

    /* Initialize tone detection */
    ippsToneDetectGetStateSize_EC_32f(freq, &csize);
    state->tdr = (IppsToneDetectState_EC_32f *)ptr;
    ptr += csize;
    state->tds = (IppsToneDetectState_EC_32f *)ptr;
    ippsToneDetectInit_EC_32f(state->tdr, freq);
    ippsToneDetectInit_EC_32f(state->tds, freq);

    /* zero receive-in history buffer */
    for (i = 0; i < state->windowLen; i++)
        state->pRinHistory[i] = 0;

    /* enable adaptation */
    state->mode = 1;

    state->r_in_pwr = 0;
    state->s_in_pwr = 0;
    state->td_coeff = 1 - 4.0f * state->frameSize / freq;
    state->td_thres = 57344;
    state->td_resr = state->td_ress = 0;

    return 0;
}

/* Do operation or set mode */
int ec_sb_ModeOp(void *stat, ECOpcode op)
{
    int i, j;
    _sbECState *state=(_sbECState *)stat;
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
            ippsSubbandControllerReset_EC_32f(state->controller);
        state->mode |= 1;
        break;
    case(EC_ADAPTATION_DISABLE): /* Disable adaptation */
        state->mode &= ~1;
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
        ah_SetMode(state->ah,AH_ENABLE1);// set anti-howling on
        state->mode |= 16;
        break;
    case(EC_AH_ENABLE2):     /* Enable howling control */
        ah_SetMode(state->ah,AH_ENABLE2);// set anti-howling on
        state->mode |= 32;
        break;
    case(EC_AH_DISABLE):    /* Disable howling control */
        ah_SetMode(state->ah,AH_DISABLE);// set anti-howling off
        state->mode &= ~16;
        state->mode &= ~32;
        break;
    default:
        break;
    }

    return 0;
}

/* Tone Disabler */
static int ec_sb_ToneDisabler(void *stat, Ipp32f *r_in, Ipp32f *s_in)
{
    _sbECState *state=(_sbECState *)stat;
    int resr, ress;
    int i;
    Ipp32f r_in_pwr = 0, s_in_pwr = 0;
    state = IPP_ALIGNED_PTR(state, 16);

    /* Detect 2100 Hz with periodical phase reversal */
    ippsToneDetect_EC_32f(r_in, state->frameSize, &resr, state->tdr);
    ippsToneDetect_EC_32f(s_in, state->frameSize, &ress, state->tds);
    /* Update receive-in signal and send-in signal powers */
    for (i = 0; i < state->frameSize; i++) {
        r_in_pwr += r_in[i] * r_in[i];
        s_in_pwr += s_in[i] * s_in[i];
    }

    ADDWEIGHTED(state->r_in_pwr, r_in_pwr, state->td_coeff);
    ADDWEIGHTED(state->s_in_pwr, s_in_pwr, state->td_coeff);

    if (state->td_ress || state->td_resr) {
       if ((!state->td_resr || state->r_in_pwr < state->td_thres) &&
            (!state->td_ress || state->s_in_pwr < state->td_thres))
        {
            /* Restore previous mode */
            state->td_resr = state->td_ress = 0;// 4767-th frame
            if (state->td_mode & 1)
                ec_sb_ModeOp(state, EC_ADAPTATION_ENABLE);
            if (state->td_mode & 2)
                ec_sb_ModeOp(state, EC_NLP_ENABLE);
        }
    } else if (resr || ress) {
        /* Zero coeffs, disable adaptation and NLP */
        if (resr) state->td_resr = 1;
        if (ress) state->td_ress = 1;// 3265-th frame

        state->td_mode = state->mode;
        ec_sb_ModeOp(state, EC_COEFFS_ZERO);
        ec_sb_ModeOp(state, EC_ADAPTATION_DISABLE);
        ec_sb_ModeOp(state, EC_NLP_DISABLE);
    }

    return 0;
}
/* Process one frame */
int ec_sb_ProcessFrame(void *stat, Ipp32f *rin, Ipp32f *sin, Ipp32f *sout)
{
    _sbECState *state=(_sbECState *)stat;
    int i, k, s;
    Ipp32fc fira_err[SB_SUBBANDS], firf_err[SB_SUBBANDS], fira_out[SB_SUBBANDS], firf_out[SB_SUBBANDS];
    double pStepSize[SB_SUBBANDS*2];
    Ipp32f sgain;
    Ipp32fc* pSegment; /* Pointer to segment of input history */
    int iSegment;
    Ipp32f pTimeFiltOut[SB_FFT_LEN], pTimeError[SB_FFT_LEN];
    Ipp32f *pRinHistory;
    int numSegments;
    int windowLen;
    int frameSize;
    state = IPP_ALIGNED_PTR(state, 16);
    pRinHistory = state->pRinHistory;
    numSegments = state->numSegments;
    windowLen = state->windowLen;
    frameSize = state->frameSize;

    /* Tone Disabler */
    if (TD_ENABLED)
        ec_sb_ToneDisabler(state, rin, sin);

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
    ippsFFTFwd_RToCCS_32f(pRinHistory, (Ipp32f *)(state->ppRinSubbandHistory[0]), state->spec_fft, state->pBuf);

    /* Do filtering with fixed coeffs */
    ippsFIRSubband_EC_32fc(state->ppRinSubbandHistory, state->ppFixedCoefs, firf_out,
        numSegments, state->numSubbands);

    /* Get fixed filter output in time domain: apply inverse FFT */
    ippsFFTInv_CCSToR_32f((Ipp32f *)firf_out, pTimeFiltOut, state->spec_fft, state->pBuf);

    /* First half of error is set to zero. */
    for (i = 0; i < SB_FRAME_SIZE; i++)
        pTimeError[i] = 0;

    /* Subtract microphone signal from fixed filter output. */
    for (i = 0; i < SB_FRAME_SIZE; i++) {
        pTimeError[SB_FRAME_SIZE + i] = sin[i] - pTimeFiltOut[SB_FRAME_SIZE + i];
        sout[i] = pTimeError[SB_FRAME_SIZE + i];
    }

    if (ADAPTATION_ENABLED) // Adaptation Enabled
    {
        /* Get fixed filter error in frequency domain */
        ippsFFTFwd_RToCCS_32f(pTimeError, (Ipp32f *)firf_err, state->spec_fft, state->pBuf);

        /* Do filtering with adaptive coeffs */
        ippsFIRSubband_EC_32fc(state->ppRinSubbandHistory, state->ppAdaptiveCoefs, fira_out,
            numSegments, state->numSubbands);
        /* Get adaptive filter output in time domain */
        ippsFFTInv_CCSToR_32f((Ipp32f *)fira_out, pTimeFiltOut, state->spec_fft, state->pBuf);

        /* Subtract microphone signal from adaptive filter output. */
        for (i = 0; i < SB_FRAME_SIZE; i++) {
            pTimeError[SB_FRAME_SIZE + i] = sin[i] - pTimeFiltOut[SB_FRAME_SIZE + i];
        }

        ippsFFTFwd_RToCCS_32f(pTimeError, (Ipp32f *)fira_err, state->spec_fft, state->pBuf);

        /* Update subband controller */
        ippsSubbandControllerUpdate_EC_32f(rin, sin, (const Ipp32fc **)(state->ppRinSubbandHistory),
            0, pStepSize, state->controller);
        /* Update adaptive coeffs */
        ippsFIRSubbandCoeffUpdate_EC_32fc_I(pStepSize, (const Ipp32fc **)(state->ppRinSubbandHistory), fira_err,
            state->ppAdaptiveCoefs, numSegments, state->numSubbands);

        /* Constrain coeffs. */
        for (i = 0; i < numSegments; i++) {
            Ipp32f bsrc[SB_SUBBANDS * 4];
            Ipp32f bsrc1[SB_SUBBANDS * 4];
            Ipp32f bdst[SB_FFT_LEN];

            ippsFFTInv_CCSToR_32f((Ipp32f *)(state->ppAdaptiveCoefs[i]), bdst, state->spec_fft, state->pBuf);

            for (k = 0; k < SB_FRAME_SIZE; k++) {
                bsrc1[2 * k] = SB_FFT_LEN * bdst[k];
                bsrc1[2 * k + 1] = 0;
                bsrc1[2 * k + SB_FFT_LEN] = 0;
                bsrc1[2 * k + SB_FFT_LEN + 1] = 0;
            }

            ippsFFTFwd_CToC_32fc((const Ipp32fc *)bsrc1, (Ipp32fc *)bsrc, state->spec_fftC, state->pBuf);

            for (s = 0; s < SB_SUBBANDS; s++) {
                state->ppAdaptiveCoefs[i][s].re = bsrc[2 * s];
                state->ppAdaptiveCoefs[i][s].im = bsrc[2 * s + 1];
            }
        }

        /* Apply subband controller */
        ippsSubbandController_EC_32f(fira_err, firf_err, state->ppAdaptiveCoefs, state->ppFixedCoefs,
            &sgain, state->controller);

        /* Apply NLP coeff */
        if (NLP_ENABLED) // NLP Enabled
            for (i = 0; i < SB_FRAME_SIZE; i++)
                sout[i] *= sgain;
    }
    ah_ProcessFrame(state->ah,sout,sout);
    return 0;
}
