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
#ifndef ONICONTEXT_H
#define ONICONTEXT_H

#include <list>
#include <string>

#include "OniStream.h"
#include "OniDevice.h"
#include "OniSyncedStreamsFrameHolder.h"
#include "OniDeviceDriver.h"
#include "OniRecorder.h"
#include "OniFrameManager.h"

#include "OniDriverHandler.h"
#include "OniCommon.h"

#include <XnHash.h>
#include <XnEvent.h>

struct _OniDevice
{
	oni::implementation::Device* pDevice;
};
struct _OniStream
{
	oni::implementation::VideoStream* pStream;
};
struct _OniFrameSync
{
	oni::implementation::SyncedStreamsFrameHolder* pSyncedStreamsFrameHolder;
	oni::implementation::DeviceDriver* pDeviceDriver;
	void* pFrameSyncHandle;
};
struct _OniRecorder
{
	oni::implementation::Recorder* pRecorder;
};

ONI_NAMESPACE_IMPLEMENTATION_BEGIN

class Context final
{
public:
	Context();
	~Context();

	OniStatus initialize();
	void shutdown();

	OniStatus registerDeviceConnectedCallback(OniDeviceInfoCallback handler, void* pCookie, OniCallbackHandle& handle);
	void unregisterDeviceConnectedCallback(OniCallbackHandle handle);
	OniStatus registerDeviceDisconnectedCallback(OniDeviceInfoCallback handler, void* pCookie, OniCallbackHandle& handle);
	void unregisterDeviceDisconnectedCallback(OniCallbackHandle handle);
	OniStatus registerDeviceStateChangedCallback(OniDeviceStateCallback handler, void* pCookie, OniCallbackHandle& handle);
	void unregisterDeviceStateChangedCallback(OniCallbackHandle handle);

	OniStatus getDeviceList(OniDeviceInfo** pDevices, int* pDeviceCount);
	OniStatus releaseDeviceList(OniDeviceInfo* pDevices);

	OniStatus deviceOpen(const char* uri, const char* mode, OniDeviceHandle* pDevice);
	OniStatus deviceClose(OniDeviceHandle device);

	const OniSensorInfo* getSensorInfo(OniDeviceHandle device, OniSensorType sensorType);

	OniStatus createStream(OniDeviceHandle device, OniSensorType sensorType, OniStreamHandle* pStream);
	OniStatus streamDestroy(OniStreamHandle stream);

	const OniSensorInfo* getSensorInfo(OniStreamHandle stream);

	OniStatus readFrame(OniStreamHandle stream, OniFrame** pFrame);

	void frameRelease(OniFrame* pFrame);
	void frameAddRef(OniFrame* pFrame);

	OniStatus waitForStreams(OniStreamHandle* pStreams, int streamCount, int* pStreamIndex, int timeout);

	OniStatus enableFrameSync(OniStreamHandle* pStreams, int numStreams, OniFrameSyncHandle* pFrameSyncHandle);
	OniStatus enableFrameSyncEx(VideoStream** pStreams, int numStreams, DeviceDriver* pDriver, OniFrameSyncHandle* pFrameSyncHandle);
	void disableFrameSync(OniFrameSyncHandle frameSyncHandle);

	void clearErrorLogger();
	const char* getExtendedError();

	void addToLogger(const char* cpFormat, ...);

	OniStatus recorderOpen(const char* fileName, OniRecorderHandle* pRecorder);
	OniStatus recorderClose(OniRecorderHandle* pRecorder);
	OniStatus recorderClose(Recorder* pRecorder);

	static bool s_valid;
protected:
	OniStatus streamDestroy(VideoStream* pStream);
	static void ONI_CALLBACK_TYPE deviceDriver_DeviceConnected(Device* pDevice, void* pCookie);
	static void ONI_CALLBACK_TYPE deviceDriver_DeviceDisconnected(Device* pDevice, void* pCookie);
	static void ONI_CALLBACK_TYPE deviceDriver_DeviceStateChanged(Device* pDevice, OniDeviceState deviceState, void* pCookie);

private:
	Context(const Context& other);
	Context& operator=(const Context&other);

	XnStatus resolvePathToOpenNI();
	XnStatus configure();
	XnStatus resolveConfigurationFile(char* strConfigurationFile);
	XnStatus loadLibraries();
	void onNewFrame();
	XN_EVENT_HANDLE getThreadEvent();
	static void XN_CALLBACK_TYPE newFrameCallback(void* pCookie);

	FrameManager m_frameManager;

	xnl::ErrorLogger& m_errorLogger;

	xnl::Event1Arg<const OniDeviceInfo*> m_deviceConnectedEvent;
	xnl::Event1Arg<const OniDeviceInfo*> m_deviceDisconnectedEvent;
	xnl::Event2Args<const OniDeviceInfo*, OniDeviceState> m_deviceStateChangedEvent;

	std::list<oni::implementation::DeviceDriver*> m_deviceDrivers;
	std::list<oni::implementation::Device*> m_devices;
	std::list<oni::implementation::VideoStream*> m_streams;
	std::list<oni::implementation::Recorder*> m_recorders;

	xnl::Lockable<std::list<OniStreamHandle> > m_streamsToAutoRecord;
	bool m_autoRecording;
	bool m_autoRecordingStarted;
	OniRecorderHandle m_autoRecorder;

	xnl::Hash<XN_THREAD_ID, XN_EVENT_HANDLE> m_waitingThreads;

	xnl::CriticalSection m_cs;

	char m_pathToOpenNI[XN_FILE_MAX_PATH];
	char m_overrideDevice[XN_FILE_MAX_PATH];
	char m_driverRepo[XN_FILE_MAX_PATH];
	std::vector<std::string> m_driversList;

	int m_initializationCounter;
	uint64_t m_lastFPSPrint;
};

ONI_NAMESPACE_IMPLEMENTATION_END

#endif // ONICONTEXT_H
