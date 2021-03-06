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
#ifndef XNACTUALGENERALPROPERTY_H
#define XNACTUALGENERALPROPERTY_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <DDK/XnGeneralProperty.h>

//---------------------------------------------------------------------------
// Class
//---------------------------------------------------------------------------

/**
* A property of type general which actually holds a value.
*/
class XnActualGeneralProperty final : public XnGeneralProperty
{
public:
	XnActualGeneralProperty(uint32_t propertyId, const char* strName, void* pData, uint32_t nDataSize, ReadValueFromFileFuncPtr pReadFromFileFunc = NULL, const char* strModule = "");
	XnActualGeneralProperty(uint32_t propertyId, const char* strName, const OniGeneralBuffer& gbValue, ReadValueFromFileFuncPtr pReadFromFileFunc = NULL, const char* strModule = "");
	~XnActualGeneralProperty();

	void SetAsBufferOwner(bool bOwner);

	inline const OniGeneralBuffer& GetValue() const { return m_gbValue; }

	typedef XnStatus (XN_CALLBACK_TYPE* SetFuncPtr)(XnActualGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	typedef XnStatus (XN_CALLBACK_TYPE* GetFuncPtr)(const XnActualGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);

	inline void UpdateSetCallback(SetFuncPtr pFunc, void* pCookie)
	{
		XnGeneralProperty::UpdateSetCallback((XnGeneralProperty::SetFuncPtr)pFunc, pCookie);
	}

	inline void UpdateSetCallbackToDefault()
	{
		UpdateSetCallback(SetCallback, this);
	}

	inline void UpdateGetCallback(GetFuncPtr pFunc, void* pCookie)
	{
		XnGeneralProperty::UpdateGetCallback((XnGeneralProperty::GetFuncPtr)pFunc, pCookie);
	}

	inline void ReplaceBuffer(void* pData, uint32_t nDataSize)
	{
		m_gbValue.data = pData;
		m_gbValue.dataSize = nDataSize;
	}

	XnStatus AddToPropertySet(XnPropertySet* pSet) override;

private:
	static XnStatus XN_CALLBACK_TYPE SetCallback(XnActualGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetCallback(const XnActualGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);

	OniGeneralBuffer m_gbValue;
	bool m_bOwner;
};

#endif // XNACTUALGENERALPROPERTY_H
