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


#ifndef __AH_API_H__
#define __AH_API_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __ahState ahState;
/* set mode */
int ah_SetMode(ahState *state, AHOpcode op);

/* set frequency shift value */
int ah_SetFreqShift(ahState *state, int shift);

/* Returns size of buffer for state structure */
int ah_GetSize(int frameSize, int *size);

/* Set the number of frames to be filtered after the howling has been detected */
int ah_SetHowlPeriod(ahState *state,int period);

/* Initializes state structure */
int ah_Init(ahState *state,int samplingRate,int frameSize);

/* Process one frame */
AHOpcode ah_ProcessFrame(ahState *state, Ipp32f *sin, Ipp32f *sout);

#ifdef __cplusplus
}
#endif
#endif /* __AH_API_H__ */
