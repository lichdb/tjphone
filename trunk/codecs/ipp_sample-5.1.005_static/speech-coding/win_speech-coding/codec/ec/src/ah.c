/* /////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
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
#include "ah_api.h"
#include "ownah.h"
#include "math.h"
//#define _USC_ARCH_XSC
static Ipp32f bqTaps2[]={
    0.068460f,0.13692f,0.06846f,/* b00 b01 b02 */
    1.f,-1.1500138f,0.42438f,   /* a00 a01 a02 */

    0.98531f,-1.9706f,0.06846f, /* b10 b11 b12 */
    1.f,-1.1500138f,0.42438f    /* a10 a11 a12 */

};
int ah_SetFreqShift(_ahState *state,int shift){
    if (shift<0) {
        state->shift = 0;
        return 1;
    }else{
        state->shift = shift;
        return 0;
    }
}
int ah_SetHowlPeriod(_ahState *state,int howlPeriod){
    if (howlPeriod<=0) {
        state->howlPeriodInit=1;
        return 1;
    }else{
        state->howlPeriodInit=howlPeriod;
        return 0;
    }
}

int ah_SetMode(_ahState *state, AHOpcode op){
    if((op==AH_ENABLE1) || (op==AH_ENABLE2) || (op==AH_DISABLE)){
        state->mode=op;
        return 0;
    } else
        return 1;
}

