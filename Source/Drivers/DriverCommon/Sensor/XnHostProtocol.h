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
#ifndef XNHOSTPROTOCOL_H
#define XNHOSTPROTOCOL_H

#include <XnStreamParams.h>
#include "XnParams.h"
#include "XnDeviceSensor.h"
#include "XnFirmwareTypes.h"

#define XN_HOST_MAGIC_25	0x5053	//PS
#define XN_FW_MAGIC_25		0x5350	//SP
#define XN_HOST_MAGIC_26	0x4d47	//MG
#define XN_FW_MAGIC_26		0x4252	//BR

#define XN_FPGA_VER_FPDB_26	0x21
#define XN_FPGA_VER_FPDB_25	0x0
#define XN_FPGA_VER_CDB 	0x1
#define XN_FPGA_VER_RD3 	0x2
#define XN_FPGA_VER_RD5 	0x3
#define XN_FPGA_VER_RD1081	0x4
#define XN_FPGA_VER_RD1082	0x5
#define XN_FPGA_VER_RD109	0x6

#define XN_CHIP_VER_PS1000	0x00101010
#define XN_CHIP_VER_PS1080	0x00202020
#define XN_CHIP_VER_PS1080A6	0x00212020

enum EPsProtocolOpCodes
{
	OPCODE_GET_VERSION = 0,
	OPCODE_KEEP_ALIVE = 1,
	OPCODE_GET_PARAM = 2,
	OPCODE_SET_PARAM = 3,
	OPCODE_GET_FIXED_PARAMS = 4,
	OPCODE_GET_MODE = 5,
	OPCODE_SET_MODE = 6,
	OPCODE_GET_LOG = 7,
	OPCODE_RESERVED_0 = 8,
	OPCODE_RESERVED_1 = 9,
	OPCODE_I2C_WRITE = 10,
	OPCODE_I2C_READ = 11,
	OPCODE_TAKE_SNAPSHOT = 12,
	OPCODE_INIT_FILE_UPLOAD = 13,
	OPCODE_WRITE_FILE_UPLOAD = 14,
	OPCODE_FINISH_FILE_UPLOAD = 15,
	OPCODE_DOWNLOAD_FILE = 16,
	OPCODE_DELETE_FILE = 17,
	OPCODE_GET_FLASH_MAP = 18,
	OPCODE_GET_FILE_LIST = 19,
	OPCODE_READ_AHB = 20,
	OPCODE_WRITE_AHB = 21,
	OPCODE_ALGORITM_PARAMS = 22,
	OPCODE_SET_FILE_ATTRIBUTES = 23,
	OPCODE_EXECUTE_FILE = 24,
	OPCODE_READ_FLASH = 25,
	OPCODE_SET_GMC_PARAMS = 26,
	OPCODE_GET_CPU_STATS = 27,
	OPCODE_BIST = 28,
	OPCODE_CALIBRATE_TEC = 29,
	OPCODE_GET_TEC_DATA = 30,
	OPCODE_CALIBRATE_EMITTER = 31,
	OPCODE_GET_EMITTER_DATA = 32,
	OPCODE_CALIBRATE_PROJECTOR_FAULT = 33,
	OPCODE_SET_CMOS_BLANKING = 34,
	OPCODE_GET_CMOS_BLANKING = 35,
	OPCODE_GET_CMOS_PRESETS = 36,
	OPCODE_GET_SERIAL_NUMBER = 37,
	OPCODE_GET_FAST_CONVERGENCE_TEC = 38,
	OPCODE_GET_PLATFORM_STRING = 39,
	OPCODE_GET_USB_CORE_TYPE = 40,
	OPCODE_SET_LED_STATE = 41,
	OPCODE_ENABLE_EMITTER = 42,
	OPCODE_KILL = 999,
};

enum EPsProtocolOpCodes_V300
{
	OPCODE_V300_BIST = 26,
};

