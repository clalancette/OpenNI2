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
#include "XnStringProperty.h"
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStringProperty::XnStringProperty(uint32_t propertyId, const XnChar* strName, XnChar* pValueHolder, const XnChar* strModule /* = "" */) :
	XnProperty(XN_PROPERTY_TYPE_STRING, pValueHolder, propertyId, strName, strModule)
{
}

XnStatus XnStringProperty::ReadValueFromFile(const XnChar* csINIFile, const XnChar* csSection)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar csValue[XN_DEVICE_MAX_STRING_LENGTH];
	nRetVal = xnOSReadStringFromINI(csINIFile, csSection, GetName(), csValue, XN_DEVICE_MAX_STRING_LENGTH);
	if (nRetVal == XN_STATUS_OK)
	{
		nRetVal = SetValue(csValue);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnStringProperty::CopyValueImpl(void* pDest, const void* pSource) const
{
	strncpy((char*)pDest, (const char*)pSource, XN_DEVICE_MAX_STRING_LENGTH);
	return XN_STATUS_OK;
}

XnBool XnStringProperty::IsEqual(const void* pValue1, const void* pValue2) const
{
	return (strncmp((const XnChar*)pValue1, (const XnChar*)pValue2, XN_DEVICE_MAX_STRING_LENGTH) == 0);
}

XnStatus XnStringProperty::CallSetCallback(const void* pValue)
{
	if (m_pSetCallback == NULL)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_PROPERTY_READ_ONLY, XN_MASK_DDK, "Property %s.%s is read only.", GetModule(), GetName());
	}
	return m_pSetCallback(this, (const XnChar*)pValue, m_pSetCallbackCookie);
}

XnStatus XnStringProperty::CallGetCallback(void* pValue) const
{
	if (m_pGetCallback == NULL)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_PROPERTY_WRITE_ONLY, XN_MASK_DDK, "Property %s.%s is write only.", GetModule(), GetName());
	}
	return m_pGetCallback(this, (XnChar*)pValue, m_pGetCallbackCookie);
}

XnBool XnStringProperty::ConvertValueToString(XnChar* csValue, const void* pValue) const
{
	strncpy(csValue, (const XnChar*)pValue, XN_DEVICE_MAX_STRING_LENGTH);
	return TRUE;
}

XnStatus XnStringProperty::AddToPropertySet(XnPropertySet* pSet)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar strValue[XN_DEVICE_MAX_STRING_LENGTH];
	nRetVal = GetValue(strValue);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnPropertySetAddStringProperty(pSet, GetModule(), GetId(), strValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}
