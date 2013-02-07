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
// Purpose: AMRWB speech codec: common utilities.
//
*/

#include "ownamrwb.h"

__ALIGN32 CONST IppSpchBitRate Mode2RateTbl[11] = {
   IPP_SPCHBR_6600,      /* 6.60 kbps */
   IPP_SPCHBR_8850,      /* 8.85 kbps */
   IPP_SPCHBR_12650,     /* 12.65 kbps */
   IPP_SPCHBR_14250,     /* 14.25 kbps */
   IPP_SPCHBR_15850,     /* 15.85 kbps */
   IPP_SPCHBR_18250,     /* 18.25 kbps */
   IPP_SPCHBR_19850,     /* 19.85 kbps */
   IPP_SPCHBR_23050,     /* 23.05 kbps */
   IPP_SPCHBR_23850,     /* 23.85 kbps */
   IPP_SPCHBR_DTX
};
short  amrExtractBits( const char **pBitStrm, int *currBitstrmOffset, int numParamBits )
{
    int  i;
    int  unpackedParam = 0;

    for ( i = 0; i < numParamBits; i ++ ){
        int  lTemp;
        lTemp = ((*pBitStrm)[(i + *currBitstrmOffset)>>3] >> ((7 - i - *currBitstrmOffset) & 0x7)) & 1;
        unpackedParam <<= 1;
        unpackedParam +=  lTemp;
    }

    *pBitStrm += (numParamBits + *currBitstrmOffset)>>3;
    *currBitstrmOffset = (numParamBits + *currBitstrmOffset) & 0x7;

    return (short)unpackedParam;
}
/* Find the voicing factor (1=voice to -1=unvoiced) */
short ownVoiceFactor(short *pPitchExc, short valExcFmt, short valPitchGain,
                   short *pFixCdbkExc, short valCodeGain, short len)
{
    short valExpDiff, tmp, valExp, valEner, valExp1, valEner2, valExp2, valVoiceFac;
    int s;

    ippsDotProd_16s32s_Sfs( pPitchExc, pPitchExc, len, &s, -1);
    s = Add_32s(s, 1);

    valExp1 = (short)(30 - Norm_32s_I(&s));
    valEner = (short)(s>>16);
    valExp1 = (short)(valExp1 - (valExcFmt << 1));

    s = (valPitchGain * valPitchGain) << 1;
    valExp = Norm_32s_I(&s);
    tmp = (short)(s>>16);

    valEner = Mul_16s_Sfs(valEner, tmp, 15);
    valExp1 = (short)(valExp1 - (valExp + 10));

    ippsDotProd_16s32s_Sfs( pFixCdbkExc, pFixCdbkExc,len, &s, -1);
    s = Add_32s(s, 1);

    valExp2 = (short)(30 - Norm_32s_I(&s));
    valEner2 = (short)(s>>16);

    valExp = Exp_16s(valCodeGain);
    tmp = (short)(valCodeGain << valExp);
    tmp = Mul_16s_Sfs(tmp, tmp, 15);
    valEner2 = Mul_16s_Sfs(valEner2, tmp, 15);
    valExp2 = (short)(valExp2 - (valExp << 1));

    valExpDiff = (short)(valExp1 - valExp2);

    if (valExpDiff >= 0)
    {
        valEner >>= 1;
        valEner2 >>= (valExpDiff + 1);
    } else
    {
        if ((1 - valExpDiff) >= 15)
            valEner = 0;
        else
            valEner >>= (1 - valExpDiff);
        valEner2 >>= 1;
    }

    valVoiceFac = (short)(valEner - valEner2);
    valEner = (short)(valEner + (valEner2 + 1));

    if (valVoiceFac >= 0)
        valVoiceFac = (short)((valVoiceFac << 15) / valEner);
    else
        valVoiceFac = (short)(-(((-valVoiceFac) << 15) / valEner));

    return (valVoiceFac);
}

/* 2.0 - 6.4 kHz phase dispersion */
static __ALIGN32 CONST short impPhaseDispLowTbl[SUBFRAME_SIZE] =
{
    20182,  9693,  3270, -3437,  2864, -5240,  1589, -1357,   600,
     3893, -1497,  -698,  1203, -5249,  1199,  5371, -1488,  -705,
    -2887,  1976,   898,   721, -3876,  4227, -5112,  6400, -1032,
    -4725,  4093, -4352,  3205,  2130, -1996, -1835,  2648, -1786,
     -406,   573,  2484, -3608,  3139, -1363, -2566,  3808,  -639,
    -2051,  -541,  2376,  3932, -6262,  1432, -3601,  4889,   370,
      567, -1163, -2854,  1914,    39, -2418,  3454,  2975, -4021,
     3431
};