enum XnHostProtocolOpcodes_V110
{
	OPCODE_V110_GET_VERSION = 0,
	OPCODE_V110_KEEP_ALIVE = 1,
	OPCODE_V110_GET_PARAM = 2,
	OPCODE_V110_SET_PARAM = 3,
	OPCODE_V110_GET_FIXED_PARAMS = 4,
	OPCODE_V110_GET_MODE = 5,
	OPCODE_V110_SET_MODE = 6,
	OPCODE_V110_GET_LOG = 7,
	OPCODE_V110_GET_CMOS_REGISTER = 8,
	OPCODE_V110_SET_CMOS_REGISTER = 9,
	OPCODE_V110_GET_CODEC_REGISTER = 10,
	OPCODE_V110_SET_CODEC_REGISTER = 11,
	OPCODE_V110_TAKE_SNAPSHOT = 12,
	OPCODE_V110_INIT_FILE_UPLOAD = 13,
	OPCODE_V110_WRITE_FILE_UPLOAD = 14,
	OPCODE_V110_FINISH_FILE_UPLOAD = 15,
	OPCODE_V110_DOWNLOAD_FILE = 16,
	OPCODE_V110_DELETE_FILE = 17,
	OPCODE_V110_GET_FLASH_MAP = 18,
	OPCODE_V110_GET_FILE_LIST = 19,
	OPCODE_V110_READ_AHB = 20,
	OPCODE_V110_WRITE_AHB = 21,
	OPCODE_V110_ALGORITHM_PARAMS = 22,
	OPCODE_V110_SET_FILE_ATTRIBUTES = 23,
	OPCODE_V110_EXECUTE_FILE = 24,
};

enum EPsProtocolOpCodes_V017
{
	OPCODE_V017_GET_VERSION = 0,
	OPCODE_V017_KEEP_ALIVE = 1,
	OPCODE_V017_GET_PARAM = 2,
	OPCODE_V017_SET_PARAM = 3,
	OPCODE_V017_GET_FIXED_PARAMS = 4,
	OPCODE_V017_RESET = 5,
	OPCODE_V017_GET_LOG = 6,
	OPCODE_V017_GET_CMOS_REGISTER = 7,
	OPCODE_V017_SET_CMOS_REGISTER = 8,
	OPCODE_V017_GET_CODEC_REGISTER = 9,
	OPCODE_V017_SET_CODEC_REGISTER = 10,
	OPCODE_V017_TAKE_SNAPSHOT = 11,
	OPCODE_V017_INIT_FILE_UPLOAD = 12,
	OPCODE_V017_WRITE_FILE_UPLOAD = 13,
	OPCODE_V017_FINISH_FILE_UPLOAD = 14,
	OPCODE_V017_DOWNLOAD_FILE = 15,
	OPCODE_V017_DELETE_FILE = 16,
	OPCODE_V017_GET_FLASH_MAP = 17,
	OPCODE_V017_GET_FILE_LIST = 18,
	OPCODE_V017_READ_AHB = 19,
	OPCODE_V017_WRITE_AHB = 20,
	OPCODE_V017_ALGORITM_PARAMS = 21,
};

#define OPCODE_INVALID 0xffff

typedef enum
{
	XN_HOST_PROTOCOL_ALGORITHM_DEPTH_INFO	= 0x00,
	XN_HOST_PROTOCOL_ALGORITHM_REGISTRATION = 0x02,
	XN_HOST_PROTOCOL_ALGORITHM_PADDING		= 0x03,
	XN_HOST_PROTOCOL_ALGORITHM_BLANKING		= 0x06,
	XN_HOST_PROTOCOL_ALGORITHM_DEVICE_INFO	= 0x07,
	XN_HOST_PROTOCOL_ALGORITHM_FREQUENCY	= 0x80
} XnHostProtocolAlgorithmType;

typedef enum
{
	XN_HOST_PROTOCOL_MODE_WEBCAM = 0,
	XN_HOST_PROTOCOL_MODE_PS,
	XN_HOST_PROTOCOL_MODE_MAINTENANCE,
	XN_HOST_PROTOCOL_MODE_SOFT_RESET,
	XN_HOST_PROTOCOL_MODE_REBOOT,
	XN_HOST_PROTOCOL_MODE_SUSPEND,
	XN_HOST_PROTOCOL_MODE_RESUME,
	XN_HOST_PROTOCOL_MODE_INIT,
	XN_HOST_PROTOCOL_MODE_SYSTEM_RESTORE,
	XN_HOST_PROTOCOL_MODE_WAIT_FOR_ENUM,
	XN_HOST_PROTOCOL_MODE_SAFE_MODE
} XnHostProtocolModeType;

