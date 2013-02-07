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
//  Purpose: Echo Canceller, fast sub band algorithm
//
*/
#include <ipp_w7.h>
#include "ipps.h"
#include "ippsc.h"
#include "ec_api.h"
#include "ownec.h"
#include "ah_api.h"

int ec_sbf_setAHFreqShift(void *stat, int shift){
    int status=0;
    _sbfECState *state=(_sbfECState *)stat;
    state = IPP_ALIGNED_PTR(state, 16);
    status = ah_SetFreqShift(state->ah,shift);
    return status;
}
int ec_sbf_setAHhowlPeriod(void *stat, int howlPeriod){
    int status=0;
    _sbfECState *state=(_sbfECState *)stat;
    state = IPP_ALIGNED_PTR(state, 16);
    status = ah_SetHowlPeriod(state->ah,howlPeriod);
    return status;
}
int ec_sbf_GetInfo(sbfECState *state,ECOpcode *isNLP,ECOpcode *isTD,
                  ECOpcode *isAdapt,ECOpcode *isAdaptLite, ECOpcode *isAH)
{
    state = IPP_ALIGNED_PTR(state, 16);
    if(state->mode & 2)
        *isNLP = EC_NLP_ENABLE;
    else
        *isNLP = EC_NLP_DISABLE;

    if(state->mode & 4)
        *isTD = EC_TD_ENABLE;
    else
        *isTD = EC_TD_DISABLE;

    if(state->mode & 16)
        *isAH = EC_AH_ENABLE1;
    else
        *isAH = EC_AH_DISABLE;

    if(state->mode & 32)
        *isAH = EC_AH_ENABLE2;
    else
        *isAH = EC_AH_DISABLE;

    *isAdaptLite = EC_ADAPTATION_DISABLE_LITE;
    if(state->mode & 1){
        *isAdapt = EC_ADAPTATION_ENABLE;
    } else
        *isAdapt = EC_ADAPTATION_DISABLE;

    return 0;
}
/* Returns size of frame */
int ec_sbf_GetFrameSize(int *s)
{
    *s = SBF_FRAME_SIZE;
    return 0;
}

/* Returns size of buffer for state structure */
int ec_sbf_GetSize(IppPCMFrequency freq, int taptime_ms, int *s)
{
    int numSegments;
    int size = 0, csize;
    int sbsize, sbinitBufSize, sbbufSize;

    size += ALIGN(sizeof(_sbfECState));

    ippsSubbandProcessGetSize_32f(SBF_FFT_ORDER, SBF_WIN_LEN, &sbsize, &sbinitBufSize, &sbbufSize);

    size += ALIGN(sbsize);
    size += ALIGN(sbsize);
    size += ALIGN(sbsize);
    size += ALIGN(sbinitBufSize);
    size += ALIGN(sbbufSize);

    if (freq == IPP_PCM_FREQ_8000) {
        numSegments = (taptime_ms * 8 - 1) / SBF_FRAME_SIZE + 1;
    } else if (freq == IPP_PCM_FREQ_16000) {
        numSegments = (taptime_ms * 16 - 1) / SBF_FRAME_SIZE + 1;
    } else {
        return 1;
    }

    size += ALIGN(3 * numSegments * sizeof(Ipp32fc *));
    size += ALIGN(3 * numSegments * SBF_SUBBANDS * sizeof(Ipp32fc));

    ippsSubbandControllerGetSize_EC_32f(SBF_SUBBANDS, SBF_FRAME_SIZE, numSegments, freq, &sbsize);

    size += ALIGN(sbsize);

    ippsToneDetectGetStateSize_EC_32f(freq, &csize);
    size += ALIGN(csize * 2);
    ah_GetSize(SBF_FRAME_SIZE, &csize);
    size += ALIGN(csize);
    *s = ALIGN(size+15);
    return 0;
}

/* Returns delay introduced to send-path */
int ec_sbf_GetSendPathDelay(int *delay)
{
    *delay = SBF_WIN_LEN - SBF_FRAME_SIZE;
    return 0;
}

