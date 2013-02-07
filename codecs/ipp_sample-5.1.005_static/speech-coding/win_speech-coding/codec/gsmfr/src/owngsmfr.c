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
// Purpose: GSM FR 06.10: internal functions.
//
*/

#include <ipp_w7.h>
#include    "ippsc.h"
#include    "owngsmfr.h"

static __ALIGN32 CONST short NRFAC[8] = { 29128, 26215, 23832,
    21846, 20165, 18725,
    17476, 16384};
static __ALIGN32 CONST short INVA[8]  = {13107,13107,13107,
    13107,19223,17476,
    31454,29708};
static __ALIGN32 CONST short B[8]     = {0,0,2048,
    -2560,94,-1792,
    -341,-1144};
static __ALIGN32 CONST short MIC[8]   = { -32,-32,-16,
    -16,-8,-8,
    -4,-4};
static __ALIGN32 CONST short A[8]     = {20480,20480,20480,
    20480,13964,15360,
    8534,9036};
static __ALIGN32 CONST short MAC[8]   = {31,31,15,
    15,7,7,
    3,3};
__ALIGN32 CONST short eTab[256] = {
   7,6,5,5,4,4,4,4, 3,3,3,3,3,3,3,3, 2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,
   1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,
   -1,-1,-1,-1,-1,-1,-1,-1,   -1,-1,-1,-1,-1,-1,-1,-1,   -1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1,-1,-1,-1,-1,-1,-1,   -1,-1,-1,-1,-1,-1,-1,-1,   -1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1,-1,-1,-1,-1,-1,-1,   -1,-1,-1,-1,-1,-1,-1,-1,   -1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1,-1,-1,-1,-1,-1,-1,   -1,-1,-1,-1,-1,-1,-1,-1,   -1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1,-1,-1,-1,-1,-1,-1,   -1,-1,-1,-1,-1,-1,-1,-1,   -1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1,-1,-1,-1,-1,-1,-1
};
__ALIGN32 CONST short eTab2[256] = {
  15,14,13,13,12,12,12,12,  11,11,11,11,11,11,11,11,  10,10,10,10,10,10,10,10,
  10,10,10,10,10,10,10,10,   9, 9, 9, 9, 9, 9, 9, 9,   9, 9, 9, 9, 9, 9, 9, 9,
   9, 9, 9, 9, 9, 9, 9, 9,   9, 9, 9, 9, 9, 9, 9, 9,   8, 8, 8, 8, 8, 8, 8, 8,
   8, 8, 8, 8, 8, 8, 8, 8,   8, 8, 8, 8, 8, 8, 8, 8,   8, 8, 8, 8, 8, 8, 8, 8,
   8, 8, 8, 8, 8, 8, 8, 8,   8, 8, 8, 8, 8, 8, 8, 8,   8, 8, 8, 8, 8, 8, 8, 8,
   8, 8, 8, 8, 8, 8, 8, 8,   7, 7, 7, 7, 7, 7, 7, 7,   7, 7, 7, 7, 7, 7, 7, 7,
   7, 7, 7, 7, 7, 7, 7, 7,   7, 7, 7, 7, 7, 7, 7, 7,   7, 7, 7, 7, 7, 7, 7, 7,
   7, 7, 7, 7, 7, 7, 7, 7,   7, 7, 7, 7, 7, 7, 7, 7,   7, 7, 7, 7, 7, 7, 7, 7,
   7, 7, 7, 7, 7, 7, 7, 7,   7, 7, 7, 7, 7, 7, 7, 7,   7, 7, 7, 7, 7, 7, 7, 7,
   7, 7, 7, 7, 7, 7, 7, 7,   7, 7, 7, 7, 7, 7, 7, 7,   7, 7, 7, 7, 7, 7, 7, 7,
   7, 7, 7, 7, 7, 7, 7, 7,   7, 7, 7, 7, 7, 7, 7, 7
};

