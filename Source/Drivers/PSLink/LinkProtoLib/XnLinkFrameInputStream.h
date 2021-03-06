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
#ifndef XNLINKFRAMEINPUTSTREAM_H
#define XNLINKFRAMEINPUTSTREAM_H

#include <vector>

#include "XnLinkMsgParser.h"
#include "XnLinkInputStream.h"
#include <XnEvent.h>
#include <XnStatus.h>

#include <Driver/OniDriverAPI.h>

struct XnDumpFile;

namespace xn
{

class LinkControlEndpoint;

typedef struct NewFrameEventArgs
{
	OniFrame* pFrame;
} NewFrameEventArgs;

typedef xnl::Event<NewFrameEventArgs> NewFrameEvent;

class LinkFrameInputStream : public LinkInputStream
{
public:
	LinkFrameInputStream();
	virtual ~LinkFrameInputStream();
	virtual XnStatus Init(LinkControlEndpoint* pLinkControlEndpoint,
				XnStreamType streamType,
				uint16_t nStreamID,
				IConnection* pConnection);

	virtual void SetStreamServices(oni::driver::StreamServices* pServices) { m_pServices = pServices; }

	uint32_t GetRequiredFrameSize() const { return CalcBufferSize(); }

	virtual void Reset();

	virtual bool IsInitialized() const;
	virtual void Shutdown();
	virtual XnStatus HandlePacket(const LinkPacketHeader& header, const uint8_t* pData, bool& bPacketLoss);

	virtual void SetDumpName(const char* strDumpName);
	virtual void SetDumpOn(bool bDumpOn);

	virtual XnStreamFragLevel GetStreamFragLevel() const { return XN_LINK_STREAM_FRAG_LEVEL_FRAMES; }

	typedef void (XN_CALLBACK_TYPE* NewFrameEventHandler)(const NewFrameEventArgs& args, void* pCookie);
	NewFrameEvent::Interface& GetNewFrameEvent() { return m_newFrameEvent; }

	virtual bool IsOutputFormatSupported(OniPixelFormat format) const;

	virtual const std::vector<XnFwStreamVideoMode>& GetSupportedVideoModes() const;
	virtual const XnFwStreamVideoMode& GetVideoMode() const;
	virtual XnStatus SetVideoMode(const XnFwStreamVideoMode& videoMode);

	virtual const XnShiftToDepthConfig& GetShiftToDepthConfig() const;
	virtual XnStatus GetShiftToDepthTables(const XnShiftToDepthTables*& pTables) const;

	virtual const XnLinkCameraIntrinsics& GetCameraIntrinsics() const { return m_cameraIntrinsics; }

	virtual const OniCropping& GetCropping() const;
	virtual XnStatus SetCropping(OniCropping cropping);

	virtual void GetFieldOfView(float* pHFOV, float* pVFOV) const { if (pHFOV) *pHFOV = m_fHFOV; if (pVFOV) *pVFOV = m_fVFOV; }

protected:
	virtual XnStatus StartImpl();
	virtual XnStatus StopImpl();

	virtual LinkMsgParser* CreateLinkMsgParser();

private:
	class DefaultStreamServices : public oni::driver::StreamServices
	{
	public:
		DefaultStreamServices();
		void setStream(LinkFrameInputStream* pStream);
		static int ONI_CALLBACK_TYPE getDefaultRequiredFrameSizeCallback(void* streamServices);
		static OniFrame* ONI_CALLBACK_TYPE acquireFrameCallback(void* streamServices);
		static void ONI_CALLBACK_TYPE releaseFrameCallback(void* streamServices, OniFrame* pFrame);
		static void ONI_CALLBACK_TYPE addFrameRefCallback(void* streamServices, OniFrame* pFrame);
	};

	struct LinkOniFrame : public OniFrame
	{
		int refCount;
	};

	static void Swap(uint32_t& nVal1, uint32_t& nVal2);
	uint32_t GetOutputBytesPerPixel() const;
	virtual uint32_t CalcBufferSize() const;
	uint32_t CalcExpectedSize() const;
	XnStatus UpdateCameraIntrinsics();

	DefaultStreamServices m_defaultServices;

	oni::driver::StreamServices* m_pServices;

	volatile bool m_bInitialized;

	NewFrameEvent m_newFrameEvent;
	OniFrame* m_pCurrFrame;

	bool m_currentFrameCorrupt;
	mutable XN_CRITICAL_SECTION_HANDLE m_hCriticalSection; //Protects buffers info

	uint32_t m_nBufferSize;
	LinkMsgParser* m_pLinkMsgParser;

	XnDumpFile* m_pDumpFile;
	char m_strDumpName[XN_FILE_MAX_PATH];
	uint32_t m_nDumpFrameID;

	std::vector<XnFwStreamVideoMode> m_supportedVideoModes;
	XnFwStreamVideoMode m_videoMode;

	int m_frameIndex;
	OniCropping m_cropping;

	XnShiftToDepthConfig m_shiftToDepthConfig;
	XnShiftToDepthTables m_shiftToDepthTables;

	XnLinkCameraIntrinsics m_cameraIntrinsics;

	// Field of View
	float m_fHFOV;
	float m_fVFOV;
};

}

#endif // XNLINKFRAMEINPUTSTREAM_H
