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
/// Contains the definition of Device class that implements a virtual OpenNI
/// device, capable of reading data from a *.ONI file.

#include <algorithm>
#include <list>
#include <string>

#include "PlayerDevice.h"
#include "PlayerSource.h"
#include "PlayerStream.h"
#include "XnPropNames.h"
#include "OniProperties.h"
#include "XnMemory.h"
#include "Formats/XnCodec.h"
#include "PlayerCodecFactory.h"
#include "PS1080.h"
#include "PSLink.h"
#include "XnOSStrings.h"

namespace oni_file {

namespace driver = oni::driver;

#define XN_PLAYER_CONFIGURATION_FILE "OniFile.ini"
#define DEVICE_DESTROY_THREAD_TIMEOUT				3000
#define DEVICE_READY_FOR_DATA_EVENT_SANITY_SLEEP	2000
#define DEVICE_MANUAL_TRIGGER_STANITY_SLEEP			2000
#define XN_PLAYBACK_SPEED_SANITY_SLEEP				2000
#define XN_PLAYBACK_SPEED_FASTEST					0.0
#define XN_PLAYBACK_SPEED_MANUAL					(-1.0)

#define ONI_INIFILE_SECTION_PLAYER "Player"
#define ONI_INIFILE_ENTRY_SPEED  "Speed"
#define ONI_INIFILE_ENTRY_REPEAT "Repeat"

#ifndef ARRAYSIZE
#define ARRAYSIZE(a)								(sizeof(a)/sizeof((a)[0]))
#endif //ARRAYSIZE

typedef struct
{
	// Identifier of the property.
	uint32_t propertyId;

	// Name of the property.
	XnChar propertyName[40];

} PropertyEntry;

static PropertyEntry PS1080PropertyList[] =
{
	{ XN_STREAM_PROPERTY_INPUT_FORMAT,			"InputFormat" },
	{ XN_STREAM_PROPERTY_CROPPING_MODE,			"CroppingMode" },
	{ XN_STREAM_PROPERTY_WHITE_BALANCE_ENABLED,		"WhiteBalancedEnabled" },
	{ XN_STREAM_PROPERTY_GAIN,				"Gain" },
	{ XN_STREAM_PROPERTY_HOLE_FILTER,			"HoleFilter" },
	{ XN_STREAM_PROPERTY_REGISTRATION_TYPE,			"RegistrationType" },
	{ XN_STREAM_PROPERTY_AGC_BIN,				"AGCBin" },
	{ XN_STREAM_PROPERTY_CONST_SHIFT,			"ConstShift" },
	{ XN_STREAM_PROPERTY_PIXEL_SIZE_FACTOR,			"PixelSizeFactor" },
	{ XN_STREAM_PROPERTY_MAX_SHIFT,				"MaxShift" },
	{ XN_STREAM_PROPERTY_PARAM_COEFF,			"ParamCoeff" },
	{ XN_STREAM_PROPERTY_SHIFT_SCALE,			"ShiftScale" },
	{ XN_STREAM_PROPERTY_S2D_TABLE,				"S2D" },
	{ XN_STREAM_PROPERTY_D2S_TABLE,				"D2S" },
	{ XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE,		"ZPD" },
	{ XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE,		"ZPPS" },
	{ XN_STREAM_PROPERTY_EMITTER_DCMOS_DISTANCE,		"LDDIS" },
	{ XN_STREAM_PROPERTY_DCMOS_RCMOS_DISTANCE,		"DCRCDIS" },
	{ XN_STREAM_PROPERTY_CLOSE_RANGE,			"CloseRange" },
	{ XN_STREAM_PROPERTY_FAST_ZOOM_CROP,			"FastZoomCrop" },
	{ XN_STREAM_PROPERTY_PIXEL_REGISTRATION,		"PixelRegistration" },
};

static PropertyEntry PSLinkPropertyList[] =
{
	{ LINK_PROP_MAX_SHIFT,				"MaxShift" },
	{ LINK_PROP_ZERO_PLANE_DISTANCE,		"ZPD" },
	{ LINK_PROP_CONST_SHIFT,			"ConstShift" },
	{ LINK_PROP_PARAM_COEFF,			"ParamCoeff" },
	{ LINK_PROP_SHIFT_SCALE,			"ShiftScale" },
	{ LINK_PROP_ZERO_PLANE_PIXEL_SIZE,		"ZPPS" },
	{ LINK_PROP_ZERO_PLANE_OUTPUT_PIXEL_SIZE,	"ZPOPS" },
	{ LINK_PROP_EMITTER_DEPTH_CMOS_DISTANCE,	"LDDIS" },
	{ LINK_PROP_SHIFT_TO_DEPTH_TABLE,		"S2D" },
	{ LINK_PROP_DEPTH_TO_SHIFT_TABLE,		"D2S" },
};

XnStatus PlayerDevice::ResolveGlobalConfigFileName(XnChar* strConfigFile, uint32_t nBufSize, const XnChar* strConfigDir)
{
	XnStatus rc = XN_STATUS_OK;

	// If strConfigDir is NULL, tries to resolve the config file based on the driver's directory
	XnChar strBaseDir[XN_FILE_MAX_PATH];
	if (strConfigDir == NULL)
	{
		if (xnOSGetModulePathForProcAddress((void*)(&PlayerDevice::ResolveGlobalConfigFileName), strBaseDir) == XN_STATUS_OK &&
			xnOSGetDirName(strBaseDir, strBaseDir, XN_FILE_MAX_PATH) == XN_STATUS_OK)
		{
			// Successfully obtained the driver's path
			strConfigDir = strBaseDir;
		}
		else
		{
			// Something wrong happened. Use the current directory as the fallback.
			strConfigDir = ".";
		}
	}

	XN_VALIDATE_STR_COPY(strConfigFile, strConfigDir, nBufSize, rc);
	return xnOSAppendFilePath(strConfigFile, XN_PLAYER_CONFIGURATION_FILE, nBufSize);
}

void PlayerDevice::LoadConfigurationFromIniFile()
{
	XnStatus nRetVal;
	double dSpeed = 0;
	int32_t nRepearMode = 0;

	nRetVal = xnOSReadDoubleFromINI(m_iniFilePath,ONI_INIFILE_SECTION_PLAYER, ONI_INIFILE_ENTRY_SPEED, &dSpeed);

	if (XN_STATUS_OK == nRetVal)
	{
		m_dPlaybackSpeed = dSpeed;
	}

	nRetVal = xnOSReadIntFromINI(m_iniFilePath, ONI_INIFILE_SECTION_PLAYER, ONI_INIFILE_ENTRY_REPEAT, &nRepearMode);

	if (XN_STATUS_OK == nRetVal)
	{
		m_bRepeat = nRepearMode;
	}


}

PlayerDevice::PlayerDevice(const std::string& filePath) :
	m_filePath(filePath), m_fileHandle(0), m_threadHandle(NULL), m_running(false), m_isSeeking(false), m_seekingFailed(false),
	m_dPlaybackSpeed(1.0), m_nStartTimestamp(0), m_nStartTime(0), m_bHasTimeReference(false),
	m_bRepeat(true), m_player(filePath.c_str()), m_driverEOFCallback(NULL), m_driverCookie(NULL)
{
	xnOSMemSet(m_originalDevice, 0, sizeof(m_originalDevice));

	// Create the events.
	m_readyForDataInternalEvent.Create(false);
	m_manualTriggerInternalEvent.Create(false);
	m_SeekCompleteInternalEvent.Create(false);
}

PlayerDevice::~PlayerDevice()
{
	close();
}

OniStatus PlayerDevice::Initialize()
{
	static XnNodeNotifications notifications =
	{
		OnNodeAdded,
		OnNodeRemoved,
		OnNodeIntPropChanged,
		OnNodeRealPropChanged,
		OnNodeStringPropChanged,
		OnNodeGeneralPropChanged,
		OnNodeStateReady,
		OnNodeNewData,
	};
	static XnPlayerInputStreamInterface inputInterface =
	{
		FileOpen,
		FileRead,
		FileSeek,
		FileTell,
		FileClose,
		FileSeek64,
		FileTell64,
	};
	static PlayerNode::CodecFactory codecFactory =
	{
		CodecCreate,
		CodecDestroy
	};

	// Initialize the player.
	XnStatus rc = m_player.Init();
	if (rc != XN_STATUS_OK)
	{
		return ONI_STATUS_ERROR;
	}

	// Set the notifications.
	rc = m_player.SetNodeNotifications(this, &notifications);
	if (rc != XN_STATUS_OK)
	{
		return ONI_STATUS_ERROR;
	}

	// Set the codec factory.
	rc = m_player.SetNodeCodecFactory(this, &codecFactory);
	if (rc != XN_STATUS_OK)
	{
		return ONI_STATUS_ERROR;
	}

	// Register to end of file reached event.
	XnCallbackHandle handle;
	rc = m_player.RegisterToEndOfFileReached(OnEndOfFileReached, this, handle);
	if (rc != XN_STATUS_OK)
	{
		return ONI_STATUS_ERROR;
	}

	// Set the input interface.
	rc = m_player.SetInputStream(this, &inputInterface);
	if (rc != XN_STATUS_OK)
	{
		return ONI_STATUS_ERROR;
	}

	// Create thread for running the player.
	XnStatus status = xnOSCreateThread(ThreadProc, this, &m_threadHandle);
	if (status != XN_STATUS_OK)
	{
		return ONI_STATUS_ERROR;
	}

	status = ResolveGlobalConfigFileName(m_iniFilePath, sizeof(m_iniFilePath), NULL);
	if (XN_STATUS_OK != status)
	{
		return ONI_STATUS_ERROR;
	}

	bool bIsExist = false;
	xnOSDoesFileExist(m_iniFilePath, &bIsExist);

	if (bIsExist)
	{
		LoadConfigurationFromIniFile();
	}
	return ONI_STATUS_OK;
}

void PlayerDevice::close()
{
	// Destroy the thread.
	m_running = false;
	m_readyForDataInternalEvent.Set();
	m_manualTriggerInternalEvent.Set();
	XnStatus rc = xnOSWaitForThreadExit(m_threadHandle, DEVICE_DESTROY_THREAD_TIMEOUT);
	if (rc != XN_STATUS_OK)
	{
		xnOSTerminateThread(&m_threadHandle);
	}
	else
	{
		xnOSCloseThread(&m_threadHandle);
	}

	// Destroy the player.
	m_player.Destroy();

	// Delete all the sources and streams.
	xnl::AutoCSLocker lock(m_cs);
	while (m_streams.begin() != m_streams.end())
	{
		std::list<PlayerStream*>::iterator pStream = m_streams.begin();
		m_streams.erase(pStream);
	}
	while (m_sources.begin() != m_sources.end())
	{
		std::list<PlayerSource*>::iterator pSource = m_sources.begin();
		m_sources.erase(pSource);
		XN_DELETE(*pSource);
	}
}

OniStatus PlayerDevice::getSensorInfoList(OniSensorInfo** pSources, int* numSources)
{
	xnl::AutoCSLocker lock(m_cs);

	// Update source count.
	*numSources = (int)m_sources.size();
	*pSources = XN_NEW_ARR(OniSensorInfo, *numSources);

	// Copy sources.
	std::list<PlayerSource*>::iterator iter = m_sources.begin();
	for (int i = 0; i < *numSources; ++i, ++iter)
	{
		xnOSMemCopy(&(*pSources)[i],  (*iter)->GetInfo(), sizeof(OniSensorInfo));
	}

    return ONI_STATUS_OK;
}

driver::StreamBase* PlayerDevice::createStream(OniSensorType sensorType)
{
	// Find the requested source.
	PlayerSource* pSource = NULL;

	{
		xnl::AutoCSLocker lock(m_cs);
		for (std::list<PlayerSource*>::iterator iter = m_sources.begin(); iter != m_sources.end(); ++iter)
		{
			if ((*iter)->GetInfo()->sensorType == sensorType)
			{
				pSource = (*iter);
				break;
			}
		}
	}

	// Check if source was found.
	if (pSource == NULL)
	{
		return NULL;
	}

	// Create a new stream using the source.
	PlayerStream* pStream = XN_NEW(PlayerStream, this, pSource);
	if (pStream == NULL)
	{
		return NULL;
	}

	// Initialize the stream.
	OniStatus rc = pStream->Initialize();
	if (rc != ONI_STATUS_OK)
	{
		XN_DELETE(pStream);
		return NULL;
	}

	xnl::AutoCSLocker lock(m_cs);
	m_streams.push_back(pStream);

	// Register to ready for data event.
	// NOTE: handle is discarded, as device will always exist longer than stream, therefore device can never unregister.
	OniCallbackHandle handle;
	rc = pStream->RegisterReadyForDataEvent(ReadyForDataCallback, this, handle);
	if (rc != ONI_STATUS_OK)
	{
		m_streams.pop_back();
		XN_DELETE(pStream);
		return NULL;
	}

	// Register to stream destroy event.
	// NOTE: handle is discarded, as device will always exist longer than stream, therefore device can never unregister.
	rc = pStream->RegisterDestroyEvent(StreamDestroyCallback, this, handle);
	if (rc != ONI_STATUS_OK)
	{
		m_streams.pop_back();
		XN_DELETE(pStream);
		return NULL;
	}

	return pStream;
}

void PlayerDevice::destroyStream(oni::driver::StreamBase* pStream)
{
	xnl::AutoCSLocker lock(m_cs);
	std::list<PlayerStream*>::iterator it = std::find(m_streams.begin(), m_streams.end(), (PlayerStream*)pStream);
	if (it != m_streams.end())
	{
		m_streams.erase(it);
	}
	XN_DELETE(pStream);
}

OniStatus PlayerDevice::tryManualTrigger()
{
	// Set and reset the trigger.
	XnStatus rc = m_manualTriggerInternalEvent.Set();
	if (rc != XN_STATUS_OK)
	{
		return ONI_STATUS_ERROR;
	}

	return ONI_STATUS_OK;
}

/// Get property.
OniStatus PlayerDevice::getProperty(int propertyId, void* data, int* pDataSize)
{
	OniStatus rc = ONI_STATUS_OK;

	// Check if property requires special handling.
	if (propertyId == ONI_DEVICE_PROPERTY_PLAYBACK_SPEED)
	{
		// Verify data size.
		if (*pDataSize != sizeof(float))
		{
			return ONI_STATUS_BAD_PARAMETER;
		}

		// Return the playback speed.
		*((float*)data) = (float)m_dPlaybackSpeed;
	}
	else if (propertyId == ONI_DEVICE_PROPERTY_PLAYBACK_REPEAT_ENABLED)
	{
		// Validate parameter size.
		if (*pDataSize != sizeof(bool))
		{
			return ONI_STATUS_BAD_PARAMETER;
		}

		// Return the repeat value.
		*((bool*)data) = m_bRepeat;
	}
	else
	{
		// Get the property.
		xnl::AutoCSLocker lock(m_cs);
		rc = m_properties.GetProperty(propertyId, data, pDataSize);
	}

	return rc;
}

/// Set property.
OniStatus PlayerDevice::setProperty(int propertyId, const void* data, int dataSize)
{
	OniStatus rc = ONI_STATUS_OK;

	// Check if property requires special handling.
	if (propertyId == ONI_DEVICE_PROPERTY_PLAYBACK_SPEED)
	{
		// Validate parameter size.
		if (dataSize != sizeof(float))
		{
			return ONI_STATUS_BAD_PARAMETER;
		}

		// Update the playback speed.
		m_dPlaybackSpeed = (double)*((float*)data);

		// Reset the timing reference.
		m_bHasTimeReference = false;
	}
	else if (propertyId == ONI_DEVICE_PROPERTY_PLAYBACK_REPEAT_ENABLED)
	{
		// Validate parameter size.
		if (dataSize != sizeof(bool))
		{
			return ONI_STATUS_BAD_PARAMETER;
		}

		// Update the repeat.
		m_bRepeat = *((bool*)data);
		m_player.SetRepeat(m_bRepeat);
	}
	else
	{
		// Set the property.
		xnl::AutoCSLocker lock(m_cs);
		rc = m_properties.SetProperty(propertyId, data, dataSize);
	}

	return rc;
}

bool PlayerDevice::isPropertySupported(int propertyId)
{
	return propertyId == ONI_DEVICE_PROPERTY_PLAYBACK_SPEED ||
			propertyId == ONI_DEVICE_PROPERTY_PLAYBACK_REPEAT_ENABLED ||
			m_properties.Exists(propertyId);
}

/// @copydoc OniDeviceBase::Invoke(int, const void*, int)
OniStatus PlayerDevice::invoke(int commandId, void* data, int dataSize)
{
	if (commandId == ONI_DEVICE_COMMAND_SEEK)
	{
		if (m_player.IsEOF())
		{
			return ONI_STATUS_ERROR;
		}

		// Verify data size.
		if (dataSize != sizeof(Seek))
		{
			return ONI_STATUS_BAD_PARAMETER;
		}

		// Seek the frame ID for all sources.
		Seek* pSeek = (Seek*)data;
		m_seek.frameId = pSeek->frameId;
		m_seek.pStream = pSeek->pStream;
		m_isSeeking = true;
        m_seekingFailed = false;

		// Set the ready for data and manual trigger events, to make sure player thread wakes up.
		m_readyForDataInternalEvent.Set();
		m_manualTriggerInternalEvent.Set();

		// Wait for seek to complete.
		m_SeekCompleteInternalEvent.Wait(XN_WAIT_INFINITE);

        if (m_seekingFailed)
            return ONI_STATUS_ERROR;
	}
	else
	{
		return ONI_STATUS_NOT_IMPLEMENTED;
	}

	return ONI_STATUS_OK;
}

bool PlayerDevice::isCommandSupported(int commandId)
{
	return commandId == ONI_DEVICE_COMMAND_SEEK;
}

PlayerSource* PlayerDevice::FindSource(const XnChar* strNodeName)
{
	xnl::AutoCSLocker lock(m_cs);

	// Find the relevant source.
	for (std::list<PlayerSource*>::iterator iter = m_sources.begin(); iter != m_sources.end(); ++iter)
	{
		if (strcmp((*iter)->GetNodeName(), strNodeName) == 0)
		{
			PlayerSource* pSource = *iter;
			return pSource;
		}
	}

	return NULL;
}

void PlayerDevice::SleepToTimestamp(uint64_t nTimeStamp)
{
	uint64_t nNow;
	xnOSGetHighResTimeStamp(&nNow);

	bool bHasTimeReference = true;
	{
		xnl::AutoCSLocker lock(m_cs);
		if (!m_bHasTimeReference /*&& (nTimeStamp <= m_nStartTimestamp)*/)
		{
			m_nStartTimestamp = nTimeStamp;
			m_nStartTime = nNow;

			m_bHasTimeReference = true;
			bHasTimeReference = false;
		}
	}

	if (bHasTimeReference && (m_dPlaybackSpeed > 0.0f))
	{
		// check this data timestamp compared to when we started
		int64_t nTimestampDiff = nTimeStamp - m_nStartTimestamp;

		// in some recordings, frames are not ordered by timestamp. Make sure this does not break the mechanism
		if (nTimestampDiff > 0)
		{
			int64_t nTimeDiff = nNow - m_nStartTime;

			// check if we need to wait some time
			int64_t nRequestedTimeDiff = (int64_t)(nTimestampDiff / m_dPlaybackSpeed);
			if (nTimeDiff < nRequestedTimeDiff)
			{
				uint32_t nSleep = uint32_t((nRequestedTimeDiff - nTimeDiff)/1000);
				nSleep = XN_MIN(nSleep, XN_PLAYBACK_SPEED_SANITY_SLEEP);
				xnOSSleep(nSleep);
			}

			// update reference to current frame (this will handle cases in which application
			// stopped reading frames and continued after a while)
			m_nStartTimestamp = nTimeStamp;
			xnOSGetHighResTimeStamp(&m_nStartTime);
		}
	}
}

void PlayerDevice::MainLoop()
{
	m_running = true;
	while (m_running)
	{
		// Process data only when at least one of the streams within has started
		bool waitForStreamStart = true;
		for (std::list<PlayerStream*>::iterator iter = m_streams.begin(); iter != m_streams.end(); iter++)
		{
			PlayerStream* pStream = *iter;
			if (pStream->isStreamStarted())
			{
				waitForStreamStart = false;
				break;
			}
		}
		if (waitForStreamStart)
		{
			xnOSSleep(10);
			continue;
		}

		if (m_isSeeking)
		{
			// Set playback speed to maximum.
			double playbackSpeed = m_dPlaybackSpeed;
			m_dPlaybackSpeed = XN_PLAYBACK_SPEED_FASTEST;

			// Seek the frame ID for first source (seek to (frame ID-1) so next read frame is frameId).
			PlayerSource* pSource = m_seek.pStream->GetSource();

			XnStatus xnrc;
			if(pSource) {
				xnrc = m_player.SeekToFrame(pSource->GetNodeName(), m_seek.frameId, XN_PLAYER_SEEK_SET);
			}else{
				xnrc = XN_STATUS_ERROR;
			}

			if (xnrc != XN_STATUS_OK)
			{
				// Failure to seek.
				m_seekingFailed = true;
			}

			// Return playback speed to normal.
			m_dPlaybackSpeed = playbackSpeed;

			// Reset the wait events.
			m_readyForDataInternalEvent.Reset();
			m_manualTriggerInternalEvent.Reset();

			// Reset the time reference.
			m_bHasTimeReference = false;

			// Mark the seeking flag as false.
			m_isSeeking = false;

			// Raise the seek complete event.
			m_SeekCompleteInternalEvent.Set();
		}
		else
		{
			// Read the next frame (delay between frames must be dealt with in the OnNodeNewData callback).
			m_player.ReadNext();
		}
	}
}

XN_THREAD_PROC PlayerDevice::ThreadProc(XN_THREAD_PARAM pThreadParam)
{
	PlayerDevice* pThis = reinterpret_cast<PlayerDevice*>(pThreadParam);
	pThis->MainLoop();

	XN_THREAD_PROC_RETURN(XN_STATUS_OK);
}

void ONI_CALLBACK_TYPE PlayerDevice::ReadyForDataCallback(const PlayerStream::ReadyForDataEventArgs& /*newDataEventArgs*/, void* pCookie)
{
	PlayerDevice* pThis = (PlayerDevice*)(pCookie);
	pThis->m_readyForDataInternalEvent.Set();
}

void ONI_CALLBACK_TYPE PlayerDevice::StreamDestroyCallback(const PlayerStream::DestroyEventArgs& destroyEventArgs, void* pCookie)
{
	PlayerDevice* pThis = (PlayerDevice*)(pCookie);
	xnl::AutoCSLocker lock(pThis->m_cs);
	std::list<PlayerStream*>::iterator it = std::find(pThis->m_streams.begin(), pThis->m_streams.end(), destroyEventArgs.pStream);
	if (it != pThis->m_streams.end())
	{
		pThis->m_streams.erase(it);
	}
}

XnStatus XN_CALLBACK_TYPE PlayerDevice::OnNodeAdded(void* pCookie, const XnChar* strNodeName, XnProductionNodeType type, XnCodecID /*compression*/, uint32_t nNumberOfFrames)
{
	PlayerDevice* pThis = (PlayerDevice*)pCookie;

	// Check node type.
	switch (type)
	{
		case XN_NODE_TYPE_DEVICE:
		{
			// Store the node name.
			pThis->m_nodeName = strNodeName;
			break;
		}
		case XN_NODE_TYPE_DEPTH:
		case XN_NODE_TYPE_IMAGE:
		case XN_NODE_TYPE_IR:
		{
			// Check if the source already exists.
			PlayerSource* pSource = pThis->FindSource(strNodeName);
			if (pSource == NULL)
			{
				// Create the new source.
				OniSensorType sensorType = (type == XN_NODE_TYPE_DEPTH) ? ONI_SENSOR_DEPTH :
					(type == XN_NODE_TYPE_IMAGE) ? ONI_SENSOR_COLOR : ONI_SENSOR_IR;
				pSource = XN_NEW(PlayerSource, strNodeName, sensorType);
				if (pSource == NULL)
				{
					return XN_STATUS_ERROR;
				}
				pSource->SetProperty(ONI_STREAM_PROPERTY_NUMBER_OF_FRAMES, &nNumberOfFrames, sizeof(int));

				// Add the source.
				xnl::AutoCSLocker lock(pThis->m_cs);
				pThis->m_sources.push_back(pSource);
			}

			break;
		}
		/*case XN_NODE_TYPE_AUDIO:
		{
			break;
		}*/
		default:
		{

		}
	}

	return XN_STATUS_OK;
}

XnStatus XN_CALLBACK_TYPE PlayerDevice::OnNodeRemoved(void* /*pCookie*/, const XnChar* /*strNodeName*/)
{
	// Do not remove the node (sensors can't disappear)
	return XN_STATUS_OK;
}

XnStatus XN_CALLBACK_TYPE PlayerDevice::OnNodeIntPropChanged(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, uint64_t nValue)
{
	PlayerDevice* pThis = (PlayerDevice*)pCookie;
	XnStatus nRetVal = XN_STATUS_OK;
	OniStatus rc;

	// Find the source.
	PlayerSource* pSource = pThis->FindSource(strNodeName);
	if (pSource != NULL)
	{
		// find the relevant property ID for the node.
		if (strcmp(strPropName, XN_PROP_DEVICE_MAX_DEPTH) == 0)
		{
			int oniValue = (int)nValue;
			rc = pSource->SetProperty(ONI_STREAM_PROPERTY_MAX_VALUE, &oniValue, sizeof(oniValue));
			if (rc != ONI_STATUS_OK)
			{
				nRetVal = XN_STATUS_ERROR;
			}
		}
		else if (strcmp(strPropName, XN_PROP_BYTES_PER_PIXEL) == 0)
		{
			// TODO: update stride.
			//ONI_STREAM_PROPERTY_STRIDE
		}
		else if (strcmp(strPropName, XN_PROP_MIRROR) == 0)
		{
			bool oniValue = (bool)nValue;
			rc = pSource->SetProperty(ONI_STREAM_PROPERTY_MIRRORING, &oniValue, sizeof(oniValue));
			if (rc != ONI_STATUS_OK)
			{
				nRetVal = XN_STATUS_ERROR;
			}
		}
		else if (strcmp(strPropName, XN_PROP_PIXEL_FORMAT) == 0)
		{
			OniVideoMode videoMode;

			// Get the previous property.
			int dataSize = sizeof(videoMode);
			pSource->GetProperty(ONI_STREAM_PROPERTY_VIDEO_MODE, &videoMode, &dataSize);

			// Update the video mode value and set the property.
			if (pSource->GetInfo()->sensorType == ONI_SENSOR_DEPTH)
			{
				videoMode.pixelFormat = ONI_PIXEL_FORMAT_DEPTH_1_MM;
			}
			else
			{
				switch (nValue)
				{
				case XN_PIXEL_FORMAT_RGB24:
					videoMode.pixelFormat = ONI_PIXEL_FORMAT_RGB888;
					break;
				case XN_PIXEL_FORMAT_YUV422:
					videoMode.pixelFormat = ONI_PIXEL_FORMAT_YUV422;
					break;
				case XN_PIXEL_FORMAT_GRAYSCALE_8_BIT:
					videoMode.pixelFormat = ONI_PIXEL_FORMAT_GRAY8;
					break;
				case XN_PIXEL_FORMAT_GRAYSCALE_16_BIT:
					videoMode.pixelFormat = ONI_PIXEL_FORMAT_GRAY16;
					break;
				case XN_PIXEL_FORMAT_MJPEG:
					videoMode.pixelFormat = ONI_PIXEL_FORMAT_JPEG;
					break;
				default:
					nRetVal = XN_STATUS_BAD_PARAM;
					break;
				}
			}

			if (nRetVal != XN_STATUS_BAD_PARAM)
			{
				rc = pSource->SetProperty(ONI_STREAM_PROPERTY_VIDEO_MODE, &videoMode, sizeof(videoMode));
				if (rc != ONI_STATUS_OK)
				{
					nRetVal = XN_STATUS_ERROR;
				}
			}
		}
		else if (strcmp(strPropName, XN_PROP_ONI_PIXEL_FORMAT) == 0)
		{
			// Get the previous property.
			OniVideoMode videoMode;
			int dataSize = sizeof(videoMode);
			pSource->GetProperty(ONI_STREAM_PROPERTY_VIDEO_MODE, &videoMode, &dataSize);
			videoMode.pixelFormat = (OniPixelFormat)nValue;
			rc = pSource->SetProperty(ONI_STREAM_PROPERTY_VIDEO_MODE, &videoMode, sizeof(videoMode));
			if (rc != ONI_STATUS_OK)
			{
				nRetVal = XN_STATUS_ERROR;
			}
		}
		else if (strcmp(strPropName, XN_PROP_ONI_REQUIRED_FRAME_SIZE) == 0 || strcmp(strPropName, "RequiredDataSize") == 0)
		{
			pSource->SetRequiredFrameSize((int)nValue);
		}
		else
		{
			nRetVal = pThis->AddPrivateProperty(pSource, strPropName, sizeof(nValue), &nValue);
		}
	}

	return nRetVal;
}

XnStatus XN_CALLBACK_TYPE PlayerDevice::OnNodeRealPropChanged(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, double dValue)
{
	PlayerDevice* pThis = (PlayerDevice*)pCookie;
	XnStatus nRetVal = XN_STATUS_OK;

	// Find the source.
	PlayerSource* pSource = pThis->FindSource(strNodeName);
	if (pSource != NULL)
	{
		nRetVal = pThis->AddPrivateProperty(pSource, strPropName, sizeof(dValue), &dValue);
	}

	return nRetVal;
}

XnStatus XN_CALLBACK_TYPE PlayerDevice::OnNodeStringPropChanged(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, const XnChar* strValue)
{
	PlayerDevice* pThis = (PlayerDevice*)pCookie;
	XnStatus nRetVal = XN_STATUS_OK;

	// Find the source.
	PlayerSource* pSource = pThis->FindSource(strNodeName);
	if (pSource != NULL)
	{
		nRetVal = pThis->AddPrivateProperty(pSource, strPropName, (uint32_t)strlen(strValue)+1, strValue);
	}

	return nRetVal;
}

XnStatus XN_CALLBACK_TYPE PlayerDevice::OnNodeGeneralPropChanged(void* pCookie, const XnChar* strNodeName, const XnChar* strPropName, uint32_t nBufferSize, const void* pBuffer)
{
	PlayerDevice* pThis = (PlayerDevice*)pCookie;
	XnStatus nRetVal = XN_STATUS_OK;
	OniStatus rc;

	// Find the source.
	PlayerSource* pSource = pThis->FindSource(strNodeName);
	if (pSource != NULL)
	{
		// find the relevant property ID for the node.
		if (strcmp(strPropName, XN_PROP_CROPPING) == 0)
		{
			if (nBufferSize != sizeof(XnCropping))
			{
				nRetVal = XN_STATUS_BAD_PARAM;
			}
			else
			{
				// Convert the XnCropping structure to OniCropping.
				XnCropping* pCropping = (XnCropping*)pBuffer;
				OniCropping cropping;
				cropping.enabled = pCropping->bEnabled;
				cropping.originX = pCropping->nXOffset;
				cropping.originY = pCropping->nYOffset;
				cropping.width = pCropping->nXSize;
				cropping.height = pCropping->nYSize;

				// Set the property.
				rc = pSource->SetProperty(ONI_STREAM_PROPERTY_CROPPING, &cropping, sizeof(cropping));
				if (rc != ONI_STATUS_OK)
				{
					nRetVal = XN_STATUS_ERROR;
				}
			}
		}
		else if (strcmp(strPropName, XN_PROP_MAP_OUTPUT_MODE) == 0)
		{
			if (nBufferSize != sizeof(XnMapOutputMode))
			{
				nRetVal = XN_STATUS_BAD_PARAM;
			}
			else
			{
				XnMapOutputMode* pMapOutputMode = (XnMapOutputMode*)pBuffer;
				OniVideoMode videoMode;

				// Get the previous property (to retain the existing format).
				int dataSize = sizeof(videoMode);
				//int bpp = 2;
				rc = pSource->GetProperty(ONI_STREAM_PROPERTY_VIDEO_MODE, &videoMode, &dataSize);
				if (rc != ONI_STATUS_OK)
				{
					// Set default values.
					// NOTE: those defaults better be overridden by an XN_PROP_PIXEL_FORMAT, or bugs may occur in the non-default cases!
					OniSensorType sensorType = pSource->GetInfo()->sensorType;
					switch (sensorType)
					{
						case ONI_SENSOR_DEPTH:
						{
							videoMode.pixelFormat = ONI_PIXEL_FORMAT_DEPTH_1_MM;
							//bpp = 2;
							break;
						}
						case ONI_SENSOR_COLOR:
						{
							videoMode.pixelFormat = ONI_PIXEL_FORMAT_RGB888;
							//bpp = 3;
							break;
						}
						case ONI_SENSOR_IR:
						{
							videoMode.pixelFormat = ONI_PIXEL_FORMAT_GRAY16;
							//bpp = 2;
							break;
						}
						default:
						{
							return XN_STATUS_BAD_PARAM;
						}
					}
				}

				// Convert the XnMapOutputMode structure to OniVideoMode.
				videoMode.resolutionX = pMapOutputMode->nXRes;
				videoMode.resolutionY = pMapOutputMode->nYRes;
				videoMode.fps = pMapOutputMode->nFPS;

				// Set the property.
				rc = pSource->SetProperty(ONI_STREAM_PROPERTY_VIDEO_MODE, &videoMode, sizeof(videoMode));
				if (rc != ONI_STATUS_OK)
				{
					nRetVal = XN_STATUS_ERROR;
				}
			}
		}
		else if (strcmp(strPropName, XN_PROP_FIELD_OF_VIEW) == 0)
		{
			XnFieldOfView* pFieldOfView = (XnFieldOfView*)pBuffer;

			// Set the HFOV.
			float fov = (float)pFieldOfView->fHFOV;
			rc = pSource->SetProperty(ONI_STREAM_PROPERTY_HORIZONTAL_FOV, &fov, sizeof(fov));
			if (rc != ONI_STATUS_OK)
			{
				nRetVal = XN_STATUS_ERROR;
			}
			else
			{
				// Set the VFOV.
				fov = (float)pFieldOfView->fVFOV;
				rc = pSource->SetProperty(ONI_STREAM_PROPERTY_VERTICAL_FOV, &fov, sizeof(fov));
				if (rc != ONI_STATUS_OK)
				{
					nRetVal = XN_STATUS_ERROR;
				}
			}
		}
		else if (strcmp(strPropName, XN_PROP_ORIGINAL_DEVICE) == 0)
		{
			xnOSStrCopy(pThis->m_originalDevice, (char*)pBuffer, sizeof(pThis->m_originalDevice));
		}
		else
		{
			nRetVal = pThis->AddPrivateProperty(pSource, strPropName, nBufferSize, pBuffer);
		}
	}

	return nRetVal;
}

XnStatus XN_CALLBACK_TYPE PlayerDevice::OnNodeStateReady(void* /*pCookie*/, const XnChar* /*strNodeName*/)
{
	// Ignore
	return XN_STATUS_OK;
}

XnStatus XN_CALLBACK_TYPE PlayerDevice::OnNodeNewData(void* pCookie, const XnChar* strNodeName, uint64_t nTimeStamp, uint32_t nFrame, const void* pData, uint32_t nSize)
{
	PlayerDevice* pThis = (PlayerDevice*)pCookie;

	// Ignore zero frame (may be returned in case of seek error).
	if ((nTimeStamp == 0) && (nFrame == 0))
	{
		return XN_STATUS_OK;
	}

	// Find the relevant source.
	PlayerSource* pSource = pThis->FindSource(strNodeName);
	if (pSource != NULL)
	{
		// Make sure streams are ready to receive the frame.
		bool ready = false;
		bool hasStreams = true;
		while (hasStreams && !ready && pThis->m_running)
		{
			// Check if any stream is ready to receive the frames.
			// NOTE: all the streams have a local 'last frame' buffer, so worst case other streams on source will buffer the frame.
			{
				xnl::AutoCSLocker lock(pThis->m_cs);
				hasStreams = false;
				for (std::list<PlayerStream*>::iterator iter = pThis->m_streams.begin(); iter != pThis->m_streams.end(); iter++)
				{
					PlayerStream* pStream = *iter;
					if (pStream->GetSource() == pSource)
					{
						hasStreams = true;
						ready = true;
						break;
					}
				}
			}

			// If no ready device found, wait for ready for data event.
			if (hasStreams)
			{
				if (ready)
				{
					// Check if waiting for manual trigger (playback speed is zero).
					if (pThis->m_dPlaybackSpeed == XN_PLAYBACK_SPEED_MANUAL)
					{
						// Wait for manual trigger.
						XnStatus rc = pThis->m_manualTriggerInternalEvent.Wait(DEVICE_MANUAL_TRIGGER_STANITY_SLEEP);
						if (rc == XN_STATUS_OK)
						{
							pThis->m_manualTriggerInternalEvent.Reset();
						}
						else
						{
							ready = false;
						}
					}
				}
				else
				{
					// Wait for streams to become ready.
					pThis->m_readyForDataInternalEvent.Wait(DEVICE_READY_FOR_DATA_EVENT_SANITY_SLEEP);
				}
			}
		}

		// Sleep until next timestamp has expired.
		pThis->SleepToTimestamp(nTimeStamp);

		// Continue processing in the source.
		void* data = const_cast<void*>(pData);
		pSource->ProcessNewData(nTimeStamp, nFrame, data, nSize);
	}

	return XN_STATUS_OK;
}

void XN_CALLBACK_TYPE PlayerDevice::OnEndOfFileReached(void* pCookie)
{
	// Reset time reference for all streams.
	PlayerDevice* pThis = (PlayerDevice*)pCookie;
	{
		xnl::AutoCSLocker lock(pThis->m_cs);
		pThis->m_bHasTimeReference = false;
	}

	// Notify the driver in case the player has finished playing (no-rewind)
	if (pThis->isPlayerEOF())
	{
		pThis->TriggerDriverEOFCallback();
	}
}

XnStatus PlayerDevice::AddPrivateProperty(PlayerSource* pSource, const XnChar* strPropName, uint32_t nBufferSize, const void* pBuffer)
{
	if (xnOSStrCmp(m_originalDevice, "PSLink") == 0)
	{
		return AddPrivateProperty_PSLink(pSource, strPropName, nBufferSize, pBuffer);
	}
	return AddPrivateProperty_PS1080(pSource, strPropName, nBufferSize, pBuffer);
}

XnStatus PlayerDevice::AddPrivateProperty_PSLink(PlayerSource* pSource, const XnChar* strPropName, uint32_t nBufferSize, const void* pBuffer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// Find the property name in the PSLink properties.
	int numProperties = ARRAYSIZE(PSLinkPropertyList);
	for (int i = 0; i < numProperties; ++i)
	{
		if (strcmp(strPropName, PSLinkPropertyList[i].propertyName) == 0)
		{
			OniStatus rc = pSource->SetProperty(PSLinkPropertyList[i].propertyId, pBuffer, nBufferSize);
			if (rc != ONI_STATUS_OK)
			{
				nRetVal = XN_STATUS_ERROR;
			}
			break;
		}
	}

	return nRetVal;
}
XnStatus PlayerDevice::AddPrivateProperty_PS1080(PlayerSource* pSource, const XnChar* strPropName, uint32_t nBufferSize, const void* pBuffer)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// Find the property name in the PS1080 properties.
	int numProperties = ARRAYSIZE(PS1080PropertyList);
	for (int i = 0; i < numProperties; ++i)
	{
		if (strcmp(strPropName, PS1080PropertyList[i].propertyName) == 0)
		{
			OniStatus rc = pSource->SetProperty(PS1080PropertyList[i].propertyId, pBuffer, nBufferSize);
			if (rc != ONI_STATUS_OK)
			{
				nRetVal = XN_STATUS_ERROR;
			}
			break;
		}
	}

