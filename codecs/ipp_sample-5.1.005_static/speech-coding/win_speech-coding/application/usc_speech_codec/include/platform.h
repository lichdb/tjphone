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
// Purpose: Purpose: speech codec platform header file.
//
////////////////////////////////////////////////////////////////////////*/
#ifndef _PLATFORM_H
#define _PLATFORM_H

#include <stdio.h>
#include <ipp_w7.h>
#include <ipps.h>
#include <ippsc.h>
#include <ippcore.h>
#if !defined( _WIN32_WCE )
#include <time.h>
#else /*Win CE*/
#include <windows.h>
#include <winbase.h>
#endif/*!_WIN32_WCE*/

typedef struct {
   Ipp64u p_start;
   Ipp64u p_end;
   Ipp64u p_diff;
   float speed_in_mhz;
   int k;
}MeasureIt;

void measure_opendrv(MeasureIt *m1, MeasureIt *m2);
void measure_closedrv(MeasureIt *m1, MeasureIt *m2);

void measure_start(MeasureIt *m);
void measure_end(MeasureIt *m);
void measure_init(MeasureIt *m);
void measure_pause(MeasureIt *m);
void measure_finish(MeasureIt *m);
void measure_output(FILE *f_log, MeasureIt *m, double speech_sec, int nTh, int optionreports);

#endif /* _PLATFORM_H */
