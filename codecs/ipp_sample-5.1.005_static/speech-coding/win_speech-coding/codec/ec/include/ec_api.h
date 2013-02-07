/* /////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2006 Intel Corporation. All Rights Reserved.
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
//  Purpose: Echo Canceller header file.
//
*/


#ifndef __EC_API_H__
#define __EC_API_H__

#include "ippsc.h"
#include "scratchmem.h"
#include "usc_ec.h"
#include "ah_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __fbECState fbECState;
typedef struct __sbECState sbECState;
typedef struct __sbfECState sbfECState;

typedef int (*ec_GetFrameSize_ptr)(int *s);
typedef int (*ec_GetSize_ptr)(IppPCMFrequency freq, int taptime_ms, int *s);
typedef int (*ec_GetSendPathDelay_ptr)(int *delay);
typedef int (*ec_Init_ptr)(void *state, IppPCMFrequency freq, int taptime_ms);
typedef int (*ec_ModeOp_ptr)(void *state, ECOpcode op);
typedef int (*ec_ProcessFrame_ptr)(void *state, Ipp32f *rin, Ipp32f *sin, Ipp32f *sout);
typedef int (*ec_setAHFreqShift_ptr)(void *state, int shift);
typedef int (*ec_setHDperiod_ptr)(void *state, int num);
typedef int (*ec_GetInfo_ptr)(void *state,ECOpcode *isNLP,ECOpcode *isTD,
                              ECOpcode *isAdapt,ECOpcode *isAdaptLite, ECOpcode *isAH);

int ec_fb_GetInfo(fbECState *state,ECOpcode *isNLP,ECOpcode *isTD,
                   ECOpcode *isAdapt,ECOpcode *isAdaptLite, ECOpcode *isAH);
int ec_sb_GetInfo(sbECState *state,ECOpcode *isNLP,ECOpcode *isTD,
                   ECOpcode *isAdapt,ECOpcode *isAdaptLite, ECOpcode *isAH);
int ec_sbf_GetInfo(sbfECState *state,ECOpcode *isNLP,ECOpcode *isTD,
                   ECOpcode *isAdapt,ECOpcode *isAdaptLite, ECOpcode *isAH);
/* Returns size of buffer for state structure */
int ec_fb_GetSize(IppPCMFrequency freq, int taptime_ms,int *s);
int ec_sb_GetSize(IppPCMFrequency freq, int taptime_ms,int *s);
int ec_sbf_GetSize(IppPCMFrequency freq, int taptime_ms,int *s);
/* Initializes state structure */
int ec_fb_Init(void *state, IppPCMFrequency freq, int taptime_ms);
int ec_sb_Init(void *state, IppPCMFrequency freq, int taptime_ms);
int ec_sbf_Init(void *state, IppPCMFrequency freq, int taptime_ms);

/* Do operation or set mode */
int ec_fb_ModeOp(void *state, ECOpcode op);
int ec_sb_ModeOp(void *state, ECOpcode op);
int ec_sbf_ModeOp(void *state, ECOpcode op);

/* Process one frame */
int ec_fb_ProcessFrame(void *state, Ipp32f *rin, Ipp32f *sin, Ipp32f *sout);
int ec_sb_ProcessFrame(void *state, Ipp32f *rin, Ipp32f *sin, Ipp32f *sout);
int ec_sbf_ProcessFrame(void *state, Ipp32f *rin, Ipp32f *sin, Ipp32f *sout);

/* Returns size of frame */
int ec_fb_GetFrameSize(int *s);
int ec_sbf_GetFrameSize(int *s);
int ec_sb_GetFrameSize(int *s);

/* Returns delay introduced to send-path */
int ec_fb_GetSendPathDelay(int *delay);
int ec_sb_GetSendPathDelay(int *delay);
int ec_sbf_GetSendPathDelay(int *delay);

int ec_sb_setAHFreqShift(void *state, int shift);
int ec_sbf_setAHFreqShift(void *state, int shift);
int ec_fb_setAHFreqShift(void *state, int shift);

int ec_sbf_setAHhowlPeriod(void *stat, int howlPeriod);
int ec_fb_setAHhowlPeriod(void *stat, int howlPeriod);
int ec_sb_setAHhowlPeriod(void *stat, int howlPeriod);
#ifdef __cplusplus
}
#endif
#endif /* __EC_API_H__ */