/* 3.2 - 6.4 kHz phase dispersion */
static __ALIGN32 CONST short impPhaseDispMidTbl[SUBFRAME_SIZE] =
{
    24098, 10460, -5263,  -763,  2048,  -927, 1753, -3323,  2212,
      652, -2146,  2487, -3539,  4109, -2107,  -374, -626,  4270,
    -5485,  2235,  1858, -2769,   744,  1140,  -763, -1615, 4060,
    -4574,  2982, -1163,   731, -1098,   803,   167,  -714,  606,
     -560,   639,    43, -1766,  3228, -2782,   665,   763,  233,
    -2002,  1291,  1871, -3470,  1032,  2710, -4040,  3624,-4214,
     5292, -4270,  1563,   108,  -580,  1642, -2458,   957,  544,
     2540
};

void ownPhaseDispInit(short *pDispvec)
{
    ippsZero_16s(pDispvec, 8);
    return;
}

void ownPhaseDisp(short valCodeGain, short valPitchGain, short *pFixCdbkExc,
                  short valLevelDisp, short *pDispvec)
{
    short valState;
    short *pPrevPitchGain, *pPrevCodeGain, *pPrevState;
    int i, j;
    IPP_ALIGNED_ARRAY (16, short, pCodevec, SUBFRAME_SIZE*2);

    pPrevState = pDispvec;
    pPrevCodeGain = pDispvec + 1;
    pPrevPitchGain = pDispvec + 2;

    ippsZero_16s(pCodevec, SUBFRAME_SIZE*2);

    if (valPitchGain < MIN_GAIN_PITCH)
        valState = 0;
    else if (valPitchGain < THRESH_GAIN_PITCH)
        valState = 1;
    else
        valState = 2;

    ippsMove_16s(pPrevPitchGain, &pPrevPitchGain[1], 5);
    pPrevPitchGain[0] = valPitchGain;

    if ((valCodeGain - *pPrevCodeGain) > (*pPrevCodeGain << 1)) {
        if (valState < 2) valState += 1;
    } else {
        j = 0;
        for (i = 0; i < 6; i++)
            if (pPrevPitchGain[i] < MIN_GAIN_PITCH) j += 1;
        if (j > 2) valState = 0;
        if (valState > (*pPrevState + 1)) valState -= 1;
    }

    *pPrevCodeGain = valCodeGain;
    *pPrevState = valState;

    valState = (short)(valState + valLevelDisp);
    if (valState == 0)
    {
        for (i = 0; i < SUBFRAME_SIZE; i++)
        {
            if (pFixCdbkExc[i] != 0)
            {
                for (j = 0; j < SUBFRAME_SIZE; j++)
                {
                    pCodevec[i + j] = (short)(pCodevec[i + j] + ((pFixCdbkExc[i] * impPhaseDispLowTbl[j] + 0x00004000) >> 15));
                }
            }
        }
    } else if (valState == 1)
    {
        for (i = 0; i < SUBFRAME_SIZE; i++)
        {
            if (pFixCdbkExc[i] != 0)
            {
                for (j = 0; j < SUBFRAME_SIZE; j++)
                {
                    pCodevec[i + j] = (short)(pCodevec[i + j] + ((pFixCdbkExc[i] * impPhaseDispMidTbl[j] + 0x00004000) >> 15));
                }
            }
        }
    }
    if (valState < 2)
        ippsAdd_16s(pCodevec, &pCodevec[SUBFRAME_SIZE], pFixCdbkExc, SUBFRAME_SIZE);

    return;
}

/*-------------------------------------------------------------------*
 * Decimate a vector by 2 with 2nd order fir filter.                 *
 *-------------------------------------------------------------------*/

#define L_FIR  5
#define L_MEM  (L_FIR-2)
static __ALIGN32 CONST short hFirTbl[L_FIR] = {4260, 7536, 9175, 7536, 4260};

void ownLPDecim2(short *pSignal, short len, short *pMem)
{
    short *pSignalPtr;
    int i, j, s;
    IPP_ALIGNED_ARRAY (16, short, pTmpvec, FRAME_SIZE + L_MEM);

    ippsCopy_16s(pMem, pTmpvec, L_MEM);
    ippsCopy_16s(pSignal, &pTmpvec[L_MEM], len);
    ippsCopy_16s(&pSignal[len-L_MEM], pMem, L_MEM);

    for (i = 0, j = 0; i < len; i += 2, j++)
    {
        pSignalPtr = &pTmpvec[i];
        ippsDotProd_16s32s_Sfs(pSignalPtr, hFirTbl, L_FIR, &s, -1);
        pSignal[j] = Cnvrt_NR_32s16s(s);
    }
    return;
}