/* Initializes state structure */
int ec_sbf_Init(void *stat, IppPCMFrequency freq, int taptime_ms)
{
    _sbfECState *state=(_sbfECState *)stat;
    int i, numSegments, csize;
    int sbsize, sbinitBufSize, sbbufSize;
    Ipp8u  *ptr,*buf;
    state = IPP_ALIGNED_PTR(state, 16);

    ptr = (Ipp8u *)state;

    ptr += ALIGN(sizeof(_sbfECState));

/* Initialize SubbandProcess structures */
    ippsSubbandProcessGetSize_32f(SBF_FFT_ORDER, SBF_WIN_LEN, &sbsize, &sbinitBufSize, &sbbufSize);

    state->state_syn = (IppsSubbandProcessState_32f *)ptr;
    ptr += ALIGN(sbsize);
    state->state_ana_rin = (IppsSubbandProcessState_32f *)ptr;
    ptr += ALIGN(sbsize);
    state->state_ana_sin = (IppsSubbandProcessState_32f *)ptr;
    ptr += ALIGN(sbsize);

    if (ippsSubbandProcessInit_32f(state->state_syn,     SBF_FFT_ORDER, SBF_FRAME_SIZE, SBF_WIN_LEN, 0, ptr) != ippStsOk)
        return 1;
    if (ippsSubbandProcessInit_32f(state->state_ana_rin, SBF_FFT_ORDER, SBF_FRAME_SIZE, SBF_WIN_LEN, 0, ptr) != ippStsOk)
        return 1;
    if (ippsSubbandProcessInit_32f(state->state_ana_sin, SBF_FFT_ORDER, SBF_FRAME_SIZE, SBF_WIN_LEN, 0, ptr) != ippStsOk)
        return 1;

    ptr += ALIGN(sbinitBufSize);

    state->FFTSize = SBF_FFT_LEN;
    state->numSubbands = SBF_SUBBANDS;
    state->frameSize = SBF_FRAME_SIZE;
    state->windowLen = SBF_WIN_LEN;
    state->pBuf = ptr;
    ptr += ALIGN(sbbufSize);

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
    ptr += ALIGN(3 * numSegments * SBF_SUBBANDS * sizeof(Ipp32fc));

    /* Zero coeffs buffer and history buffer */
    ippsZero_32fc((Ipp32fc *)buf, 3 * numSegments * SBF_SUBBANDS);

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

    ippsSubbandControllerGetSize_EC_32f(state->numSubbands, state->frameSize, numSegments, freq, &csize);

    ptr += ALIGN(csize);

    state->ah = (ahState *)ptr;
    ah_Init(state->ah, freq, SBF_FRAME_SIZE);
    ah_GetSize(SBF_FRAME_SIZE, &csize);
    ptr += ALIGN(csize);

    /* Initialize tone detection */
    ippsToneDetectGetStateSize_EC_32f(freq, &csize);
    state->tdr = (IppsToneDetectState_EC_32f *)ptr;
    ptr += csize;
    state->tds = (IppsToneDetectState_EC_32f *)ptr;
    ippsToneDetectInit_EC_32f(state->tdr, freq);
    ippsToneDetectInit_EC_32f(state->tds, freq);

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
int ec_sbf_ModeOp(void *stat, ECOpcode op)
{
    _sbfECState *state=(_sbfECState *)stat;
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
static int ec_sbf_ToneDisabler(void *stat, Ipp32f *r_in, Ipp32f *s_in)
{
    _sbfECState *state=(_sbfECState *)stat;
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
            state->td_resr = state->td_ress = 0;
            if (state->td_mode & 1)
                ec_sbf_ModeOp(state, EC_ADAPTATION_ENABLE);
            if (state->td_mode & 2)
                ec_sbf_ModeOp(state, EC_NLP_ENABLE);
        }
    } else if (resr || ress) {
/* Zero coeffs, disable adaptation and NLP */
        if (resr) state->td_resr = 1;
        if (ress) state->td_ress = 1;

        state->td_mode = state->mode;
        ec_sbf_ModeOp(state, EC_COEFFS_ZERO);
        ec_sbf_ModeOp(state, EC_ADAPTATION_DISABLE);
        ec_sbf_ModeOp(state, EC_NLP_DISABLE);
    }

    return 0;
}

