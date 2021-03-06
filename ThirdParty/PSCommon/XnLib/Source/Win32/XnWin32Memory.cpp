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
#include "XnLib.h"

XN_C_API void* xnOSMalloc(const size_t nAllocSize)
{
	// Return a pointer to the requested allocation size
	return (malloc(nAllocSize));
}

XN_C_API void* xnOSMallocAligned(const size_t nAllocSize, const size_t nAlignment)
{
	// Return a pointer to the aligned requested allocation size
	return (_aligned_malloc(nAllocSize, nAlignment));
}

XN_C_API void* xnOSCalloc(const size_t nAllocNum, const size_t nAllocSize)
{
	// Return a pointer to the requested allocation size and write zeros on the entire memory block (C Alloc)
	return (calloc(nAllocNum, nAllocSize));
}

XN_C_API void* xnOSCallocAligned(const size_t nAllocNum, const size_t nAllocSize, const size_t nAlignment)
{
	// Local function variables
	void* pMemBlock = NULL;
	size_t nBlockSize = nAllocNum * nAllocSize;

	// Allocate the aligned memory block
	pMemBlock = xnOSMallocAligned(nBlockSize, nAlignment);

	// If it succeeded (the returned block wasn't NULL, let's zero it)
	if (pMemBlock != NULL)
	{
		xnOSMemSet(pMemBlock, 0, nBlockSize);
	}

	return (pMemBlock);
}

XN_C_API void* xnOSRealloc(void* pMemory, const size_t nAllocSize)
{
	// Return a pointer to the requested allocation size
	return (realloc(pMemory, nAllocSize));
}

XN_C_API void* xnOSReallocAligned(void* pMemory, const size_t nAllocSize, const size_t nAlignment)
{
	// Return a pointer to the aligned requested allocation size
	return (_aligned_realloc(pMemory, nAllocSize, nAlignment));
}

XN_C_API void* xnOSRecalloc(void* pMemory, const size_t nAllocNum, const size_t nAllocSize)
{
	// Return a pointer to the requested allocation size and write zeros on the entire memory block (C Alloc)
	return (_recalloc(pMemory, nAllocNum, nAllocSize));
}

XN_C_API void xnOSFree(const void* pMemBlock)
{
	// Note: No need to check if pMemBlock is NULL because free is guaranteed to handle this case correctly.

	// Free the requested memory block
	free ((void*)pMemBlock);
}

XN_C_API void  xnOSFreeAligned(const void* pMemBlock)
{
	// Note: No need to check if pMemBlock is NULL because free is guaranteed to handle this case correctly.

	// Free the requested aligned memory block
	_aligned_free((void*)pMemBlock);
}

XN_C_API void xnOSMemCopy(void* pDest, const void* pSource, size_t nCount)
{
	memcpy(pDest, pSource, nCount);
}

XN_C_API int32_t xnOSMemCmp(const void *pBuf1, const void *pBuf2, size_t nCount)
{
	return memcmp(pBuf1, pBuf2, nCount);
}

XN_C_API void xnOSMemSet(void* pDest, uint8_t nValue, size_t nCount)
{
	memset(pDest, nValue, nCount);
}

XN_C_API void xnOSMemMove(void* pDest, const void* pSource, size_t nCount)
{
	memmove(pDest, pSource, nCount);
}

XN_C_API uint64_t  xnOSEndianSwapUINT64(uint64_t nValue)
{
	return ((nValue >> 56) ) | ((nValue >> 40) & 0x000000000000ff00ULL) |
		((nValue >> 24) & 0x0000000000ff0000ULL) | ((nValue >> 8 ) & 0x00000000ff000000ULL) |
		((nValue << 8 ) & 0x000000ff00000000ULL) | ((nValue << 24) & 0x0000ff0000000000ULL) |
		((nValue << 40) & 0x00ff000000000000ULL) | ((nValue << 56) );
}

XN_C_API uint32_t  xnOSEndianSwapUINT32(uint32_t nValue)
{
	return  (nValue>>24) |
		((nValue<<8) & 0x00FF0000) |
		((nValue>>8) & 0x0000FF00) |
		(nValue<<24);
}

XN_C_API uint16_t xnOSEndianSwapUINT16(uint16_t nValue)
{
	return ((nValue>>8) | (nValue<<8));
}
XN_C_API float xnOSEndianSwapFLOAT(float fValue)
{
	uint32_t* pnValue = (uint32_t*)&fValue;
	uint32_t nValue = xnOSEndianSwapUINT32(*pnValue);
	float* pfValue = (float*)&nValue;
	return *pfValue;
}
