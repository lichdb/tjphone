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
// Purpose: speech codec platform file.
//
////////////////////////////////////////////////////////////////////////*/
#include "platform.h"

/* This object and functions are used to measure codec speed */
#if defined _USC_TIME_DRV
   #define ENABLE_COUNTER          100
   #define DISABLE_COUNTER         101
   #define GET_TSC_LOW             102
   #define GET_TSC_HIGH            103
#endif

static Ipp64s diffclocks(Ipp64u s, Ipp64u e)
{
   if(s < e) return (e-s);
   return (IPP_MAX_64S-e+s);
}

static double getUSec(Ipp64u s, Ipp64u e)
{
   return (double) (diffclocks(s,e)/1000000.0);
}

#if defined _USC_TIME_DRV
   void measure_opendrv(MeasureIt *m1, MeasureIt *m2)
   {
      m1->k = -1;
      m2->k = -1;
      m1->k = open("/dev/tsc", 0);
      if(m1->k > 0) {
         m2->k = m1->k;
         ioctl(m1->k, ENABLE_COUNTER, 0);
      }
      return;
   }
   void measure_closedrv(MeasureIt *m1, MeasureIt *m2)
   {
      if(m1->k > 0) {
         ioctl(m1->k, DISABLE_COUNTER, 0);
         close(m1->k);
         m1->k = -1;
         m2->k = -1;
      }
      return;
   }
#else
   void measure_opendrv(MeasureIt *m1, MeasureIt *m2)
   {
      m1->k = -1;
      m2->k = -1;
      return;
   }
   void measure_closedrv(MeasureIt *m1, MeasureIt *m2)
   {
      m1->k = -1;
      m2->k = -1;
      return;
   }
#endif
void measure_start(MeasureIt *m)
{
#if defined _USC_TIME_DRV
   if(m->k > 0) {
      unsigned int startHigh, startLow;
      startLow   = ioctl(m->k, GET_TSC_LOW, 0);
      startHigh  = ioctl(m->k, GET_TSC_HIGH, 0);
      m->p_start = ((Ipp64u)startHigh << 32) + (Ipp64u)startLow;
   } else {
      m->p_start = ippGetCpuClocks();
   }
#else
   m->p_start = ippGetCpuClocks();
#endif
}

void measure_end(MeasureIt *m)
{
#if defined _USC_TIME_DRV
   if(m->k > 0) {
      unsigned int startHigh, startLow;
      startLow   = ioctl(m->k, GET_TSC_LOW, 0);
      startHigh  = ioctl(m->k, GET_TSC_HIGH, 0);
      m->p_end = ((Ipp64u)startHigh << 32) + (Ipp64u)startLow;
   } else {
      m->p_end = ippGetCpuClocks();
   }
#else
   m->p_end = ippGetCpuClocks();
#endif
}

void measure_init(MeasureIt *m)
{
   m->p_diff = 0;
}
void measure_pause(MeasureIt *m)
{
#if defined _USC_TIME_DRV
   if(m->k > 0) {
      unsigned int startHigh, startLow;
      startLow   = ioctl(m->k, GET_TSC_LOW, 0);
      startHigh  = ioctl(m->k, GET_TSC_HIGH, 0);
      m->p_end = ((Ipp64u)startHigh << 32) + (Ipp64u)startLow;
   } else {
      m->p_end = ippGetCpuClocks();
   }
   m->p_diff += diffclocks(m->p_start,m->p_end);
#else
   m->p_end = ippGetCpuClocks();
   m->p_diff += diffclocks(m->p_start,m->p_end);
#endif
}

void measure_finish(MeasureIt *m)
{
   m->p_start = 0;
   m->p_end = m->p_diff;
}

void measure_output(FILE *f_log, MeasureIt *m, double speech_sec, int nTh, int optionreports)
{
   m->speed_in_mhz = (float)(getUSec(m->p_start,m->p_end)/(nTh*speech_sec));
#if defined( _USC_ARCH_XSC )
   {
       int MHz;
       ippGetCpuFreqMhz(&MHz); // for XScale platforms it is necessary
       m->speed_in_mhz *= MHz; // to multiply on CPU frequency
   }
#endif

   fprintf(f_log,"%4.2f MHz per channel\n",m->speed_in_mhz);
   optionreports = optionreports;/*Fust to avoid warning*/
}
/**/
