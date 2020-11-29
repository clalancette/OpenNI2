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
#ifndef XNINTPROPERTY_H
#define XNINTPROPERTY_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <DDK/XnProperty.h>

//---------------------------------------------------------------------------
// Class
//---------------------------------------------------------------------------

/**
* A property of type integer.
*/
class XnIntProperty : public XnProperty
{
public:
	XnIntProperty(uint32_t propertyId, const XnChar* strName, uint64_t* pValueHolder = NULL, const XnChar* strModule = "");

	typedef XnStatus (XN_CALLBACK_TYPE* SetFuncPtr)(XnIntProperty* pSender, uint64_t nValue, void* pCookie);
	typedef XnStatus (XN_CALLBACK_TYPE* GetFuncPtr)(const XnIntProperty* pSender, uint64_t* pnValue, void* pCookie);

	inline XnStatus SetValue(uint64_t nValue)
	{
		return XnProperty::SetValue(&nValue);
	}

	inline XnStatus GetValue(uint64_t* pnValue) const
	{
		XN_VALIDATE_OUTPUT_PTR(pnValue);
		return XnProperty::GetValue(pnValue);
	}

	XnStatus UnsafeUpdateValue(uint64_t nValue)
	{
		return XnProperty::UnsafeUpdateValue(&nValue);
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

	XnBool IsReadOnly() const override { return (m_pGetCallback == NULL); }

	virtual XnStatus ReadValueFromFile(const XnChar* csINIFile, const XnChar* csSection) override;

	XnStatus AddToPropertySet(XnPropertySet* pSet) override;

protected:
	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	virtual XnStatus CopyValueImpl(void* pDest, const void* pSource) const override;
	virtual XnBool IsEqual(const void* pValue1, const void* pValue2) const override;
	virtual XnStatus CallSetCallback(const void* pValue) override;
	virtual XnStatus CallGetCallback(void* pValue) const override;
	virtual XnBool ConvertValueToString(XnChar* csValue, const void* pValue) const override;

private:
	// Set callback
	SetFuncPtr m_pSetCallback = NULL;
	void* m_pSetCallbackCookie = NULL;

	// Get callback
	GetFuncPtr m_pGetCallback = NULL;
	void* m_pGetCallbackCookie = NULL;
};

#endif // XNINTPROPERTY_H
