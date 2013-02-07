/*/////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2006 Intel Corporation. All Rights Reserved.
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
//
//
// Purpose: Echo Canceller [float point]: USC funtions.
//
*/

#include <ipp_w7.h>
#include "ipps.h"
#include "ippsc.h"
#include "ec_api.h"
#include <string.h>
#include "ecusc.h"

#define  SUBBAND_FRAME_SIZE    64
#define  FULLBAND_FRAME_SIZE    8
#define  FASTSBBAND_FRAME_SIZE 44
#define  SB_MAX_LEN_TAIL      200
#define  FB_MAX_LEN_TAIL       16
#define  FREQ_SHIFT_EC         10
#define  MAX_BLOCK_SIZE       128

static USC_Status GetInfoSize(int *pSize);
static USC_Status GetInfo(USC_Handle handle, USC_EC_Info *pInfo);
static USC_Status NumAlloc(const USC_EC_Option *options, int *nbanks);
static USC_Status MemAlloc(const USC_EC_Option *options, USC_MemBank *pBanks);
static USC_Status Init(const USC_EC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle);
static USC_Status Reinit(const USC_EC_Modes *modes, USC_Handle handle );
static USC_Status Control(const USC_EC_Modes *modes, USC_Handle handle );
static USC_Status CancelEcho(USC_Handle handle, short *pSin, short *pRin, short *pSout);

typedef struct {
   ec_GetFrameSize_ptr ec_GetFrameSize;
   ec_GetSize_ptr ec_GetSize;
   ec_Init_ptr ec_Init;
   ec_ModeOp_ptr ec_ModeOp;
   ec_ProcessFrame_ptr ec_ProcessFrame;
   ec_setAHFreqShift_ptr ec_setAHFreqShift;
   ec_setHDperiod_ptr ec_setHDperiod;
} ecfp_fun;

typedef struct {
   USC_EC_Algs   algType;
   USC_PCMType   pcmType;
   int           echotail;
   USC_AdaptType adapt;
   int           zeroCoeff;
   int           nlp;
   int           td;
   int           ah;
   int           reserve1;
   ecfp_fun      fun;
   int           hd_period;
   int           freq_shift;
} ECFP_Handle_Header;

/* global usc vector table */
USCFUN USC_EC_Fxns USC_ECFP_Fxns=
{
    {
        USC_EC,
        GetInfoSize,
        GetInfo,
        NumAlloc,
        MemAlloc,
        Init,
        Reinit,
        Control
    },
    CancelEcho

};


static USC_Status GetInfoSize(int *pSize)
{
    *pSize = sizeof(USC_EC_Info);
    return USC_NoError;
}
static USC_Status GetInfo(USC_Handle handle, USC_EC_Info *pInfo)
{
    ECFP_Handle_Header *ecfp_header;

    pInfo->name = "IPP_EC_FP";
    if (handle == NULL) {
      pInfo->params.framesize = SUBBAND_FRAME_SIZE*sizeof(short);
      pInfo->params.algType = EC_SUBBAND;
      pInfo->params.pcmType.sample_frequency = 8000;
      pInfo->params.pcmType.bitPerSample = 16;
      pInfo->params.echotail = 16;
      pInfo->params.modes.adapt = AD_FULLADAPT;
      pInfo->params.modes.zeroCoeff = 0;
      pInfo->params.modes.nlp = 0;
      pInfo->params.modes.td = 1;
      pInfo->params.modes.ah = 0;         /* AH off*/
      pInfo->params.modes.hd_period = -1; /* Set the number of frames to be filtered after the howling has been detected */
      pInfo->params.modes.freq_shift = 0; /* Set the frequency shift number.*/
    } else {
      ecfp_header = (ECFP_Handle_Header*)handle;
      if(ecfp_header->algType == EC_SUBBAND) {
        pInfo->params.algType = EC_SUBBAND;
        pInfo->params.framesize = SUBBAND_FRAME_SIZE*sizeof(short);
      } else if(ecfp_header->algType == EC_FULLBAND) {
        pInfo->params.algType = EC_FULLBAND;
        pInfo->params.framesize = FULLBAND_FRAME_SIZE*sizeof(short);
      } else {
        pInfo->params.algType = EC_FASTSUBBAND;
        pInfo->params.framesize = FASTSBBAND_FRAME_SIZE*sizeof(short);
      }
      pInfo->params.pcmType.sample_frequency = ecfp_header->pcmType.sample_frequency;
      pInfo->params.pcmType.bitPerSample = ecfp_header->pcmType.bitPerSample;
      pInfo->params.echotail = ecfp_header->echotail;
      pInfo->params.modes.adapt = ecfp_header->adapt;
      pInfo->params.modes.zeroCoeff = ecfp_header->zeroCoeff;
      pInfo->params.modes.nlp = ecfp_header->nlp;
      pInfo->params.modes.td = ecfp_header->td;
      pInfo->params.modes.ah = ecfp_header->ah;
      pInfo->params.modes.hd_period = ecfp_header->hd_period;
      pInfo->params.modes.freq_shift = ecfp_header->freq_shift;
      pInfo->params.nModes = sizeof(USC_EC_Modes)/sizeof(int);
    }
    return USC_NoError;
}

