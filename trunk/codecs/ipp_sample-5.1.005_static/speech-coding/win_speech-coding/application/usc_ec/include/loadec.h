/*////////////////////////////////////////////////////////////////////////
//
// INTEL CORPORATION PROPRIETARY INFORMATION
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Intel Corporation and may not be copied
// or disclosed except in accordance with the terms of that agreement.
// Copyright (c) 2006 Intel Corporation. All Rights Reserved.
//
//   Intel(R)  Integrated Performance Primitives
//
//     USC Echo Canceller sample
//
// By downloading and installing this sample, you hereby agree that the
// accompanying Materials are being provided to you under the terms and
// conditions of the End User License Agreement for the Intel(R) Integrated
// Performance Primitives product previously accepted by you. Please refer
// to the file ipplic.htm located in the root directory of your Intel(R) IPP
// product installation for more information.
//
// Purpose: Load static EC components.
//
////////////////////////////////////////////////////////////////////////*/
#ifndef __LOADEC_H__
#define __LOADEC_H__

#include "usc_ec.h"
#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (defined _USC_EC_FP || defined _USC_EC_ALL)
   USCFUN USC_EC_Fxns USC_ECFP_Fxns;
#endif
//#if (defined _USC_EC_INT || defined _USC_EC_ALL)
   USCFUN USC_EC_Fxns USC_ECINT_Fxns;
//#endif


void EnumerateStaticLinkedEC(FILE *fptrLog);
int GetNumLinkedEC(void);
int LoadECByName(const char *name, USC_EC_Params *params, FILE *f_log);
#ifdef __cplusplus
}
#endif

#endif/* __LOADEC_H__*/
