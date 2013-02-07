/*/////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2005-2006 Intel Corporation. All Rights Reserved.
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
// Purpose: AMRWB speech codec own header file
//
*/
#ifndef __OWNAMRWB_H__
#define __OWNAMRWB_H__

#if defined( _WIN32_WCE)
#pragma warning( disable : 4505 )
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ipp_w7.h>
#include <ipps.h>
#include <ippsc.h>
#include "amrwbapi.h"

#include "scratchmem.h"

extern CONST IppSpchBitRate Mode2RateTbl[11];
extern CONST short NumberPrmsTbl[NUM_OF_MODES];
extern CONST short *pNumberBitsTbl[NUM_OF_MODES];

#define ENC_KEY 0xecd7222
#define DEC_KEY 0xdec7222

/* Codec constant parameters (coder and decoder) */

#define CODEC_VERSION "5.5.0"
#define FRAME_SIZE          256              /* Frame size */
#define SUBFRAME_SIZE_16k   (L_FRAME16k/NUMBER_SUBFRAME) /* Subframe size at 16kHz */
#define MAX_PARAM_SIZE      57               /* Maximum number of params */

#define SUBFRAME_SIZE      (FRAME_SIZE/NUMBER_SUBFRAME) /* Subframe size */
#define NUMBER_SUBFRAME     4                /* Number of subframe per frame */

#define WINDOW_SIZE         384              /* window size in LP analysis */
#define SPEECH_SIZE         WINDOW_SIZE      /* Total size of speech buffer */
#define LP_ORDER            16               /* Order of LP filter */
#define LP_ORDER_16K        20

#define UP_SAMPL_FILT_DELAY 12
#define DOWN_SAMPL_FILT_DELAY (UP_SAMPL_FILT_DELAY+3)

#define PITCH_GAIN_CLIP    15565              /* Pitch gain clipping */
#define PITCH_SHARP_FACTOR 27853              /* pitch sharpening factor */

#define PITCH_LAG_MAX      231                /* Maximum pitch lag */
#define OPL_DECIM          2                  /* Decimation in open-loop pitch analysis     */
#define INTERPOL_LEN       (LP_ORDER+1)       /* Length of filter for interpolation */

#define PREEMPH_FACTOR     22282              /* preemphasis factor */
#define TILT_FACTOR        22282              /* tilt factor (denominator) */
#define WEIGHT_FACTOR      30147              /* Weighting factor (numerator) */

#define MAX_SCALE_FACTOR   8                  /* scaling max for signal */
#define RND_INIT           21845              /* random init value */

/* Constants for Voice Activity Detection */

#define DTX_MAX_EMPTY_THRESH 50
#define DTX_HIST_SIZE       8
#define DTX_HIST_SIZE_MIN_ONE (DTX_HIST_SIZE-1)
#define DTX_ELAPSED_FRAMES_THRESH 30
#define DTX_HANG_CONST      7                  /* yields eight frames of SP HANGOVER */
#define INV_MED_THRESH      14564

#define ISF_GAP             128
#define ISF_FACTOR_LOW      256
#define ISF_DITHER_GAP      448
#define ISF_FACTOR_STEP     2

#define GAIN_FACTOR         75
#define GAIN_THRESH         180
#define THRESH_GAIN_PITCH   14746

#define DTX                 1
#define DTX_MUTE            2
#define SPEECH              0

#define MAX_DISTANCE_ISF    307                /* 120 Hz (6400Hz=16384) */
#define THRESH_DISTANCE_ISF 154
#define MIN_GAIN_PITCH      9830

#define NB_COEF_DOWN        15
#define NB_COEF_UP          12
#define PRED_ORDER          4

/*
   Some arrays reference
 */
