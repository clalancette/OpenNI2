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
#ifndef XNACTUALREALPROPERTY_H
#define XNACTUALREALPROPERTY_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <DDK/XnRealProperty.h>

//---------------------------------------------------------------------------
// Class
//---------------------------------------------------------------------------

/**
* A property of type integer.
*/
class XnActualRealProperty final : public XnRealProperty
{
public:
	XnActualRealProperty(uint32_t propertyId, const char* strName, double dInitialValue = 0.0, const char* strModule = "");

	inline double GetValue() const { return m_dValue; }

	typedef XnStatus (XN_CALLBACK_TYPE* SetFuncPtr)(XnActualRealProperty* pSender, double dValue, void* pCookie);
	typedef XnStatus (XN_CALLBACK_TYPE* GetFuncPtr)(const XnActualRealProperty* pSender, double* pdValue, void* pCookie);

	inline void UpdateSetCallback(SetFuncPtr pFunc, void* pCookie)
	{
		XnRealProperty::UpdateSetCallback((XnRealProperty::SetFuncPtr)pFunc, pCookie);
	}

	inline void UpdateSetCallbackToDefault()
	{
		UpdateSetCallback(SetCallback, this);
	}

	inline void UpdateGetCallback(GetFuncPtr pFunc, void* pCookie)
	{
		XnRealProperty::UpdateGetCallback((XnRealProperty::GetFuncPtr)pFunc, pCookie);
	}

private:
	static XnStatus XN_CALLBACK_TYPE SetCallback(XnActualRealProperty* pSender, double dValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetCallback(const XnActualRealProperty* pSender, double* pdValue, void* pCookie);

	double m_dValue;
};

#endif // XNACTUALREALPROPERTY_H
