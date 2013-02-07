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
//  Purpose: Echo Canceller, fast subband algorithm
//
*/

#include <stdlib.h>
#include <ipp_w7.h>
#include "ipps.h"
#include "ippsc.h"
#include "ec_api_int.h"
#include "ownec_int.h"
#include "ah_api_int.h"

int ec_isbf_GetSubbandNum(isbfECState *state)
{
    state = IPP_ALIGNED_PTR(state, 16);
    return state->numSubbands;
}
int ec_isbf_setAHFreqShift(void *stat, int shift){
    int status=0;
    _isbfECState *state=(_isbfECState *)stat;
    state = IPP_ALIGNED_PTR(state, 16);
    status = ah_SetFreqShift_int(state->ah,shift);
    return status;
}
int ec_isbf_setAHhowlPeriod(void *stat, int howlPeriod){
    int status=0;
    _isbfECState *state=(_isbfECState *)stat;
    state = IPP_ALIGNED_PTR(state, 16);
    status = ah_SetHowlPeriod_int(state->ah,howlPeriod);
    return status;
}
/* init scratch memory buffer */
int ec_isbf_InitBuff(_isbfECState *state, char *buff) {
    if (buff==NULL) {
        return 1;
    }
    if (state==NULL) {
        return 1;
    }
#if !defined (NO_SCRATCH_MEMORY_USED)
    state = IPP_ALIGNED_PTR(state, 16);
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

/* Returns size of frame */
int ec_isbf_GetFrameSize(IppPCMFrequency freq, int taptime_ms, int *s)
{
    taptime_ms=0;freq=(IppPCMFrequency)0;
    *s = SBF_FRAME_SIZE;
    return 0;
}

/* Returns size of buffer for state structure */
int ec_isbf_GetSize(IppPCMFrequency freq, int taptime_ms, int *s)
{
    int numSegments;
    int size = 0, csize;
    int sbsize, sbinitBufSize, sbbufSize;

    size += ALIGN(sizeof(_isbfECState));

    ippsSubbandProcessGetSize_16s(SBF_FFT_ORDER, SBF_WIN_LEN, &sbsize, &sbinitBufSize, &sbbufSize);

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

    size += ALIGN(3 * numSegments * sizeof(Ipp32sc *));
    size += ALIGN(3 * numSegments * SBF_SUBBANDS * sizeof(Ipp32sc));

    ippsSubbandControllerGetSize_EC_16s(SBF_SUBBANDS, SBF_FRAME_SIZE, numSegments, freq, &sbsize);

    size += ALIGN(sbsize);

    ippsToneDetectGetStateSize_EC_16s(freq, &csize);
    size += ALIGN(csize * 2);

    ah_GetSize_int(SBF_FRAME_SIZE, &csize);
    size += ALIGN(csize);
    *s = ALIGN(size+15);

    return 0;
}

/* Returns delay introduced to send-path */
int ec_isbf_GetSendPathDelay(int *delay)
{
    *delay = SBF_WIN_LEN - SBF_FRAME_SIZE;
    return 0;
}

/* acquire NLP gain coeff */
int ec_isbf_GetNLPGain(_isbfECState *state)
{
    state = IPP_ALIGNED_PTR(state, 16);
    return state->sgain;
}
/* Initializes state structure */
int ec_isbf_Init(_isbfECState *state, IppPCMFrequency freq, int taptime_ms)
{
    int i, numSegments, csize;
    int sbsize, sbinitBufSize, sbbufSize;
    Ipp8u *ptr, *buf;
    state = IPP_ALIGNED_PTR(state, 16);
    ptr = (Ipp8u *)state;

    ptr += ALIGN(sizeof(_isbfECState));

/* Initialize SubbandProcess structures */
    ippsSubbandProcessGetSize_16s(SBF_FFT_ORDER, SBF_WIN_LEN, &sbsize, &sbinitBufSize, &sbbufSize);

    state->state_syn = (IppsSubbandProcessState_16s *)ptr;
    ptr += ALIGN(sbsize);
    state->state_ana_rin = (IppsSubbandProcessState_16s *)ptr;
    ptr += ALIGN(sbsize);
    state->state_ana_sin = (IppsSubbandProcessState_16s *)ptr;
    ptr += ALIGN(sbsize);

    if (ippsSubbandProcessInit_16s(state->state_syn,     SBF_FFT_ORDER, SBF_FRAME_SIZE, SBF_WIN_LEN, 0, ptr) != ippStsOk)
        return 1;// zero state->pSigBuf[256]
    if (ippsSubbandProcessInit_16s(state->state_ana_rin, SBF_FFT_ORDER, SBF_FRAME_SIZE, SBF_WIN_LEN, 0, ptr) != ippStsOk)
        return 1;// zero state->pSigBuf[256]
    if (ippsSubbandProcessInit_16s(state->state_ana_sin, SBF_FFT_ORDER, SBF_FRAME_SIZE, SBF_WIN_LEN, 0, ptr) != ippStsOk)
        return 1;// zero state->pSigBuf[256]

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
    state->ppRinSubbandHistory = (Ipp32sc **)ptr;
    ptr += ALIGN(3 * numSegments * sizeof(Ipp32sc *));
    /* adaptive coeffs */
    state->ppAdaptiveCoefs = state->ppRinSubbandHistory + numSegments;
    /* fixed coeffs */
    state->ppFixedCoefs = state->ppRinSubbandHistory + numSegments * 2;

    buf = ptr;
    ptr += ALIGN(3 * numSegments * SBF_SUBBANDS * sizeof(Ipp32sc));

    /* Zero coeffs buffer and history buffer */
    ippsZero_32sc((Ipp32sc *)buf, 3 * numSegments * SBF_SUBBANDS);

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

    ippsSubbandControllerGetSize_EC_16s(state->numSubbands, state->frameSize, numSegments, freq, &csize);

    ptr += ALIGN(csize);

    state->ah = (ahState_int *)ptr;
    ah_Init_int(state->ah, freq, SBF_FRAME_SIZE);
    ah_GetSize_int(SBF_FRAME_SIZE, &csize);
    ptr += ALIGN(csize);
    /* Initialize tone detection */
    ippsToneDetectGetStateSize_EC_16s(freq, &csize);
    state->tdr = (IppsToneDetectState_EC_16s *)ptr;
    ptr += csize;
    state->tds = (IppsToneDetectState_EC_16s *)ptr;
    ippsToneDetectInit_EC_16s(state->tdr, freq);
    ippsToneDetectInit_EC_16s(state->tds, freq);

    /* enable adaptation */
    state->mode = 1;
    state->r_in_pwr = 0;
    state->s_in_pwr = 0;
    state->td_coeff = COEF_ONE - COEF_ONE * 4 / state->frameSize / freq;
    state->td_thres = 57344;
    state->td_resr = state->td_ress = 0;
    return 0;
}

int ec_isbf_GetInfo(isbfECState *state,ECOpcode *isNLP,ECOpcode *isTD,
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
        if(state->mode & 8)
            *isAdaptLite = EC_ADAPTATION_ENABLE_LITE;
    } else
        *isAdapt = EC_ADAPTATION_DISABLE;

    return 0;
}
/* Do operation or set mode */
int ec_isbf_ModeOp(_isbfECState *state, ECOpcode op)
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
    case(EC_ADAPTATION_ENABLE_LITE): /* Enable adaptation */
        if (!(state->mode & 1))
            ippsSubbandControllerReset_EC_16s(state->controller);
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
static int ec_isbf_ToneDisabler(_isbfECState *state, Ipp16s *r_in, Ipp16s *s_in)
{
    int     resr, ress;
    int     i;
    Ipp64s  r_in_pwr = 0, s_in_pwr = 0;
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
/* Restore previous mode */
            state->td_resr = state->td_ress = 0;
            if (state->td_mode & 1)
                ec_isbf_ModeOp(state, EC_ADAPTATION_ENABLE);
            if (state->td_mode & 2)
                ec_isbf_ModeOp(state, EC_NLP_ENABLE);
        }
    } else if (resr || ress) {
/* Zero coeffs, disable adaptation and NLP */
        if (resr) state->td_resr = 1;
        if (ress) state->td_ress = 1;

        state->td_mode = state->mode;
        ec_isbf_ModeOp(state, EC_COEFFS_ZERO);
        ec_isbf_ModeOp(state, EC_ADAPTATION_DISABLE);
        ec_isbf_ModeOp(state, EC_NLP_DISABLE);
    }

    return 0;
}