static USC_Status NumAlloc(const USC_EC_Option *options, int *nbanks)
{
   if(options==NULL) return USC_BadDataPointer;
   if(nbanks==NULL) return USC_BadDataPointer;
   *nbanks = 1;
   return USC_NoError;
}

static USC_Status MemAlloc(const USC_EC_Option *options, USC_MemBank *pBanks)
{
   unsigned int nbytes;
   IppPCMFrequency freq;
   int taptime_ms;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
   if(options->pcmType.bitPerSample != 16) return USC_UnsupportedPCMType;
   switch(options->pcmType.sample_frequency) {
     case 8000:  freq = IPP_PCM_FREQ_8000;  break;
     case 16000: freq = IPP_PCM_FREQ_16000; break;
     default: return USC_UnsupportedPCMType;
   }

   pBanks->pMem = NULL;
   if(options->algType == EC_SUBBAND) {
     if((options->echotail > 0) && (options->echotail <= SB_MAX_LEN_TAIL)) taptime_ms = options->echotail;
     else return USC_UnsupportedEchoTail;
     ec_sb_GetSize(freq, taptime_ms, (int *)&nbytes);
   } else if(options->algType == EC_FULLBAND) {
     if((options->echotail > 0) && (options->echotail <= FB_MAX_LEN_TAIL)) taptime_ms = options->echotail;
     else return USC_UnsupportedEchoTail;
     ec_fb_GetSize(freq, taptime_ms, (int *)&nbytes);
   } else {
     if((options->echotail > 0) && (options->echotail <= SB_MAX_LEN_TAIL)) taptime_ms = options->echotail;
     else return USC_UnsupportedEchoTail;
     ec_sbf_GetSize(freq, taptime_ms, (int *)&nbytes);
   }
   pBanks->nbytes = nbytes + sizeof(ECFP_Handle_Header); /* room for USC header */
   return USC_NoError;
}

