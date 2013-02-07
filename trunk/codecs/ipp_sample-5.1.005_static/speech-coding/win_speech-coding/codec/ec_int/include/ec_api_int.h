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
//
//  Purpose: Echo Canceller header file.
//
*/

#ifndef __EC_API_INT_H__
#define __EC_API_INT_H__
#include <ipp_w7.h>
#include <ippsc.h>
#include "scratchmem.h"
#include "usc_ec.h"
#include "ah_api_int.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __ifbECState ifbECState;
typedef struct __isbECState isbECState;
typedef struct __isbfECState isbfECState;

typedef int (*ec_GetFrameSizeI_ptr)(IppPCMFrequency freq, int taptime_ms, int *s);
typedef int (*ec_GetSizeI_ptr)(IppPCMFrequency freq, int taptime_ms, int *s);
typedef int (*ec_GetSendPathDelayI_ptr)(int *delay);
typedef int (*ec_InitI_ptr)(void *state, IppPCMFrequency freq, int taptime_ms);
typedef int (*ec_ModeOpI_ptr)(void *state, ECOpcode op);
typedef int (*ec_ProcessFrameI_ptr)(void *state, const Ipp16s *rin, Ipp16s *sin, Ipp16s *sout);
typedef int (*ec_setAHFreqShiftI_ptr)(void *state, int shift);
typedef int (*ec_setHDperiodI_ptr)(void *state, int num);
typedef int (*ec_GetInfo_ptr)(void *state,ECOpcode *isNLP,ECOpcode *isTD,
             ECOpcode *isAdapt,ECOpcode *isAdaptLite, ECOpcode *isAH);
typedef int (*ec_GetSubbandNum_ptr)(void *state);

int ec_ifb_GetSize(IppPCMFrequency freq, int taptime_ms, int *s);
int ec_ifb_Init(ifbECState *state, IppPCMFrequency freq, int taptime_ms);
int ec_ifb_InitBuff(ifbECState *state, char *buff);
int ec_ifb_ModeOp(ifbECState *state, ECOpcode op);
int ec_ifb_ProcessFrame(ifbECState *state, Ipp16s *rin, Ipp16s *sin, Ipp16s *sout);
int ec_ifb_GetFrameSize(IppPCMFrequency freq, int taptime_ms, int *s);
int ec_ifb_GetSendPathDelay(int *delay);
int ec_ifb_GetNLPGain(ifbECState *state);
int ec_ifb_GetInfo(ifbECState *state,ECOpcode *isNLP,ECOpcode *isTD,
                   ECOpcode *isAdapt,ECOpcode *isAdaptLite, ECOpcode *isAH);
int ec_ifb_setAHFreqShift(void *state, int shift);
int ec_ifb_GetSubbandNum(ifbECState *state);

int ec_isb_GetSize(IppPCMFrequency freq, int taptime_ms, int *s);
int ec_isb_Init(isbECState *state, IppPCMFrequency freq, int taptime_ms);
int ec_isb_InitBuff(isbECState *state, char *buff);
int ec_isb_ModeOp(isbECState *state, ECOpcode op);
int ec_isb_ModeCheck(isbECState *state, ECOpcode op);
int ec_isb_GetSegNum(isbECState *state);
int ec_isb_GetSubbandNum(isbECState *state);
int ec_isb_GetNLPGain(isbECState *state);

int ec_isb_ProcessFrame(isbECState *state, Ipp16s *rin, Ipp16s *sin, Ipp16s *sout);
int ec_isb_GetFrameSize(IppPCMFrequency freq, int taptime_ms, int *s);
int ec_isb_GetSendPathDelay(int *delay);
int ec_isb_GetInfo(isbECState *state,ECOpcode *isNLP,ECOpcode *isTD,
                   ECOpcode *isAdapt,ECOpcode *isAdaptLite, ECOpcode *isAH);
int ec_isb_setAHFreqShift(void *state, int shift);
int ec_isb_GetSubbandNum(isbECState *state);

int ec_isbf_GetSize(IppPCMFrequency freq, int taptime_ms, int *s);
int ec_isbf_Init(isbfECState *state, IppPCMFrequency freq, int taptime_ms);
int ec_isbf_InitBuff(isbfECState *state, char *buff);
int ec_isbf_ModeOp(isbfECState *state, ECOpcode op);
int ec_isbf_ProcessFrame(isbfECState *state, Ipp16s *rin, Ipp16s *sin, Ipp16s *sout);
int ec_isbf_GetFrameSize(IppPCMFrequency freq, int taptime_ms, int *s);
int ec_isbf_GetSendPathDelay(int *delay);
int ec_isbf_GetNLPGain(isbfECState *state);

int ec_isbf_GetInfo(isbfECState *state,ECOpcode *isNLP,ECOpcode *isTD,
                    ECOpcode *isAdapt,ECOpcode *isAdaptLite, ECOpcode *isAH);
int ec_isbf_setAHFreqShift(void *state, int shift);
int ec_isbf_GetSubbandNum(isbfECState *state);

int ec_isbf_setAHhowlPeriod(void *stat, int howlPeriod);
int ec_ifb_setAHhowlPeriod(void *stat, int howlPeriod);
int ec_isb_setAHhowlPeriod(void *stat, int howlPeriod);

/* sb - 6544 , sbf - 1644 , fb */
#define EC_SCRATCH_MEMORY_SIZE        (6544)


#ifdef __cplusplus
}
#endif

#endif /* __EC_API_INT_H__ */