extern CONST int   LagWindowTbl[LP_ORDER];
extern CONST short LPWindowTbl[WINDOW_SIZE];
extern CONST short FirUpSamplTbl[120];
extern CONST short FirDownSamplTbl[120];
extern CONST short IspInitTbl[LP_ORDER];
extern CONST short IsfInitTbl[LP_ORDER];
extern CONST short HPGainTbl[16];
extern CONST short Fir6k_7kTbl[31];
extern CONST short Fir7kTbl[31];
extern CONST short InterpolFracTbl[NUMBER_SUBFRAME];
extern CONST short BCoeffHP50Tbl[3];
extern CONST short ACoeffHP50Tbl[3];
extern CONST short BCoeffHP400Tbl[3];
extern CONST short ACoeffHP400Tbl[3];
extern CONST short PCoeffDownUnusableTbl[7];
extern CONST short CCoeffDownUnusableTbl[7];
extern CONST short PCoeffDownUsableTbl[7];
extern CONST short CCoeffDownUsableTbl[7];
extern CONST short ACoeffTbl[4];
extern CONST short BCoeffTbl[4];

/*
   Procedure definitions
 */
int   ownPow2_AMRWB(short valExp, short valFrac);
void  ownLagWindow_AMRWB_32s_I(int *pSrcDst);
void  ownAutoCorr_AMRWB_16s32s(short *pSrcSignal, int valLPCOrder, int *pDst);
void  ownScaleSignal_AMRWB_16s_ISfs(short *pSrcDstSignal, int   len, short ScaleFactor);
short ownChkStab(short *pSrc1, short *pSrc2, int len);
void  ownDecimation_AMRWB_16s(short *pSrcSignal16k, short len,
                             short *pDstSignal12k8, short *pMem);
void  ownOversampling_AMRWB_16s(short *pSrcSignal12k8, short len,
                               unsigned short *pDstSignal16k,short *pMem);
short ownMedian5(short *x);
void  ownInvSqrt_AMRWB_32s16s_I(int *frac,short *exp);
void  ownagc2(short *pSrcPFSignal, short *pDstPFSignal, short len);
void  ownIsfExtrapolation(short *pHfIsf);
void  ownLPDecim2(short *pSignal, short len, short *pMem);
short ownVoiceFactor(short *pPitchExc, short valExcFmt, short valPitchGain,
                   short *pFixCdbkExc, short valCodeGain, short len);
short ownGpClip(short *pMem);
void  ownCheckGpClipIsf(short *pIsfvec, short *pMem);
void  ownCheckGpClipPitchGain(short valPitchGain, short *pMem);
void  ownPhaseDispInit(short *pDispvec);
void  ownPhaseDisp(short valCodeGain, short valPitchGain, short *pFixCdbkExc,
                  short valLevelDisp, short *pDispvec);
short  amrExtractBits( const char **pBitStrm, int *currBitstrmOffset, int numParamBits );
void ownSynthesisFilter_AMRWB_16s32s_ISfs
       ( const Ipp16s *pSrcLpc, int valLpcOrder, const Ipp16s *pSrcExcitation,
       Ipp32s *pSrcDstSignal, int len, int scaleFactor );
void ownISPToLPC_AMRWB_16s
       ( const Ipp16s *pSrcIsp, Ipp16s *pDstLpc, Ipp32s len, Ipp32s adaptive_scaling );

#define HIGH_PASS_MEM_SIZE 31

typedef struct _HighPassFIRState_AMRWB_16s_ISfs{
   Ipp16s pFilterMemory[HIGH_PASS_MEM_SIZE-1];
   Ipp16s pFilterCoeff[HIGH_PASS_MEM_SIZE];
   Ipp32s ScaleFactor;
}HighPassFIRState_AMRWB_16s_ISfs;

/* /////////////////////////////////////////////////////////////////////////////
//  Name:        HighPassFIRGetSize_AMRWB_16s_ISfs
//  Purpose:     Knowing of AMR WB high pass FIR filter size demand
//  Parameters:
//    pDstSize      Pointer to the output value of the memory size needed for filtering
//  Returns:  ippStsNoErr, if no errors
*/

void HighPassFIRGetSize_AMRWB_16s_ISfs(Ipp32s *pDstSize);