	return nRetVal;
}

XnStatus XN_CALLBACK_TYPE PlayerDevice::FileOpen(void* pCookie)
{
	PlayerDevice* pThis = (PlayerDevice*)pCookie;
	return xnOSOpenFile(pThis->m_filePath.c_str(), XN_OS_FILE_READ, &pThis->m_fileHandle);
}

XnStatus XN_CALLBACK_TYPE PlayerDevice::FileRead(void* pCookie, void* pBuffer, uint32_t nSize, uint32_t* pnBytesRead)
{
	PlayerDevice* pThis = (PlayerDevice*)pCookie;
	uint32_t bufferSize = nSize;
	XnStatus rc = xnOSReadFile(pThis->m_fileHandle, pBuffer, &bufferSize);
	*pnBytesRead = bufferSize;
	return rc;
}

XnStatus XN_CALLBACK_TYPE PlayerDevice::FileSeek(void* pCookie, XnOSSeekType seekType, const int32_t nOffset)
{
	return PlayerDevice::FileSeek64(pCookie, seekType, nOffset);
}

uint32_t XN_CALLBACK_TYPE PlayerDevice::FileTell(void* pCookie)
{
	return (uint32_t)PlayerDevice::FileTell64(pCookie);
}

void XN_CALLBACK_TYPE PlayerDevice::FileClose(void* pCookie)
{
	PlayerDevice* pThis = (PlayerDevice*)pCookie;
	xnOSCloseFile(&pThis->m_fileHandle);
	pThis->m_fileHandle = 0;
}