static Ipp32f ms_en(Ipp32f *src, int len)
{
    Ipp64f en;
    ippsDotProd_32f64f(src,src,len,&en);
    return (Ipp32f)(en/len);
}
static int addHist(_ahState *state, Ipp32f *src, Ipp32f **histEnd ){
    if(state->howlHistCurLen < state->howlHistLen){
        state->prevHistExist=0;
        /* create the history */
        ippsCopy_32f(src,state->howlHist+state->howlHistCurLen,state->frameSize);
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
        ippsCopy_32f(state->howlHist+state->frameSize,state->howlHist,
        state->howlHistLen-state->frameSize);
    ippsCopy_32f(src,state->howlHist+state->howlHistLen-state->frameSize,
        state->frameSize);
    *histEnd=state->howlHist+state->howlHistCurLen;
    state->prevHistExist=1;
    return state->howlHistCurLen;
}
static int detectHowlingIIR(_ahState *state, Ipp32f *in, Ipp32f rms){
    float arms;
    int   dstLen,phase=0,dstLen2;
    Ipp32f out[SB_FRAME_SIZE];// must be the max frame size???
    ippsSampleUp_32f(in,state->frameSize,state->upDownBuf,&dstLen,4,&phase);
    ippsIIR_32f_I(state->upDownBuf,dstLen,state->bq2StateTest);
    ippsSampleDown_32f(state->upDownBuf,dstLen,out,&dstLen2,4,&phase);
    arms = ms_en(out,state->frameSize);
    if ((rms*100>arms*120)){
        ippsIIRInit_BiQuad_32f(&state->bq2StateTest,bqTaps2,2,0,
            (Ipp8u *)state->bq2StateTest);
        return 2; /* howling started */
    }
    return 0;
}
#define BW_AMPL_PRECISION 2
static Ipp32f mean_32f(const Ipp32f *src, int len){
    double sum=0;
    int i;
    for (i=0;i<len;i++) {
        sum+=src[i];
    }
    return (Ipp32f)(sum/len);
}
static int detectHowlingFFT(_ahState *state,Ipp32f *in){
    Ipp32f   curMagn[(AH_IN_WORK_LEN+2)>>1],cmean;
    int      per[3]={11,22,33};
    int      max_idx[3]={0,0,0};
    int      i, flen=(AH_IN_WORK_LEN+2)>>1,disp[3];
    IppStatus stat;
    stat=ippsFFTFwd_RToCCS_32f((const Ipp32f *)in,(Ipp32f *)state->howlSpectrum,
        state->howlSpecFFTx,state->pBuf);
    stat=ippsMagnitude_32fc(state->howlSpectrum,curMagn,flen);
    stat=ippsMean_32f(curMagn,flen,&cmean,ippAlgHintNone);
    if(fabs(cmean)<1.e-5f)
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
static void shiftCutFreq(_ahState *state,Ipp32fc *src, int len){
    int shift,i;
    Ipp32fc val={1,1};
    ippsSet_32fc(val,src,state->howlLowBand);
    src[0].im=0;
    shift = ((state->shift<<dFScale)+(state->dF-1))/state->dF;
    if(shift>0){
        i=len-state->howlHighBand;
        shift=shift>i?i:shift;
        ippsCopy_32fc(&src[state->howlLowBand+shift],&src[state->howlLowBand],
            state->howlHighBand-state->howlLowBand);
    }
    ippsSet_32fc(val,&src[state->howlHighBand],len-state->howlHighBand);
    src[len-1].im=0;
    return;
}
static void filterHowlingFFT(_ahState *state, Ipp32f *out){
    ippsFFTFwd_RToCCS_32f((const Ipp32f *)state->howlHist+state->howlHistLen-(1<<AH_FFT_ORDER),
        (Ipp32f *)state->howlSpectrum, state->howlSpecFFT,state->pBuf);
    shiftCutFreq(state,state->howlSpectrum,((1<<AH_FFT_ORDER)+2)>>1);
    ippsFFTInv_CCSToR_32f((Ipp32f *)state->howlSpectrum,state->sBuf, state->howlSpecFFT,
        state->pBuf);
    ippsCopy_32f(state->sBuf+(1<<AH_FFT_ORDER)-state->frameSize,out,state->frameSize);
    //ippsMulC_32f_I(12.f,out,state->frameSize);
}
static void initFilter(_ahState *state){
    ippsIIRInit_BiQuad_32f(&state->bq2State,bqTaps2,2,0,
        (Ipp8u *)state->bq2State);
}
static void filterHowlingIIR(_ahState *state, Ipp32f *in, Ipp32f *out){
    int dstLen,phase=0,dstLen2;
    ippsSampleUp_32f(in,state->frameSize,state->upDownBuf,&dstLen,4,&phase);
    ippsIIR_32f_I(state->upDownBuf,dstLen,state->bq2State);
    ippsSampleDown_32f(state->upDownBuf,dstLen,out,&dstLen2,4,&phase);
    //ippsMulC_32f_I(12.f,out,state->frameSize);
}
AHOpcode ah_ProcessFrame(_ahState *state, Ipp32f *in, Ipp32f *out)
{
    int     detect,det;
    Ipp32f *histEnd,rms;
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
int ah_GetSize(int frameSize, int *size)
{
    int iirSize=0;
    int specSize,specSize1,bufSize,bufSize1,initSize,initSize1,gsize,hsize,ssize;
    ippsIIRGetStateSize_BiQuad_32f(2,&bufSize);
    iirSize=ALIGN(bufSize);
    iirSize+=ALIGN(bufSize);
    *size = ALIGN(frameSize*4*sizeof(Ipp32f)) +/* up buf size */
        iirSize;

    hsize  = (((1<<AH_FFT_ORDER)+frameSize-1)/frameSize)*frameSize*sizeof(Ipp32f);//howlHistLen
    ippsFFTGetSize_R_32f(AH_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &specSize, &initSize, &bufSize);
    ippsFFTGetSize_R_32f(AH_FFT_ORDERx, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &specSize1, &initSize1, &bufSize1);
    gsize = initSize;
    if (gsize < bufSize)
        gsize = bufSize;
    if (gsize < initSize1)
        gsize = initSize1;
    if (gsize < bufSize1)
        gsize = bufSize1;
    ssize = hsize;// sBuf length
    *size += ALIGN(sizeof(_ahState))+ALIGN(hsize)+ALIGN(ssize)+
        ALIGN((((1<<AH_FFT_ORDER)+2))*sizeof(Ipp32f))+// for spectrum cur
        ALIGN(specSize)+ALIGN(specSize1)+ ALIGN(gsize);
    return 0;
}
int ah_Init(_ahState *state,int samplingRate,int frameSize)
{
    int    bufSize, specSize, initSize,bufSize1, specSize1, initSize1, gsize, hlen;
    IppStatus status=ippStsOk;
    Ipp8u *pInit0,*pInit = (Ipp8u *)state,*pInit1;

    state->howlHistLen = (((1<<AH_FFT_ORDER)+frameSize-1)/frameSize)*frameSize;
    hlen=ALIGN(state->howlHistLen*sizeof(Ipp32f));
    pInit += ALIGN(sizeof(_ahState));
    state->upDownBuf = (Ipp32f *)pInit;
    pInit += ALIGN(frameSize*4*sizeof(Ipp32f));
    state->bq2State = (IppsIIRState_32f *)pInit;
    status = ippsIIRGetStateSize_BiQuad_32f(2,&bufSize);
    pInit += ALIGN(bufSize);
    state->bq2StateTest = (IppsIIRState_32f *)pInit;
    status = ippsIIRGetStateSize_BiQuad_32f(2,&bufSize);
    pInit += ALIGN(bufSize);
    status = ippsIIRInit_BiQuad_32f(&state->bq2State,bqTaps2,2,0,
        (Ipp8u *)state->bq2State);
    status = ippsIIRInit_BiQuad_32f(&state->bq2StateTest,bqTaps2,2,0,
        (Ipp8u *)state->bq2StateTest);

    status=ippsFFTGetSize_R_32f(AH_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &specSize, &initSize, &bufSize);
    status=ippsFFTGetSize_R_32f(AH_FFT_ORDERx, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &specSize1, &initSize1, &bufSize1);
    pInit0 = pInit;
    pInit += ALIGN(specSize);
    pInit1 = pInit;
    pInit += ALIGN(specSize1);
    status=ippsFFTInit_R_32f(&state->howlSpecFFT, AH_FFT_ORDER, IPP_FFT_DIV_INV_BY_N,
        ippAlgHintAccurate, pInit0, pInit);
    status=ippsFFTInit_R_32f(&state->howlSpecFFTx, AH_FFT_ORDERx, IPP_FFT_DIV_INV_BY_N,
        ippAlgHintAccurate, pInit1, pInit);
    gsize = initSize;
    if (gsize < bufSize)
        gsize = bufSize;
    if (gsize < initSize1)
        gsize = initSize1;
    if (gsize < bufSize1)
        gsize = bufSize1;
    state->pBuf = pInit;
    pInit += ALIGN(gsize);

    state->sBuf = (Ipp32f *)pInit;
    pInit += hlen;
    state->howlHist=(Ipp32f *)pInit;
    pInit += hlen;
    state->howlSpectrum =(Ipp32fc *)pInit;
    pInit += ALIGN((((1<<AH_FFT_ORDER)+2))*sizeof(Ipp32f));// for fft output in CC
    state->samplingRate = samplingRate;
    state->frameSize = frameSize;
    state->shift = 0;
    state->mode = AH_DISABLE;
    state->howlHistCurLen = 0;
    state->howlPeriodInit=(int)((state->samplingRate/state->frameSize));/* number of
          frames to be filtered after the howling has been detected*/
    state->howlPeriod=0;
    state->prevHistExist=0;
    state->dF=(samplingRate<<dFScale)>>AH_FFT_ORDER;    // fft spectrum freq step
    state->howlLowBand=(LOW_BAND<<dFScale)/state->dF;
    state->howlHighBand=(HIGH_BAND<<dFScale)/state->dF;
    return status;
}
