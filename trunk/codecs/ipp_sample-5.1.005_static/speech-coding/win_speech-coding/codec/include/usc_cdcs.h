/*////////////////////////////////////////////////////////////////////////
//
// INTEL CORPORATION PROPRIETARY INFORMATION
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Intel Corporation and may not be copied
// or disclosed except in accordance with the terms of that agreement.
// Copyright (c) 2005-2006 Intel Corporation. All Rights Reserved.
//
//   Intel(R)  Integrated Performance Primitives
//
//     USC speech codec sample
//
// By downloading and installing this sample, you hereby agree that the
// accompanying Materials are being provided to you under the terms and
// conditions of the End User License Agreement for the Intel(R) Integrated
// Performance Primitives product previously accepted by you. Please refer
// to the file ipplic.htm located in the root directory of your Intel(R) IPP
// product installation for more information.
//
// Purpose: External USC function tables.
//
////////////////////////////////////////////////////////////////////////*/
#ifndef _USCCDCS_H_
#define _USCCDCS_H_

#include "usc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (defined _USC_G729 || defined _USC_ALL)
   USCFUN USC_Fxns USC_G729I_Fxns;
   USCFUN USC_Fxns USC_G729A_Fxns;
#endif
#if (defined _USC_G729FP || defined _USC_ALL)
   USCFUN USC_Fxns USC_G729IFP_Fxns;
   USCFUN USC_Fxns USC_G729AFP_Fxns;
#endif
#if (defined _USC_AMRWB || defined _USC_ALL)
   USCFUN USC_Fxns USC_AMRWB_Fxns;
#endif
#if (defined _USC_G728 || defined _USC_ALL)
   USCFUN USC_Fxns USC_G728_Fxns;
#endif
#if (defined _USC_G722 || defined _USC_ALL)
   USCFUN USC_Fxns USC_G722_Fxns;
#endif
#if (defined _USC_G726 || defined _USC_ALL)
   USCFUN USC_Fxns USC_G726_Fxns;
#endif
#if (defined _USC_G723 || defined _USC_ALL)
   USCFUN USC_Fxns USC_G723_Fxns;
#endif
#if (defined _USC_GSMAMR || defined _USC_ALL)
   USCFUN USC_Fxns USC_GSMAMR_Fxns;
#endif
#if (defined _USC_GSMFR || defined _USC_ALL)
   USCFUN USC_Fxns USC_GSMFR_Fxns;
#endif
#if (defined _USC_G722SB || defined _USC_ALL)
   USCFUN USC_Fxns USC_G722SB_Fxns;
#endif
#if (defined _USC_G711 || defined _USC_ALL)
   USCFUN USC_Fxns USC_G711A_Fxns;
   USCFUN USC_Fxns USC_G711U_Fxns;
#endif

#ifdef __cplusplus
}
#endif

#endif // _USCCDCS_H_