XnStatus XN_CALLBACK_TYPE PlayerDevice::FileSeek64(void* pCookie, XnOSSeekType seekType, const int64_t nOffset)
{
	PlayerDevice* pThis = (PlayerDevice*)pCookie;
	return xnOSSeekFile64(pThis->m_fileHandle, seekType, nOffset);
}

uint64_t XN_CALLBACK_TYPE PlayerDevice::FileTell64(void* pCookie)
{
	PlayerDevice* pThis = (PlayerDevice*)pCookie;
	uint64_t pos = 0xffffffff;
	XnStatus rc = xnOSTellFile64(pThis->m_fileHandle, &pos);
	if (rc == XN_STATUS_OK)
	{
		return pos;
	}
	return 0xffffffff;
}

XnStatus XN_CALLBACK_TYPE PlayerDevice::CodecCreate(void* pCookie, const char* strNodeName, XnCodecID nCodecID, XnCodec** ppCodec)
{
	PlayerDevice* pThis = (PlayerDevice*)pCookie;

	// Find the relevant source.
	PlayerSource* pSource = pThis->FindSource(strNodeName);
	if (pSource == NULL)
	{
		return XN_STATUS_NO_MATCH;
	}

	// Create the matching codec.
	XnStatus rc = PlayerCodecFactory::Create(nCodecID, pSource, ppCodec);
	XN_IS_STATUS_OK(rc);

	return XN_STATUS_OK;
}

void XN_CALLBACK_TYPE PlayerDevice::CodecDestroy(void* /*pCookie*/, XnCodec* pCodec)
{
	// Destroy the codec.
	PlayerCodecFactory::Destroy(pCodec);
}

} // namespace oni_files_player
