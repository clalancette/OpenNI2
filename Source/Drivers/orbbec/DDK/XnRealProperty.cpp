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
#include "XnRealProperty.h"
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnRealProperty::XnRealProperty(XnUInt32 propertyId, const XnChar* strName, XnDouble* pValueHolder, const XnChar* strModule /* = "" */) :
	XnProperty(XN_PROPERTY_TYPE_REAL, pValueHolder, propertyId, strName, strModule)
{
}

XnStatus XnRealProperty::ReadValueFromFile(const XnChar* csINIFile, const XnChar* csSection)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDouble dValue;

	nRetVal = xnOSReadDoubleFromINI(csINIFile, csSection, GetName(), &dValue);
	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = SetValue(dValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnRealProperty::CopyValueImpl(void* pDest, const void* pSource) const
{
	(*(XnDouble*)pDest) = (*(const XnDouble*)pSource);
	return XN_STATUS_OK;
}

XnBool XnRealProperty::IsEqual(const void* pValue1, const void* pValue2) const
{
	return (*(XnDouble*)pValue1) == (*(XnDouble*)pValue2);
}

XnStatus XnRealProperty::CallSetCallback(const void* pValue)
{
	if (m_pSetCallback == NULL)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_PROPERTY_READ_ONLY, XN_MASK_DDK, "Property %s.%s is read only.", GetModule(), GetName());
	}
	return m_pSetCallback(this, *(const XnDouble*)pValue, m_pSetCallbackCookie);
}

XnStatus XnRealProperty::CallGetCallback(void* pValue) const
{
	if (m_pGetCallback == NULL)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_PROPERTY_WRITE_ONLY, XN_MASK_DDK, "Property %s.%s is write only.", GetModule(), GetName());
	}
	return m_pGetCallback(this, (XnDouble*)pValue, m_pGetCallbackCookie);
}

XnBool XnRealProperty::ConvertValueToString(XnChar* csValue, const void* pValue) const
{
	sprintf(csValue, "%f", *(XnDouble*)pValue);
	return TRUE;
}

XnStatus XnRealProperty::AddToPropertySet(XnPropertySet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnDouble dValue;
	nRetVal = GetValue(&dValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddRealProperty(pSet, GetModule(), GetId(), dValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}