/* Process one frame */
int ec_sbf_ProcessFrame(void *stat, Ipp32f *rin, Ipp32f *sin, Ipp32f *sout)
{
    _sbfECState *state=(_sbfECState *)stat;
    Ipp32fc sin_sub[SBF_SUBBANDS];
    Ipp32fc fira_err[SBF_SUBBANDS], firf_err[SBF_SUBBANDS];
    Ipp32fc fira_out[SBF_SUBBANDS], firf_out[SBF_SUBBANDS];
    double pStepSize[SBF_SUBBANDS];
    Ipp32f sgain;
    Ipp32fc* pSegment; /* Pointer to segment of input history */
    int iSegment;
    int numSegments;
    state = IPP_ALIGNED_PTR(state, 16);
    numSegments = state->numSegments;

    /* Tone Disabler */
    if (TD_ENABLED)
        ec_sbf_ToneDisabler(state, rin, sin);

    /* update receive-in subband history buffer */
    pSegment = state->ppRinSubbandHistory[state->numSegments-1];
    for(iSegment = state->numSegments-1; iSegment > 0; iSegment--)
    {
      state->ppRinSubbandHistory[iSegment] = state->ppRinSubbandHistory[iSegment-1];
    }
    state->ppRinSubbandHistory[0] = pSegment;

    /* do subband analysis */
    ippsSubbandAnalysis_32f32fc(rin, state->ppRinSubbandHistory[0], state->state_ana_rin, state->pBuf);
    ippsSubbandAnalysis_32f32fc(sin, sin_sub, state->state_ana_sin, state->pBuf);

    if (ADAPTATION_ENABLED) {
        /* update subband controller */
        ippsSubbandControllerUpdate_EC_32f(rin, sin, (const Ipp32fc **)(state->ppRinSubbandHistory),
            sin_sub, pStepSize, state->controller);

        /* do filtering with adaptive coeffs */
        ippsFIRSubband_EC_32fc(state->ppRinSubbandHistory, state->ppAdaptiveCoefs, fira_out,
            numSegments, state->numSubbands);

        /* do filtering with fixed coeffs */
        ippsFIRSubband_EC_32fc(state->ppRinSubbandHistory, state->ppFixedCoefs, firf_out,
            numSegments, state->numSubbands);

        /* Get adaptive filter error */
        ippsSub_32fc(fira_out, sin_sub, fira_err, SBF_SUBBANDS);
        /* Get fixed filter error */
        ippsSub_32fc(firf_out, sin_sub, firf_err, SBF_SUBBANDS);

        /* Update adaptive coeffs */
        ippsFIRSubbandCoeffUpdate_EC_32fc_I(pStepSize, (const Ipp32fc **)(state->ppRinSubbandHistory), fira_err,
            state->ppAdaptiveCoefs, numSegments, state->numSubbands);

        /* Apply subband controller */
        ippsSubbandController_EC_32f(fira_err, firf_err, state->ppAdaptiveCoefs, state->ppFixedCoefs,
            &sgain, state->controller);

        /* do subband synthesis */
        ippsSubbandSynthesis_32fc32f(firf_err, sout, state->state_syn, state->pBuf);

        /* Apply NLP coeff */
        ah_ProcessFrame(state->ah,sout,sout);
        if (NLP_ENABLED)
            ippsMulC_32f_I(sgain, sout, SBF_FRAME_SIZE);
    } else {
        /* do filtering with fixed coeffs */
        ippsFIRSubband_EC_32fc(state->ppRinSubbandHistory, state->ppFixedCoefs, firf_out,
            numSegments, state->numSubbands);

        /* Get fixed filter error */
        ippsSub_32fc(firf_out, sin_sub, firf_err, SBF_SUBBANDS);

        /* do subband synthesis */
        ippsSubbandSynthesis_32fc32f(firf_err, sout, state->state_syn, state->pBuf);
        ah_ProcessFrame(state->ah,sout,sout);
    }
    return 0;
}