/* Conversion of 16th-order 12.8kHz ISF vector into 20th-order 16kHz ISF vector */
#define INV_LENGTH 2731

void ownIsfExtrapolation(short *pHfIsf)
{
    IPP_ALIGNED_ARRAY (16, short, pIsfDiffvec, LP_ORDER - 2);
    int i, s, pIsfCorrvec[3];
    short valCoeff, valMean, valExp, valExp2, valHigh, valLow, valMaxCorr;
    short tmp, tmp2, tmp3;

    pHfIsf[LP_ORDER_16K - 1] = pHfIsf[LP_ORDER - 1];
    ippsSub_16s(pHfIsf, &pHfIsf[1], pIsfDiffvec, (LP_ORDER - 2));

    s = 0;
    for (i = 3; i < (LP_ORDER - 1); i++)
        s += pIsfDiffvec[i - 1] * INV_LENGTH;
    valMean = Cnvrt_NR_32s16s(s<<1);

    pIsfCorrvec[0] = 0;
    pIsfCorrvec[1] = 0;
    pIsfCorrvec[2] = 0;

    ippsMax_16s(pIsfDiffvec, (LP_ORDER - 2), &tmp);
    valExp = Exp_16s(tmp);
    ippsLShiftC_16s_I(valExp, pIsfDiffvec, (LP_ORDER - 2));
    valMean <<= valExp;
    for (i = 7; i < (LP_ORDER - 2); i++)
    {
        tmp2 = (short)(pIsfDiffvec[i] - valMean);
        tmp3 = (short)(pIsfDiffvec[i - 2] - valMean);
        s = tmp2 * tmp3;
        valHigh = (short)(s >> 15);
        valLow = (short)(s & 0x7fff);
        s = (valHigh*valHigh + ((valHigh*valLow + valLow*valHigh)>>15)) << 1;
        pIsfCorrvec[0] += s;
    }
    for (i = 7; i < (LP_ORDER - 2); i++)
    {
        tmp2 = (short)(pIsfDiffvec[i] - valMean);
        tmp3 = (short)(pIsfDiffvec[i - 3] - valMean);
        s = tmp2 * tmp3;
        valHigh = (short)(s >> 15);
        valLow = (short)(s & 0x7fff);
        s = (valHigh*valHigh + ((valHigh*valLow + valLow*valHigh)>>15)) << 1;
        pIsfCorrvec[1] += s;
    }
    for (i = 7; i < (LP_ORDER - 2); i++)
    {
        tmp2 = (short)(pIsfDiffvec[i] - valMean);
        tmp3 = (short)(pIsfDiffvec[i - 4] - valMean);
        s = tmp2 * tmp3;
        valHigh = (short)(s >> 15);
        valLow = (short)(s & 0x7fff);
        s = (valHigh*valHigh + ((valHigh*valLow + valLow*valHigh)>>15) ) << 1;
        pIsfCorrvec[2] += s;
    }

    if (pIsfCorrvec[0] > pIsfCorrvec[1])
        valMaxCorr = 0;
    else
        valMaxCorr = 1;

    if (pIsfCorrvec[2] > pIsfCorrvec[valMaxCorr]) valMaxCorr = 2;
    valMaxCorr += 1;

    for (i = LP_ORDER - 1; i < (LP_ORDER_16K - 1); i++)
    {
        tmp = (short)(pHfIsf[i - 1 - valMaxCorr] - pHfIsf[i - 2 - valMaxCorr]);
        pHfIsf[i] = (short)(pHfIsf[i - 1] + tmp);
    }

    tmp = (short)(pHfIsf[4] + pHfIsf[3]);
    tmp = (short)(pHfIsf[2] - tmp);
    tmp = Mul_16s_Sfs(tmp, 5461, 15);
    tmp += 20390;
    if (tmp > 19456) tmp = 19456;
    tmp = (short)(tmp - pHfIsf[LP_ORDER - 2]);
    tmp2 = (short)(pHfIsf[LP_ORDER_16K - 2] - pHfIsf[LP_ORDER - 2]);

    valExp2 = Exp_16s(tmp2);
    valExp = Exp_16s(tmp);
    valExp -= 1;
    tmp <<= valExp;
    tmp2 <<= valExp2;
    valCoeff = (short)((tmp << 15) / tmp2);
    valExp = (short)(valExp2 - valExp);

    for (i = LP_ORDER - 1; i < (LP_ORDER_16K - 1); i++)
    {
        tmp = (short)(((pHfIsf[i] - pHfIsf[i - 1]) * valCoeff) >> 15);
        if (valExp > 0)
            pIsfDiffvec[i - (LP_ORDER - 1)] = (short)(tmp << valExp);
        else
            pIsfDiffvec[i - (LP_ORDER - 1)] = (short)(tmp >> (-valExp));
    }

    for (i = LP_ORDER; i < (LP_ORDER_16K - 1); i++)
    {
        tmp = (short)((pIsfDiffvec[i - (LP_ORDER - 1)] + pIsfDiffvec[i - LP_ORDER]) - 1280);
        if (tmp < 0)
        {
            if (pIsfDiffvec[i - (LP_ORDER - 1)] > pIsfDiffvec[i - LP_ORDER])
                pIsfDiffvec[i - LP_ORDER] = (short)(1280 - pIsfDiffvec[i - (LP_ORDER - 1)]);
            else
                pIsfDiffvec[i - (LP_ORDER - 1)] = (short)(1280 - pIsfDiffvec[i - LP_ORDER]);
        }
    }
    for (i = LP_ORDER-1; i < (LP_ORDER_16K-1); i++)
        pHfIsf[i] = (short)(Add_16s(pHfIsf[i-1], pIsfDiffvec[i-(LP_ORDER-1)]));

    ownMulC_16s_ISfs(26214, pHfIsf, LP_ORDER_16K-1, 15);

    ippsISFToISP_AMRWB_16s(pHfIsf, pHfIsf, LP_ORDER_16K);

    return;
}