/* /////////////////////////////////////////////////////////////////////////////
//  Name:        HighPassFIRInit_AMRWB_16s_ISfs
//  Purpose:     Initialization of the memory allocated for AMR WB high pass FIR filter
//  Parameters:
//    pSrcFilterCoeff    pointer to the filter coefficienst vector of lenght 31
//    ScaleFactor        scale factor value
//    pState             pointer to the memory supplied for filtering
//  Returns:  ippStsNoErr, if no errors
//
*/

void HighPassFIRInit_AMRWB_16s_ISfs(Ipp16s *pSrcFilterCoeff, Ipp32s ScaleFactor,
                                                       HighPassFIRState_AMRWB_16s_ISfs *pState);
/* /////////////////////////////////////////////////////////////////////////////
//  Name:        HighPassFIR_AMRWB_16s_ISfs
//  Purpose:     High-pass FIR filtering
//  Parameters:
//    pSrcDstSignal        pointer to the vector for inplace operation
//    pState    pointer to the memory supplied for filtering
//  Returns:  ippStsNoErr, if no errors
*/

void HighPassFIR_AMRWB_16s_ISfs(
     Ipp16s *pSrcDstSignal,
     Ipp32s len,
     HighPassFIRState_AMRWB_16s_ISfs *pState
);

/*
   The encoder and decoder objects structures definitions
*/
typedef struct _SDtxEncoderState
{
    short asiIsfHistory[LP_ORDER * DTX_HIST_SIZE];
    short siLogEnerHist[DTX_HIST_SIZE];
    short siHistPtr;
    short siLogEnerIndex;
    short siCngSeed;
    short siHangoverCount;
    short siAnaElapsedCount;
    int   aiDistMatrix[28];
    int   aiSumDist[DTX_HIST_SIZE];
} SDtxEncoderState;

typedef struct _SDtxDecoderState
{
    short siSidLast;
    short siSidPeriodInv;
    short siLogEnergy;
    short siLogEnergyOld;
    short asiIsf[LP_ORDER];
    short asiIsfOld[LP_ORDER];
    short siCngSeed;
    short asiIsfHistory[LP_ORDER * DTX_HIST_SIZE];
    short siLogEnerHist[DTX_HIST_SIZE];
    short siHistPtr;
    short siHangoverCount;
    short siAnaElapsedCount;
    short siSidFrame;
    short siValidData;
    short siHangoverAdded;
    short siGlobalState;
    short siDataUpdated;
    short siDitherSeed;
    short siComfortNoiseDith;
} SDtxDecoderState;

typedef struct _SAMRWBCoderObj
{
   int          iObjSize;
   int          iKey;
   unsigned int uiMode;         /* coder mode's */
   unsigned int uiRez;          /* rezerved */
}SAMRWBCoderObj;

struct _AMRWBEncoder_Obj{
   SAMRWBCoderObj objPrm;
   short asiSpeechDecimate[2*DOWN_SAMPL_FILT_DELAY];
   short siPreemph;
   short asiSpeechOld[SPEECH_SIZE - FRAME_SIZE];
   short asiWspOld[PITCH_LAG_MAX / OPL_DECIM];
   short asiExcOld[PITCH_LAG_MAX + INTERPOL_LEN];
   short asiLevinsonMem[2*LP_ORDER];
   short asiIspOld[LP_ORDER];
   short asiIspQuantOld[LP_ORDER];
   short asiIsfQuantPast[LP_ORDER];
   short siWsp;
   short asiWspDecimate[3];
   short siSpeechWgt;
   short asiSynthesis[LP_ORDER];
   short siTiltCode;
   short siWspOldMax;
   short siWspOldShift;
   short siScaleFactorOld;
   short asiScaleFactorMax[2];
   short asiGainPitchClip[2];
   short asiGainQuant[4];
   short siMedianOld;
   short siOpenLoopGain;
   short siAdaptiveParam;
   short siWeightFlag;
   short asiPitchLagOld[5];
   IppsHighPassFilterState_AMRWB_16s *pSHPFiltStateWsp;
   short asiHypassFiltWspOld[FRAME_SIZE / OPL_DECIM + (PITCH_LAG_MAX / OPL_DECIM)];
   IppsVADState_AMRWB_16s *pSVadState;
   int   iDtx;
   SDtxEncoderState dtxEncState;
   short siFrameFirst;
   short asiIsfOld[LP_ORDER];
   int   iNoiseEnhancerThres;
   int   aiSynthMem[LP_ORDER];
   short siDeemph;
   short asiSynthHighFilt[LP_ORDER];
   HighPassFIRState_AMRWB_16s_ISfs *pSHPFIRState;
   HighPassFIRState_AMRWB_16s_ISfs *pSHPFIRState2;
   IppsHighPassFilterState_AMRWB_16s *pSHPFiltStateSgnlIn;
   IppsHighPassFilterState_AMRWB_16s *pSHPFiltStateSgnlOut;
   IppsHighPassFilterState_AMRWB_16s *pSHPFiltStateSgnl400;
   short siHFSeed;
   short siVadHist;
   short siAlphaGain;
   short siScaleExp;
   short siToneFlag;
};

