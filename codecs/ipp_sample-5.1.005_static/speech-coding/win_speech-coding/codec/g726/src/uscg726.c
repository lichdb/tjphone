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
// Purpose: G.726 speech codec: USC functions.
//
*/
#if defined( _WIN32_WCE)
#pragma warning( disable : 4505 )
#endif

#include <string.h>
#include <usc.h>
#include <ipp_w7.h>
#include <ipps.h>
#include <ippsc.h>

#include "scratchmem.h"

#define  BITSTREAM_SIZE   5
#define  G726_NUM_RATES   4
#define  G726_FRAME_SIZE  8
#define  G726_BITS_PER_SAMPLE 16

static USC_Status GetInfoSize(int *pSize);
static USC_Status GetInfo(USC_Handle handle, USC_CodecInfo *pInfo);
static USC_Status NumAlloc(const USC_Option *options, int *nbanks);
static USC_Status MemAlloc(const USC_Option *options, USC_MemBank *pBanks);
static USC_Status Init(const USC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle);
static USC_Status Reinit(const USC_Modes *modes, USC_Handle handle );
static USC_Status Control(const USC_Modes *modes, USC_Handle handle );
static USC_Status Encode(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out);
static USC_Status Decode(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out);
static USC_Status GetOutStreamSize(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst);
static USC_Status SetFrameSize(const USC_Option *options, USC_Handle handle, int frameSize);


typedef struct {
    int direction;
    int bitrate;
    int law;
    int frameSize; // for future extension
} G726_Handle_Header;

/* global usc vector table */
USCFUN USC_Fxns USC_G726_Fxns=
{
    {
        USC_Codec,
        GetInfoSize,
        GetInfo,
        NumAlloc,
        MemAlloc,
        Init,
        Reinit,
        Control
    },
    Encode,
    Decode,
    GetOutStreamSize,
    SetFrameSize

};

static USC_Option  params;  /* what is supported  */
static USC_PCMType pcmType; /* codec audio source */


static __ALIGN32 CONST short LostFrame[8]=
{ 0, 0, 0, 0, 0, 0, 0, 0 };

static __ALIGN32 CONST USC_Rates pTblRates_G726[G726_NUM_RATES]={
    {40000},
    {32000},
    {24000},
    {16000}
};

static int CheckRate_G726(int rate_bps)
{
    int rate;

    switch(rate_bps) {
      case 16000: rate = 0; break;
      case 24000: rate = 1; break;
      case 32000: rate = 2; break;
      case 40000: rate = 3; break;
      default: rate = -1; break;
    }
    return rate;
}

static USC_Status GetInfoSize(int *pSize)
{
    *pSize = sizeof(USC_CodecInfo);
    return USC_NoError;
}

static USC_Status GetInfo(USC_Handle handle, USC_CodecInfo *pInfo)
{
    G726_Handle_Header *g726_header;

    pInfo->name = "IPP_G726";
    if (handle == NULL) {
      pInfo->params.modes.bitrate = 16000;
      pInfo->params.direction = 0;
      pInfo->params.law = 3;
      pInfo->params.framesize = G726_FRAME_SIZE*sizeof(short);
      pInfo->maxbitsize = BITSTREAM_SIZE;
    } else {
      g726_header = (G726_Handle_Header*)handle;
      pInfo->params.modes.bitrate = g726_header->bitrate;
      pInfo->params.direction = g726_header->direction;
      pInfo->params.law = g726_header->law;
      pInfo->params.framesize = g726_header->frameSize;
      pInfo->maxbitsize = (g726_header->frameSize / (G726_FRAME_SIZE*sizeof(short))) * BITSTREAM_SIZE;
    }
    pInfo->params.modes.vad = 0;
    pInfo->params.modes.truncate = 0;
    pInfo->pcmType.sample_frequency = 8000;
    pInfo->pcmType.bitPerSample = G726_BITS_PER_SAMPLE;
    pInfo->params.modes.hpf = 0;
    pInfo->params.modes.pf = 0;
    pInfo->nRates = G726_NUM_RATES;
    pInfo->pRateTbl = (const USC_Rates *)&pTblRates_G726;

    return USC_NoError;
}

static USC_Status NumAlloc(const USC_Option *options, int *nbanks)
{
   if(options==NULL) return USC_BadDataPointer;
   if(nbanks==NULL) return USC_BadDataPointer;
   *nbanks = 1;
   return USC_NoError;
}

