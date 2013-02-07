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
//  Purpose: AH header file.
//
*/


#ifndef __AH_API_INT_H__
#define __AH_API_INT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __ahState_int ahState_int;
/* set mode */
int ah_SetMode_int(ahState_int *state, AHOpcode op);

/* set frequency shift value */
int ah_SetFreqShift_int(ahState_int *state, int shift);

/* Returns size of buffer for state structure */
int ah_GetSize_int(int frameSize, int *size);

/* Set the number of frames to be filtered after the howling has been detected */
int ah_SetHowlPeriod_int(ahState_int *state,int period);

/* Initializes state structure */
int ah_Init_int(ahState_int *state,int samplingRate,int frameSize);

/* Process one frame */
AHOpcode ah_ProcessFrame_int(ahState_int *state, Ipp16s *sin, Ipp16s *sout);

#ifdef __cplusplus
}
#endif
#endif /* __AH_API_INT_H__ */
