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
#ifndef _XN_DUMP_WRITERS_H_
#define _XN_DUMP_WRITERS_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDump.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnDumpWriterFileHandle
{
	void* pInternal;
} XnDumpWriterFileHandle;

typedef struct XnDumpWriter
{
	void* pCookie;
	XnDumpWriterFileHandle (XN_CALLBACK_TYPE* OpenFile)(void* pCookie, const XnChar* strDumpName, bool bSessionDump, const XnChar* strFileName);
	void (XN_CALLBACK_TYPE* Write)(void* pCookie, XnDumpWriterFileHandle hFile, const void* pBuffer, uint32_t nBufferSize);
	void (XN_CALLBACK_TYPE* CloseFile)(void* pCookie, XnDumpWriterFileHandle hFile);
} XnDumpWriter;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------
XN_C_API XnStatus XN_C_DECL xnDumpRegisterWriter(XnDumpWriter* pWriter);

XN_C_API void XN_C_DECL xnDumpUnregisterWriter(XnDumpWriter* pWriter);

XN_C_API XnStatus XN_C_DECL xnDumpSetFilesOutput(bool bOn);

//---------------------------------------------------------------------------
// Helpers
//---------------------------------------------------------------------------

class XnDumpWriterBase
{
public:
	XnDumpWriterBase() : m_bRegistered(false)
	{
		m_cObject.pCookie = this;
		m_cObject.OpenFile = OpenFileCallback;
		m_cObject.Write = WriteCallback;
		m_cObject.CloseFile = CloseFileCallback;
	}

	virtual ~XnDumpWriterBase()
	{
		Unregister();
	}

	XnStatus Register()
	{
		XnStatus nRetVal = XN_STATUS_OK;

		if (!m_bRegistered)
		{
			OnRegister();

			nRetVal = xnDumpRegisterWriter(&m_cObject);
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
			xnDumpUnregisterWriter(&m_cObject);
			m_bRegistered = false;

			OnUnregister();
		}
	}

	inline bool IsRegistered() { return m_bRegistered; }

	virtual XnDumpWriterFileHandle OpenFile(const XnChar* strDumpName, bool bSessionDump, const XnChar* strFileName) = 0;
	virtual void Write(XnDumpWriterFileHandle hFile, const void* pBuffer, uint32_t nBufferSize) = 0;
	virtual void CloseFile(XnDumpWriterFileHandle hFile) = 0;

	operator const XnDumpWriter*() const
	{
		return &m_cObject;
	}

protected:
	virtual void OnRegister() {}
	virtual void OnUnregister() {}

private:
	static XnDumpWriterFileHandle XN_CALLBACK_TYPE OpenFileCallback(void* pCookie, const XnChar* strDumpName, bool bSessionDump, const XnChar* strFileName)
	{
		XnDumpWriterBase* pThis = (XnDumpWriterBase*)pCookie;
		return pThis->OpenFile(strDumpName, bSessionDump, strFileName);
	}

	static void XN_CALLBACK_TYPE WriteCallback(void* pCookie, XnDumpWriterFileHandle hFile, const void* pBuffer, uint32_t nBufferSize)
	{
		XnDumpWriterBase* pThis = (XnDumpWriterBase*)pCookie;
		return pThis->Write(hFile, pBuffer, nBufferSize);
	}

	static void XN_CALLBACK_TYPE CloseFileCallback(void* pCookie, XnDumpWriterFileHandle hFile)
	{
		XnDumpWriterBase* pThis = (XnDumpWriterBase*)pCookie;
		return pThis->CloseFile(hFile);
	}

	XnDumpWriter m_cObject;
	bool m_bRegistered;
};

#endif // _XN_DUMP_WRITERS_H_
