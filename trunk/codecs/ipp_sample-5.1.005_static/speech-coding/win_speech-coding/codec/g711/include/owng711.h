/*/////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2004-2006 Intel Corporation. All Rights Reserved.
//
//     Intel(R) Integrated Performance Primitives
//     USC - Unified Speech Codec interface library
//
// By downloading and installing USC codec, you hereby agree that the
// accompanying Materials are being provided to you under the terms and
// conditions of the End User License Agreement for the Intel(R) Integrated
// Performance Primitives product previously accepted by you. Please refer
// to the file ipplic.htm located in the root directory of your Intel(R) IPP
// product installation for more information.
//
// A speech coding standards promoted by ITU, ETSI, 3GPP and other
// organizations. Implementations of these standards, or the standard enabled
// platforms may require licenses from various entities, including
// Intel Corporation.
//
//
// Purpose: G711 speech codec: main own header file.
//
*/

#ifndef __OWNG711_H__
#define __OWNG711_H__

#if defined( _WIN32_WCE)
#pragma warning( disable : 4505 )
#endif

#include <stdlib.h>
#include <ipp_w7.h>
#include <ipps.h>
#include <ippsc.h>
#include "g711api.h"
#include "scratchmem.h"

#if defined(__ICC) || defined( __ICL ) || defined ( __ECL )
    #define __INLINE static __inline
#elif defined( __GNUC__ )
    #define __INLINE static __inline__
#else
    #define __INLINE static
#endif
#include "scratchmem.h"

#define ENC_KEY 0xecd711
#define DEC_KEY 0xdec711

#define G711_ENCODER_SCRATCH_MEMORY_SIZE  (5120+40)

#define G711_CODECFUN(type,name,arg) extern type name arg

#define LP_FRAME_DIM        80
#define LP_SUBFRAME_DIM     40
#define BWF_HARMONIC        (1<<14)
#define BWF_HARMONIC_E      (1<<13)

#define PITCH_MIN       40                    /* minimum allowed pitch, 200 Hz */
#define PITCH_MAX       120                   /* maximum allowed pitch, 66 Hz */
#define POVERLAPMAX     (PITCH_MAX >> 2)      /* maximum pitch OLA window */
#define HISTORYLEN      (PITCH_MAX*3 + POVERLAPMAX) /* history buffer length*/

typedef struct _SynthesisFilterState {
    int    nTaps;
    short *buffer;
} SynthesisFilterState;

typedef struct _G711Coder_Obj {
    int                 objSize;
    int                 key;
    unsigned int        mode;
    G711Codec_Type      codecType;
} G711Coder_Obj;

typedef struct { /* State of Packet Loss Concealment */
  int        _erasecnt;                     /* consecutive erased frames */
  int        _poverlap;                     /* overlap based on pitch */
  int        _poffset;                      /* offset into pitch period */
  int        _pitch;                        /* pitch estimate */
  int        _pitchblen;                    /* current pitch buffer length */
  short      *_pitchbufend;                 /* end of pitch buffer */
  short      *_pitchbufstart;               /* start of pitch buffer */
  short      _pitchbuf[HISTORYLEN];         /* buffer for cycles of speech */
  short      _lastq[POVERLAPMAX];           /* saved last quarter wavelength */
  short      _history[HISTORYLEN];          /* history buffer */
} G711_PLC_state;

struct _G711Decoder_Obj {
    G711Coder_Obj  objPrm;
    G711_PLC_state PLCstate;
    G711Codec_Type codecType;
    void     *g729_obj;
    int      lastGoodFrame;
#if !defined (NO_SCRATCH_MEMORY_USED)
    ScratchMem_Obj Mem;
#endif
};

struct _G711Encoder_Obj {
    G711Coder_Obj  objPrm;
    G711Codec_Type codecType;
    void     *g729_obj;
    short    inputHistory[LP_SUBFRAME_DIM];
#if !defined (NO_SCRATCH_MEMORY_USED)
    ScratchMem_Obj Mem;
#endif
};

void PLC_init(G711_PLC_state *PLCstate);
void PLC_dofe(G711_PLC_state *PLCstate, short *s); /* synthesize speech for erasure */
void PLC_addtohistory(G711_PLC_state *PLCstate, short *s);  /* add a good frame to history buffer */

extern const short  NormTG729i[256];
extern const short  NormTG729i2[256];

__INLINE short Exp_16s_Pos(unsigned short x)
{
   if((x>>8)==0)
      return NormTG729i2[x];
   else {
      return NormTG729i[(x>>8)];
   }
}
__INLINE short Exp_32s_Pos(unsigned int x){
   if (x == 0) return 0;
   if((x>>16)==0) return (short)(16+Exp_16s_Pos((unsigned short) x));
   return (short)Exp_16s_Pos((unsigned short)(x>>16));
}
__INLINE short Norm_32s_I(int *x){
   short i;
   int y = *x;
   if (y == 0) return 0;
   if (y == -1) {*x =IPP_MIN_32S;return 31;}
   if (y < 0) y = ~y;
   i = Exp_32s_Pos(y);
   *x <<= i;
   return i;
}
__INLINE short Norm_32s16s(int x) {
   short i;
   int y =  x;
   if (y == 0) return 0;
   if (y == -1) { return 31;}
   if (y < 0) y = ~y;
   i = Exp_32s_Pos(y);
   return i;
}
__INLINE short Exp_16s(short val) {
    short i;
    if(val == 0)
        return 0;
    if(IPP_MAX_16U == val)
        return 15;
    if(val < 0)
        val = (short)(~val);
    for(i = 0; val < (short)BWF_HARMONIC; i++)
        val =(short)(val<<1);
    return i;
}

#endif /*__OWNG711_H__*/