static USC_Status Init(const USC_EC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle)
{
   ECFP_Handle_Header *ecfp_header;
   IppPCMFrequency freq;
   int taptime_ms;
   USC_Handle *obj_ec;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
   if(pBanks->pMem==NULL) return USC_NotInitialized;
   if(pBanks->nbytes<=0) return USC_NotInitialized;
   if(handle==NULL) return USC_InvalidHandler;
   if(options->pcmType.bitPerSample != 16) return USC_UnsupportedPCMType;
   switch(options->pcmType.sample_frequency) {
     case 8000:  freq = IPP_PCM_FREQ_8000;  break;
     case 16000: freq = IPP_PCM_FREQ_16000; break;
     default: return USC_UnsupportedPCMType;
   }

   *handle = (USC_Handle*)pBanks->pMem;
   ecfp_header = (ECFP_Handle_Header*)*handle;

   ecfp_header->algType = options->algType;
   ecfp_header->pcmType.sample_frequency = options->pcmType.sample_frequency;
   ecfp_header->pcmType.bitPerSample = options->pcmType.bitPerSample;
   if(ecfp_header->algType == EC_SUBBAND) {
     if((options->echotail > 0) && (options->echotail <= SB_MAX_LEN_TAIL)) taptime_ms = options->echotail;
     else return USC_UnsupportedEchoTail;
     ecfp_header->fun.ec_GetFrameSize = (ec_GetFrameSize_ptr)ec_sb_GetFrameSize;
     ecfp_header->fun.ec_GetSize = (ec_GetSize_ptr)ec_sb_GetSize;
     ecfp_header->fun.ec_Init = (ec_Init_ptr)ec_sb_Init;
     ecfp_header->fun.ec_ModeOp = (ec_ModeOp_ptr)ec_sb_ModeOp;
     ecfp_header->fun.ec_ProcessFrame = (ec_ProcessFrame_ptr)ec_sb_ProcessFrame;

     ecfp_header->fun.ec_setAHFreqShift= (ec_setAHFreqShift_ptr)ec_sb_setAHFreqShift;
     ecfp_header->fun.ec_setHDperiod= (ec_setHDperiod_ptr)ec_sb_setAHhowlPeriod;
   } else if(ecfp_header->algType == EC_FULLBAND) {
     if((options->echotail > 0) && (options->echotail <= FB_MAX_LEN_TAIL)) taptime_ms = options->echotail;
     else return USC_UnsupportedEchoTail;
     ecfp_header->fun.ec_GetFrameSize = (ec_GetFrameSize_ptr)ec_fb_GetFrameSize;
     ecfp_header->fun.ec_GetSize = (ec_GetSize_ptr)ec_fb_GetSize;
     ecfp_header->fun.ec_Init = (ec_Init_ptr)ec_fb_Init;
     ecfp_header->fun.ec_ModeOp = (ec_ModeOp_ptr)ec_fb_ModeOp;
     ecfp_header->fun.ec_ProcessFrame = (ec_ProcessFrame_ptr)ec_fb_ProcessFrame;

     ecfp_header->fun.ec_setAHFreqShift= (ec_setAHFreqShift_ptr)ec_fb_setAHFreqShift;
     ecfp_header->fun.ec_setHDperiod= (ec_setHDperiod_ptr)ec_fb_setAHhowlPeriod;
   } else {
     if((options->echotail > 0) && (options->echotail <= SB_MAX_LEN_TAIL)) taptime_ms = options->echotail;
     else return USC_UnsupportedEchoTail;
     ecfp_header->fun.ec_GetFrameSize = (ec_GetFrameSize_ptr)ec_sbf_GetFrameSize;
     ecfp_header->fun.ec_GetSize = (ec_GetSize_ptr)ec_sbf_GetSize;
     ecfp_header->fun.ec_Init = (ec_Init_ptr)ec_sbf_Init;
     ecfp_header->fun.ec_ModeOp = (ec_ModeOp_ptr)ec_sbf_ModeOp;
     ecfp_header->fun.ec_ProcessFrame = (ec_ProcessFrame_ptr)ec_sbf_ProcessFrame;

     ecfp_header->fun.ec_setAHFreqShift= (ec_setAHFreqShift_ptr)ec_sbf_setAHFreqShift;
     ecfp_header->fun.ec_setHDperiod= (ec_setHDperiod_ptr)ec_sbf_setAHhowlPeriod;
   }
   ecfp_header->echotail = taptime_ms;
   ecfp_header->adapt = options->modes.adapt;
   ecfp_header->zeroCoeff = options->modes.zeroCoeff;
   ecfp_header->nlp = options->modes.nlp;
   ecfp_header->td = options->modes.td;
   ecfp_header->ah = options->modes.ah;
   ecfp_header->hd_period = options->modes.hd_period;
   ecfp_header->freq_shift = options->modes.freq_shift;

   obj_ec = (USC_Handle*)((char*)*handle + sizeof(ECFP_Handle_Header));
   ecfp_header->fun.ec_Init(obj_ec, freq, ecfp_header->echotail);

   if(ecfp_header->zeroCoeff) ecfp_header->fun.ec_ModeOp(obj_ec, EC_COEFFS_ZERO);
   if((ecfp_header->adapt == AD_FULLADAPT) || (ecfp_header->adapt == AD_LITEADAPT)) ecfp_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_ENABLE);
   else ecfp_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_DISABLE);
   if(ecfp_header->nlp) ecfp_header->fun.ec_ModeOp(obj_ec, EC_NLP_ENABLE);
   else ecfp_header->fun.ec_ModeOp(obj_ec, EC_NLP_DISABLE);
   if(ecfp_header->td) ecfp_header->fun.ec_ModeOp(obj_ec, EC_TD_ENABLE);
   else ecfp_header->fun.ec_ModeOp(obj_ec, EC_TD_DISABLE);

   switch(ecfp_header->ah) {
    case 0:
        ecfp_header->fun.ec_ModeOp(obj_ec, EC_AH_DISABLE);
        break;
    case 1:
        ecfp_header->fun.ec_ModeOp(obj_ec, EC_AH_ENABLE1);
        break;
    case 2:
        ecfp_header->fun.ec_ModeOp(obj_ec, EC_AH_ENABLE2);
        break;
    default:
        ecfp_header->fun.ec_ModeOp(obj_ec, EC_AH_DISABLE);
   }
   ecfp_header->fun.ec_setAHFreqShift(obj_ec, ecfp_header->freq_shift);
   ecfp_header->fun.ec_setHDperiod(obj_ec, ecfp_header->hd_period);

   return USC_NoError;
}

