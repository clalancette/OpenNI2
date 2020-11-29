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
#ifndef XNFIRMWAREINFO_H
#define XNFIRMWAREINFO_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <vector>

#include <XnStreamParams.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnFirmwareInfo final
{
public:
	XnFWVer nFWVer;
	uint16_t nHostMagic;
	uint16_t nFWMagic;
	uint16_t nProtocolHeaderSize;
	uint16_t nProtocolMaxPacketSize;

	XnParamCurrentMode nCurrMode;

	bool bAudioSupported;
	bool bGetPresetsSupported;
	bool bDeviceInfoSupported;
	bool bImageAdjustmentsSupported;

	uint16_t nOpcodeGetVersion;
	uint16_t nOpcodeKeepAlive;
	uint16_t nOpcodeGetParam;
	uint16_t nOpcodeSetParam;
	uint16_t nOpcodeGetFixedParams;
	uint16_t nOpcodeGetMode;
	uint16_t nOpcodeSetMode;
	uint16_t nOpcodeAlgorithmParams;
	uint16_t nOpcodeReset;
	uint16_t nOpcodeSetCmosBlanking;
	uint16_t nOpcodeGetCmosBlanking;
	uint16_t nOpcodeGetCmosPresets;
	uint16_t nOpcodeGetSerialNumber;
	uint16_t nOpcodeGetFastConvergenceTEC;
	uint16_t nOpcodeGetCMOSReg;
	uint16_t nOpcodeSetCMOSReg;
	uint16_t nOpcodeWriteI2C;
	uint16_t nOpcodeReadI2C;
	uint16_t nOpcodeReadAHB;
	uint16_t nOpcodeWriteAHB;
	uint16_t nOpcodeGetPlatformString;
	uint16_t nOpcodeGetUsbCore;
	uint16_t nOpcodeSetLedState;
	uint16_t nOpcodeEnableEmitter;

	uint16_t nOpcodeGetLog;
	uint16_t nOpcodeTakeSnapshot;
	uint16_t nOpcodeInitFileUpload;
	uint16_t nOpcodeWriteFileUpload;
	uint16_t nOpcodeFinishFileUpload;
	uint16_t nOpcodeDownloadFile;
	uint16_t nOpcodeDeleteFile;
	uint16_t nOpcodeGetFlashMap;
	uint16_t nOpcodeGetFileList;
	uint16_t nOpcodeSetFileAttribute;
	uint16_t nOpcodeExecuteFile;
	uint16_t nOpcodeReadFlash;
	uint16_t nOpcodeBIST;
	uint16_t nOpcodeSetGMCParams;
	uint16_t nOpcodeGetCPUStats;
	uint16_t nOpcodeCalibrateTec;
	uint16_t nOpcodeGetTecData;
	uint16_t nOpcodeCalibrateEmitter;
	uint16_t nOpcodeGetEmitterData;
	uint16_t nOpcodeCalibrateProjectorFault;

	uint16_t nLogStringType;
	uint16_t nLogOverflowType;

	bool bMirrorSupported;

	uint16_t nUSBDelayReceive;
	uint16_t nUSBDelayExecutePreSend;
	uint16_t nUSBDelayExecutePostSend;
	uint16_t nUSBDelaySoftReset;
	uint16_t nUSBDelaySetParamFlicker;
	uint16_t nUSBDelaySetParamStream0Mode;
	uint16_t nUSBDelaySetParamStream1Mode;
	uint16_t nUSBDelaySetParamStream2Mode;

	uint8_t nISOAlternativeInterface;
	uint8_t nBulkAlternativeInterface;
	uint8_t nISOLowDepthAlternativeInterface;

	bool bGetImageCmosTypeSupported;
	bool bImageSupported;
	bool bIncreasedFpsCropSupported;
	bool bHasFilesystemLock;

	std::vector<XnCmosPreset> depthModes;
	std::vector<XnCmosPreset> _imageBulkModes;
	std::vector<XnCmosPreset> _imageIsoModes;
	std::vector<XnCmosPreset> imageModes;
	std::vector<XnCmosPreset> irModes;
};

#endif // XNFIRMWAREINFO_H
