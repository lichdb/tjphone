/*****************************************************************************
//
// INTEL CORPORATION PROPRIETARY INFORMATION
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Intel Corporation and may not be copied
// or disclosed except in accordance with the terms of that agreement.
// Copyright (c) 2005-2006 Intel Corporation. All Rights Reserved.
//
// Intel(R) Integrated Performance Primitives
//
//     USC - Unified Speech Codec interface library
//
//  Purpose: Unified Speech Codec Interface header file.
//               Structures and functions declarations.
*****************************************************************************/
#ifndef __USC_H__
#define __USC_H__
#include "usc_base.h"

/* USC codec modes, (may be alternated) */
typedef struct {
   int bitrate;    /* in bps */
   int truncate;   /* 0 - no truncate */
   int vad;        /* 0 - disabled, otherwize vad type (1,2, more if any) */
   int hpf;        /* high pass filter: 1- on, 0- off */
   int pf;         /* post filter: 1- on, 0- off */
}USC_Modes;

typedef struct {
  int bitrate;         /* in bps */
} USC_Rates;

/* USC codec option */
typedef struct {
   int       direction;  /* 0 - encode only, 1 - decode only, 2 - both */
   int       law;        /* 0 - pcm, 1 - aLaw, 2 -muLaw */
   int       framesize;  /* a codec frame size */
   int       nModes;
   USC_Modes modes;
}USC_Option;


/* USC codec information */
typedef struct {
   const char        *name;      /* codec name */
   int               maxbitsize; /* bitstream max frame size in bytes */
   USC_PCMType       pcmType;    /* codec audio source */
   int               nRates;
   const USC_Rates   *pRateTbl;  /* supported bitrates lookup table */
   USC_Option        params;     /* what is supported */
}USC_CodecInfo;

/* USC compressed bits */
typedef struct {
   char        *pBuffer;
   int          nbytes;     /* bitstream size in byte */
   int          frametype;  /* codec specific frame type (transmission frame type) */
   int          bitrate;    /* in bps */
}USC_Bitstream;

/* USC functions table.
   Each codec should supply a function table structure, which is derived from this base table.
   Codec table is to be specified for each codec by name as follows:
       USC_<codec-name>_Fxns, for example USC_g729_Fxns.
   The typical usage model:
    - Questing a codec about memory requirement using  MemAlloc() function
      which returns a memory banks description table with required bank sizes.
    - Use Init() function to create codec instance according to modes (vad, rate etc) requested.
      Codec handle is returned. Thus different instances of particular codec may be created
      and used in parallel.
    - Encode(), Decode() - compression/decompression functions.
    - GetInfo() - inquire codec state or codec requirement.
*/

typedef struct {

    USC_baseFxns std;


   /*   Encode()
        in - input audio stream (pcm) pointer,
        out - output bitstream pointer ,
    */
    USC_Status (*Encode)(USC_Handle handle, USC_PCMStream *in, USC_Bitstream *out);

    /*  Decode()
        in -  input bitstream pointer,
        out - output audio stream (pcm) pointer,
    */
    USC_Status (*Decode)(USC_Handle handle, USC_Bitstream *in, USC_PCMStream *out);

   /*   GetOutStreamSize()
        bitrate -  input bitrate in kbit/s,
        nBytesSrc - lenght in bytes of the input stream,
         nBytesDst - output lenght in bytes of the output stream,
    */
   USC_Status (*GetOutStreamSize)(const USC_Option *options, int bitrate, int nbytesSrc, int *nbytesDst);
   /*   SetFrameSize()
        frameSize -  Desired frame size in bytes,
    */
   USC_Status (*SetFrameSize)(const USC_Option *options, USC_Handle handle, int frameSize);

} USC_Fxns;

#endif /* __USC_H__ */