struct _AMRWBDecoder_Obj
{
    SAMRWBCoderObj objPrm;
    short asiExcOld[PITCH_LAG_MAX + INTERPOL_LEN];
    short asiIspOld[LP_ORDER];
    short asiIsfOld[LP_ORDER];
    short asiIsf[3*LP_ORDER];
    short asiIsfQuantPast[LP_ORDER];
    short siTiltCode;
    short siScaleFactorOld;
    short asiScaleFactorMax[4];
    int   iNoiseEnhancerThres;
    int   asiSynthesis[LP_ORDER];
    short siDeemph;
    short asiOversampFilt[2*UP_SAMPL_FILT_DELAY];
    short asiSynthHighFilt[LP_ORDER_16K];
    HighPassFIRState_AMRWB_16s_ISfs *pSHighPassFIRState;
    HighPassFIRState_AMRWB_16s_ISfs *pSHighPassFIRState2;
    IppsHighPassFilterState_AMRWB_16s *pSHPFiltStateSgnlOut;
    IppsHighPassFilterState_AMRWB_16s *pSHPFiltStateSgnl400;
    short siSeed;
    short siHFSeed;
    short asiQuantEnerPast[4];
    short asiCodeGainPast[5];
    short asiQuantGainPast[5];
    short siCodeGainPrev;
    short asiPhaseDisp[8];
    short siBfiPrev;
    short siBFHState;
    short siFrameFirst;
    SDtxDecoderState dtxDecState;
    short siVadHist;
    IppsAdaptiveCodebookDecodeState_AMRWB_16s *pSAdaptCdbkDecState;
};

#define   AMRWB_CODECFUN(type,name,arg)                extern type name arg

short ownDTXEncReset(SDtxEncoderState *st, short *pIsfInit);
short ownDTXEnc(SDtxEncoderState *st, short *pIsfvec, short *pExc2vec, unsigned short *pPrmsvec);
short ownDTXDecReset(SDtxDecoderState * st, short *pIsfInit);
short ownDTXDec(SDtxDecoderState *st, short *pExc2vec, short valDTXState, short *pIsfvec,
              const unsigned short *pPrmsvec);
short ownRXDTXHandler(SDtxDecoderState * st, short frameType);

/********************************************************
*      auxiliary inline functions declarations
*********************************************************/
__INLINE void ownMulC_16s_ISfs(short val, short *pSrcDst, int len, int scaleFactor)
{
    int i;
    for (i = 0; i < len; i++)
    {
        pSrcDst[i] = (short)((pSrcDst[i] * val) >> scaleFactor);
    }
    return;
}

__INLINE void ownMulC_16s_Sfs(short *pSrc, short val, short *pDst, int len, int scaleFactor)
{
    int i;
    for (i = 0; i < len; i++)
    {
        pDst[i] = (short)((pSrc[i] * val) >> scaleFactor);
    }
    return;
}

/* Random generator  */
__INLINE short Random(short *seed)
{
  *seed = (short)(*seed * 31821 + 13849);

  return(*seed);
}

__INLINE short Abs_16s(short x){
   if(x<0){
      if(IPP_MIN_16S == x) return IPP_MAX_16S;
      x = (short)-x;
   }
   return x;
}

