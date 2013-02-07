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
// Purpose: GSM FR 06.10: main own header filen.
//
*/

#ifndef __OWNGSMFR_H__
#define __OWNGSMFR_H__

#if defined( _WIN32_WCE)
#pragma warning( disable : 4505 )
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ipp_w7.h>
#include <ipps.h>
#include <ippsc.h>
#include "gsmfrapi.h"

#include "scratchmem.h"

typedef struct _GSMFC_Obj {
   int          objSize;
   GSMFR_LTP_t  cutLTPFlag;
   int          key;
   int          reserved;
}GSMFRCoder_Obj;

struct _GSMFREncoder_Obj {
   GSMFRCoder_Obj objPrm;

   short          dp0[ 280 ];

   int            L_sacf[27];
   int            L_sav0[36];
   int            L_lastdm;

   int            L_av0[9];
   int            stat;
   int            L_av1[9];
   int            ptch;

   short          LARpp[2][8];

   short          ep[50];
   short          e_pvad;
   short          e_acf0;

   short          u[8];

   short          rvad[9];
   short          m_acf0;

   short          rav1[9];
   short          m_pvad;

   short          saveLAR[32];
   short          saveXMax[16];
   short          sof[160];

   uchar          oldSID[33];
   short          normrvad;
   GSMFR_VAD_t    vadMode;
   GSMFR_VAD_t    vadDirection;
   GSMFR_VAD_t    nextFrameVadMode;
   GSMFR_VAD_t    nextFrameVadDirection;
   int            hpMem[2];
   short          mp;
   int            vvad,vad;
   int            oldestLAR;
   int            oldestxmax;
   int            SIDAveragePeriodCount;
   int            scalauto;
   short          pt_sacf;        /* ??? 32 pointer on the delay line L_sacf*/
   short          pt_sav0;        /* ??? 32 pointer on the delay line L_sav0*/
   short          oldlagcount;    /* periodicity counter*/
   short          veryoldlagcount;/* periodicity counter*/
   short          e_thvad;        /* adaptive threshold exponent */
   short          m_thvad;        /* adaptive threshold mantissa */
   short          adaptcount;     /* counter for adaptation */
   short          burstcount;     /* hangover flags */
   short          hangcount;      /* hangover flags */
   short          oldlag;         /* LTP lag memory */
   short          tone;           /* tone detection */
   short          scalvad;
   short          normrav1,lagcount;
   short          myburstcount;
   short          idx;

};
struct _GSMFRDecoder_Obj {
   GSMFRCoder_Obj objPrm;
   short          dp0[ 280 ];
   short          LARpp[2][8];
   short          v[9];
   short          nrp;
   short          msr;
   short          idx;
};
#define M            10
#define DTX_HIST_SIZE 4
#define DTX_MAX_EMPTY_THRESH 50
#define DTX_ELAPSED_FRAMES_THRESH 23

#define GSMFR_CODECFUN(type, name, arg) extern type name arg
extern int  GsmRPEDec(short xmaxcr, short Mcr, short *xMcr, short *erp);
extern void Ref2bits(const short *parm,uchar *bitStream);
extern void Bits2Ref(uchar *bitStream, short *parm);
extern void Ltaf(short *dp,const short *d,short *e,short bp,short Nc);
extern void Shift_32s16s(const int *pSrc, short *pDst,int len,int shift);
extern int  RPEGridPos( short *xMp, short Mc, short *ep );
extern int  APCMQuant( short xmaxc, short *exp, short *mant);
extern int  Ref2LAR(short *pSrc,int len);
extern int  QuantCoding(short *pSrc,int len);
extern void RPEGrid( short *pSrc, short *pDst1, short *pDst2);
extern void DecCodLAR (const short *LARc, short *LAR,int len);
extern void LARp2RC(short *LARp, int len);
extern int  APCMQuant2(const short *xM, short *xMc, short *mant, short *exp, short *xmaxc);
extern void Interpolate_GSMFR_16s(const Ipp16s *pSrc1, const Ipp16s *pSrc2, Ipp16s *pDst,int len);
extern int  predValComp(const short *pSrc,int *pDst,int len);
#define DTX_ELAPSED_FRAMES 23
#define DTX_HANG_CONST     5
#define ENC_KEY 0xecdaaa
#define DEC_KEY 0xdecaaa
__INLINE short mult_r (short var1, short var2)
{
    return (short)(((int) var1 *(int) var2 + 16384)>>15);
}
/*======================================================================*/
extern CONST short eTab [256];
extern CONST short eTab2[256];

__INLINE short Mul_16s_Sfs(short x, short y, int scaleFactor) {
   return (short)(((int)x * (int)y) >> scaleFactor);
}
__INLINE short Abs_16s(short x){
   if(x<0){
      if(IPP_MIN_16S == x)
          return IPP_MAX_16S;
      x = (short)-x;
   }
   return x;
}
__INLINE short Exp_16s_Pos(unsigned short x)
{
   if((x>>8)==0)
      return eTab2[x];
   else {
      return eTab [(x>>8)];
   }
}
__INLINE short Exp_32s_Pos(int x){
   if (!x )
       return 0;
   if((x>>16)==0)
       return (short)(16+Exp_16s_Pos((unsigned short)x));
   return (short)
       Exp_16s_Pos((unsigned short)(x>>16));
}
/*======================================================================*/

#endif /*__OWNGSMFR_H__*/
