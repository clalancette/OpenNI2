/*****************************************************************************
*                                                                            *
*  PrimeSense PSCommon Library                                               *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PSCommon.                                            *
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
#ifndef _XN_LOG_WRITER_BASE_H_
#define _XN_LOG_WRITER_BASE_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnLogTypes.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnLogWriterBase
{
public:
	XnLogWriterBase() : m_bRegistered(false)
	{
		m_cObject.pCookie = this;
		m_cObject.WriteEntry = WriteEntryCallback;
		m_cObject.WriteUnformatted = WriteUnformattedCallback;
		m_cObject.OnConfigurationChanged = OnConfigurationChangedCallback;
		m_cObject.OnClosing = OnClosingCallback;
	}

	virtual ~XnLogWriterBase()
	{
		Unregister();
	}

	XnStatus Register()
	{
		XnStatus nRetVal = XN_STATUS_OK;

		if (!m_bRegistered)
		{
			OnRegister();

			nRetVal = xnLogRegisterLogWriter(&m_cObject);
			if (nRetVal != XN_STATUS_OK)
			{
				OnUnregister();
				return (nRetVal);
			}

			m_bRegistered = true;
		}

		return (XN_STATUS_OK);
	}

	void Unregister()
	{
		if (m_bRegistered)
		{
			xnLogUnregisterLogWriter(&m_cObject);
			m_bRegistered = false;

			OnUnregister();
		}
	}

	inline bool IsRegistered() { return m_bRegistered; }

	virtual void WriteEntry(const XnLogEntry* pEntry) = 0;
	virtual void WriteUnformatted(const char* strMessage) = 0;
	virtual void OnConfigurationChanged() {};
	virtual void OnClosing()
	{
		Unregister();
	};

	operator const XnLogWriter*() const
	{
		return &m_cObject;
	}

protected:
	virtual void OnRegister() {}
	virtual void OnUnregister() {}

private:
	static void XN_CALLBACK_TYPE WriteEntryCallback(const XnLogEntry* pEntry, void* pCookie)
	{
		XnLogWriterBase* pThis = (XnLogWriterBase*)pCookie;
		pThis->WriteEntry(pEntry);
	}
	static void XN_CALLBACK_TYPE WriteUnformattedCallback(const char* strMessage, void* pCookie)
	{
		XnLogWriterBase* pThis = (XnLogWriterBase*)pCookie;
		pThis->WriteUnformatted(strMessage);
	}
	static void XN_CALLBACK_TYPE OnConfigurationChangedCallback(void* pCookie)
	{
		XnLogWriterBase* pThis = (XnLogWriterBase*)pCookie;
		pThis->OnConfigurationChanged();
	}
	static void XN_CALLBACK_TYPE OnClosingCallback(void* pCookie)
	{
		XnLogWriterBase* pThis = (XnLogWriterBase*)pCookie;
		pThis->OnClosing();
	}

	XnLogWriter m_cObject;
	bool m_bRegistered;
};

#endif // _XN_LOG_WRITER_BASE_H_
