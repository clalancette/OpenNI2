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
	XnRealProperty(uint32_t propertyId, const char* strName, double* pValueHolder, const char* strModule = "");

	typedef XnStatus (XN_CALLBACK_TYPE* SetFuncPtr)(XnRealProperty* pSender, double dValue, void* pCookie);
	typedef XnStatus (XN_CALLBACK_TYPE* GetFuncPtr)(const XnRealProperty* pSender, double* pdValue, void* pCookie);

	inline XnStatus SetValue(double dValue)
	{
		return XnProperty::SetValue(&dValue);
	}

	inline XnStatus GetValue(double* pdValue) const
	{
		XN_VALIDATE_OUTPUT_PTR(pdValue);
		return XnProperty::GetValue(pdValue);
	}

	inline XnStatus UnsafeUpdateValue(double dValue)
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

	bool IsReadOnly() const override { return (m_pGetCallback == NULL); }

	virtual XnStatus ReadValueFromFile(const char* csINIFile, const char* csSection) override;

	XnStatus AddToPropertySet(XnPropertySet* pSet) override;

protected:
	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	virtual XnStatus CopyValueImpl(void* pDest, const void* pSource) const override;
	virtual bool IsEqual(const void* pValue1, const void* pValue2) const override;
	virtual XnStatus CallSetCallback(const void* pValue) override;
	virtual XnStatus CallGetCallback(void* pValue) const override;
	virtual bool ConvertValueToString(char* csValue, const void* pValue) const override;

private:
	// Set callback
	SetFuncPtr m_pSetCallback = NULL;
	void* m_pSetCallbackCookie = NULL;

	// Get callback
	GetFuncPtr m_pGetCallback = NULL;
	void* m_pGetCallbackCookie = NULL;
};

#endif // XNREALPROPERTY_H
