/*****************************************************************************
*									     *
*  OpenNI 2.x Alpha							     *
*  Copyright (C) 2012 PrimeSense Ltd.					     *
*									     *
*  This file is part of OpenNI. 					     *
*									     *
*  Licensed under the Apache License, Version 2.0 (the "License");	     *
*  you may not use this file except in compliance with the License.	     *
*  You may obtain a copy of the License at				     *
*									     *
*      http://www.apache.org/licenses/LICENSE-2.0			     *
*									     *
*  Unless required by applicable law or agreed to in writing, software	     *
*  distributed under the License is distributed on an "AS IS" BASIS,	     *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and	     *
*  limitations under the License.					     *
*									     *
*****************************************************************************/
#ifndef XNDEVICESENSOR_H
#define XNDEVICESENSOR_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnDevice.h>
#include <XnDDK.h>
#include "XnDeviceSensorIO.h"
#include <XnFPSCalculator.h>
#include <XnLog.h>
#include <XnScheduler.h>
#include <Core/XnBuffer.h>
#include <DDK/XnFrameBufferManager.h>
#include "XnSensorFPS.h"
#include "XnFirmwareInfo.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_DEVICE_SENSOR_THREAD_KILL_TIMEOUT 5000

#define XN_DEVICE_SENSOR_DEPTH_CMOS_XRES 1280
#define XN_DEVICE_SENSOR_DEPTH_CMOS_YRES 1024

#define XN_DEVICE_SENSOR_VGA_DEPTH_XRES 640
#define XN_DEVICE_SENSOR_VGA_DEPTH_YRES 480

#define XN_DEVICE_SENSOR_MIN_DEPTH 0
#define XN_DEVICE_SENSOR_MAX_DEPTH_1_MM 10000
#define XN_DEVICE_SENSOR_MAX_DEPTH_100_UM 65534
#define XN_DEVICE_SENSOR_NO_DEPTH_VALUE 0
#define XN_DEVICE_SENSOR_MAX_SHIFT_VALUE 2048/*336*/

#define XN_DEVICE_SENSOR_MAX_IR 1023

#define XN_DEVICE_SENSOR_BOARDID_SEP ":"
#define XN_DEVICE_SENSOR_DEFAULT_ID "*"

#define XN_DEVICE_SENSOR_INI_FILE_EXT ".ini"

#define XN_SENSOR_PROTOCOL_SENSOR_ID_LENGTH	16

#define XN_SENSOR_TIMESTAMP_SANITY_DIFF 10 // in ms

#define XN_MASK_DEVICE_SENSOR			"DeviceSensor"
#define XN_MASK_DEVICE_IO				"DeviceIO"
#define XN_MASK_SENSOR_PROTOCOL 		"DeviceSensorProtocol"
#define XN_MASK_SENSOR_PROTOCOL_IMAGE	XN_MASK_SENSOR_PROTOCOL "Image"
#define XN_MASK_SENSOR_PROTOCOL_DEPTH	XN_MASK_SENSOR_PROTOCOL "Depth"
#define XN_MASK_SENSOR_PROTOCOL_AUDIO	XN_MASK_SENSOR_PROTOCOL "Audio"
#define XN_MASK_SENSOR_READ				"DeviceSensorRead"
#define XN_MASK_SENSOR_READ_IMAGE		XN_MASK_SENSOR_READ "Image"
#define XN_MASK_SENSOR_READ_DEPTH		XN_MASK_SENSOR_READ "Depth"
#define XN_MASK_SENSOR_READ_AUDIO		XN_MASK_SENSOR_READ "Audio"
#define XN_DUMP_AUDIO_IN				"AudioIn"
#define XN_DUMP_IMAGE_IN				"ImageIn"
#define XN_DUMP_DEPTH_IN				"DepthIn"
#define XN_DUMP_MINI_PACKETS			"MiniPackets"
#define XN_DUMP_TIMESTAMPS				"SensorTimestamps"
#define XN_DUMP_BANDWIDTH				"SensorBandwidth"
#define XN_DUMP_BAD_IMAGE				"BadImage"
#define XN_DUMP_FRAME_SYNC				"FrameSync"
#define XN_DUMP_SENSOR_LOG				"SensorLog"

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
class XnSensorFirmware;
struct XnDevicePrivateData;
class XnSensorFixedParams;
class XnSensorFPS;
class XnCmosInfo;

//---------------------------------------------------------------------------
// Structures & Enums
//---------------------------------------------------------------------------

