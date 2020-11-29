/*****************************************************************************
*                                                                            *
*  OpenNI 2.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnIntProperty.h"
#include <XnLog.h>
#include <XnOS.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnIntProperty::XnIntProperty(uint32_t propertyId, const XnChar* strName, uint64_t* pValueHolder /* = NULL */, const XnChar* strModule /* = "" */ ) :
	XnProperty(XN_PROPERTY_TYPE_INTEGER, pValueHolder, propertyId, strName, strModule)
{
}

XnStatus XnIntProperty::ReadValueFromFile(const XnChar* csINIFile, const XnChar* csSection)
{
	XnStatus nRetVal = XN_STATUS_OK;

	int32_t nValue;

	nRetVal = xnOSReadIntFromINI(csINIFile, csSection, GetName(), &nValue);
	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = SetValue(nValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnIntProperty::CopyValueImpl(void* pDest, const void* pSource) const
{
	(*(uint64_t*)pDest) = (*(const uint64_t*)pSource);
	return XN_STATUS_OK;
}

bool XnIntProperty::IsEqual(const void* pValue1, const void* pValue2) const
{
	return (*(uint64_t*)pValue1) == (*(uint64_t*)pValue2);
}

XnStatus XnIntProperty::CallSetCallback(const void* pValue)
{
	if (m_pSetCallback == NULL)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_PROPERTY_READ_ONLY, XN_MASK_DDK, "Property %s.%s is read only.", GetModule(), GetName());
	}
	return m_pSetCallback(this, *(const uint64_t*)pValue, m_pSetCallbackCookie);
}

XnStatus XnIntProperty::CallGetCallback(void* pValue) const
{
	if (m_pGetCallback == NULL)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_PROPERTY_WRITE_ONLY, XN_MASK_DDK, "Property %s.%s is write only.", GetModule(), GetName());
	}
	return m_pGetCallback(this, (uint64_t*)pValue, m_pGetCallbackCookie);
}

bool XnIntProperty::ConvertValueToString(XnChar* csValue, const void* pValue) const
{
	sprintf(csValue, "%" PRIu64, *(uint64_t*)pValue);
	return true;
}

XnStatus XnIntProperty::AddToPropertySet(XnPropertySet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	uint64_t nValue;
	nRetVal = GetValue(&nValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddIntProperty(pSet, GetModule(), GetId(), nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}
