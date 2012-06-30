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
//     USC base header
//
*****************************************************************************/
#ifndef __USC_BASE_H__
#define __USC_BASE_H__

#if defined( USC_W32DLL )
  #if defined( _MSC_VER ) || defined( __ICL ) || defined ( __ECL )
    #define USCFUN  __declspec(dllexport)
  #else
    #define USCFUN  extern
  #endif
#else
  #define USCFUN  extern
#endif

/* USC error code */
typedef enum {
   /* errors: negative response */
   USC_UnsupportedEchoTail  = -9,
   USC_NotInitialized       = -8,
   USC_InvalidHandler       = -7,
   USC_NoOperation          = -6,
   USC_UnsupportedPCMType   = -5,
   USC_UnsupportedBitRate   = -4,
   USC_UnsupportedFrameType = -3,
   USC_UnsupportedVADType   = -2,
   USC_BadDataPointer       = -1,
   USC_NoError              =  0,
   /* warnings: positive response */
   USC_StateNotChanged      =  1
}USC_Status;

/* USC algorithm type */
typedef enum {
   USC_Codec = 0,
   USC_EC = 1,
   USC_MAX_ALG
}USC_AlgType;

/* USC PCM stream type */
typedef struct {
   int  sample_frequency; /* sample rate in Hz */
   int  bitPerSample;     /* bit per sample */
}USC_PCMType;

/* USC memory types */
typedef enum {
   USC_OBJECT = 0, /* persistent memory */
   USC_BUFFER = 1, /* scratch memory */
   USC_CONST = 2, /* memory for tables, constants  */
   USC_MAX_MEM_TYPES, /* Number of memory types */
} USC_MemType;


/* USC memory space types */
typedef enum {
   USC_NORMAL = 0,   /* normal memory space */
   USC_MEM_CHIP = 1, /* high speed memory*/
   USC_MAX_SPACE ,   /* Number of memory space types */
}USC_MemSpaceType;

/* USC memory banks */
typedef struct {
   char *pMem;
   int   nbytes;
   int   align;
   USC_MemType memType;
   USC_MemSpaceType memSpaceType;
}USC_MemBank;

/* USC PCM stream */
typedef struct {
   char        *pBuffer;
   int          nbytes;     /* pcm data size in byte */
   USC_PCMType  pcmType;
   int          bitrate;    /* in bps */
}USC_PCMStream;

typedef void* USC_Handle;

/* USC base functions table.
    - Questing an USC algorithm about memory requirement using  MemAlloc() function
      which returns a memory banks description table with required bank sizes.
    - Use Init() function to create an algorithm instance according to an options requested.
      An algorithm handle is returned. Thus different instances of particular algorithm may be created
      and used in parallel.
*/

typedef struct {
   USC_AlgType algType;

   /*  GetInfoSize() - Returns a size of memory buffer an application
   to allocate which can be used as the GetInfo function pInfo parameter
    */
   USC_Status (*GetInfoSize)(int *pSize);

   /*  Get_Info() - quest an algorithm specific information
        General inquiry is possible without initialization when handle==NULL.
        pInfo - pointer to the structure to be filled by USC algorithm
    */
    USC_Status (*GetInfo)(USC_Handle handle, void *pInfo);
    /* NumAlloc() - inquiring number of memory buffers
           memOptions - poiter to the algorithm specific memory options structure
           nbanks  - number of table entries (size of pBanks table).
   */
    USC_Status (*NumAlloc)(const void *memOptions, int *nbanks);

   /*   MemAlloc() - inquiring information about memory requirement
                     (buffers to be allocated)
           memOptions - poiter to the algorithm specific memory options structure
           pBanks  - pointer to the input table of size nbanks to be filled with memory requirement
                   (pMem=NULL if to be allocated )
    */
    USC_Status (*MemAlloc)(const void *memOptions, USC_MemBank *pBanks);

    /*  Init() - create an USC algorithm handle and set it to initial state
      initOptions - poiter to an algorithm specific initialization options  structure
      pBanks  - allocated memory banks of number as after MemAlloc
      handle - pointer to the output algorithm instance pointer
   */
   USC_Status (*Init)(const void *initOptions, const USC_MemBank *pBanks, USC_Handle *handle );

    /*  Reinit() - set an algorithm to initial state
      reinitParams - pointer to an algorithm specific initialization options structure
      handle - pointer to the input algorithm instance pointer
   */
   USC_Status (*Reinit)(const void *reinitParams, USC_Handle handle );

   /*   Control() - alternate an algorithm modes
                  The only modes were set on Init() may be alternated.
      controlParams - pointer to the algorithm specific control options structure
      handle - pointer to the input algorithm instance pointer
   */
   USC_Status (*Control)(const void *controlParams, USC_Handle handle );

} USC_baseFxns;

#endif /* __USC_BASE_H__ */
