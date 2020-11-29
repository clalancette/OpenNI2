/*****************************************************************************
*									     *
*  OpenNI 2.x Alpha							     *
*  Copyright (C) 2012 PrimeSense Ltd.					     *
*									     *
*  This file is part of OpenNI. 					     *
*									     *
*  Licensed under the Apache License, Version 2.0 (the "License");	     *
*  you may not use this file except in compliance with the License.	     *
*  You may obtain a copy of the License at				     *
*									     *
*      http://www.apache.org/licenses/LICENSE-2.0			     *
*									     *
*  Unless required by applicable law or agreed to in writing, software	     *
*  distributed under the License is distributed on an "AS IS" BASIS,	     *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and	     *
*  limitations under the License.					     *
*									     *
*****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnBuffer.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnBuffer::Allocate(uint32_t nAllocSize)
{
	Free();
	XN_VALIDATE_ALIGNED_CALLOC(m_pData, unsigned char, nAllocSize, XN_DEFAULT_MEM_ALIGN);
	m_nMaxSize = nAllocSize;
	m_nSize = 0;
	m_bAllocated = true;
	return (XN_STATUS_OK);
}

void XnBuffer::SetExternalBuffer(unsigned char* pBuffer, uint32_t nSize)
{
	Free();
	m_pData = pBuffer;
	m_nMaxSize = nSize;
	m_nSize = 0;
	m_bAllocated = false;
}

XnStatus XnBuffer::Write(const unsigned char* pData, uint32_t nDataSize)
{
	if (GetFreeSpaceInBuffer() < nDataSize)
	{
		return XN_STATUS_INTERNAL_BUFFER_TOO_SMALL;
	}

	UnsafeWrite(pData, nDataSize);

	return (XN_STATUS_OK);
}
