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
#ifndef XNREALPROPERTY_H
#define XNREALPROPERTY_H

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
class XnRealProperty : public XnProperty
{
public:
	XnRealProperty(XnUInt32 propertyId, const XnChar* strName, XnDouble* pValueHolder, const XnChar* strModule = "");

	typedef XnStatus (XN_CALLBACK_TYPE* SetFuncPtr)(XnRealProperty* pSender, XnDouble dValue, void* pCookie);
	typedef XnStatus (XN_CALLBACK_TYPE* GetFuncPtr)(const XnRealProperty* pSender, XnDouble* pdValue, void* pCookie);

	inline XnStatus SetValue(XnDouble dValue)
	{
		return XnProperty::SetValue(&dValue);
	}

	inline XnStatus GetValue(XnDouble* pdValue) const
	{
		XN_VALIDATE_OUTPUT_PTR(pdValue);
		return XnProperty::GetValue(pdValue);
	}

	inline XnStatus UnsafeUpdateValue(XnDouble dValue)
	{
		return XnProperty::UnsafeUpdateValue(&dValue);
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

	XnBool IsReadOnly() const { return (m_pGetCallback == NULL); }

	virtual XnStatus ReadValueFromFile(const XnChar* csINIFile, const XnChar* csSection);

	XnStatus AddToPropertySet(XnPropertySet* pSet);

protected:
	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	virtual XnStatus CopyValueImpl(void* pDest, const void* pSource) const;
	virtual XnBool IsEqual(const void* pValue1, const void* pValue2) const;
	virtual XnStatus CallSetCallback(const void* pValue);
	virtual XnStatus CallGetCallback(void* pValue) const;
	virtual XnBool ConvertValueToString(XnChar* csValue, const void* pValue) const;

private:
	// Set callback
	SetFuncPtr m_pSetCallback = NULL;
	void* m_pSetCallbackCookie = NULL;

	// Get callback
	GetFuncPtr m_pGetCallback = NULL;
	void* m_pGetCallbackCookie = NULL;
};

#endif // XNREALPROPERTY_H