enum XnHostProtocolNacks
{
	ACK = 0,
	NACK_UNKNOWN_ERROR = 1,
	NACK_INVALID_COMMAND = 2,
	NACK_BAD_PACKET_CRC = 3,
	NACK_BAD_PACKET_SIZE = 4,
	NACK_BAD_PARAMS = 5,
	NACK_I2C_TRANSACTION_FAILED = 6,
	NACK_FILE_NOT_FOUND = 7,
	NACK_FILE_CREATE_FAILURE = 8,
	NACK_FILE_WRITE_FAILURE = 9,
	NACK_FILE_DELETE_FAILURE = 10,
	NACK_FILE_READ_FAILURE = 11,
	NACK_BAD_COMMAND_SIZE = 12,
	NACK_NOT_READY = 13,
	NACK_OVERFLOW = 14,
	NACK_OVERLAY_NOT_LOADED = 15,
	NACK_FILE_SYSTEM_LOCKED = 16,
};

typedef enum
{
	A2D_SAMPLE_RATE_48KHZ,
	A2D_SAMPLE_RATE_44KHZ,
	A2D_SAMPLE_RATE_32KHZ,
	A2D_SAMPLE_RATE_24KHZ,
	A2D_SAMPLE_RATE_22KHZ,
	A2D_SAMPLE_RATE_16KHZ,
	A2D_SAMPLE_RATE_12KHZ,
	A2D_SAMPLE_RATE_11KHZ,
	A2D_SAMPLE_RATE_8KHZ,
	A2D_NUM_OF_SAMPLE_RATES
} EA2d_SampleRate;

typedef enum XnHostProtocolUsbCore
{
	XN_USB_CORE_JANGO = 0,
	XN_USB_CORE_GADGETFS = 1,
} XnHostProtocolUsbCore;

#pragma pack(push,1)
typedef struct
{
	uint16_t nMagic;
	uint16_t nSize;
	uint16_t nOpcode;
	uint16_t nId;
	uint16_t nCRC16;
} XnHostProtocolHeaderV25;

typedef struct
{
	uint16_t nMagic;
	uint16_t nSize;
	uint16_t nOpcode;
	uint16_t nId;
} XnHostProtocolHeaderV26;

typedef struct
{
	uint16_t nErrorCode;
} XnHostProtocolReplyHeader;

typedef XnDeviceSensorGMCPoint XnHostProtocolGMCPoint_1080;

typedef struct XnHostProtocolGMCPoint_1000
{
	XnDeviceSensorGMCPoint m_GMCPoint;
	uint16_t m_Dummy;
} XnHostProtocolGMCPoint_1000;

typedef struct XnHostProtocolGMCLastConfData
{
	int16_t nLast;
	uint16_t nRICCLast;
	float fRICC_IIR;
} XnHostProtocolGMCLastConfData;

typedef enum XnHostProtocolGMCMode
{
	GMC_NORMAL_MODE = 0,
	GMC_SCAN_MODE
} XnHostProtocolGMCMode;

typedef struct XnHostProtocolGMCLastPacketData
{
	uint16_t m_GMCMode;
	uint16_t m_CoveragePass;
	XnHostProtocolGMCLastConfData m_LastConfData;
	float m_A;
	float m_B;
	float m_C;
	int16_t m_N;
	uint16_t m_RICC;
	uint32_t m_StartB;
	uint32_t m_DeltaB;
	int16_t m_FlashStoredRefOffset;
} XnHostProtocolGMCLastPacketData;

typedef struct XnBestTecConf
{
	uint16_t nBestHopsCount; // Lowest hops count among all unstable points
	uint32_t nBestSetPoint;  // The TEC set point that gave m_BestHopsCount
	int32_t  nBestStep;	 // The TEC scan step that gave m_BestHopsCount
} XnBestTecConf;

typedef struct XnWavelengthCorrectionDebugPacket
{
	float fBLast;
	float fBCurrent;
	uint16_t nIsHop;
	uint32_t nCurrentSlidingWindow;
	uint16_t nCurrentHopsCount;
	uint16_t nIsTecCalibrated;
	uint32_t nWaitPeriod;
	uint16_t nIsWavelengthUnstable;
	XnBestTecConf BestConf;
	uint16_t nIsTotallyUnstable; //whole scan no stable point
	uint32_t nConfiguredTecSetPoint; // 0 if not configured
	int32_t nCurrentStep;
} XnWavelengthCorrectionDebugPacket;

