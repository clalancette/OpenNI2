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
#ifndef XNDEVICEMODULE_H
#define XNDEVICEMODULE_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <DDK/XnPropertySetInternal.h>
#include <DDK/XnIntProperty.h>
#include <DDK/XnRealProperty.h>
#include <DDK/XnStringProperty.h>
#include <DDK/XnGeneralProperty.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
* Holds a set of properties related to a specific module.
*/
class XnDeviceModule
{
public:
	XnDeviceModule(const XnChar* strName);
	virtual ~XnDeviceModule();

	virtual XnStatus Init();
	virtual XnStatus Free();

	inline const XnChar* GetName() const { return m_strName; }

	XnStatus AddProperty(XnProperty* pProperty);
	XnStatus AddProperties(XnProperty** apProperties, uint32_t nCount);

	XnStatus DoesPropertyExist(uint32_t propertyId, XnBool* pbDoesExist) const;
	XnStatus GetPropertyType(uint32_t propertyId, XnPropertyType* pnType) const;

	XnStatus GetProperty(uint32_t propertyId, XnProperty** ppProperty) const;

	virtual XnStatus GetProperty(uint32_t propertyId, XnUInt64* pnValue) const;
	virtual XnStatus GetProperty(uint32_t propertyId, XnDouble* pdValue) const;
	virtual XnStatus GetProperty(uint32_t propertyId, XnChar* csValue) const;
	virtual XnStatus GetProperty(uint32_t propertyId, const OniGeneralBuffer& gbValue) const;
	virtual XnStatus GetProperty(uint32_t propertyId, void* data, int* pDataSize) const;

	virtual XnStatus SetProperty(uint32_t propertyId, XnUInt64 nValue);
	virtual XnStatus SetProperty(uint32_t propertyId, XnDouble dValue);
	virtual XnStatus SetProperty(uint32_t propertyId, const XnChar* strValue);
	virtual XnStatus SetProperty(uint32_t propertyId, const OniGeneralBuffer& gbValue);
	virtual XnStatus SetProperty(uint32_t propertyId, const void* data, int dataSize);

	virtual XnStatus UnsafeUpdateProperty(uint32_t propertyId, XnUInt64 nValue);
	virtual XnStatus UnsafeUpdateProperty(uint32_t propertyId, XnDouble dValue);
	virtual XnStatus UnsafeUpdateProperty(uint32_t propertyId, const XnChar* strValue);
	virtual XnStatus UnsafeUpdateProperty(uint32_t propertyId, const OniGeneralBuffer& gbValue);

	XnStatus GetAllProperties(XnPropertySet* pSet) const;

	XnStatus RegisterForOnPropertyValueChanged(uint32_t propertyId, XnProperty::OnValueChangedHandler pFunc, void* pCookie, XnCallbackHandle& hCallback);
	XnStatus UnregisterFromOnPropertyValueChanged(uint32_t propertyId, XnCallbackHandle hCallback);

	/**
	* Reads values for all properties in module from an INI file.
	*/
	XnStatus LoadConfigFromFile(const XnChar* csINIFilePath, const XnChar* strSectionName = NULL);

	virtual XnStatus BatchConfig(const XnActualPropertiesHash& props);
	virtual XnStatus UnsafeBatchConfig(const XnActualPropertiesHash& props);

	XnStatus GetProperty(uint32_t propertyId, XnIntProperty** ppIntProperty) const;
	XnStatus GetProperty(uint32_t propertyId, XnRealProperty** ppRealProperty) const;
	XnStatus GetProperty(uint32_t propertyId, XnStringProperty** ppStringProperty) const;
	XnStatus GetProperty(uint32_t propertyId, XnGeneralProperty** ppGeneralProperty) const;

private:
	XnStatus GetPropertyImpl(uint32_t propertyId, XnPropertyType Type, XnProperty** ppProperty) const;

	XnStatus SetLockState(XnBool bLocked);

	static XnStatus XN_CALLBACK_TYPE SetLockStateCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);

	XnChar m_strName[XN_DEVICE_MAX_STRING_LENGTH];

	XnPropertiesHash m_Properties;
	XnActualIntProperty m_Lock;
	XN_CRITICAL_SECTION_HANDLE m_hLockCS;
};

#define XN_VALIDATE_ADD_PROPERTIES(pModule, ...)	\
	{												\
		XnProperty* _aProps[] = { __VA_ARGS__ };	\
		XnStatus _nRetVal = (pModule)->AddProperties(_aProps, sizeof(_aProps)/sizeof(XnProperty*));	\
		XN_IS_STATUS_OK(_nRetVal);					\
	}


#endif // XNDEVICEMODULE_H
