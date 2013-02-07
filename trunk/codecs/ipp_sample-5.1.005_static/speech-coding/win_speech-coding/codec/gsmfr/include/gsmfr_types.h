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
// Purpose: GSM FR 06.10: types declaration.
//
*/

#ifndef __GSMFR_TYPES_H__
#define __GSMFR_TYPES_H__
#include <stdio.h>
typedef unsigned char  uchar;
#define GSMFR_EXT_FRAME_LEN  160 /* length of input to encoder output of decoder frame=160 16-bit numbers*/
#define GSMFR_INT_FRAME_LEN  76  /* length of output of encoder input to decoder frame=76  16-bit numbers*/
#define GSMFR_PACKED_FRAME_LEN  33  /* length of bitstream to decoder. 33 8-bit numbers*/
#define GSMFR_RATE 0xD /* 13 kbit/s RPE-LTP */
typedef enum{
   ULAW =0,
   ALAW ,
   LINEAR
}GSMFR_FILE_t;

typedef enum{
   LTP_OFF=0,
   LTP_ON
}GSMFR_LTP_t;

struct GSMFR_Obj;
typedef struct GSMFR_Obj *GSMFR_Handle_t;

typedef enum {
  GSMFR_VAD_OFF=0,
  GSMFR_VAD_ON ,
  GSMFR_DOWNLINK,  /*VAD on , uplink mode*/
  GSMFR_UPLINK     /*VAD on , downlink mode*/
} GSMFR_VAD_t;

/* GSMFR status */
typedef enum{
  GSMFR_OK = 0,
  GSMFR_ERROR = -1
} GSMFR_Status_t;

typedef enum {
  GSMFR_ENCODE,
  GSMFR_DECODE
} GSMFR_DIRECTION_t;

/* The instance parameter structure */
typedef struct {
  GSMFR_VAD_t       vadMode;     /* VAD_ON or VAD_OFF */
  GSMFR_VAD_t       vadDirection;/* uplink or downlink */
  GSMFR_LTP_t       ltp_f;       /* cut ltp */
  GSMFR_DIRECTION_t direction;   /* GSMFR_ENCODE or GSMFR_DECODE */
} GSMFR_Params_t;
typedef struct _frame {
   short LARc[8];   /* 6 6 5 5 4 4 3 3 bits*/

   short Ncr0;      /* 7 bits */
   short bcr0;      /* 2 bits */
   short Mcr0;      /* 2 bits */
   short xmaxc0;    /* 6 bits */
   short xmcr0[13]; /* 3 bits per one */

   short Ncr1;
   short bcr1;
   short Mcr1;
   short xmaxc1;
   short xmcr1[13];

   short Ncr2;
   short bcr2;
   short Mcr2;
   short xmaxc2;
   short xmcr2[13];

   short Ncr3;
   short bcr3;
   short Mcr3;
   short xmaxc3;
   short xmcr3[13];
} frame;
/*************************************************************************
       GSM FR recieved and transmitted frame types
**************************************************************************/

typedef enum  {
   RX_SID = 0,
   RX_SPEECH_GOOD = 1,
   RX_BAD = 2,
   RX_N_FRAMETYPES = 3     /* frame types number*/
}RXFrameType;
typedef enum  {
   TX_SID = 0,
   TX_SPEECH_GOOD = 1,
   TX_BAD = 2,
   TX_N_FRAMETYPES = 3     /* frame types number*/
}TXFrameType;

#endif /* __GSMFR_TYPES_H__ */