static USC_Status MemAlloc(const USC_Option *options, USC_MemBank *pBanks)
{
    unsigned int nbytes;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;

    pBanks->pMem = NULL;
    pBanks->align = 32;
    pBanks->memType = USC_OBJECT;
    pBanks->memSpaceType = USC_NORMAL;

    if (options->direction == 0) /* encode only */
    {
        ippsEncodeGetStateSize_G726_16s8u(&nbytes);
    }
    else if (options->direction == 1) /* decode only */
    {
        ippsDecodeGetStateSize_G726_8u16s(&nbytes);
    } else {
        return USC_NoOperation;
    }
    pBanks->nbytes = nbytes + sizeof(G726_Handle_Header); /* room for USC header */
    return USC_NoError;
}
static IppPCMLaw Usc2IppLaw(int uscLaw)
{
   if(uscLaw==0) return IPP_PCM_LINEAR;
   else if(uscLaw==1) return IPP_PCM_ALAW;
   else return IPP_PCM_MULAW;
}
static __ALIGN32 CONST IppSpchBitRate usc2g726Rate[4]={
    IPP_SPCHBR_16000,
    IPP_SPCHBR_24000,
    IPP_SPCHBR_32000,
    IPP_SPCHBR_40000
};

static USC_Status Init(const USC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle)
{
    G726_Handle_Header *g726_header;
    int bitrate_idx;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
   if(pBanks->pMem==NULL) return USC_NotInitialized;
   if(pBanks->nbytes<=0) return USC_NotInitialized;
   if(handle==NULL) return USC_InvalidHandler;

    *handle = (USC_Handle*)pBanks->pMem;
    g726_header = (G726_Handle_Header*)*handle;

    bitrate_idx = CheckRate_G726(options->modes.bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;
    g726_header->direction = options->direction;
    g726_header->bitrate = options->modes.bitrate;
    g726_header->law = options->law;
    g726_header->frameSize = G726_FRAME_SIZE*sizeof(short);

    if (options->direction == 0) /* encode only */
    {
        IppsEncoderState_G726_16s *EncObj = (IppsEncoderState_G726_16s *)((char*)*handle + sizeof(G726_Handle_Header));
        ippsEncodeInit_G726_16s8u(EncObj, usc2g726Rate[bitrate_idx]);
    }
    else if (options->direction == 1) /* decode only */
    {
        IppsDecoderState_G726_16s *DecObj = (IppsDecoderState_G726_16s *)((char*)*handle + sizeof(G726_Handle_Header));
        ippsDecodeInit_G726_8u16s(DecObj, usc2g726Rate[bitrate_idx],Usc2IppLaw(options->law));

    } else {
        return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Reinit(const USC_Modes *modes, USC_Handle handle )
{
   G726_Handle_Header *g726_header;
   int bitrate_idx;

   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

    g726_header = (G726_Handle_Header*)handle;

    bitrate_idx = CheckRate_G726(modes->bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;
    g726_header->bitrate = modes->bitrate;
    g726_header->frameSize = G726_FRAME_SIZE*sizeof(short);

    if (g726_header->direction == 0) /* encode only */
    {
        IppsEncoderState_G726_16s *EncObj = (IppsEncoderState_G726_16s *)((char*)handle + sizeof(G726_Handle_Header));
        ippsEncodeInit_G726_16s8u(EncObj, usc2g726Rate[bitrate_idx]);///////////////////////////
    }
    else if (g726_header->direction == 1) /* decode only */
    {
        IppsDecoderState_G726_16s *DecObj = (IppsDecoderState_G726_16s *)((char*)handle + sizeof(G726_Handle_Header));
        ippsDecodeInit_G726_8u16s(DecObj, usc2g726Rate[bitrate_idx],Usc2IppLaw(g726_header->law));////////////////
    } else {
        return USC_NoOperation;
    }
    return USC_NoError;
}

static USC_Status Control(const USC_Modes *modes, USC_Handle handle )
{
   if(modes==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;
   return USC_NoError;
}

int OutFrameSize(int bitrate_bps)
{
   switch(bitrate_bps) {
      case 16000: return 2;
      case 24000: return 3;
      case 32000: return 4;
      case 40000: return 5;
      default: return 0;
   }
}
__INLINE int PackCodeword(const char* inCode, char* outCode, int bitrate, int *outLen)
{
   *outLen = OutFrameSize(bitrate);
   if(*outLen==0) return 0;

   if(*outLen == 2) {
     outCode[0] = (char)((inCode[3] << 6) | ((inCode[2] & 0x3) << 4) | ((inCode[1] & 0x3) << 2) | (inCode[0] & 0x3));
     outCode[1] = (char)((inCode[7] << 6) | ((inCode[6] & 0x3) << 4) | ((inCode[5] & 0x3) << 2) | (inCode[4] & 0x3));
   } else if(*outLen == 3) {
     outCode[0] = (char)((inCode[0] & 0x7) | ((inCode[1] & 0x7)<<3) | ((inCode[2] & 0x3)<<6));
     outCode[1] = (char)(((inCode[2] & 0x4)>>2) | ((inCode[3] & 0x7)<<1) | ((inCode[4] & 0x7)<<4) | ((inCode[5] & 0x1)<<7));
     outCode[2] = (char)(((inCode[5] & 0x7)>>1) | ((inCode[6] & 0x7)<<2) | ((inCode[7] & 0x7)<<5));
   } else if(*outLen == 4) {
     outCode[0] = (char)((inCode[1] << 4) | (inCode[0] & 0xf));
     outCode[1] = (char)((inCode[3] << 4) | (inCode[2] & 0xf));
     outCode[2] = (char)((inCode[5] << 4) | (inCode[4] & 0xf));
     outCode[3] = (char)((inCode[7] << 4) | (inCode[6] & 0xf));
   } else if(*outLen == 5) {
     outCode[0] = (char)((inCode[0] & 0x1f) | ((inCode[1] & 0x7)<<5));
     outCode[1] = (char)(((inCode[1] & 0x18)>>3) | ((inCode[2] & 0x1f)<<2) | ((inCode[3] & 0x1)<<7));
     outCode[2] = (char)(((inCode[3] & 0x1e)>>1) | ((inCode[4] & 0xf)<<4));
     outCode[3] = (char)(((inCode[4] & 0x10)>>4) | ((inCode[5] & 0x1f)<<1) | ((inCode[6] & 0x3)<<6));
     outCode[4] = (char)(((inCode[6] & 0x1c)>>2) | ((inCode[7] & 0x1f)<<3));
   }

   return 1;
}

__INLINE int PackCodeword_tst(const char* inCode, char* outCode, int bitrate, int inLen, int *outLen)
{
   int i, frmLen, packedLen = 0;
   frmLen = OutFrameSize(bitrate);
   if(frmLen==0) return 0;

   if(frmLen == 2) {
      for(i=0;i<inLen;i+=8) {
         outCode[0] = (char)((inCode[3] << 6) | ((inCode[2] & 0x3) << 4) | ((inCode[1] & 0x3) << 2) | (inCode[0] & 0x3));
         outCode[1] = (char)((inCode[7] << 6) | ((inCode[6] & 0x3) << 4) | ((inCode[5] & 0x3) << 2) | (inCode[4] & 0x3));
         outCode += 2;
         inCode += 8;
         packedLen += 2;
      }
   } else if(frmLen == 3) {
      for(i=0;i<inLen;i+=8) {
         outCode[0] = (char)((inCode[0] & 0x7) | ((inCode[1] & 0x7)<<3) | ((inCode[2] & 0x3)<<6));
         outCode[1] = (char)(((inCode[2] & 0x4)>>2) | ((inCode[3] & 0x7)<<1) | ((inCode[4] & 0x7)<<4) | ((inCode[5] & 0x1)<<7));
         outCode[2] = (char)(((inCode[5] & 0x7)>>1) | ((inCode[6] & 0x7)<<2) | ((inCode[7] & 0x7)<<5));
         outCode += 3;
         inCode += 8;
         packedLen += 3;
      }
   } else if(frmLen == 4) {
      for(i=0;i<inLen;i+=8) {
         outCode[0] = (char)((inCode[1] << 4) | (inCode[0] & 0xf));
         outCode[1] = (char)((inCode[3] << 4) | (inCode[2] & 0xf));
         outCode[2] = (char)((inCode[5] << 4) | (inCode[4] & 0xf));
         outCode[3] = (char)((inCode[7] << 4) | (inCode[6] & 0xf));
         outCode += 4;
         inCode += 8;
         packedLen += 4;
      }
   } else if(frmLen == 5) {
      for(i=0;i<inLen;i+=8) {
         outCode[0] = (char)((inCode[0] & 0x1f) | ((inCode[1] & 0x7)<<5));
         outCode[1] = (char)(((inCode[1] & 0x18)>>3) | ((inCode[2] & 0x1f)<<2) | ((inCode[3] & 0x1)<<7));
         outCode[2] = (char)(((inCode[3] & 0x1e)>>1) | ((inCode[4] & 0xf)<<4));
         outCode[3] = (char)(((inCode[4] & 0x10)>>4) | ((inCode[5] & 0x1f)<<1) | ((inCode[6] & 0x3)<<6));
         outCode[4] = (char)(((inCode[6] & 0x1c)>>2) | ((inCode[7] & 0x1f)<<3));
         outCode += 5;
         inCode += 8;
         packedLen += 5;
      }
   }

   *outLen = packedLen;
   return 1;
}

static USC_Status Encode(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out)
{
    G726_Handle_Header *g726_header;
    int bitrate_idx, inputLen, outLen = 0, foo = 0;
    IPP_ALIGNED_ARRAY(16, short, work_buf, 10*G726_FRAME_SIZE);/*10 ms*/
    IPP_ALIGNED_ARRAY(16, unsigned char, outBuffer, 10*G726_FRAME_SIZE);/*10 ms*/
    IppsEncoderState_G726_16s *EncObj;
    short *pPCMPtr;
    char  *pBitstrPtr;

   if(in==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

    g726_header = (G726_Handle_Header*)handle;
    EncObj = (IppsEncoderState_G726_16s *)((char*)handle + sizeof(G726_Handle_Header));

    inputLen = in->nbytes;
    if(inputLen <= 0) return USC_NoOperation;

    if(in->nbytes > g726_header->frameSize)
      inputLen = g726_header->frameSize;
    else
      inputLen = in->nbytes;

    if(g726_header->bitrate != in->bitrate) {
      bitrate_idx = CheckRate_G726(in->bitrate);
      if(bitrate_idx < 0) return USC_UnsupportedBitRate;
      ippsEncodeInit_G726_16s8u(EncObj, usc2g726Rate[bitrate_idx]);
      g726_header->bitrate = in->bitrate;
    }

    inputLen = in->nbytes;
    pPCMPtr = (short*)in->pBuffer;
    pBitstrPtr = out->pBuffer;
    while(inputLen >= (10*G726_FRAME_SIZE*sizeof(short))) {
       ippsRShiftC_16s(pPCMPtr, 2, work_buf, 10*G726_FRAME_SIZE);
       if(ippsEncode_G726_16s8u(EncObj,work_buf,(unsigned char*)outBuffer,10*G726_FRAME_SIZE) != ippStsNoErr){
         return USC_NoOperation;
       }
       PackCodeword_tst((const char*)outBuffer, pBitstrPtr, in->bitrate, 10*G726_FRAME_SIZE, &foo);

       outLen += foo;
       pBitstrPtr += foo;
       inputLen -= (10*G726_FRAME_SIZE*sizeof(short));
       pPCMPtr += (10*G726_FRAME_SIZE);
    }
    while(inputLen > 0) {
       ippsRShiftC_16s(pPCMPtr, 2, work_buf, G726_FRAME_SIZE);
       if(ippsEncode_G726_16s8u(EncObj,work_buf,(unsigned char*)outBuffer,G726_FRAME_SIZE) != ippStsNoErr){
         return USC_NoOperation;
       }
       PackCodeword_tst((const char*)outBuffer, pBitstrPtr, in->bitrate, G726_FRAME_SIZE, &foo);
       outLen += foo;
       pBitstrPtr += foo;
       inputLen -= (G726_FRAME_SIZE*sizeof(short));
       pPCMPtr += (G726_FRAME_SIZE);
    }
    out->frametype = 0;
    out->bitrate = in->bitrate;
    out->nbytes = outLen;

    return USC_NoError;
}

static int UnpackCodeword(const char* inCode, unsigned char* outCode, int bitrate, int bitstrLen)
{

   int len, i;
   len = OutFrameSize(bitrate);
   if(len==0) return 0;

   switch(bitrate) {
      case 16000: {
         for(i = 0;i < bitstrLen;i+=len) {
            outCode[3] = (char)(((inCode[0] >> 6) & 0x3));
            outCode[2] = (char)(((inCode[0] >> 4) & 0x3));
            outCode[1] = (char)(((inCode[0] >> 2) & 0x3));
            outCode[0] = (char)((inCode[0] & 0x3));
            outCode[7] = (char)(((inCode[1] >> 6) & 0x3));
            outCode[6] = (char)(((inCode[1] >> 4) & 0x3));
            outCode[5] = (char)(((inCode[1] >> 2) & 0x3));
            outCode[4] = (char)((inCode[1] & 0x3));
            inCode += len;
            outCode += 8;
         }
         break;
      }
      case 24000: {
         for(i = 0;i < bitstrLen;i+=len) {
            outCode[0] = (char)((inCode[0] & 0x7));
            outCode[1] = (char)((inCode[0] >> 3) & 0x7);
            outCode[2] = (char)(((inCode[0] >> 6) & 0x3) | ((inCode[1] & 0x1))<<2);
            outCode[3] = (char)((inCode[1] >> 1) & 0x7);
            outCode[4] = (char)((inCode[1] >> 4) & 0x7);
            outCode[5] = (char)(((inCode[1] >> 7) & 0x1) | (inCode[2] & 0x3)<<1);
            outCode[6] = (char)((inCode[2] >> 2) & 0x7);
            outCode[7] = (char)((inCode[2] >> 5) & 0x7);
            inCode += len;
            outCode += 8;
         }
         break;
      }
      case 32000: {
         for(i = 0;i < bitstrLen;i+=len) {
            outCode[1] = (char)(((inCode[0] >> 4) & 0xf));
            outCode[0] = (char)((inCode[0] & 0xf));
            outCode[3] = (char)(((inCode[1] >> 4) & 0xf));
            outCode[2] = (char)((inCode[1] & 0xf));
            outCode[5] = (char)(((inCode[2] >> 4) & 0xf));
            outCode[4] = (char)((inCode[2] & 0xf));
            outCode[7] = (char)(((inCode[3] >> 4) & 0xf));
            outCode[6] = (char)((inCode[3] & 0xf));
            inCode += len;
            outCode += 8;
         }
         break;
      }
      case 40000: {
         for(i = 0;i < bitstrLen;i+=len) {
            outCode[0] = (char)((inCode[0] & 0x1f));
            outCode[1] = (char)(((inCode[0] >> 5) & 0x7) | ((inCode[1] & 0x3)<<3));
            outCode[2] = (char)((inCode[1] >> 2) & 0x1f);
            outCode[3] = (char)(((inCode[1] >> 7) & 0x1) | ((inCode[2] & 0xf)<<1));
            outCode[4] = (char)(((inCode[2] >> 4) & 0xf) | ((inCode[3] & 0x1)<<4));
            outCode[5] = (char)((inCode[3] >> 1) & 0x1f);
            outCode[6] = (char)(((inCode[3] >> 6) & 0x3) | ((inCode[4] & 0x7)<<2));
            outCode[7] = (char)((inCode[4]  >>3) & 0x1f);
            inCode += len;
            outCode += 8;
         }
         break;
      }
   }

   return 1;
}

static USC_Status Decode(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out)
{
   G726_Handle_Header *g726_header;
   int bitrate_idx, inputLen, frmLen;
   IPP_ALIGNED_ARRAY(16, unsigned char, inBuffer, 10*G726_FRAME_SIZE);
   IppsDecoderState_G726_16s *DecObj;
   char *pBitstrPtr;
   short *pPCMPtr;

   if(out==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;

   g726_header = (G726_Handle_Header*)handle;
   DecObj = (IppsDecoderState_G726_16s *)((char*)handle + sizeof(G726_Handle_Header));

   if(in == NULL) {
      int i;
     /* Lost frame */
      pPCMPtr = (short *)out->pBuffer;
      for(i=0; i <  g726_header->frameSize; i += G726_FRAME_SIZE*sizeof(short)) {
         if(ippsDecode_G726_8u16s(DecObj, (const unsigned char*)LostFrame, pPCMPtr, G726_FRAME_SIZE)!=ippStsNoErr){
            return USC_NoOperation;
         }
         pPCMPtr += G726_FRAME_SIZE;
      }
     out->bitrate = g726_header->bitrate;
     out->nbytes  = g726_header->frameSize;
   } else {

     if(g726_header->bitrate != in->bitrate) {
       bitrate_idx = CheckRate_G726(in->bitrate);
       if(bitrate_idx < 0) return USC_UnsupportedBitRate;
       ippsDecodeInit_G726_8u16s(DecObj, usc2g726Rate[bitrate_idx], Usc2IppLaw(g726_header->law));
       g726_header->bitrate = in->bitrate;
     }

     frmLen = OutFrameSize(in->bitrate);

     inputLen = in->nbytes;
     in->nbytes = (inputLen / frmLen) * frmLen;

     if((int)((in->nbytes/frmLen)*(G726_FRAME_SIZE*sizeof(short))) > g726_header->frameSize)
         in->nbytes = (g726_header->frameSize / (G726_FRAME_SIZE*sizeof(short)))*frmLen;

     inputLen = (in->nbytes / frmLen) * G726_FRAME_SIZE;/*Convert to the unpucked bitstream len*/

     pBitstrPtr = in->pBuffer;
     pPCMPtr = (short *)out->pBuffer;
     out->nbytes = 0;

     while(inputLen >= (10*G726_FRAME_SIZE)) {/*10 ms loop*/
        UnpackCodeword((const char*)pBitstrPtr, inBuffer, in->bitrate, 10*frmLen);

        if(ippsDecode_G726_8u16s(DecObj, (const unsigned char*)inBuffer, pPCMPtr, 10*G726_FRAME_SIZE)!=ippStsNoErr){
            return USC_NoOperation;
        }
        pBitstrPtr += (10*frmLen);
        pPCMPtr += (10*G726_FRAME_SIZE);
        out->nbytes += (10*G726_FRAME_SIZE*sizeof(short));
        inputLen -= (10*G726_FRAME_SIZE);
     }

     while(inputLen >= G726_FRAME_SIZE) {/*1 ms loop*/
        UnpackCodeword((const char*)pBitstrPtr, inBuffer, in->bitrate, frmLen);

        if(ippsDecode_G726_8u16s(DecObj, (const unsigned char*)inBuffer, pPCMPtr, G726_FRAME_SIZE)!=ippStsNoErr){
            return USC_NoOperation;
        }
        pBitstrPtr += frmLen;
        pPCMPtr += G726_FRAME_SIZE;
        out->nbytes += (G726_FRAME_SIZE*sizeof(short));
        inputLen -= (G726_FRAME_SIZE);
     }

     out->bitrate = in->bitrate;
   }

   return USC_NoError;
}

static USC_Status CalsOutStreamSize(const USC_Option *options, int bitrate_bps, int nbytesSrc, int *nbytesDst)
{
   int nBlocks, nSamples;

   if(options->modes.vad>0) return USC_UnsupportedVADType;

   if(options->direction==0) { /*Encode: src - PCMStream, dst - bitstream*/
      nSamples = nbytesSrc / (G726_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / G726_FRAME_SIZE;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % G726_FRAME_SIZE) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }

      *nbytesDst = nBlocks * OutFrameSize(bitrate_bps);
   } else if(options->direction==1) {/*Decode: src - bitstream, dst - PCMStream*/
      nBlocks = nbytesSrc / OutFrameSize(bitrate_bps);

      if (0 == nBlocks) return USC_NoOperation;

      nSamples = nBlocks * G726_FRAME_SIZE;
      *nbytesDst = nSamples * (G726_BITS_PER_SAMPLE >> 3);
   } else if(options->direction==2) {/* Both: src - PCMStream, dst - PCMStream*/
      nSamples = nbytesSrc / (G726_BITS_PER_SAMPLE >> 3);
      nBlocks = nSamples / G726_FRAME_SIZE;

      if (0 == nBlocks) return USC_NoOperation;

      if (0 != nSamples % G726_FRAME_SIZE) {
         /* Add another block to hold the last compressed fragment*/
         nBlocks++;
      }
      *nbytesDst = nBlocks * G726_FRAME_SIZE * (G726_BITS_PER_SAMPLE >> 3);
   } else return USC_NoOperation;

   return USC_NoError;
}
static USC_Status GetOutStreamSize(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst)
{
   int bitrate_idx;

   if(options==NULL) return USC_BadDataPointer;
   if(nbytesDst==NULL) return USC_BadDataPointer;
   if(nbytesSrc <= 0) return USC_NoOperation;

   bitrate_idx = CheckRate_G726(bitrate);
    if(bitrate_idx < 0) return USC_UnsupportedBitRate;

   return CalsOutStreamSize(options, bitrate, nbytesSrc, nbytesDst);
}

USC_Status SetFrameSize(const USC_Option *options, USC_Handle handle, int frameSize)
{
   G726_Handle_Header *g726_header;

   if(options==NULL) return USC_BadDataPointer;
   if(handle==NULL) return USC_InvalidHandler;
   if(frameSize <= G726_FRAME_SIZE*sizeof(short)) return USC_NoOperation;

   g726_header = (G726_Handle_Header*)handle;

   g726_header->frameSize = (frameSize / (G726_FRAME_SIZE*sizeof(short))) * (G726_FRAME_SIZE*sizeof(short));

   return USC_NoError;
}