/* Process one frame */
int ec_isbf_ProcessFrame(_isbfECState *state, Ipp16s *rin, Ipp16s *sin, Ipp16s *sout)
{
    LOCAL_ALIGN_ARRAY(32,Ipp32sc,sin_sub,SBF_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY(32,Ipp32sc,fira_err,SBF_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY(32,Ipp32sc,firf_err,SBF_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY(32,Ipp32sc,fira_out,SBF_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY(32,Ipp32sc,firf_out,SBF_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY(32,Ipp32s_EC_Sfs,pStepSize,SBF_SUBBANDS,state);
    int     numSegments;
    Ipp32sc* pSegment; /* Pointer to segment of input history */
    int     iSegment;
    state = IPP_ALIGNED_PTR(state, 16);
    numSegments = state->numSegments;
    /* Tone Disabler */
    if (TD_ENABLED)
        ec_isbf_ToneDisabler(state, rin, sin);
    /* update receive-in subband history buffer */
    pSegment = state->ppRinSubbandHistory[state->numSegments-1];
    for(iSegment = state->numSegments-1; iSegment > 0; iSegment--)
    {
      state->ppRinSubbandHistory[iSegment] = state->ppRinSubbandHistory[iSegment-1];
    }
    state->ppRinSubbandHistory[0] = pSegment;

    /* do subband analysis */
    ippsSubbandAnalysis_16s32sc_Sfs(rin, state->ppRinSubbandHistory[0], state->state_ana_rin, 0, state->pBuf);
    ippsSubbandAnalysis_16s32sc_Sfs(sin, sin_sub, state->state_ana_sin, 0, state->pBuf);

    if (ADAPTATION_ENABLED) {
        /* update subband controller */
        ippsSubbandControllerUpdate_EC_16s(rin, sin, (const Ipp32sc **)(state->ppRinSubbandHistory),
            sin_sub, pStepSize, state->controller);

        /* do filtering with adaptive coeffs */
        ippsFIRSubband_EC_32sc_Sfs(state->ppRinSubbandHistory, state->ppAdaptiveCoefs, fira_out,
            numSegments, state->numSubbands, F_SF);

        /* do filtering with fixed coeffs */
        ippsFIRSubband_EC_32sc_Sfs(state->ppRinSubbandHistory, state->ppFixedCoefs, firf_out,
            numSegments, state->numSubbands, F_SF);

        /* Get adaptive filter error */
        ippsSub_32sc_Sfs(fira_out, sin_sub, fira_err, SBF_SUBBANDS, 0);
        /* Get fixed filter error */
        ippsSub_32sc_Sfs(firf_out, sin_sub, firf_err, SBF_SUBBANDS, 0);

        /* Update adaptive coeffs */
        ippsFIRSubbandCoeffUpdate_EC_32sc_I(pStepSize, (const Ipp32sc **)(state->ppRinSubbandHistory), fira_err,
            state->ppAdaptiveCoefs, numSegments, state->numSubbands, F_SF);

        /* Apply subband controller */
        ippsSubbandController_EC_16s(fira_err, firf_err, state->ppAdaptiveCoefs, state->ppFixedCoefs,
            &state->sgain, state->controller);

        /* do subband synthesis */
        ippsSubbandSynthesis_32sc16s_Sfs(firf_err, sout, state->state_syn, 0, state->pBuf);

        /* Apply NLP coeff */
        ah_ProcessFrame_int(state->ah,sout,sout);
        if (NLP_ENABLED)
            ippsMulC_16s_ISfs(state->sgain, sout, SBF_FRAME_SIZE, 15);
    } else {
        /* do filtering with fixed coeffs */
        ippsFIRSubband_EC_32sc_Sfs(state->ppRinSubbandHistory, state->ppFixedCoefs, firf_out,
            numSegments, state->numSubbands, F_SF);

        /* Get fixed filter error */
        ippsSub_32sc_Sfs(firf_out, sin_sub, firf_err, SBF_SUBBANDS, 0);

        /* do subband synthesis */
        ippsSubbandSynthesis_32sc16s_Sfs(firf_err, sout, state->state_syn, 0, state->pBuf);
    }
    if (!ADAPTATION_ENABLED)
        ah_ProcessFrame_int(state->ah,sout,sout);

    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32s_EC_Sfs,pStepSize,SBF_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32sc,firf_out,SBF_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32sc,fira_out,SBF_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32sc,firf_err,SBF_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32sc,fira_err,SBF_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32sc,sin_sub,SBF_SUBBANDS,state);

    return 0;
}
