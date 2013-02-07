/* /////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the tems of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the tems of that agreement.
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
//  Purpose: anti-howler, tested for subband algorithm
//
*/
#include <ipp_w7.h>
#include "ipps.h"
#include "ippsc.h"
#include "usc_ec.h"
#include "ah_api_int.h"
#include "ownah_int.h"
#include "ownec_int.h"

#define tapsFactor 14
static Ipp16s bqTaps2[]={
 1122, 2244, 1122, 16384, -18842, 6953,
 16143, -32286, 1122, 16384, -18842, 6953
};
int ah_SetFreqShift_int(_ahState_int *state,int shift){
    if (shift<0) {
        state->shift = 0;
        return 1;
    }else{
        state->shift = shift;
        return 0;
    }
}

int ah_SetHowlPeriod_int(_ahState_int *state,int howlPeriod){
    if (howlPeriod<=0) {
        state->howlPeriodInit=(int)((state->samplingRate/state->frameSize));
        return 1;
    }else{
        state->howlPeriodInit=howlPeriod;
        return 0;
    }
}

int ah_SetMode_int(_ahState_int *state, AHOpcode op){
    if((op==AH_ENABLE1) || (op==AH_ENABLE2) || (op==AH_DISABLE)){
        state->mode=op;
        return 0;
    } else
        return 1;
}
static Ipp32s ms_en(Ipp16s *vec, int len)
{
    Ipp64s en;
    ippsDotProd_16s64s(vec,vec,len,&en);
    return (Ipp32s)en/len;
}
static Ipp16s Saturate_32s_16s(Ipp32s val) {
    if( val > IPP_MAX_16S ) val = IPP_MAX_16S;
    else if( val < IPP_MIN_16S ) val = IPP_MIN_16S;
    return (Ipp16s)val;
}
static void own_AH_IIRBQ_16s(const Ipp16s* pSrc, Ipp16s* pDst,
                                             int len,
                                             const Ipp16s* pTaps1, int numBq,
                                             int tapsScale,
                                             Ipp32s* pDlyLine1) {
   int jj;
   int n;
   int i; Ipp32s sum=0;Ipp32s src;Ipp32s* pDlyLine;const Ipp16s* pTaps;
   for( jj=0; jj<len; ++jj ){
       src=pSrc[jj];pDlyLine=pDlyLine1;pTaps=pTaps1;
       for( n=0; n<numBq; ++n ) {
           sum = 0;
           pDlyLine[0] = (Ipp32s)src;
           sum = (pTaps[0]*pDlyLine[0]+pTaps[1]*pDlyLine[1]+pTaps[2]*pDlyLine[2]-
               pTaps[4]*pDlyLine[4]-pTaps[5]*pDlyLine[5])>>tapsScale;
           src = pDlyLine[3] = sum;
           for( i=5; i>0; --i) pDlyLine[i] = pDlyLine[i-1];
           pTaps += 6; pDlyLine += 6;
       }
       pDst[jj] = Saturate_32s_16s(sum);
   }
    return;
}
static int addHist(_ahState_int *state, Ipp16s *src, Ipp16s **histEnd ){
    if(state->howlHistCurLen < state->howlHistLen){
        state->prevHistExist=0;
        /* create the history */
        ippsCopy_16s(src,state->howlHist+state->howlHistCurLen,state->frameSize);
        state->howlHistCurLen+=state->frameSize;
        *histEnd=state->howlHist+state->howlHistCurLen;
        if(state->howlHistCurLen==state->howlHistLen)/*howlHistCurLen is always multiple to the frame size*/
            state->prevHistExist=1;
        else
            state->prevHistExist=0;
        return state->howlHistCurLen;
    }
    /* renew the history */
    if(state->howlHistLen>state->frameSize)
        ippsCopy_16s(state->howlHist+state->frameSize,state->howlHist,
        state->howlHistLen-state->frameSize);
    ippsCopy_16s(src,state->howlHist+state->howlHistLen-state->frameSize,
        state->frameSize);
    *histEnd=state->howlHist+state->howlHistCurLen;
    state->prevHistExist=1;
    return state->howlHistCurLen;
}
static Ipp32s mean_32s(const Ipp32s *src, int len){
    Ipp64s sum=0;
    int i;
    for (i=0;i<len;i++) {
        sum+=src[i];
    }
    return (Ipp32s)(sum/len);
}
static int detectHowlingFFT(_ahState_int *state,Ipp16s *in){
    Ipp32s   curMagn[(AH_IN_WORK_LEN+2)>>1],cmean;
    int      per[3]={11,22,33};
    int      max_idx[3]={0,0,0};
    int      i, flen=(AH_IN_WORK_LEN+2)>>1,disp[3];
    IppStatus stat;
    stat=ippsFFTFwd_RToCCS_16s32s_Sfs((const Ipp16s *)in,(Ipp32s *)state->howlSpectrum,
        state->howlSpecFFTx,-8,state->pBuf);
    stat=ippsMagnitude_32sc_Sfs(state->howlSpectrum,curMagn,flen,0);
    cmean=mean_32s(curMagn,flen);
    if(!cmean)
        return 0;
    for (disp[0]=i=0;i<per[0];i++) {
        if (curMagn[i]>cmean) {
            disp[0]++;
            max_idx[0]=i;
        }
    }
    for (disp[1]=0;i<per[1];i++) {
        if (curMagn[i]>cmean) {
            disp[1]++;
            max_idx[1]=i;
        }
    }
    for (disp[2]=0;i<per[2];i++) {
        if (curMagn[i]>cmean) {
            disp[2]++;
            max_idx[2]=i;
        }
    }
    if(
        (
        ((disp[0]-disp[1])>BW_AMPL_PRECISION &&
        (disp[2]-disp[1])>BW_AMPL_PRECISION) &&
        (max_idx[0]>MAX_HOWL_IDX0 &&
        max_idx[2]>MAX_HOWL_IDX2 ))
        ){
            return 2;
        }
    return 0;
}
static int detectHowlingIIR(_ahState_int *state, Ipp16s *in,Ipp32s ms ){
    Ipp32s ams;
    int    dstLen,phase=0,dstLen2;
    ippsSampleUp_16s(in,state->frameSize,state->upDownBuf,&dstLen,4,&phase);
    own_AH_IIRBQ_16s(state->upDownBuf,(Ipp16s *)state->pBuf,dstLen,
        bqTaps2,2,tapsFactor,state->pDly2Test);
    ippsSampleDown_16s((Ipp16s *)state->pBuf,dstLen,state->upDownBuf,&dstLen2,4,&phase);
    ams = ms_en(state->upDownBuf,state->frameSize);
    if ((ms*100>ams*119)){
        ippsZero_32s(state->pDly2Test,6*2);
        return 2;
    }
    return 0;
}
static void shiftCutFreq(_ahState_int *state,Ipp32sc *src, int len){
    int shift,i;
    Ipp32sc val={1,1};
    ippsSet_32sc(val,src,state->howlLowBand);
    src[0].im=0;
    shift = ((state->shift<<dFScale)+(state->dF-1))/state->dF;
    if(shift>0){
        i=len-state->howlHighBand;
        shift=shift>i?i:shift;
        ippsCopy_32sc(&src[state->howlLowBand+shift],&src[state->howlLowBand],
            state->howlHighBand-state->howlLowBand);
    }
    ippsSet_32sc(val,&src[state->howlHighBand],len-state->howlHighBand);
    src[len-1].im=0;
    return;
}
static void filterHowlingFFT(_ahState_int *state, Ipp16s *out){
    ippsFFTFwd_RToCCS_16s32s_Sfs((const Ipp16s *)state->howlHist+state->howlHistLen-(1<<AH_FFT_ORDER),
        (Ipp32s *)state->howlSpectrum,state->howlSpecFFT,-8,state->pBuf);
    shiftCutFreq(state,state->howlSpectrum,((1<<AH_FFT_ORDER)+2)>>1);
    ippsFFTInv_CCSToR_32s16s_Sfs((Ipp32s *)state->howlSpectrum,state->sBuf,
        state->howlSpecFFT,8,state->pBuf);
    ippsCopy_16s(state->sBuf+(1<<AH_FFT_ORDER)-state->frameSize,out,state->frameSize);
}
static void initFilter(_ahState_int *state){
    ippsZero_32s(state->pDly2,6*2);
}
static void filterHowlingIIR(_ahState_int *state, Ipp16s *in, Ipp16s *out){
    int dstLen,phase=0,dstLen2;
    ippsSampleUp_16s(in,state->frameSize,state->upDownBuf,&dstLen,4,&phase);
    own_AH_IIRBQ_16s( state->upDownBuf,(Ipp16s *)state->pBuf,dstLen,
        bqTaps2,2,tapsFactor,state->pDly2);
    ippsSampleDown_16s((Ipp16s *)state->pBuf,dstLen,out,&dstLen2,4,&phase);
}
AHOpcode ah_ProcessFrame_int(_ahState_int *state, Ipp16s *in, Ipp16s *out)
{
  int     detect=0,det=0;
  Ipp32s  rms;
  Ipp16s *histEnd;
    if(state->mode==AH_ENABLE1 || state->mode==AH_ENABLE2){
        det = addHist(state,in,&histEnd);
        if(det<AH_IN_WORK_LEN)/* too short history */
            return AH_DISABLE;
        rms = ms_en(histEnd-AH_IN_WORK_LEN,AH_IN_WORK_LEN);
        if (rms<111){
            state->howlPeriod=0;
            return AH_DISABLE;
        }
        if (state->howlPeriod==0) {
            if(state->mode==AH_ENABLE1){
                detect=detectHowlingFFT(state,histEnd-AH_IN_WORK_LEN);
             }else
                detect=detectHowlingIIR(state,histEnd-AH_IN_WORK_LEN,rms);
            if(detect<=0)
                return AH_DISABLE;
            if(detect==2){ /* howling started */
                state->howlPeriod=state->howlPeriodInit;
                initFilter(state);
            }
        }
        --state->howlPeriod;
        if(state->prevHistExist){// full history
            filterHowlingFFT(state,out);
            filterHowlingIIR(state,out,out);
        }else
            filterHowlingIIR(state,in,out);
        //ippsMulC_16s_I(12.f,out,state->frameSize);
        return state->mode;
    }
    return AH_DISABLE;
}
int ah_GetSize_int(int frameSize, int *size)
{
    int specSize,specSize1,bufSize,bufSize1,initSize,initSize1,gsize,hsize,ssize;
    *size = ALIGN(frameSize*4*sizeof(Ipp16s));/* up buf size */

    hsize  = (((1<<AH_FFT_ORDER)+frameSize-1)/frameSize)*frameSize*sizeof(Ipp16s);//howlHistLen
    ippsFFTGetSize_R_16s32s(AH_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &specSize, &initSize, &bufSize);
    ippsFFTGetSize_R_16s32s(AH_FFT_ORDERx, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &specSize1, &initSize1, &bufSize1);
    gsize = initSize;
    if (gsize < bufSize)
        gsize = bufSize;
    if (gsize < initSize1)
        gsize = initSize1;
    if (gsize < bufSize1)
        gsize = bufSize1;
    if (gsize<(int)(frameSize*4*sizeof(Ipp16s))) {// reserve for work buf (same as updown)
        gsize=(int)(frameSize*4*sizeof(Ipp16s));
    }
    ssize = hsize;// sBuf length
    *size += ALIGN(sizeof(_ahState_int))+ALIGN(hsize)+ALIGN(ssize)+
        ALIGN((((1<<AH_FFT_ORDER)+2))*sizeof(Ipp32s))+// for spectrum cur
        ALIGN(specSize)+ALIGN(specSize1)+ ALIGN(gsize);
    return 0;
}
int ah_Init_int(_ahState_int *state,int samplingRate,int frameSize)
{
    int    bufSize, specSize, initSize,bufSize1, specSize1, initSize1, gsize, hlen;
    IppStatus status=ippStsOk;
    Ipp8u *pInit0,*pInit = (Ipp8u *)state,*pInit1;
    ippsZero_32s(state->pDly2,6*2);
    ippsZero_32s(state->pDly2Test,6*2);

    state->howlHistLen = (((1<<AH_FFT_ORDER)+frameSize-1)/frameSize)*frameSize;
    hlen = ALIGN(state->howlHistLen*sizeof(Ipp16s));
    pInit += ALIGN(sizeof(_ahState_int));
    state->upDownBuf = (Ipp16s *)pInit;
    pInit += ALIGN(frameSize*4*sizeof(Ipp16s));

    status=ippsFFTGetSize_R_16s32s(AH_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &specSize, &initSize, &bufSize);
    status=ippsFFTGetSize_R_16s32s(AH_FFT_ORDERx, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &specSize1, &initSize1, &bufSize1);
    pInit0 = pInit;
    pInit += ALIGN(specSize);
    pInit1 = pInit;
    pInit += ALIGN(specSize1);
    status=ippsFFTInit_R_16s32s(&(state->howlSpecFFT), AH_FFT_ORDER, IPP_FFT_DIV_INV_BY_N,
        ippAlgHintAccurate, pInit0, pInit);
    status=ippsFFTInit_R_16s32s(&(state->howlSpecFFTx), AH_FFT_ORDERx, IPP_FFT_DIV_INV_BY_N,
        ippAlgHintAccurate, pInit1, pInit);
    gsize = initSize;
    if (gsize < bufSize)
        gsize = bufSize;
    if (gsize < initSize1)
        gsize = initSize1;
    if (gsize < bufSize1)
        gsize = bufSize1;
    state->pBuf = pInit;
    if (gsize<(int)(frameSize*4*sizeof(Ipp16s))) {// reserve for updown2 buf
        gsize=(int)(frameSize*4*sizeof(Ipp16s));
    }
    pInit += ALIGN(gsize);

    state->sBuf = (Ipp16s *)pInit;
    pInit += hlen;
    state->howlHist=(Ipp16s *)pInit;
    pInit += hlen;
    state->howlSpectrum =(Ipp32sc *)pInit;
    pInit += ALIGN((((1<<AH_FFT_ORDER)+2))*sizeof(Ipp32s));// for fft output in CC
    state->samplingRate = samplingRate;
    state->frameSize = frameSize;
    state->shift = 0;
    state->mode = AH_DISABLE;
    state->howlHistCurLen = 0;
    state->howlPeriodInit=(int)((state->samplingRate/state->frameSize));/* number of
                                                                        the frames to be filtered after the howling has been detected*/
    state->howlPeriod=0;
    state->prevHistExist=0;
    state->dF=(samplingRate<<dFScale)>>AH_FFT_ORDER;    // fft spectrum freq step
    state->howlLowBand=(LOW_BAND<<dFScale)/state->dF;
    state->howlHighBand=(HIGH_BAND<<dFScale)/state->dF;
    return status;
}
