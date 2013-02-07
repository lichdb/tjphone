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
//  Purpose: Echo Canceller, fullband algorithm
//
*/

#include <stdlib.h>
#include <ipp_w7.h>
#include "ipps.h"
#include "ippsc.h"
#include "ec_api_int.h"
#include "ownec_int.h"
#include "ah_api_int.h"

int ec_ifb_GetSubbandNum(ifbECState *state)
{
    if (state==NULL) {
        return -1;
    }
    return 0;
}
int ec_ifb_setAHFreqShift(void *stat, int shift){
    int status=0;
    _ifbECState *state=(_ifbECState *)stat;
    state = IPP_ALIGNED_PTR(state, 16);
    status = ah_SetFreqShift_int(state->ah,shift);
    return status;
}
int ec_ifb_setAHhowlPeriod(void *stat, int howlPeriod){
    int status=0;
    _ifbECState *state=(_ifbECState *)stat;
    state = IPP_ALIGNED_PTR(state, 16);
    status = ah_SetHowlPeriod_int(state->ah,howlPeriod);
    return status;
}
/* init scratch memory buffer */
int ec_ifb_InitBuff(_ifbECState *state, char *buff) {
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
int ec_ifb_GetFrameSize(IppPCMFrequency freq, int taptime_ms, int *s)
{
    freq=(IppPCMFrequency)0;taptime_ms=0;
    *s = FB_FRAME_SIZE;
    return 0;
}

/* Returns size of buffer for state structure */
int ec_ifb_GetSize(IppPCMFrequency freq, int taptime_ms, int *s)
{
    int size = 0, tap, tap_m, csize;

    if (freq == IPP_PCM_FREQ_8000) {
        tap = taptime_ms * 8;
        tap_m = tap + 2000;
    } else if (freq == IPP_PCM_FREQ_16000) {
        tap = taptime_ms * 16;
        tap_m = tap + 4000;
    } else {
        return 1;
    }

    size += ALIGN(sizeof(_ifbECState));

    size += ALIGN(tap * sizeof(Ipp16s));
    size += ALIGN(tap * sizeof(Ipp16s));

    size += ALIGN(tap_m * sizeof(Ipp16s));

    ippsFullbandControllerGetSize_EC_16s(FB_FRAME_SIZE, tap, freq, &csize);

    size += ALIGN(csize);

    ippsToneDetectGetStateSize_EC_16s(freq, &csize);
    size += ALIGN(csize * 2);

    ah_GetSize_int(FB_FRAME_SIZE, &csize);
    size += ALIGN(csize);
    *s = ALIGN(size+15);

    return 0;
}

/* Returns delay introduced to send-path */
int ec_ifb_GetSendPathDelay(int *delay)
{
    *delay = 0;
    return 0;
}
/* acquire NLP gain coeff */
int ec_ifb_GetNLPGain(_ifbECState *state)
{
    state = IPP_ALIGNED_PTR(state, 16);
    return state->sgain;
}

/* Initializes state structure */
int ec_ifb_Init(_ifbECState *state, IppPCMFrequency freq, int taptime_ms)
{
    int    i, tap, tap_m, csize;
    Ipp8u *ptr;
    state = IPP_ALIGNED_PTR(state, 16);
    ptr = (Ipp8u *)state;

    ptr += ALIGN(sizeof(_ifbECState));

    /* number of filter coeffs */
    if (freq == IPP_PCM_FREQ_8000) {
        tap = taptime_ms * 8;
        tap_m = tap + 2000;
        state->td_coeff = COEF_ONE - COEF_ONE * 4 * FB_FRAME_SIZE / 8000;
    } else if (freq == IPP_PCM_FREQ_16000) {
        tap = taptime_ms * 16;
        tap_m = tap + 4000;
        state->td_coeff = COEF_ONE - COEF_ONE * 4 * FB_FRAME_SIZE / 16000;
    } else {
        return 1;
    }

    state->tap = tap;
    /* size of history buffer */
    state->tap_m = tap_m;
    /* size of frame */
    state->frameSize = FB_FRAME_SIZE;

    /* fixed filter coeffs */
    state->firf_coef = (Ipp16s *)ptr;
    ptr += ALIGN(tap * sizeof(Ipp16s));
    /* adaptive filter coeffs */
    state->fira_coef = (Ipp16s *)ptr;
    ptr += ALIGN(tap * sizeof(Ipp16s));
    /* history buffer */
    state->rin_hist = (Ipp16s *)ptr;
    ptr += ALIGN(state->tap_m * sizeof(Ipp16s));

    /* enable adaptation */
    state->mode = 1;
    /* used in ToneDisabler */
    state->td_mode = 0;

    /* current position in history buffer */
    state->pos = tap + 1;

    /* error of NLMS */
    state->err = 0;
    /* receive_in signal power */
    state->r_in_pwr = 0;
    /* send_in signal power */
    state->s_in_pwr = 0;
    /* used in ToneDisabler */
    state->td_thres = 57344;
    state->td_resr = state->td_ress = 0;

    /* Initialize fullband controller */
    state->controller = (IppsFullbandControllerState_EC_16s *)ptr;

    ippsFullbandControllerGetSize_EC_16s((state->frameSize), tap, freq, &csize);
    ippsFullbandControllerInit_EC_16s(state->controller, (state->frameSize), state->tap, freq);

    ptr += ALIGN(csize);

    state->ah = (ahState_int *)ptr;
    ah_Init_int(state->ah, freq, FB_FRAME_SIZE);
    ah_GetSize_int(FB_FRAME_SIZE, &csize);
    ptr += ALIGN(csize);
    /* Initialize tone detection  */
    ippsToneDetectGetStateSize_EC_16s(freq, &csize);

    state->tdr = (IppsToneDetectState_EC_16s *)ptr;
    ptr += csize;
    state->tds = (IppsToneDetectState_EC_16s *)ptr;
    ippsToneDetectInit_EC_16s(state->tdr, freq);
    ippsToneDetectInit_EC_16s(state->tds, freq);

    /* Zero coeffs and history buffer */
    for (i = 0; i < tap; i++)
        state->fira_coef[i] = state->firf_coef[i] = 0;

    for (i = 0; i < state->tap_m; i++)
        state->rin_hist[i] = 0;

    return 0;
}

int ec_ifb_GetInfo(ifbECState *state,ECOpcode *isNLP,ECOpcode *isTD,
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
int ec_ifb_ModeOp(_ifbECState *state, ECOpcode op)
{
    int i;
    state = IPP_ALIGNED_PTR(state, 16);
    switch (op) {
    case (EC_COEFFS_ZERO):      /* Zero coeffs of filters */
        for (i = 0; i < state->tap; i++)
            state->fira_coef[i] = state->firf_coef[i] = 0;
        break;
    case(EC_ADAPTATION_ENABLE): /* Enable adaptation */
    case(EC_ADAPTATION_ENABLE_LITE): /* Enable lite adaptation */
        if (!(state->mode & 1))
            ippsFullbandControllerReset_EC_16s(state->controller);
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
static int ec_ifb_ToneDisabler(_ifbECState *state, Ipp16s *r_in, Ipp16s *s_in)
{
    int resr, ress;
    int i;
    Ipp64s r_in_pwr = 0, s_in_pwr = 0;
    int frameSize;
    state = IPP_ALIGNED_PTR(state, 16);
    frameSize = state->frameSize;

    /* Detect 2100 Hz with periodical phase reversal */
    ippsToneDetect_EC_16s(r_in, frameSize, &resr, state->tdr);
    ippsToneDetect_EC_16s(s_in, frameSize, &ress, state->tds);

    /* Update receive-in signal and send-in signal powers */
    for (i = 0; i < frameSize; i++) {
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
                ec_ifb_ModeOp(state, EC_ADAPTATION_ENABLE);
            if (state->td_mode & 2)
                ec_ifb_ModeOp(state, EC_NLP_ENABLE);
        }
    } else if (resr || ress) {
        if (resr) state->td_resr = 1;
        if (ress) state->td_ress = 1;
    /* Zero coeffs, disable adaptation and NLP */
        state->td_mode = state->mode;
        ec_ifb_ModeOp(state, EC_COEFFS_ZERO);
        ec_ifb_ModeOp(state, EC_ADAPTATION_DISABLE);
        ec_ifb_ModeOp(state, EC_NLP_DISABLE);
    }

    return 0;
}

/* Process one frame */
int ec_ifb_ProcessFrame(_ifbECState *state, Ipp16s *r_in, Ipp16s *s_in, Ipp16s *s_out)
{
    int i;
    LOCAL_ALIGN_ARRAY(32,Ipp32s,ss,FB_FRAME_SIZE_MAX,state);
    LOCAL_ALIGN_ARRAY(32,Ipp16s,s_out_a_buf,FB_FRAME_SIZE_MAX,state);
    Ipp16s *rin_hist;
    int tap;
    int pos;
    int frameSize;
    state = IPP_ALIGNED_PTR(state, 16);
    rin_hist = state->rin_hist;
    tap = state->tap;
    pos = state->pos;
    frameSize = state->frameSize;

    /* ToneDisabler */
    if (TD_ENABLED)
        ec_ifb_ToneDisabler(state, r_in, s_in);

    /* Update history buffer */
    for (i = 0; i < frameSize; i++) {
        rin_hist[pos + i] = r_in[i];
    }

    /* Do filtering with fixed coeffs */
    ippsFIR_EC_16s(state->rin_hist + pos, s_in, s_out, frameSize, state->firf_coef, tap);

    if (ADAPTATION_ENABLED) {
    /* Update fullband controller */
        ippsFullbandControllerUpdate_EC_16s(rin_hist + pos, s_in, ss, state->controller);
    /* Do NLMS filtering */
        ippsNLMS_EC_16s(state->rin_hist + pos, s_in, ss, s_out_a_buf, frameSize, state->fira_coef, tap, &(state->err));
    /* Apply fullband controller */
        ippsFullbandController_EC_16s(s_out_a_buf, s_out, state->fira_coef, state->firf_coef, &state->sgain, state->controller);

    /* Apply NLP coeff */
       ah_ProcessFrame_int(state->ah,s_out,s_out);
       if (NLP_ENABLED)
            for (i = 0; i < frameSize; i++)
                s_out[i] = (Ipp16s)SCALE((Ipp32s)s_out[i] * state->sgain, 15);
    }

    /* Update position in history buffer */
    pos += frameSize;

    if (pos + frameSize < state->tap_m) {
        state->pos = pos;
    } else {
        for (i = 0; i < tap + 1; i++)
            state->rin_hist[i] = state->rin_hist[i + pos - tap - 1];
        state->pos = tap + 1;
    }
    if (!ADAPTATION_ENABLED)
        ah_ProcessFrame_int(state->ah,s_out,s_out);

    LOCAL_ALIGN_ARRAY_FREE(32,Ipp16s,s_out_a_buf,FB_FRAME_SIZE_MAX,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32s,ss,FB_FRAME_SIZE_MAX,state);

    return 0;
}
