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
#ifndef _XN_STRINGS_HASH_H_
#define _XN_STRINGS_HASH_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnHash.h"

namespace xnl
{

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
class XnStringsHashKeyManager
{
public:
	static xnl::HashCode Hash(const char* const& key)
	{
		uint32_t nCRC = 0;
		xnOSStrCRC32(key, &nCRC);

		// convert from UINT32 to XnHashValue
		return nCRC % (1 << (sizeof(xnl::HashCode)*8));
	}

	static int32_t Compare(const char* const& key1, const char* const& key2)
	{
		return strcmp(key1, key2);
	}
};

template<class TValue>
class XnStringsHashT : public xnl::Hash<const char*, TValue, XnStringsHashKeyManager>
{
	typedef xnl::Hash<const char*, TValue, XnStringsHashKeyManager> Base;

public:
	XnStringsHashT() : Base() {}

	XnStringsHashT(const XnStringsHashT& other) : Base()
	{
		*this = other;
	}

	XnStringsHashT& operator=(const XnStringsHashT& other)
	{
		Base::operator=(other);
		// no other members
		return *this;
	}
};

}  // namespace xnl

#endif // _XN_STRINGS_HASH_H_