short ownGpClip(short *pMem)
{
    short clip;

    clip = 0;
    if ((pMem[0] < THRESH_DISTANCE_ISF) && (pMem[1] > THRESH_GAIN_PITCH))
        clip = 1;

    return (clip);
}

void ownCheckGpClipIsf(short *pIsfvec, short *pMem)
{
    short valDist, valDistMin;
    int i;

    valDistMin = (short)(pIsfvec[1] - pIsfvec[0]);
    for (i = 2; i < LP_ORDER - 1; i++)
    {
        valDist = (short)(pIsfvec[i] - pIsfvec[i - 1]);
        if (valDist < valDistMin) valDistMin = valDist;
    }

    valDist = (short)((26214 * pMem[0] + 6554 * valDistMin) >> 15);
    if (valDist > MAX_DISTANCE_ISF) valDist = MAX_DISTANCE_ISF;
    pMem[0] = valDist;

    return;
}

void ownCheckGpClipPitchGain(short valPitchGain, short *pMem)
{
    short valGain;
    int s;

    s = 29491 * pMem[1];
    s += 3277 * valPitchGain;
    valGain = (short)(s>>15);

    if (valGain < MIN_GAIN_PITCH) valGain = MIN_GAIN_PITCH;
    pMem[1] = valGain;

    return;
}

void ownSynthesisFilter_AMRWB_16s32s_ISfs
       ( const Ipp16s *pSrcLpc, int valLpcOrder, const Ipp16s *pSrcExcitation,
       Ipp32s *pSrcDstSignal, int len, int scaleFactor )
{
//    IPP_BAD_PTR3_RET(pSrcLpc, pSrcExcitation, pSrcDstSignal);
//    IPP_BAD_SIZE_RET(valLpcOrder);
//    IPP_BAD_SIZE_RET(len);

    {
      int i, j, s, s1;
      short tmp;
      for (i = 0; i < len; i++)
      {
        s = 0;
        for (j = 1; j <= valLpcOrder; j++) {
           tmp = (short)((pSrcDstSignal[i - j]>>4)&0x0FFF);
           s -= tmp * pSrcLpc[j];
        }

        s >>= 11;

        s1 = pSrcExcitation[i] * pSrcLpc[0];

        for (j = 1; j <= valLpcOrder; j++)
            s1 -= (pSrcDstSignal[i - j]>>16) * pSrcLpc[j];

        s = Add_32s(s, Mul2_32s(s1));
        s = ShiftL_32s(s, (unsigned short)scaleFactor);

        pSrcDstSignal[i] = s;
      }
    }
    return;
}