typedef struct XnSensorObjects
{
	XnSensorObjects(XnSensorFirmware* pFirmware, XnDevicePrivateData* pDevicePrivateData, XnSensorFPS* pFPS, XnCmosInfo* pCmosInfo) :
		pFirmware(pFirmware),
		pDevicePrivateData(pDevicePrivateData),
		pFPS(pFPS),
		pCmosInfo(pCmosInfo)
	{}

	XnSensorFirmware* pFirmware;
	XnDevicePrivateData* pDevicePrivateData;
	XnSensorFPS* pFPS;
	XnCmosInfo* pCmosInfo;
} XnSensorObjects;

typedef struct XnHWInfo
{
	XnHWVer   nHWVer;
} XnHWInfo;

typedef struct XnChipInfo
{
	XnChipVer	nChipVer;
} XnChipInfo;

typedef enum {
	RGBREG_NONE = 0,
	RGBREG_FIX_IMAGE = 1,
	RGBREG_FIX_DEPTH = 2
} XnDeviceSensorRGBRegType;

typedef struct
{
	XN_THREAD_HANDLE	hThread;
	XnBool				bKillThread;
	XnBool				bThreadAlive;
} XnDeviceSensorThreadContext;

typedef struct XnRegistrationFunctionCoefficients
{
	double dA;
	double dB;
	double dC;
	double dD;
	double dE;
	double dF;
} XnRegistrationFunctionCoefficients;

typedef struct
{
	/* Is this the first time timestamp is calculated. */
	XnBool bFirst;
	/* The device TS which we use as reference for calculation. */
	uint32_t nReferenceTS;
	/* The time corresponding to the TS in nReferenceTS. */
	uint64_t nTotalTicksAtReferenceTS;
	/* The last device TS received. */
	uint32_t nLastDeviceTS;
	/* The last result time calculated. */
	uint64_t nLastResultTime;
	/* Stream name - for debug purposes. */
	const XnChar* csStreamName;
} XnTimeStampData;

typedef struct XnDeviceSensorGMCPoint
{
	uint16_t m_X;
	uint16_t m_Y;
	uint16_t m_DX;
	int16_t m_DY;
	uint16_t m_Score;
} XnDeviceSensorGMCPoint;

typedef struct XnCmosBlankingCoefficients
{
	float fA;
	float fB;
} XnCmosBlankingCoefficients;

typedef struct XnCmosBlankingInformation
{
	XnCmosBlankingCoefficients Coefficients[2];
} XnCmosBlankingInformation;

typedef struct XnDeviceInformation
{
	XnChar strDeviceName[128];
	XnChar strVendorData[128];
} XnDeviceInformation;

typedef XnStatus (XN_CALLBACK_TYPE* NewAudioDataCallback)(void* pCookie);

struct XnSpecificUsbDevice; // Forward Declaration
class XnSensor; // Forward Declaration

typedef struct XnDeviceAudioBuffer
{
	XN_CRITICAL_SECTION_HANDLE hLock;
	/** A single (big) buffer for audio. */
	XnUInt8* pAudioBuffer;
	/** An array of pointers into the audio buffer. */
	uint64_t* pAudioPacketsTimestamps;
	/** The index of the next packet that should be written. */
	volatile uint32_t nAudioWriteIndex;
	/** The index of the next packet that can be read. */
	volatile uint32_t nAudioReadIndex;
	/** Size of the audio buffer, in packets. */
	uint32_t nAudioBufferNumOfPackets;
	/** Size of the audio buffer, in bytes. */
	uint32_t nAudioBufferSize;
	uint32_t nAudioPacketSize;
	/** A callback for new data */
	NewAudioDataCallback pAudioCallback;
	void* pAudioCallbackCookie;
} XnDeviceAudioBuffer;

typedef struct XnDevicePrivateData
{
	XnVersions Version;

	XN_SENSOR_HANDLE	SensorHandle;
	XnFirmwareInfo		FWInfo;
	XnHWInfo			HWInfo;
	XnChipInfo			ChipInfo;

	XnSpecificUsbDevice* pSpecificDepthUsb;
	XnSpecificUsbDevice* pSpecificImageUsb;
	XnSpecificUsbDevice* pSpecificMiscUsb;

	float fDeviceFrequency;

	/** Keeps the global reference TS (the one marking time-zero). */
	uint32_t nGlobalReferenceTS;
	/** Keeps the OS time of global reference TS. */
	uint64_t nGlobalReferenceOSTime;

	/** A general critical section used to synch end-points threads. */
	XN_CRITICAL_SECTION_HANDLE hEndPointsCS;

	/** Used to dump timestamps data. */
	XnDumpFile* TimestampsDump;
	/** Used to dump bandwidth data. */
	XnDumpFile* BandwidthDump;
	/** Used to dump MiniPackets data. */
	XnDumpFile* MiniPacketsDump;

	XnSensor* pSensor;

	XN_MUTEX_HANDLE hExecuteMutex;

	XnDeviceSensorThreadContext		LogThread;
	/** GMC Mode. */
	uint32_t nGMCMode;
	XnBool bWavelengthCorrectionEnabled;
	XnBool bWavelengthCorrectionDebugEnabled;

} XnDevicePrivateData;

