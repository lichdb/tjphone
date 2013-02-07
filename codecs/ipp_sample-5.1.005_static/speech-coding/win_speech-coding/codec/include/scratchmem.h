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
//  Purpose: scratch/stack memory managment header file.
***************************************************************************/

#ifndef __SCRATCHMEM_H__
#define __SCRATCHMEM_H__

#if defined(__ICC) || defined( __ICL ) || defined ( __ECL )
  #define __INLINE static __inline
#elif defined( __GNUC__ )
  #define __INLINE static __inline__
#else
  #define __INLINE static
#endif

#if defined(__ICL ) || defined ( __ECL )
/*  Intel C/C++ compiler bug for __declspec(align(8)) !!! */
  #define __ALIGN(n) __declspec(align(16))
  #define __ALIGN32 __declspec(align(32))
#else
  #define __ALIGN(n)
  #define __ALIGN32
#endif


#if (defined (_WIN64) || defined(linux64) || defined(linux32e)) && !defined(_WIN32_WCE)
__INLINE
Ipp64s IPP_INT_PTR( const void* ptr )  {
    union {
        void*   Ptr;
        Ipp64s  Int;
    } dd;
    dd.Ptr = (void*)ptr;
    return dd.Int;
}
__INLINE
Ipp64u IPP_UINT_PTR( const void* ptr )  {
    union {
        void*    Ptr;
        Ipp64u   Int;
    } dd;
    dd.Ptr = (void*)ptr;
    return dd.Int;
}
#elif (defined(_WIN32) || defined(linux32)) && !defined(_WIN32_WCE)
__INLINE
Ipp32s IPP_INT_PTR( const void* ptr )  {
    union {
        void*   Ptr;
        Ipp32s  Int;
    } dd;
    dd.Ptr = (void*)ptr;
    return dd.Int;
}

__INLINE
Ipp32u IPP_UINT_PTR( const void* ptr )  {
    union {
        void*   Ptr;
        Ipp32u  Int;
    } dd;
    dd.Ptr = (void*)ptr;
    return dd.Int;
}
#else
  #define IPP_INT_PTR( ptr )  ( (long)(ptr) )
  #define IPP_UINT_PTR( ptr ) ( (unsigned long)(ptr) )
#endif

#define IPP_BYTES_TO_ALIGN(ptr, align) ((-(IPP_INT_PTR(ptr)&((align)-1)))&((align)-1))

#define IPP_ALIGNED_PTR(ptr, align) (void*)( (char*)(ptr) + (IPP_BYTES_TO_ALIGN( ptr, align )) )

#define IPP_MALLOC_ALIGNED_BYTES  32

#define IPP_MALLOC_ALIGNED_0BYTES   0
#define IPP_MALLOC_ALIGNED_1BYTES   1
#define IPP_MALLOC_ALIGNED_8BYTES   8
#define IPP_MALLOC_ALIGNED_16BYTES 16
#define IPP_MALLOC_ALIGNED_32BYTES 32

#define IPP_ALIGNED_ARRAY(align,arrtype,arrname,arrlength)\
 arrtype arrname##AlignedArrBuff[(arrlength)+IPP_MALLOC_ALIGNED_##align##BYTES/sizeof(arrtype)];\
 arrtype *arrname = (arrtype*)IPP_ALIGNED_PTR(arrname##AlignedArrBuff,align)

#if !defined (NO_SCRATCH_MEMORY_USED)

__INLINE void* GetMemory(int arrlen, int sizeOfElem, char **CurPtr)
{
   void *ret;

   ret = (void*)IPP_ALIGNED_PTR(*CurPtr,sizeOfElem);
   *CurPtr += (arrlen+1)*sizeOfElem;

   return ret;
}

__INLINE void* GetAlignMemory(int align, int arrlen, int sizeOfElem, char **CurPtr)
{
   void *ret;

   ret = (void*)IPP_ALIGNED_PTR(*CurPtr,align);
   *CurPtr += (arrlen+align/sizeOfElem)*sizeOfElem;

   return ret;
}


typedef struct _ScratchMem_Obj {
    char *base;
    char *CurPtr;
    int  *VecPtr;
    int   offset;
}ScratchMem_Obj;

   #define LOCAL_ALIGN_ARRAY(align,arrtype,arrname,arrlength,obj)\
      arrtype *arrname = (arrtype *)GetAlignMemory(align,arrlength,sizeof(arrtype),&(obj)->Mem.CurPtr)

   #define LOCAL_ARRAY(arrtype,arrname,arrlength,obj)\
      arrtype *arrname = (arrtype *)GetMemory(arrlength,sizeof(arrtype),&(obj)->Mem.CurPtr)

   #define LOCAL_ARRAY_FREE(arrtype,arrname,arrlength,obj)\
      arrname=NULL;\
      (obj)->Mem.CurPtr -= ((arrlength)+1)*sizeof(arrtype)

   #define LOCAL_ALIGN_ARRAY_FREE(align,arrtype,arrname,arrlength,obj)\
      arrname=NULL;\
      (obj)->Mem.CurPtr -= ((arrlength)+IPP_MALLOC_ALIGNED_##align##BYTES/sizeof(arrtype))*sizeof(arrtype)

   #define CLEAR_SCRATCH_MEMORY(obj)\
      (obj)->Mem.CurPtr = (obj)->Mem.base

   #define OPEN_SCRATCH_BLOCK(obj)\
      (obj)->Mem.VecPtr[(obj)->Mem.offset] = IPP_INT_PTR((obj)->Mem.CurPtr);\
      (obj)->Mem.offset++

   #define CLOSE_SCRATCH_BLOCK(obj)\
      (obj)->Mem.offset--;\
      (obj)->Mem.CurPtr = (char *)(obj)->Mem.VecPtr[(obj)->Mem.offset]
#else
   #define LOCAL_ALIGN_ARRAY(align,arrtype,arrname,arrlength,obj)\
      IPP_ALIGNED_ARRAY(align,arrtype,arrname,arrlength)

   #define LOCAL_ARRAY(arrtype,arrname,arrlength,obj)\
      arrtype arrname[arrlength]

   #define LOCAL_ARRAY_FREE(arrtype,arrname,arrlength,obj)

   #define LOCAL_ALIGN_ARRAY_FREE(align,arrtype,arrname,arrlength,obj)

   #define CLEAR_SCRATCH_MEMORY(obj)

   #define OPEN_SCRATCH_BLOCK(obj)

   #define CLOSE_SCRATCH_BLOCK(obj)
#endif


#ifdef CONST
   #undef CONST
#endif

/*#if (_IPP_ARCH == _IPP_ARCH_XSC)
   #define CONST
#else
 #define CONST const
#endif*/
#define CONST

#endif /*__SCRATCHMEM_H__*/