#pragma pack(pop)

////////////////////////////////////// Exported h file should be only from here down
// Exported params

// All implemented protocol commands
// Init
XnStatus XnHostProtocolInitFWParams(XnDevicePrivateData* pDevicePrivateData, uint8_t nMajor, uint8_t nMinor, uint16_t nBuild, XnHostProtocolUsbCore usb, bool bGuessed);

XnStatus XnHostProtocolKeepAlive		(XnDevicePrivateData* pDevicePrivateData);
XnStatus XnHostProtocolGetVersion		(const XnDevicePrivateData* pDevicePrivateData, XnVersions& Version);
XnStatus XnHostProtocolAlgorithmParams	(XnDevicePrivateData* pDevicePrivateData,
										 XnHostProtocolAlgorithmType eAlgorithmType,
										 void* pAlgorithmInformation, uint16_t nAlgInfoSize, XnResolutions nResolution, uint16_t nFPS);
XnStatus XnHostProtocolSetImageResolution(XnDevicePrivateData* pDevicePrivateData, uint32_t nResolutionParamName, XnResolutions nRes);
XnStatus XnHostProtocolSetDepthResolution(XnDevicePrivateData* pDevicePrivateData, XnResolutions nRes);
XnStatus XnHostProtocolGetFixedParams(XnDevicePrivateData* pDevicePrivateData, XnFixedParams& FixedParams);

XnStatus XnHostProtocolSetAudioSampleRate(XnDevicePrivateData* pDevicePrivateData, XnSampleRate nSampleRate);
XnStatus XnHostProtocolGetAudioSampleRate(XnDevicePrivateData* pDevicePrivateData, XnSampleRate* pSampleRate);

XnStatus XnHostProtocolSetMode			(XnDevicePrivateData* pDevicePrivateData, uint16_t nMode);
XnStatus XnHostProtocolGetMode			(XnDevicePrivateData* pDevicePrivateData, uint16_t& nMode);

XnStatus XnHostProtocolSetParam 		(XnDevicePrivateData* pDevicePrivateData, uint16_t nParam, uint16_t nValue);
XnStatus XnHostProtocolSetMultipleParams(XnDevicePrivateData* pDevicePrivateData, uint16_t nNumOfParams, XnInnerParamData* anParams);
XnStatus XnHostProtocolReset(XnDevicePrivateData* pDevicePrivateData, uint16_t nResetType);

XnStatus XnHostProtocolGetParam 		(XnDevicePrivateData* pDevicePrivateData, uint16_t nParam, uint16_t& nValue);

XnStatus XnHostProtocolSetDepthAGCBin(XnDevicePrivateData* pDevicePrivateData, uint16_t nBin, uint16_t nMinShift, uint16_t nMaxShift);
XnStatus XnHostProtocolGetDepthAGCBin(XnDevicePrivateData* pDevicePrivateData, uint16_t nBin, uint16_t* pnMinShift, uint16_t* pnMaxShift);

XnStatus XnHostProtocolSetCmosBlanking	(XnDevicePrivateData* pDevicePrivateData, uint16_t nLines, XnCMOSType nCMOSID, uint16_t nNumberOfFrames);
XnStatus XnHostProtocolGetCmosBlanking	(XnDevicePrivateData* pDevicePrivateData, XnCMOSType nCMOSID, uint16_t* pnLines);

XnStatus XnHostProtocolGetCmosPresets	(XnDevicePrivateData* pDevicePrivateData, XnCMOSType nCMOSID, XnCmosPreset* aPresets, uint32_t& nCount);

XnStatus XnHostProtocolGetSerialNumber	(XnDevicePrivateData* pDevicePrivateData, char* cpSerialNumber);
XnStatus XnHostProtocolGetPlatformString(XnDevicePrivateData* pDevicePrivateData, char* cpPlatformString);