#pragma pack (push, 1)

typedef struct XnFixedParams
{
	// Misc
	int32_t nSerialNumber;
	int32_t nWatchDogTimeout;

	// Flash
	int32_t nFlashType;
	int32_t nFlashSize;
	int32_t nFlashBurstEnable;
	int32_t nFmifReadBurstCycles;
	int32_t nFmifReadAccessCycles;
	int32_t nFmifReadRecoverCycles;
	int32_t nFmifWriteAccessCycles;
	int32_t nFmifWriteRecoverCycles;
	int32_t nFmifWriteAssertionCycles;

	// Audio
	int32_t nI2SLogicClockPolarity;

	// Depth
	int32_t nDepthCiuHorizontalSyncPolarity;
	int32_t nDepthCiuVerticalSyncPolarity;
	int32_t nDepthCmosType;
	int32_t nDepthCmosI2CAddress;
	int32_t nDepthCmosI2CBus;

	// Image
	int32_t nImageCiuHorizontalSyncPolarity;
	int32_t nImageCiuVerticalSyncPolarity;
	int32_t nImageCmosType;
	int32_t nImageCmosI2CAddress;
	int32_t nImageCmosI2CBus;

	// Geometry
	int32_t nIrCmosCloseToProjector;
	float fDCmosEmitterDistance;
	float fDCmosRCmosDistance;
	float fReferenceDistance;
	float fReferencePixelSize;

	// Clocks
	int32_t nPllValue;
	int32_t nSystemClockDivider;
	int32_t nRCmosClockDivider;
	int32_t nDCmosClockDivider;
	int32_t nAdcClocDivider;
	int32_t nI2CStandardSpeedHCount;
	int32_t nI2CStandardSpeedLCount;

	int32_t nI2CHoldFixDelay;

	int32_t nSensorType;
	int32_t nDebugMode;
	int32_t nUseExtPhy;
	int32_t bProjectorProtectionEnabled;
	int32_t nProjectorDACOutputVoltage;
	int32_t nProjectorDACOutputVoltage2;
	int32_t nTecEmitterDelay;
} XnFixedParams;

typedef struct XnFixedParamsV26
{
	// Misc
	int32_t nSerialNumber;
	int32_t nWatchDogTimeout;

	// Flash
	int32_t nFlashType;
	int32_t nFlashSize;
	int32_t nFlashBurstEnable;
	int32_t nFmifReadBurstCycles;
	int32_t nFmifReadAccessCycles;
	int32_t nFmifReadRecoverCycles;
	int32_t nFmifWriteAccessCycles;
	int32_t nFmifWriteRecoverCycles;
	int32_t nFmifWriteAssertionCycles;

	// Audio
	int32_t nI2SLogicClockPolarity;

	// Depth
	int32_t nDepthCiuHorizontalSyncPolarity;
	int32_t nDepthCiuVerticalSyncPolarity;
	int32_t nDepthCmosType;
	int32_t nDepthCmosI2CAddress;
	int32_t nDepthCmosI2CBus;

	// Image
	int32_t nImageCiuHorizontalSyncPolarity;
	int32_t nImageCiuVerticalSyncPolarity;
	int32_t nImageCmosType;
	int32_t nImageCmosI2CAddress;
	int32_t nImageCmosI2CBus;

	// Geometry
	int32_t nIrCmosCloseToProjector;
	float fDCmosEmitterDistance;
	float fDCmosRCmosDistance;
	float fReferenceDistance;
	float fReferencePixelSize;

	// Clocks
	int32_t nPllValue;
	int32_t nSystemClockDivider;
	int32_t nRCmosClockDivider;
	int32_t nDCmosClockDivider;
	int32_t nAdcClocDivider;
	int32_t nI2CStandardSpeedHCount;
	int32_t nI2CStandardSpeedLCount;

	int32_t nI2CHoldFixDelay;

	int32_t nSensorType;
	int32_t nDebugMode;
	int32_t nTecEmitterDelay;
	int32_t nUseExtPhy;
} XnFixedParamsV26;

