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
#ifndef XNFRAMESTREAM_H
#define XNFRAMESTREAM_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceStream.h"
#include "XnFrameBufferManager.h"
#include "Driver/OniDriverTypes.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/** Represents a stream that is frame-based. */
class XnFrameStream : public XnDeviceStream
{
public:
	XnFrameStream(const char* csType, const char* csName);
	~XnFrameStream() { Free(); }

	//---------------------------------------------------------------------------
	// Getters
	//---------------------------------------------------------------------------
	inline uint32_t GetFPS() const { return (uint32_t)m_FPS.GetValue(); }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	virtual XnStatus Init() override;
	virtual XnStatus Free() override;

	virtual XnStatus Close() override;

protected:
	//---------------------------------------------------------------------------
	// Properties Getters
	//---------------------------------------------------------------------------
	inline XnActualIntProperty& FPSProperty() { return m_FPS; }

	//---------------------------------------------------------------------------
	// Getters
	//---------------------------------------------------------------------------
	XnStatus StartBufferManager(XnFrameBufferManager** pBufferManager);

	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	virtual XnStatus SetFPS(uint32_t nFPS);

	//---------------------------------------------------------------------------
	// Virtual Methods
	//---------------------------------------------------------------------------
	virtual XnStatus PostProcessFrame(OniFrame* /*pFrame*/) { return XN_STATUS_OK; }

private:
	XN_DISABLE_COPY_AND_ASSIGN(XnFrameStream);

	static XnStatus XN_CALLBACK_TYPE SetFPSCallback(XnActualIntProperty* pSenser, uint64_t nValue, void* pCookie);
	static void XN_CALLBACK_TYPE OnTripleBufferNewData(OniFrame* pFrame, void* pCookie);

	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnFrameBufferManager m_bufferManager;

	uint32_t m_nLastReadFrame; // the ID that was given

	XnActualIntProperty m_IsFrameStream;
	XnActualIntProperty m_FPS;
};

#endif // XNFRAMESTREAM_H
