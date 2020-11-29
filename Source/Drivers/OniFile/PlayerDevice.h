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
/// @file
/// Contains the declaration of Device class that implements a virtual OpenNI
/// device, capable of reading data from a *.ONI file.

#ifndef PLAYERDEVICE_H
#define PLAYERDEVICE_H

#include <list>
#include <string>

#include "Driver/OniDriverAPI.h"
#include "XnOSCpp.h"
#include "PlayerNode.h"
#include "PlayerProperties.h"
#include "PlayerStream.h"

namespace oni_file {

class PlayerSource;

/// Implements a virtual OpenNI device, which reads is adata from a *.ONI file.
class PlayerDevice final : public oni::driver::DeviceBase
{
public:
	/// Constructs a device from the given file path.
	/// @param[in] filePath The path to a *.ONI file.
	PlayerDevice(const std::string& filePath);

	~PlayerDevice();

	/// Initialize the device object.
	OniStatus Initialize();

	/// @copydoc OniDeviceBase::GetStreamSourceInfoList(OniSourceInfo**, int*)
	OniStatus getSensorInfoList(OniSensorInfo** pSources, int* numSources) override;

	/// @copydoc OniDeviceBase::CreateStream(OniStreamSource)
	oni::driver::StreamBase* createStream(OniSensorType) override;

	void destroyStream(oni::driver::StreamBase* pStream) override;

	/// @copydoc OniDeviceBase::TryManualTrigger()
	OniStatus tryManualTrigger() override;

	/// Get property.
	OniStatus getProperty(int propertyId, void* data, int* pDataSize) override;

	/// Set property.
	OniStatus setProperty(int propertyId, const void* data, int dataSize) override;

	OniBool isPropertySupported(int propertyId) override;

	/// @copydoc OniDeviceBase::Invoke(int, void*, int)
	OniStatus invoke(int commandId, void* data, int dataSize) override;
	OniBool isCommandSupported(int commandId) override;

	OniBool isPlayerEOF() { return m_player.IsEOF(); };

	typedef void (XN_CALLBACK_TYPE *DriverEOFCallback)(void* pCookie, const char* uri);
	void SetEOFEventCallback(DriverEOFCallback pFunc, void* pDriverCookie)
	{
		m_driverEOFCallback = pFunc;
		m_driverCookie      = pDriverCookie;
	};
	void TriggerDriverEOFCallback() { if(m_driverEOFCallback) (m_driverEOFCallback)(m_driverCookie, m_filePath.c_str()); };

	const char* getOriginalDevice() {return m_originalDevice;}
private:
	PlayerSource* FindSource(const XnChar* strNodeName);

	// Wake up when timestamp is valid.
	void SleepToTimestamp(uint64_t nTimeStamp);

	void LoadConfigurationFromIniFile();

	void close();

	typedef struct
	{
		int frameId;
		PlayerStream* pStream;
	} Seek;

	void MainLoop();

	static XN_THREAD_PROC ThreadProc(XN_THREAD_PARAM pThreadParam);

	static void     ONI_CALLBACK_TYPE ReadyForDataCallback(const PlayerStream::ReadyForDataEventArgs& newDataEventArgs, void* pCookie);
	static void     ONI_CALLBACK_TYPE StreamDestroyCallback(const PlayerStream::DestroyEventArgs& destroyEventArgs, void* pCookie);

