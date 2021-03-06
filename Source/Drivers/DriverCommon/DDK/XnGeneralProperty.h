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
#ifndef XNGENERALPROPERTY_H
#define XNGENERALPROPERTY_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <DDK/XnProperty.h>
#include <Driver/OniDriverTypes.h>

//---------------------------------------------------------------------------
// Class
//---------------------------------------------------------------------------

/**
* A property of type general.
*/
class XnGeneralProperty : public XnProperty
{
public:
	typedef XnStatus (XN_CALLBACK_TYPE* ReadValueFromFileFuncPtr)(XnGeneralProperty* pSender, const char* csINIFile, const char* csSection);

	XnGeneralProperty(uint32_t propertyId, const char* strName, OniGeneralBuffer* pValueHolder = NULL, ReadValueFromFileFuncPtr pReadFromFileFunc = NULL, const char* strModule = "");

	typedef XnStatus (XN_CALLBACK_TYPE* SetFuncPtr)(XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	typedef XnStatus (XN_CALLBACK_TYPE* GetFuncPtr)(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);

	inline XnStatus SetValue(const OniGeneralBuffer& gbValue)
	{
		return XnProperty::SetValue(&gbValue);
	}

	inline XnStatus GetValue(const OniGeneralBuffer& gbValue) const
	{
		return XnProperty::GetValue((void*)&gbValue);
	}

	inline XnStatus UnsafeUpdateValue(const OniGeneralBuffer& gbValue)
	{
		return XnProperty::UnsafeUpdateValue(&gbValue);
	}

	inline void UpdateSetCallback(SetFuncPtr pFunc, void* pCookie)
	{
		m_pSetCallback = pFunc;
		m_pSetCallbackCookie = pCookie;
	}

	inline void UpdateGetCallback(GetFuncPtr pFunc, void* pCookie)
	{
		m_pGetCallback = pFunc;
		m_pGetCallbackCookie = pCookie;
	}

	virtual XnStatus AddToPropertySet(XnPropertySet* pSet) override;

	bool IsReadOnly() const override { return (m_pGetCallback == NULL); }

protected:
	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	virtual XnStatus CopyValueImpl(void* pDest, const void* pSource) const override;
	virtual bool IsEqual(const void* pValue1, const void* pValue2) const override;
	virtual XnStatus CallSetCallback(const void* pValue) override;
	virtual XnStatus CallGetCallback(void* pValue) const override;
	virtual XnStatus ReadValueFromFile(const char* csINIFile, const char* csSection) override;

private:
	ReadValueFromFileFuncPtr m_pReadFromFileFunc;

	// Set callback
	SetFuncPtr m_pSetCallback = NULL;
	void* m_pSetCallbackCookie = NULL;

	// Get callback
	GetFuncPtr m_pGetCallback = NULL;
	void* m_pGetCallbackCookie = NULL;
};

#endif // XNGENERALPROPERTY_H