XnStatus XnHostProtocolGetCMOSRegister(XnDevicePrivateData* pDevicePrivateData, XnCMOSType nCMOS, uint16_t nAddress, uint16_t& nValue);
XnStatus XnHostProtocolSetCMOSRegister	(XnDevicePrivateData* pDevicePrivateData, XnCMOSType nCMOS, uint16_t nAddress, uint16_t nValue);
XnStatus XnHostProtocolGetCMOSRegisterI2C(XnDevicePrivateData* pDevicePrivateData, XnCMOSType nCMOS, uint16_t nAddress, uint16_t& nValue);
XnStatus XnHostProtocolSetCMOSRegisterI2C (XnDevicePrivateData* pDevicePrivateData, XnCMOSType nCMOS, uint16_t nAddress, uint16_t nValue);
XnStatus XnHostProtocolReadAHB			(XnDevicePrivateData* pDevicePrivateData, uint32_t nAddress, uint32_t &nValue);
XnStatus XnHostProtocolWriteAHB 		(XnDevicePrivateData* pDevicePrivateData, uint32_t nAddress, uint32_t nValue, uint32_t nMask);
XnStatus XnHostProtocolGetUsbCoreType	(XnDevicePrivateData* pDevicePrivateData, XnHostProtocolUsbCore& nValue);
XnStatus XnHostProtocolSetLedState	(XnDevicePrivateData* pDevicePrivateData, uint16_t nLedId, uint16_t nState);
XnStatus XnHostProtocolSetEmitterState	(XnDevicePrivateData* pDevicePrivateData, bool bActive);
XnStatus XnHostProtocolUpdateSupportedImageModes(XnDevicePrivateData* pDevicePrivateData);

// Commands.txt
XnStatus XnHostProtocolGetLog			(XnDevicePrivateData* pDevicePrivateData, char* csBuffer, uint32_t nBufferSize);
XnStatus XnHostProtocolFileUpload		(XnDevicePrivateData* pDevicePrivateData, uint32_t nOffset, const char* strFileName, uint16_t nAttributes);
XnStatus XnHostProtocolFileDownload		(XnDevicePrivateData* pDevicePrivateData, uint16_t nFileType, const char* strFileName);
XnStatus XnHostProtocolReadFlash		(XnDevicePrivateData* pDevicePrivateData, uint32_t nOffset, uint32_t nSize, unsigned char* pBuffer);
XnStatus XnHostProtocolRunBIST			(XnDevicePrivateData* pDevicePrivateData, uint32_t nTestsMask, uint32_t* pnFailures);
XnStatus XnHostProtocolGetCPUStats		(XnDevicePrivateData* pDevicePrivateData, XnTaskCPUInfo* pTasks, uint32_t *pnTimesCount);
XnStatus XnHostProtocolCalibrateTec		(XnDevicePrivateData* pDevicePrivateData, uint16_t nSetPoint);
XnStatus XnHostProtocolGetTecData		(XnDevicePrivateData* pDevicePrivateData, XnTecData* pTecData);
XnStatus XnHostProtocolGetTecFastConvergenceData (XnDevicePrivateData* pDevicePrivateData, XnTecFastConvergenceData* pTecData);
XnStatus XnHostProtocolCalibrateEmitter (XnDevicePrivateData* pDevicePrivateData, uint16_t nSetPoint);
XnStatus XnHostProtocolGetEmitterData	(XnDevicePrivateData* pDevicePrivateData, XnEmitterData* pEmitterData);
XnStatus XnHostProtocolCalibrateProjectorFault	(XnDevicePrivateData* pDevicePrivateData, uint16_t nMinThreshold, uint16_t nMaxThreshold, bool* pbProjectorFaultEvent);
XnStatus XnHostProtocolGetFileList(XnDevicePrivateData* pDevicePrivateData, uint16_t nFirstFileId, XnFlashFile* pFileList, uint16_t& nNumOfEntries);
XnStatus XnHostProtocolDeleteFile(XnDevicePrivateData* pDevicePrivateData, uint16_t nFileId);
XnStatus XnHostProtocolWriteI2C(XnDevicePrivateData* pDevicePrivateData, const XnI2CWriteData* pI2CWriteData);
XnStatus XnHostProtocolReadI2C(XnDevicePrivateData* pDevicePrivateData, XnI2CReadData* pI2CReadData);
XnStatus XnHostProtocolSetFileAttributes(XnDevicePrivateData* pDevicePrivateData, uint16_t nFileId, uint16_t nAttributes);

#endif // XNHOSTPROTOCOL_H
