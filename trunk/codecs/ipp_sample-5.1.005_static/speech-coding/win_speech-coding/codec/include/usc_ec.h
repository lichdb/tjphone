/*****************************************************************************
//
// INTEL CORPORATION PROPRIETARY INFORMATION
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Intel Corporation and may not be copied
// or disclosed except in accordance with the terms of that agreement.
// Copyright (c) 2006 Intel Corporation. All Rights Reserved.
//
// Intel(R) Integrated Performance Primitives
//
//     USC - Unified Speech Codec interface library
//
//  Purpose: Unified Echo Canceller Interface header file.
//               Structures and functions declarations.
//***************************************************************************/
#ifndef __USC_EC_H__
#define __USC_EC_H__
#include "usc_base.h"

typedef enum {
   EC_SUBBAND     = 0,
   EC_FULLBAND    = 1,
   EC_FASTSUBBAND = 2
} USC_EC_Algs;

typedef enum {
    AD_OFF = 0,       /*            - no adaptation*/
    AD_FULLADAPT = 1, /*   -  full adaptation */
    AD_LITEADAPT = 2  /*     - lite adaptation*/
} USC_AdaptType;

typedef enum ah_op_code_enum {
    AH_DISABLE=0,
    AH_ENABLE1= 1,
    AH_ENABLE2= 2
} AHOpcode;

typedef enum op_code {
    EC_COEFFS_ZERO = 0,
    EC_ADAPTATION_ENABLE,
    EC_ADAPTATION_ENABLE_LITE,
    EC_ADAPTATION_DISABLE,
    EC_ADAPTATION_DISABLE_LITE,
    EC_NLP_ENABLE,
    EC_NLP_DISABLE,
    EC_TD_ENABLE,
    EC_TD_DISABLE,
    EC_AH_ENABLE1, /* Enable AH, Howling Detector mode 1 - spectra based*/
    EC_AH_ENABLE2, /* Enable AH, HD mode 2 - energy based*/
    EC_AH_DISABLE,
    EC_ALG1
} ECOpcode;
/* USC echo canceller modes */
typedef struct {
   USC_AdaptType adapt;      /* 0 - disable adaptation, 1 - enable full adaptation, 2 - enable lite adaptation  */
   int           zeroCoeff;  /* 0 - no zero coeffs of filters, 1 - zero coeffs of filters  */
   int           nlp;        /* 0 - disable NLP, 1 - enable NLP */
   int           td;         /* 0 - disable ToneDisabler, 1 - enable ToneDisabler */
   int           ah;         /* 0 - disable anti-howling,  1-spectra-based HD, 2- energy-based HD */
   int           hd_period;  /* The period of obligatory howling suppression when detected*/
   int           freq_shift; /* The value frequency to be shift by */
}USC_EC_Modes;

/* USC echo canceller option */
typedef struct {
   USC_EC_Algs  algType;    /* EC algorithm type */
   USC_PCMType  pcmType;    /* PCM type supported by the codec*/
   int          echotail;   /* echo tail length value, ms: in range [1, 200] for subband algorithm, [1, 16] for fullband algorithm */
   int          framesize;  /* EC frame size in bytes */
   int          nModes;     /* Number of modes ( actually, supposed to be equal to sizeof(USC_EC_Modes)/sizeof(int) ) */
   USC_EC_Modes modes;      /* EC modes*/
}USC_EC_Option;

/* USC echo canceller information */
typedef struct {
   const char        *name;     /* codec name */
   USC_EC_Option     params;    /*options supported by the EC type or the current options of an instance*/
}USC_EC_Info;

typedef struct {
    USC_baseFxns std;
   /*   CancelEcho()
        pSin - pointer to send-in PCM data [input]
        pSin - pointer to receive-in PCM data [input]
        pSin - pointer to send-out PCM data [output]
    */
    USC_Status (*CancelEcho)(USC_Handle handle, short *pSin, short *pRin, short *pSout);
} USC_EC_Fxns;

#endif /* __USC_EC_H__ */
