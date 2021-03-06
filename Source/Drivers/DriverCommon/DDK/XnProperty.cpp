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
#include <list>

#include "XnProperty.h"
#include <XnDDKStatus.h>
#include <XnOS.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnProperty::XnProperty(XnPropertyType Type, void* pValueHolder, uint32_t propertyId, const char* strName, const char* strModule) :
	m_propertyId(propertyId),
	m_Type(Type),
	m_pValueHolder(pValueHolder),
	m_LogSeverity(XN_LOG_INFO),
	m_bAlwaysSet(false)
{
	UpdateName(strModule, strName);
}

XnProperty::~XnProperty()
{
}

void XnProperty::UpdateName(const char* strModule, const char* strName)
{
	strncpy(m_strModule, strModule, XN_DEVICE_MAX_STRING_LENGTH - 1);
	m_strModule[XN_DEVICE_MAX_STRING_LENGTH - 1] = '\0';
	if (m_strName != strName) {
		strncpy(m_strName, strName, XN_DEVICE_MAX_STRING_LENGTH - 1);
		m_strName[XN_DEVICE_MAX_STRING_LENGTH - 1] = '\0';
	}
}

XnStatus XnProperty::SetValue(const void* pValue)
{
	if (m_LogSeverity != -1)
	{
		char strValue[XN_DEVICE_MAX_STRING_LENGTH];
		if (ConvertValueToString(strValue, pValue))
		{
			xnLogWrite(XN_MASK_DDK, (XnLogSeverity)m_LogSeverity, __FILE__, __LINE__, "Setting %s.%s to %s...", GetModule(), GetName(), strValue);
		}
		else
		{
			xnLogWrite(XN_MASK_DDK, (XnLogSeverity)m_LogSeverity, __FILE__, __LINE__, "Setting %s.%s...", GetModule(), GetName());
		}
	}

	if (!m_bAlwaysSet && IsActual() && IsEqual(m_pValueHolder, pValue))
	{
		xnLogWrite(XN_MASK_DDK, (XnLogSeverity)m_LogSeverity, __FILE__, __LINE__, "%s.%s value did not change.", GetModule(), GetName());
	}
	else
	{
		XnStatus nRetVal = CallSetCallback(pValue);
		if (nRetVal != XN_STATUS_OK)
		{
			if (m_LogSeverity != -1)
			{
				xnLogWrite(XN_MASK_DDK, (XnLogSeverity)m_LogSeverity, __FILE__, __LINE__, "Failed setting %s.%s: %s", GetModule(), GetName(), xnGetStatusString(nRetVal));
			}
			return (nRetVal);
		}
		else
		{
			xnLogWrite(XN_MASK_DDK, (XnLogSeverity)m_LogSeverity, __FILE__, __LINE__, "%s.%s was successfully set.", GetModule(), GetName());
		}
	}

	return (XN_STATUS_OK);
}

XnStatus XnProperty::GetValue(void* pValue) const
{
	return CallGetCallback(pValue);
}

XnStatus XnProperty::UnsafeUpdateValue(const void* pValue /* = NULL */)
{
	XnStatus nRetVal = XN_STATUS_OK;

	bool bValueChanged = true;

	if (IsActual())
	{
		if (IsEqual(m_pValueHolder, pValue))
		{
			bValueChanged = false;
		}
		else
		{
			// update the value
			nRetVal = CopyValueImpl(m_pValueHolder, pValue);
			XN_IS_STATUS_OK(nRetVal);
		}
	}

	if (bValueChanged)
	{
		// print a message
		if (m_LogSeverity != -1)
		{
			char strValue[XN_DEVICE_MAX_STRING_LENGTH];
			bool bValueString = false;

			if (IsActual())
			{
				bValueString = ConvertValueToString(strValue, pValue);
			}

			xnLogWrite(XN_MASK_DDK, (XnLogSeverity)m_LogSeverity, __FILE__, __LINE__, "Property %s.%s was changed%s%s.", GetModule(), GetName(),
				bValueString ? " to " : "", bValueString ? strValue : "");
		}

		// raise the event
		nRetVal = m_OnChangeEvent.Raise(this);
		XN_IS_STATUS_OK(nRetVal);
	}

	return XN_STATUS_OK;
}

bool XnProperty::ConvertValueToString(char* /*csValue*/, const void* /*pValue*/) const
{
	return false;
}

XnStatus XnProperty::ChangeEvent::Raise(const XnProperty* pSender)
{
	XnStatus nRetVal = XN_STATUS_OK;
	xnl::AutoCSLocker locker(m_hLock);
	ApplyListChanges();

	for (std::list<Callback*>::const_iterator it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
	{
		Callback* pCallback = *it;
		nRetVal = pCallback->pFunc(pSender, pCallback->pCookie);
		if (nRetVal != XN_STATUS_OK)
		{
			break;
		}
	}

	ApplyListChanges();
	return (nRetVal);
}