static USC_Status Reinit(const USC_EC_Modes *modes, USC_Handle handle )
{
    ECFP_Handle_Header *ecfp_header;
    USC_Handle *obj_ec;

    if(modes==NULL) return USC_BadDataPointer;
    if(handle==NULL) return USC_InvalidHandler;

    ecfp_header = (ECFP_Handle_Header*)handle;
    obj_ec = (USC_Handle*)((char*)handle + sizeof(ECFP_Handle_Header));

    ecfp_header->adapt = modes->adapt;
    ecfp_header->zeroCoeff = modes->zeroCoeff;
    ecfp_header->nlp = modes->nlp;
    ecfp_header->td = modes->td;
    ecfp_header->ah = modes->ah;

    if(ecfp_header->zeroCoeff) ecfp_header->fun.ec_ModeOp(obj_ec, EC_COEFFS_ZERO);
    if((ecfp_header->adapt == AD_FULLADAPT) || (ecfp_header->adapt == AD_LITEADAPT)) ecfp_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_ENABLE);
    else ecfp_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_DISABLE);
    if(ecfp_header->nlp) ecfp_header->fun.ec_ModeOp(obj_ec, EC_NLP_ENABLE);
    else ecfp_header->fun.ec_ModeOp(obj_ec, EC_NLP_DISABLE);
    if(ecfp_header->td) ecfp_header->fun.ec_ModeOp(obj_ec, EC_TD_ENABLE);
    else ecfp_header->fun.ec_ModeOp(obj_ec, EC_TD_DISABLE);

    switch(ecfp_header->ah) {
    case 0:
        ecfp_header->fun.ec_ModeOp(obj_ec, EC_AH_DISABLE);
        break;
    case 1:
        ecfp_header->fun.ec_ModeOp(obj_ec, EC_AH_ENABLE1);
        break;
    case 2:
        ecfp_header->fun.ec_ModeOp(obj_ec, EC_AH_ENABLE2);
        break;
    default:
        ecfp_header->fun.ec_ModeOp(obj_ec, EC_AH_DISABLE);
    }
//    ecfp_header->fun.ec_ModeOp(obj_ec, ecfp_header->ah);
    ecfp_header->fun.ec_setAHFreqShift(obj_ec, ecfp_header->freq_shift);
    ecfp_header->fun.ec_setHDperiod(obj_ec, ecfp_header->hd_period);
    return USC_NoError;
}