	static XnStatus XN_CALLBACK_TYPE OnNodeAdded(void* pCookie, const XnChar* strNodeName, XnProductionNodeType type, XnCodecID compression, uint32_t nNumberOfFrames);
	static XnStatus XN_CALLBACK_TYPE OnNodeRemoved(void* pCookie, const XnChar* strNodeName);
	static XnStatus XN_CALLBACK_TYPE OnNodeIntPropChanged(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, uint64_t nValue);
	static XnStatus XN_CALLBACK_TYPE OnNodeRealPropChanged(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, double dValue);
	static XnStatus XN_CALLBACK_TYPE OnNodeStringPropChanged(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, const XnChar* strValue);
	static XnStatus XN_CALLBACK_TYPE OnNodeGeneralPropChanged(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, uint32_t nBufferSize, const void* pBuffer);
	static XnStatus XN_CALLBACK_TYPE OnNodeStateReady(void* pCookie, const XnChar* strNodeName);
	static XnStatus XN_CALLBACK_TYPE OnNodeNewData(void* pCookie, const XnChar* strNodeName, uint64_t nTimeStamp, uint32_t nFrame, const void* pData, uint32_t nSize);
	static void		XN_CALLBACK_TYPE OnEndOfFileReached(void* pCookie);
	XnStatus AddPrivateProperty(PlayerSource* pSource, const XnChar* strPropName, uint32_t nBufferSize, const void* pBuffer);
	XnStatus AddPrivateProperty_PS1080(PlayerSource* pSource, const XnChar* strPropName, uint32_t nBufferSize, const void* pBuffer);
	XnStatus AddPrivateProperty_PSLink(PlayerSource* pSource, const XnChar* strPropName, uint32_t nBufferSize, const void* pBuffer);

	static XnStatus XN_CALLBACK_TYPE FileOpen(void* pCookie);
	static XnStatus XN_CALLBACK_TYPE FileRead(void* pCookie, void* pBuffer, uint32_t nSize, uint32_t* pnBytesRead);
	static XnStatus XN_CALLBACK_TYPE FileSeek(void* pCookie, XnOSSeekType seekType, const int32_t nOffset);
	static uint32_t XN_CALLBACK_TYPE FileTell(void* pCookie);
	static void     XN_CALLBACK_TYPE FileClose(void* pCookie);
	static XnStatus XN_CALLBACK_TYPE FileSeek64(void* pCookie, XnOSSeekType seekType, const int64_t nOffset);
	static uint64_t XN_CALLBACK_TYPE FileTell64(void* pCookie);

	static XnStatus XN_CALLBACK_TYPE CodecCreate(void* pCookie, const char* strNodeName, XnCodecID nCodecId, XnCodec** ppCodec);
	static void     XN_CALLBACK_TYPE CodecDestroy(void* pCookie, XnCodec* pCodec);

	static XnStatus ResolveGlobalConfigFileName(XnChar* strConfigFile, uint32_t nBufSize, const XnChar* strConfigDir);

	// Name of the node (used for identifying the device in the callbacks).
	std::string m_nodeName;

	// The path to a *.ONI file which is mounted by this device.
	const std::string m_filePath;

	// Handle to the opened file.
	XN_FILE_HANDLE m_fileHandle;

	// Thread handle.
	XN_THREAD_HANDLE m_threadHandle;

	// Running flag.
	OniBool m_running;

	// Seek frame.
	Seek m_seek;
	OniBool m_isSeeking;
	OniBool m_seekingFailed;

	// Speed of playback.
	double m_dPlaybackSpeed;

	// Timestamps.
	uint64_t m_nStartTimestamp;
	uint64_t m_nStartTime;
	XnBool m_bHasTimeReference;

	// Repeat recording in loop.
	OniBool m_bRepeat;

	// Player object.
	PlayerNode m_player;

	// Driver EOF callback
	DriverEOFCallback m_driverEOFCallback;
	void *m_driverCookie;

	// Properties.
	PlayerProperties m_properties;

	// List of sources.
	std::list<PlayerSource*> m_sources;

	// List of streams.
	std::list<PlayerStream*> m_streams;

	// Internal event for stream ready for data.
	xnl::OSEvent m_readyForDataInternalEvent;

	// Internal event for manual trigger (more frames requested).
	xnl::OSEvent m_manualTriggerInternalEvent;

	// Internal event for seek complete.
	xnl::OSEvent m_SeekCompleteInternalEvent;

	// Critical section.
	xnl::CriticalSection m_cs;

	char m_originalDevice[ONI_MAX_STR];

	char m_iniFilePath[XN_FILE_MAX_PATH];
};

} // namespace oni_files_player

#endif // PLAYERDEVICE_H