__INLINE int Abs_32s(int x){
   if(x<0){
      if(IPP_MIN_32S == x) return IPP_MAX_32S;
      x = -x;
   }
   return x;
}

__INLINE int Add_32s(int x, int y) {
   Ipp64s z = (Ipp64s)x + y;
   if (z>IPP_MAX_32S) return IPP_MAX_32S;
   else if(z<IPP_MIN_32S)return IPP_MIN_32S;
   return (int)z;
}

extern CONST short NormTbl[256];
extern CONST short NormTbl2[256];

__INLINE short Exp_16s_Pos(unsigned short x)
{
   if((x>>8)==0)
      return NormTbl2[x];
   else {
      return NormTbl[(x>>8)];
   }
}

__INLINE short Exp_16s(short x){
   if (x == -1) return 15;
   if (x == 0) return 0;
   if (x < 0) x = (short)(~x);
   return Exp_16s_Pos(x);
}

__INLINE short Exp_32s_Pos(int x){
   short i;
   if (x == 0) return 0;
   for(i = 0; x < (int)0x40000000; i++) x <<= 1;
   return i;
}

__INLINE short Exp_32s(int x){
   if (x == 0) return 0;
   if (x == -1) return 31;
   if (x < 0) x = ~x;
   return Exp_32s_Pos(x);
}

__INLINE short Norm_32s_I(int *x){
   short i;
   if (*x == 0) return 0;
   if (*x < 0){
      for(i = 0; *x >= (int)0xC0000000; i++) *x <<= 1;
   }else
      for(i = 0; *x < (int)0x40000000; i++) *x <<= 1;
   return i;
}

__INLINE short Cnvrt_32s16s(int x){
   if (IPP_MAX_16S < x) return IPP_MAX_16S;
   else if (IPP_MIN_16S > x) return IPP_MIN_16S;
   return (short)(x);
}

__INLINE short Cnvrt_NR_32s16s(int x) {
   short s = IPP_MAX_16S;
   if(x<(int)0x7fff8000) s = (short)((x+0x8000)>>16);
   return s;
}

__INLINE int Negate_32s(int x) {
   if(IPP_MIN_32S == x)
      return IPP_MAX_32S;
   return (int)-x;
}

__INLINE int Mul2_32s(int x) {
    if(x > IPP_MAX_32S/2) return IPP_MAX_32S;
    else if( x < IPP_MIN_32S/2) return IPP_MIN_32S;
    return x <<= 1;
}

__INLINE int ShiftL_32s(int x, unsigned short n)
{
   int max = IPP_MAX_32S >> n;
   int min = IPP_MIN_32S >> n;
   if(x > max) return IPP_MAX_32S;
   else if(x < min) return IPP_MIN_32S;
   return (x<<n);
}

__INLINE short ShiftR_NR_16s(short x, unsigned short n){
   return (short)((x + (1<<(n-1)))>>n);
}

__INLINE void L_Extract (int L_32, short *hi, short *lo)
{
    *hi = (short)(L_32 >> 16);
    *lo = (short)((L_32>>1)&0x7fff);
    return;
}

__INLINE int Mpy_32_16 (short hi, short lo, short n)
{
    return (2 * (hi * n) +  2 * ((lo * n) >> 15));
}

__INLINE short Mul_16s_Sfs(short x, short y, int scaleFactor) {
   return (short)((x * y) >> scaleFactor);
}

__INLINE int Mul_32s(int x,int y) {
   int z,z1,z2;
   short xh, xl, yh, yl;
   xh  = (short)(x >> 15);
   yh  = (short)(y >> 15);
   xl  = (short)(x & 0x7fff);
   yl  = (short)(y & 0x7fff);
   z1  = Mul_16s_Sfs(xh,yl,15);
   z2  = Mul_16s_Sfs(xl,yh,15);
   z   = xh * yh;
   z   += z1;
   z   += z2;
   return (z<<1);
}

__INLINE int Add_16s(short x, short y) {
   return Cnvrt_32s16s((int)x + (int)y);
}

#endif /*__OWNAMRWB_H__*/
