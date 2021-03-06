/*****************************************************************************
*                                                                            *
*  PrimeSense PSCommon Library                                               *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PSCommon.                                            *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
#ifndef _XN_MEMORY_H_
#define _XN_MEMORY_H_

#include "XnLib.h"

XN_C_API void* XN_C_DECL xnOSMalloc(const size_t nAllocSize);
XN_C_API void* XN_C_DECL xnOSMallocAligned(const size_t nAllocSize, const size_t nAlignment);
XN_C_API void* XN_C_DECL xnOSCalloc(const size_t nAllocNum, const size_t nAllocSize);
XN_C_API void* XN_C_DECL xnOSCallocAligned(const size_t nAllocNum, const size_t nAllocSize, const size_t nAlignment);
XN_C_API void* XN_C_DECL xnOSRealloc(void* pMemory, const size_t nAllocSize);
XN_C_API void* XN_C_DECL xnOSReallocAligned(void* pMemory, const size_t nAllocSize, const size_t nAlignment);
XN_C_API void* XN_C_DECL xnOSRecalloc(void* pMemory, const size_t nAllocNum, const size_t nAllocSize);
XN_C_API void XN_C_DECL xnOSFree(const void* pMemBlock);
XN_C_API void XN_C_DECL xnOSFreeAligned(const void* pMemBlock);
XN_C_API void XN_C_DECL xnOSMemCopy(void* pDest, const void* pSource, size_t nCount);
XN_C_API int32_t XN_C_DECL xnOSMemCmp(const void *pBuf1, const void *pBuf2, size_t nCount);
XN_C_API void XN_C_DECL xnOSMemSet(void* pDest, uint8_t nValue, size_t nCount);
XN_C_API void XN_C_DECL xnOSMemMove(void* pDest, const void* pSource, size_t nCount);
XN_C_API uint64_t XN_C_DECL xnOSEndianSwapUINT64(uint64_t nValue);
XN_C_API uint32_t XN_C_DECL xnOSEndianSwapUINT32(uint32_t nValue);
XN_C_API uint16_t XN_C_DECL xnOSEndianSwapUINT16(uint16_t nValue);
XN_C_API float XN_C_DECL xnOSEndianSwapFLOAT(float fValue);

#endif // _XN_MEMORY_H_