void Ltaf(short *dp,const short *d,short *ep,short bp,short Nc)
{
  ippsMulC_NR_16s_Sfs(&dp[-Nc],bp,dp,40,15);
  ippsSub_16s(dp,d,ep,40);
}
void Shift_32s16s(const int *pSrc, short *pDst,int len,int shift)
{
  IPP_ALIGNED_ARRAY(16, int, xxx, 20);

  if(shift > 0)
     ippsRShiftC_32s(pSrc,shift,xxx,len);
  else
     ippsLShiftC_32s(pSrc,-shift,xxx,len);
  ippsConvert_32s16s(xxx,pDst,len);
}
int RPEGridPos(short *val1, short idx, short *dst )
{
    int i;
    for (i = 0; i <13; i++) {
        dst[ idx + (3*i) ] = val1[i];
    }
   return 0;
}
int APCMQuant( short val, short *exponentDst, short *mantissaDst)
{
    short   exponent, mantissa;
    exponent = 0;
    if (val > 15)
      exponent = (short)((val >> 3) - 1);
    mantissa = (short)(val - (exponent << 3));

    if (!mantissa) {
        mantissa = 7;
        exponent  = -4;
    }else {
        while (mantissa <= 7) {
            mantissa = (short)(mantissa << 1 | 1);
            exponent--;
        }
        mantissa -= 8;
    }
    *exponentDst  = exponent;
    *mantissaDst = mantissa;
   return 0;
}
int Ref2LAR(short *pSrcDst,int len)
{
  short  tmp[8];
  int    i;
   if(len>8)
      return -1;
   ippsAbs_16s(pSrcDst,tmp,len);
   for(i = 0; i < len; i++) {
      if(tmp[i] < 22118) {
         tmp[i] >>= 1;
      } else
          if(tmp[i] < 31130) {
            tmp[i] = (short)(tmp[i]-11059);
      } else {
         tmp[i] = (short)((tmp[i]-26112)<<2);
      }
      if(pSrcDst[i]<0)
         tmp[i]=(short)(-tmp[i]);
      pSrcDst[i]=tmp[i];
   }
   return 0;
}
int QuantCoding (short *pSrcDst,int len)
{
  short tmp;
  int   i;
  if(len>8)
     return -1;
   for(i=0;i<len;i++){
      tmp = Mul_16s_Sfs( A[i], pSrcDst[i],15);
      tmp = (short)( (tmp+B[i]+ 256) >> 9 );
      pSrcDst[i]  = (short)( tmp>MAC[i] ? MAC[i] - MIC[i] : (tmp<MIC[i] ? 0 : tmp - MIC[i]));
   }
   return 0;
}
void RPEGrid(short *pSrc, short *pDst, short *pmaxIdx)
{
  short xxx[40];
  int   m,i,maxIdx=0,resL,Lx[40],maxEl=0,sum[3];
   ippsRShiftC_16s(pSrc,2,xxx,40);
   ippsMul_16s32s_Sfs(xxx,xxx,Lx,40,0);
   for (m = 0; m <3 ; m++) {
       for (resL = i = 0; i < 13; i++) {
           resL += Lx[m + 3*i];
       }
       sum[m]=resL;
       if (resL > maxEl) {
           maxIdx = m;
           maxEl  = resL;
       }
   }
   resL=sum[0]-Lx[0]+Lx[39];
    if (resL > maxEl) {
        maxIdx = 3;
        maxEl  = resL;
    }
   for(i = 0; i < 13; i ++)
      pDst[i] = pSrc[maxIdx + 3*i];
   *pmaxIdx = (short)maxIdx;
}
void DecCodLAR(const short *pSrc, short *pDst,int len)
{
  short   tmp1, tmp2 ,i;
   for (i = 0; i < len; i++)
   {
      tmp1  = (short)(( pSrc[i]+MIC[i] ) << 10);
      tmp2  = (short)(B[i] << 1);
      tmp1  = (short)(tmp1-tmp2);
      tmp1  = (short)(((int)INVA[i] * (int)tmp1 +16384)>>15);
      pDst[i]= (short)(tmp1<<1);
   }
}
void LARp2RC(short *pSrcDst, int len)
{
  int   i;
  short tmp;
   for(i = 0; i < len; i++ )
   {
       tmp = Abs_16s(pSrcDst[i]);
       if (tmp < 11059)
           tmp <<= 1;
       else
           if (tmp < 20070)
               tmp += 11059;
       else
           tmp = (short)((short)(tmp >> 2)+26112);
       pSrcDst[i] = (short)(pSrcDst[i] < 0 ? -tmp : tmp);
   }
}
int APCMQuant2(const short *pSrc, short *pDst, short *pMant, short *pexponent,
               short *pmaxxc){
   int       i, j;
   short     maxx, maxxc, tmp, tmp1, tmp2, exponent, mant, xxx[13];
   ippsAbs_16s(pSrc,xxx,13);
   ippsMax_16s(xxx,13,&maxx);
    exponent   = 0;
    tmp  = (short)( maxx >> 9 );
    j = 0;
    for (i = 0; i <= 5; i++) {
        j |= (tmp <= 0);
        tmp = (short)( tmp >> 1 );
        if (!j )
         exponent++;
    }
    tmp = (short)(exponent + 5);

    maxxc = (short)((short)(maxx >> tmp)+(short)(exponent << 3 ));
    APCMQuant( maxxc, &exponent, &mant );
    tmp1 = (short)(6 - exponent);
    tmp2 = NRFAC[ mant ];
    for (i = 0; i <13; i++) {
        tmp = (short)(pSrc[i] << tmp1);
        tmp = Mul_16s_Sfs( tmp, tmp2, 15 );
        tmp = (short)(tmp >> 12);
        pDst[i] = (short)(tmp + 4);
    }
    *pMant  = mant;
    *pexponent   = exponent;
    *pmaxxc = maxxc;
   return 0;
}
void Interpolate_GSMFR_16s(const Ipp16s *pSrc1, const Ipp16s *pSrc2, Ipp16s *pDst, int  len)
{
  int    i;
   for(i = 0; i < len; i++ ) {
      pDst[i] = (short)((pSrc1[i] >> 2) + (pSrc2[i]>>2));
      pDst[i] = (short)(pDst[i] + (pSrc1[i] >> 1));
   }
}
int predValComp(const short *pSrc,
                int *pDst,int len)
{
  int    i,m,L_val[9] ;
  short  tmp,val1[9];
  if (len>9)
     return -1;
   L_val[0] = 16384<< 15;
   L_val[1] = pSrc[1]<< 14;
   for( m = 2 ;m< len;++m){
      for( i = 1;i<= m-1;++i){
         tmp = (short)(L_val[m-i]>> 16);
         pDst[i] = L_val[i] + (pSrc[m]<<1)*tmp;
      }
      for( i = 1;i<= m-1;++i){
         L_val[i] = pDst[i];
      }
      L_val[m] = pSrc[m]<< 14;
   }
   for( i = 0 ;i< len;++i){
      val1[i] = (short)(L_val[i]>> 19);
   }
   ippsAutoCorr_16s32s(val1,len,pDst,len); return (0);
}

