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
#include "XnGeneralProperty.h"
#include <XnLog.h>
#include <XnCore.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnGeneralProperty::XnGeneralProperty(XnUInt32 propertyId, const XnChar* strName, OniGeneralBuffer* pValueHolder /* = NULL */, ReadValueFromFileFuncPtr pReadFromFileFunc /* = NULL */, const XnChar* strModule /* = "" */ ) :
	XnProperty(XN_PROPERTY_TYPE_GENERAL, pValueHolder, propertyId, strName, strModule),
	m_pReadFromFileFunc(pReadFromFileFunc)
{
}

XnStatus XnGeneralProperty::CopyValueImpl(void* pDest, const void* pSource) const
{
	return XnGeneralBufferCopy((OniGeneralBuffer*)pDest, (const OniGeneralBuffer*)pSource);
}

XnBool XnGeneralProperty::IsEqual(const void* pValue1, const void* pValue2) const
{
	const OniGeneralBuffer* pgb1 = (const OniGeneralBuffer*)pValue1;
	const OniGeneralBuffer* pgb2 = (const OniGeneralBuffer*)pValue2;

	if (pgb1->dataSize != pgb2->dataSize)
		return FALSE;

	return (memcmp(pgb1->data, pgb2->data, pgb1->dataSize) == 0);
}

XnStatus XnGeneralProperty::CallSetCallback(const void* pValue)
{
	if (m_pSetCallback == NULL)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_PROPERTY_READ_ONLY, XN_MASK_DDK, "Property %s.%s is read only.", GetModule(), GetName());
	}
	return m_pSetCallback(this, *(const OniGeneralBuffer*)pValue, m_pSetCallbackCookie);
}

XnStatus XnGeneralProperty::CallGetCallback(void* pValue) const
{
	if (m_pGetCallback == NULL)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_PROPERTY_WRITE_ONLY, XN_MASK_DDK, "Property %s.%s is write only.", GetModule(), GetName());
	}
	return m_pGetCallback(this, *(const OniGeneralBuffer*)pValue, m_pGetCallbackCookie);
}

XnStatus XnGeneralProperty::ReadValueFromFile(const XnChar* csINIFile, const XnChar* csSection)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_pReadFromFileFunc != NULL)
	{
		nRetVal = m_pReadFromFileFunc(this, csINIFile, csSection);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnGeneralProperty::AddToPropertySet(XnPropertySet* /*pSet*/)
{
	return (XN_STATUS_ERROR);
}
