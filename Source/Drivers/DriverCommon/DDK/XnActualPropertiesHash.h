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
#ifndef XNACTUALPROPERTIESHASH_H
#define XNACTUALPROPERTIESHASH_H

#include "XnProperty.h"
#include "XnActualIntProperty.h"
#include "XnActualRealProperty.h"
#include "XnActualStringProperty.h"
#include "XnActualGeneralProperty.h"

/**
* A hash table of actual properties. The user can safely assume that every property in this
* hash is actual.
*/
class XnActualPropertiesHash
{
public:
	XnActualPropertiesHash(const char* strName);
	~XnActualPropertiesHash();

	typedef XnPropertiesHash::Iterator Iterator;
	typedef XnPropertiesHash::ConstIterator ConstIterator;

	XnStatus Add(uint32_t propertyId, const char* strName, uint64_t nValue);
	XnStatus Add(uint32_t propertyId, const char* strName, double dValue);
	XnStatus Add(uint32_t propertyId, const char* strName, const char* strValue);
	XnStatus Add(uint32_t propertyId, const char* strName, const OniGeneralBuffer& gbValue);

	XnStatus Remove(uint32_t propertyId);
	XnStatus Remove(ConstIterator where);
	inline bool IsEmpty() const { return m_Hash.IsEmpty(); }
	XnStatus Clear();

	inline XnStatus Find(uint32_t propertyId, Iterator& iter) { return m_Hash.Find(propertyId, iter); }
	inline XnStatus Find(uint32_t propertyId, ConstIterator& iter) const { return m_Hash.Find(propertyId, iter); }
	inline XnStatus Get(uint32_t propertyId, XnProperty*& pProp) const { return m_Hash.Get(propertyId, pProp); }

	inline Iterator Begin() { return m_Hash.Begin(); }
	inline ConstIterator Begin() const { return m_Hash.Begin(); }
	inline Iterator End() { return m_Hash.End(); }
	inline ConstIterator End() const { return m_Hash.End(); }

	XnStatus CopyFrom(const XnActualPropertiesHash& other);

protected:
	XnPropertiesHash m_Hash;
	char m_strName[XN_DEVICE_MAX_STRING_LENGTH];
};

#endif // XNACTUALPROPERTIESHASH_H