#define NC16k_AMRWB 10
#define ownLSPPolinomials(pLSP,shift, pPolinom,len,ScaleFactor)\
{\
    short high, low;\
    int i, j, n, k, tmp;\
\
    pPolinom[0] = 1 << (21 + ScaleFactor);\
    pPolinom[1] = (-pLSP[0+shift]) << (7 + ScaleFactor);\
\
    for(i=2,k=2; i<=len; i++,k+=2){\
        pPolinom[i] = pPolinom[i-2];\
        for(j=1,n=i; j<i; j++,n--) {\
            high = (short)(pPolinom[n-1] >> 16);\
            low = (short)((pPolinom[n-1] >> 1) & 0x7fff);\
            tmp = high * pLSP[k+shift] + ((low * pLSP[k+shift])>>15);\
            pPolinom[n] -= 4*tmp;\
            pPolinom[n] += pPolinom[n-2];\
        }\
        pPolinom[n] -=  pLSP[k+shift] << (7 + ScaleFactor);\
    }\
}

#define GetLPCFromPoli(pDstLpc,len,lenBy2,f1,f2,scaleFactor,RoundOperation,tmax)\
{\
   int i,j;\
   int tmp;\
\
   pDstLpc[0] = 4096;\
   tmax = 1;\
   for (i = 1, j = len - 1; i < lenBy2; i++, j--) {\
      tmp = Add_32s(f1[i],f2[i]);\
      tmax |= tmp;\
      pDstLpc[i] = RoundOperation((tmp + (1 << scaleFactor)) >> (scaleFactor+1));\
\
      tmp = f1[i] - f2[i];\
      tmax |= Abs_32s(tmp);\
      pDstLpc[j] = RoundOperation((tmp + (1 << scaleFactor)) >> (scaleFactor+1));\
   }\
}

void ownISPToLPC_AMRWB_16s
       ( const Ipp16s *pSrcIsp, Ipp16s *pDstLpc, Ipp32s len, Ipp32s adaptive_scaling )
{
    short high, low, sq, sq_sug;
    int i, j, nc, tmp, tmax;
    IPP_ALIGNED_ARRAY(16, int, f1, NC16k_AMRWB + 1);
    IPP_ALIGNED_ARRAY(16, int, f2, NC16k_AMRWB);

//    IPP_BAD_PTR2_RET(pSrcIsp, pDstLpc)
//    IPP_BAD_SIZE_RET(len);
//    IPP_BADARG_RET( (len > 20), ippStsSizeErr )

    nc = len >> 1;
    if (nc > 8) {
        ownLSPPolinomials(pSrcIsp,0,f1,nc,0);
        ippsLShiftC_32s_I(2, f1, nc);
        f1[nc] = ShiftL_32s(f1[nc], 2);
        ownLSPPolinomials(pSrcIsp,1,f2,nc-1,0);
        ippsLShiftC_32s_I(2, f2, nc);
    } else {
        ownLSPPolinomials(pSrcIsp,0,f1,nc,2);
        ownLSPPolinomials(pSrcIsp,1,f2,nc-1,2);
    }

    for (i = nc - 1; i > 1; i--) {
        f2[i] -= f2[i - 2];
    }

    for (i = 0; i < nc; i++) {
        L_Extract(f1[i], &high, &low);
        tmp = Mpy_32_16(high, low, pSrcIsp[len - 1]);
        f1[i] = Add_32s(f1[i], tmp);

        L_Extract(f2[i], &high, &low);
        tmp = Mpy_32_16(high, low, pSrcIsp[len - 1]);
        f2[i] -= tmp;
    }
    GetLPCFromPoli(pDstLpc,len,nc,f1,f2,11,(short),tmax);

    /* rescale data if overflow has occured and reprocess the loop */

    if ( adaptive_scaling == 1 )
       sq = (short)(4 - Exp_32s(tmax));
    else
       sq = 0;

    if (sq > 0)
    {
      sq_sug = (short)(sq + 12);
      for (i = 1, j = (len - 1); i < nc; i++, j--)
        {
          tmp = Add_32s(f1[i], f2[i]);
          pDstLpc[i] = (short)((tmp + (1 << (sq_sug-1))) >> sq_sug);

          tmp = f1[i] - f2[i];
          pDstLpc[j] = (short)((tmp + (1 << (sq_sug-1))) >> sq_sug);
        }
      pDstLpc[0] >>= sq;
    }
    else
    {
      sq_sug = 12;
      sq     = 0;
    }

    L_Extract(f1[nc], &high, &low);
    tmp = Mpy_32_16(high, low, pSrcIsp[len - 1]);

    tmp = Add_32s(f1[nc], tmp);
    pDstLpc[nc] = (short)((tmp + (1 << 11)) >> sq_sug);

    pDstLpc[len] = ShiftR_NR_16s(pSrcIsp[len - 1], (short)(sq+3));

    return;
}
