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
#ifndef _XN_BIT_SET_H_
#define _XN_BIT_SET_H_

#include <vector>

#include "XnStatus.h"

namespace xnl
{

class BitSet
{
public:
	BitSet() : m_nSize(0) {}

	/** Reserves space in this bitset for the specified number of bits.
	    This saves you re-allocations and data copies if you know the size in advance. **/
	XnStatus Reserve(uint32_t nBits)
	{
		// note: dividing by 8 to get to bytes count
		m_array.reserve((nBits / 8) + 1);
		return XN_STATUS_OK;
	}

	/** Sets the size of the bitset to the specified number of bits and sets them all to 0. **/
	XnStatus SetSize(uint32_t nBits)
	{
		// note: dividing by 8 to get to bytes count
		m_array.resize((nBits / 8) + 1, 0);
		return XN_STATUS_OK;
	}

	/** Sets the bit at nIndex to bValue. **/
	XnStatus Set(uint32_t nIndex, bool bValue)
	{
		uint32_t nArrayIndex = (nIndex / 8);
		uint32_t nBitIndex = (nIndex % 8);
		uint8_t nMask = (1 << nBitIndex);
		uint8_t nOldVal = nArrayIndex < m_array.size() ? m_array[nArrayIndex] : 0;
		uint8_t nNewVal = bValue ? (nOldVal | nMask) : (nOldVal & (~nMask));
		m_array.resize(nArrayIndex + 1);
		m_array[nArrayIndex] = nNewVal;
		m_nSize = XN_MAX(m_nSize, nIndex + 1);
		return XN_STATUS_OK;
	}

	/** @returns the value of the bit specified by nIndex. **/
	bool IsSet(uint32_t nIndex) const
	{
		uint32_t nArrayIndex = (nIndex / 8);
		uint32_t nBitIndex = (nIndex % 8);
		if (nArrayIndex >= m_array.size())
		{
			return false;
		}
		return (m_array[nArrayIndex] & (1 << nBitIndex)) ? true : false;
	}

	/** Copies raw data from a buffer of bytes to this bitset. **/
	XnStatus SetData(const uint8_t* pData, uint32_t nSizeInBytes)
	{
		m_array.resize(nSizeInBytes);
		memcpy(&m_array[0], pData, nSizeInBytes);

		m_nSize = (nSizeInBytes * 8);

		return XN_STATUS_OK;
	}

	/** @returns The raw data of this bitset as a buffer of bytes. **/
	const uint8_t* GetData() const
	{
		return m_array.data();
	}

	/** @returns The raw data of this bitset as a buffer of bytes. Allows modification of underlying data. **/
	uint8_t* GetData()
	{
		return m_array.data();
	}

	/** @returns size in bytes of this bitset. **/
	uint32_t GetDataSizeInBytes() const
	{
		return m_array.size();
	}

	/** @returns size in bits of this bitset. **/
	uint32_t GetSize() const
	{
		return m_nSize;
	}

	/** Clears data in this bitset and sets size to 0. **/
	void Clear()
	{
		m_array.clear();
		m_nSize = 0;
	}

	/** @returns true if this bitset is empty, false otherwise. **/
	bool IsEmpty() const
	{
		return m_array.empty();
	}

private:
	std::vector<uint8_t> m_array;
	uint32_t m_nSize;
};

}
#endif // _XN_BIT_SET_H_