typedef struct XnFixedParamsV20
{
	// Misc
	int32_t nSerialNumber;
	int32_t nWatchDogTimeout;

	// Flash
	int32_t nFlashType;
	int32_t nFlashSize;
	int32_t nFlashBurstEnable;
	int32_t nFmifReadBurstCycles;
	int32_t nFmifReadAccessCycles;
	int32_t nFmifReadRecoverCycles;
	int32_t nFmifWriteAccessCycles;
	int32_t nFmifWriteRecoverCycles;
	int32_t nFmifWriteAssertionCycles;

	// Audio
	int32_t nI2SLogicClockPolarity;

	// Depth
	int32_t nDepthCiuHorizontalSyncPolarity;
	int32_t nDepthCiuVerticalSyncPolarity;
	int32_t nDepthCmosType;
	int32_t nDepthCmosI2CAddress;
	int32_t nDepthCmosI2CBus;

	// Image
	int32_t nImageCiuHorizontalSyncPolarity;
	int32_t nImageCiuVerticalSyncPolarity;
	int32_t nImageCmosType;
	int32_t nImageCmosI2CAddress;
	int32_t nImageCmosI2CBus;

	// Geometry
	int32_t nIrCmosCloseToProjector;
	float fDCmosEmitterDistance;
	float fDCmosRCmosDistance;
	float fReferenceDistance;
	float fReferencePixelSize;

	// Clocks
	int32_t nPllValue;
	int32_t nSystemClockDivider;
	int32_t nRCmosClockDivider;
	int32_t nDCmosClockDivider;
	int32_t nAdcClocDivider;
	int32_t nI2CStandardSpeedHCount;
	int32_t nI2CStandardSpeedLCount;

	int32_t nI2CHoldFixDelay;

	int32_t nSensorType;
	int32_t nDebugMode;
	int32_t nTecEmitterDelay;
} XnFixedParamsV20;

typedef struct XnRegistrationInformation1000
{
	XnRegistrationFunctionCoefficients FuncX;
	XnRegistrationFunctionCoefficients FuncY;
	double dBeta;
} XnRegistrationInformation1000;

typedef struct XnRegistrationInformation1080
{
	int32_t nRGS_DX_CENTER;
	int32_t nRGS_AX;
	int32_t nRGS_BX;
	int32_t nRGS_CX;
	int32_t nRGS_DX;
	int32_t nRGS_DX_START;
	int32_t nRGS_AY;
	int32_t nRGS_BY;
	int32_t nRGS_CY;
	int32_t nRGS_DY;
	int32_t nRGS_DY_START;
	int32_t nRGS_DX_BETA_START;
	int32_t nRGS_DY_BETA_START;
	int32_t nRGS_ROLLOUT_BLANK;
	int32_t nRGS_ROLLOUT_SIZE;
	int32_t nRGS_DX_BETA_INC;
	int32_t nRGS_DY_BETA_INC;
	int32_t nRGS_DXDX_START;
	int32_t nRGS_DXDY_START;
	int32_t nRGS_DYDX_START;
	int32_t nRGS_DYDY_START;
	int32_t nRGS_DXDXDX_START;
	int32_t nRGS_DYDXDX_START;
	int32_t nRGS_DXDXDY_START;
	int32_t nRGS_DYDXDY_START;
	int32_t nBACK_COMP1;
	int32_t nRGS_DYDYDX_START;
	int32_t nBACK_COMP2;
	int32_t nRGS_DYDYDY_START;
} XnRegistrationInformation1080;

typedef struct XnRegistrationPaddingInformation
{
	uint16_t nStartLines;
	uint16_t nEndLines;
	uint16_t nCroppingLines;
} XnRegistrationPaddingInformation;

typedef struct XnDepthInformation
{
	uint16_t nConstShift;
} XnDepthInformation;

typedef struct XnFrequencyInformation
{
	float fDeviceFrequency;
} XnFrequencyInformation;

typedef struct XnAudioSharedBuffer
{
	uint32_t nPacketCount;
	uint32_t nPacketSize;
	uint32_t nWritePacketIndex;
} XnAudioSharedBuffer;

#pragma pack (pop)

XnStatus XnDeviceSensorSetParam(XnDevicePrivateData* pDevicePrivateData, const XnChar* cpParamName, const int32_t nValue);

#endif // XNDEVICESENSOR_H