static USC_Status Control(const USC_EC_Modes *modes, USC_Handle handle )
{

    ECFP_Handle_Header *ecfp_header;
    USC_Handle *obj_ec;

    if(modes==NULL) return USC_BadDataPointer;
    if(handle==NULL) return USC_InvalidHandler;

    ecfp_header = (ECFP_Handle_Header*)handle;
    obj_ec = (USC_Handle*)((char*)handle + sizeof(ECFP_Handle_Header));

    ecfp_header->zeroCoeff = modes->zeroCoeff;
    if(ecfp_header->zeroCoeff) ecfp_header->fun.ec_ModeOp(obj_ec, EC_COEFFS_ZERO);
    if(ecfp_header->adapt != modes->adapt) {
      ecfp_header->adapt = modes->adapt;
      if((ecfp_header->adapt == AD_FULLADAPT) || (ecfp_header->adapt == AD_LITEADAPT)) ecfp_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_ENABLE);
      else ecfp_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_DISABLE);
    }
    if(ecfp_header->nlp != modes->nlp) {
      ecfp_header->nlp = modes->nlp;
      if(ecfp_header->nlp) ecfp_header->fun.ec_ModeOp(obj_ec, EC_NLP_ENABLE);
      else ecfp_header->fun.ec_ModeOp(obj_ec, EC_NLP_DISABLE);
    }
    if(ecfp_header->td != modes->td) {
      ecfp_header->td = modes->td;
      if(ecfp_header->td) ecfp_header->fun.ec_ModeOp(obj_ec, EC_TD_ENABLE);
      else ecfp_header->fun.ec_ModeOp(obj_ec, EC_TD_DISABLE);
    }
    if(ecfp_header->ah != modes->ah) {
      ecfp_header->ah = modes->ah;
      switch(ecfp_header->ah) {
        case 0:
            ecfp_header->fun.ec_ModeOp(obj_ec, EC_AH_DISABLE);
            break;
        case 1:
            ecfp_header->fun.ec_ModeOp(obj_ec, EC_AH_ENABLE1);
            break;
        case 2:
            ecfp_header->fun.ec_ModeOp(obj_ec, EC_AH_ENABLE2);
            break;
        default:
            ecfp_header->fun.ec_ModeOp(obj_ec, EC_AH_DISABLE);
        }
    }
    if(ecfp_header->freq_shift != modes->freq_shift)
        ecfp_header->freq_shift = modes->freq_shift;
    ecfp_header->fun.ec_setAHFreqShift(obj_ec, modes->freq_shift);
    if(ecfp_header->hd_period != modes->hd_period)
        ecfp_header->hd_period = modes->hd_period;
    ecfp_header->fun.ec_setHDperiod(obj_ec, modes->hd_period);

    return USC_NoError;
}

static USC_Status CancelEcho(USC_Handle handle, short *pSin, short *pRin, short *pSout)
{
   ECFP_Handle_Header *ecfp_header;
   USC_Handle *obj_ec;
   int framesize;
   Ipp32f r_in_32f_cur[MAX_BLOCK_SIZE];
   Ipp32f s_in_32f_cur[MAX_BLOCK_SIZE];
   Ipp32f s_out_32f_cur[MAX_BLOCK_SIZE];

   if(handle==NULL) return USC_InvalidHandler;
   if(pSin==NULL) return USC_BadDataPointer;
   if(pRin==NULL) return USC_BadDataPointer;
   if(pSout==NULL) return USC_BadDataPointer;

   ecfp_header = (ECFP_Handle_Header*)handle;
   obj_ec = (USC_Handle*)((char*)handle + sizeof(ECFP_Handle_Header));
   if(ecfp_header->algType == EC_SUBBAND) framesize = SUBBAND_FRAME_SIZE;
   else if(ecfp_header->algType == EC_FULLBAND) framesize = FULLBAND_FRAME_SIZE;
   else framesize = FASTSBBAND_FRAME_SIZE;

   ippsConvert_16s32f_Sfs((Ipp16s *)pRin, r_in_32f_cur, framesize, 0);
   ippsConvert_16s32f_Sfs((Ipp16s *)pSin, s_in_32f_cur, framesize, 0);
   ecfp_header->fun.ec_ProcessFrame(obj_ec, r_in_32f_cur, s_in_32f_cur, s_out_32f_cur);
   ippsConvert_32f16s_Sfs(s_out_32f_cur, (Ipp16s *)pSout, framesize, ippRndZero, 0);

   return USC_NoError;

}


