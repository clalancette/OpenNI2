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
#ifndef XNSENSOR_H
#define XNSENSOR_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <vector>

#include <DDK/XnDeviceBase.h>
#include "XnDeviceSensorIO.h"
#include "XnParams.h"
#include "XnDeviceSensor.h"
#include "XnSensorFixedParams.h"
#include "XnSensorFirmwareParams.h"
#include <DDK/XnDeviceStream.h>
#include "XnSensorFirmware.h"
#include "XnCmosInfo.h"
#include "IXnSensorStream.h"
#include <DDK/XnIntPropertySynchronizer.h>

//---------------------------------------------------------------------------
// XnSensor class
//---------------------------------------------------------------------------
class XnSensor : public XnDeviceBase
{
	friend class XnServerSensorInvoker;

public:
	XnSensor(bool bResetOnStartup = true, bool bLeanInit = false);
	~XnSensor();

	virtual XnStatus InitImpl(const XnDeviceConfig* pDeviceConfig);
	virtual XnStatus Destroy();
	virtual XnStatus OpenAllStreams();
	virtual XnStatus LoadConfigFromFile(const char* csINIFilePath, const char* csSectionName);

public:
	inline XnSensorFixedParams* GetFixedParams() { return GetFirmware()->GetFixedParams(); }
	inline XnSensorFirmware* GetFirmware() { return &m_Firmware; }
	inline XnSensorFPS* GetFPSCalculator() { return &m_FPS; }

	XnStatus SetCmosConfiguration(XnCMOSType nCmos, XnResolutions nRes, uint32_t nFPS);

	inline XnDevicePrivateData* GetDevicePrivateData() { return &m_DevicePrivateData; }

	XnStatus ConfigPropertyFromFile(XnStringProperty* pProperty, const char* csINIFilePath, const char* csSectionName);
	XnStatus ConfigPropertyFromFile(XnIntProperty* pProperty, const char* csINIFilePath, const char* csSectionName);

	inline bool IsMiscSupported() const { return m_SensorIO.IsMiscEndpointSupported(); }
	inline bool IsLowBandwidth() const { return m_SensorIO.IsLowBandwidth(); }
	inline XnSensorUsbInterface GetCurrentUsbInterface() const { return m_SensorIO.GetCurrentInterface(*m_Firmware.GetInfo()); }

	XnStatus GetStream(const char* strStream, XnDeviceStream** ppStream);

	inline XnStatus GetErrorState() { return (XnStatus)m_ErrorState.GetValue(); }
	XnStatus SetErrorState(XnStatus errorState);

	/**
	 * Resolves the config file's path.
	 * Specify NULL to strConfigDir to resolve it based on the driver's directory.
	 */
	static XnStatus ResolveGlobalConfigFileName(char* strConfigFile, uint32_t nBufSize, const char* strConfigDir);

	XnStatus SetGlobalConfigFile(const char* strConfigFile);
	XnStatus ConfigureModuleFromGlobalFile(const char* strModule, const char* strSection = NULL);

	const char* GetUSBPath() { return m_SensorIO.GetDevicePath(); }
	bool ShouldUseHostTimestamps() { return (m_HostTimestamps.GetValue() == true); }
	bool HasReadingStarted() { return (m_ReadData.GetValue() == true); }
	inline bool IsTecDebugPring() const { return (bool)m_FirmwareTecDebugPrint.GetValue(); }

	XnStatus SetFrameSyncStreamGroup(XnDeviceStream** ppStreamList, uint32_t numStreams);

protected:
	virtual XnStatus CreateStreamImpl(const char* strType, const char* strName, const XnActualPropertiesHash* pInitialSet);

	XnStatus CreateDeviceModule(XnDeviceModuleHolder** ppModuleHolder);
	XnStatus CreateStreamModule(const char* StreamType, const char* StreamName, XnDeviceModuleHolder** ppStream);
	void DestroyStreamModule(XnDeviceModuleHolder* pStreamHolder);

	virtual void OnNewStreamData(XnDeviceStream* pStream, OniFrame* pFrame);

private:
	XnStatus InitSensor(const XnDeviceConfig* pDeviceConfig);
	XnStatus ValidateSensorID(char* csSensorID);
	XnStatus SetMirrorForModule(XnDeviceModule* pModule, uint64_t nValue);
	XnStatus FindSensorStream(const char* StreamName, IXnSensorStream** ppStream);
	XnStatus InitReading();
	XnStatus OnFrameSyncPropertyChanged();

	static XnStatus XN_CALLBACK_TYPE GetInstanceCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);

	XnStatus ChangeTaskInterval(XnScheduledTask** ppTask, XnTaskCallbackFuncPtr pCallback, uint32_t nInterval);
	void ReadFirmwareLog();
	void ReadFirmwareCPU();

	//---------------------------------------------------------------------------
	// Getters
	//---------------------------------------------------------------------------
	XnStatus GetFirmwareParam(XnInnerParamData* pParam);
	XnStatus GetCmosBlankingUnits(XnCmosBlankingUnits* pBlanking);
	XnStatus GetCmosBlankingTime(XnCmosBlankingTime* pBlanking);
	XnStatus GetFirmwareMode(XnParamCurrentMode* pnMode);
	XnStatus GetLastRawFrame(const char* strStream, XnUChar* pBuffer, uint32_t nDataSize);
	XnStatus GetFixedParams(XnDynamicSizeBuffer* pBuffer);
	XnStatus GetDepthCmosRegister(XnControlProcessingData* pRegister);
	XnStatus GetImageCmosRegister(XnControlProcessingData* pRegister);
	XnStatus ReadAHB(XnAHBData* pData);
	XnStatus GetI2C(XnI2CReadData* pI2CReadData);
	XnStatus GetTecStatus(XnTecData* pTecData);
	XnStatus GetTecFastConvergenceStatus(XnTecFastConvergenceData* pTecData);
	XnStatus GetEmitterStatus(XnEmitterData* pEmitterData);
	XnStatus ReadFlashFile(const XnParamFileData* pFile);
	XnStatus ReadFlashChunk(XnParamFlashData* pFlash);
	XnStatus GetFirmwareLog(char* csLog, uint32_t nSize);
	XnStatus GetFileList(XnFlashFileList* pFileList);

	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	XnStatus SetInterface(XnSensorUsbInterface nInterface);
	XnStatus SetHostTimestamps(bool bHostTimestamps);
	XnStatus SetNumberOfBuffers(uint32_t nCount);
	XnStatus SetReadData(bool bRead);
	XnStatus SetFirmwareParam(const XnInnerParamData* pParam);
	XnStatus SetCmosBlankingUnits(const XnCmosBlankingUnits* pBlanking);
	XnStatus SetCmosBlankingTime(const XnCmosBlankingTime* pBlanking);
	XnStatus Reset(XnParamResetType nType);
	XnStatus SetFirmwareMode(XnParamCurrentMode nMode);
	XnStatus SetDepthCmosRegister(const XnControlProcessingData* pRegister);
	XnStatus SetImageCmosRegister(const XnControlProcessingData* pRegister);
	XnStatus WriteAHB(const XnAHBData* pData);
	XnStatus SetLedState(uint16_t nLedId, uint16_t nState);
	XnStatus SetEmitterState(bool bActive);
	XnStatus SetFirmwareFrameSync(bool bOn);
	XnStatus SetI2C(const XnI2CWriteData* pI2CWriteData);
	XnStatus SetFirmwareLogFilter(uint32_t nFilter);
	XnStatus SetFirmwareLogInterval(uint32_t nMilliSeconds);
	XnStatus SetFirmwareLogPrint(bool bPrint);
	XnStatus SetFirmwareCPUInterval(uint32_t nMilliSeconds);
	XnStatus SetAPCEnabled(bool bEnabled);
	XnStatus DeleteFile(uint16_t nFileID);
	XnStatus SetTecSetPoint(uint16_t nSetPoint);
	XnStatus SetEmitterSetPoint(uint16_t nSetPoint);
	XnStatus SetFileAttributes(const XnFileAttributes* pAttributes);
	XnStatus WriteFlashFile(const XnParamFileData* pFile);
	XnStatus SetProjectorFault(XnProjectorFaultData* pProjectorFaultData);
	XnStatus RunBIST(uint32_t nTestsMask, uint32_t* pnFailures);
	XnStatus SetReadAllEndpoints(bool bEnabled);

	//---------------------------------------------------------------------------
	// Callbacks
	//---------------------------------------------------------------------------
	static void XN_CALLBACK_TYPE OnDeviceDisconnected(const OniDeviceInfo& deviceInfo, void* pCookie);

	static XnStatus XN_CALLBACK_TYPE SetInterfaceCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetHostTimestampsCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetNumberOfBuffersCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetReadDataCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetFirmwareParamCallback(XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetCmosBlankingUnitsCallback(XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetCmosBlankingTimeCallback(XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE ResetCallback(XnIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetFirmwareModeCallback(XnIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetFixedParamsCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE FrameSyncPropertyChangedCallback(const XnProperty* pSender, void* pCookie);
	static bool XN_CALLBACK_TYPE HasSynchedFrameArrived(void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetFirmwareParamCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetCmosBlankingUnitsCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetCmosBlankingTimeCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetFirmwareModeCallback(const XnIntProperty* pSender, uint64_t* pnValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetAudioSupportedCallback(const XnIntProperty* pSender, uint64_t* pnValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetImageSupportedCallback(const XnIntProperty* pSender, uint64_t* pnValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetDepthCmosRegisterCallback(XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetImageCmosRegisterCallback(XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetDepthCmosRegisterCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetImageCmosRegisterCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE ReadAHBCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE WriteAHBCallback(XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetLedStateCallback(XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetEmitterStateCallback(XnIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetFirmwareFrameSyncCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetFirmwareLogFilterCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetFirmwareLogIntervalCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetFirmwareLogPrintCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetFirmwareCPUIntervalCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetReadAllEndpointsCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetAPCEnabledCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetI2CCallback(XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE DeleteFileCallback(XnIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetTecSetPointCallback(XnIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetEmitterSetPointCallback(XnIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetFileAttributesCallback(XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE WriteFlashFileCallback(XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetProjectorFaultCallback(XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE RunBISTCallback(XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetFileListCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static void XN_CALLBACK_TYPE ExecuteFirmwareLogTask(void* pCookie);
	static void XN_CALLBACK_TYPE ExecuteFirmwareCPUTask(void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetI2CCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetTecStatusCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetTecFastConvergenceStatusCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetEmitterStatusCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE ReadFlashFileCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetFirmwareLogCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE ReadFlashChunkCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);

	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnCallbackHandle m_hDisconnectedCallback;
	XnActualIntProperty m_ErrorState;
	XnActualIntProperty m_ResetSensorOnStartup;
	XnActualIntProperty m_LeanInit;
	XnActualIntProperty m_Interface;
	XnActualIntProperty m_ReadData;
	XnActualIntProperty m_FrameSync;
	XnActualIntProperty m_FirmwareFrameSync;
	XnActualIntProperty m_CloseStreamsOnShutdown;
	XnActualIntProperty m_HostTimestamps;
	XnGeneralProperty m_FirmwareParam;
	XnGeneralProperty m_CmosBlankingUnits;
	XnGeneralProperty m_CmosBlankingTime;
	XnIntProperty m_Reset;
	XnVersions m_VersionData;
	XnDevicePrivateData m_DevicePrivateData;
	XnActualGeneralProperty m_Version;
	XnGeneralProperty m_FixedParam;
	XnActualStringProperty m_ID;
	XnActualStringProperty m_DeviceName;
	XnActualStringProperty m_VendorSpecificData;
	XnActualStringProperty m_PlatformString;
	XnIntProperty m_AudioSupported;
	XnIntProperty m_ImageSupported;
	XnGeneralProperty m_ImageControl;
	XnGeneralProperty m_DepthControl;
	XnGeneralProperty m_AHB;
	XnGeneralProperty m_LedState;
	XnIntProperty m_EmitterEnabled;
	XnActualIntProperty m_FirmwareLogFilter;
	XnActualIntProperty m_FirmwareLogInterval;
	XnActualIntProperty m_FirmwareLogPrint;
	XnActualIntProperty m_FirmwareCPUInterval;
	XnActualIntProperty m_APCEnabled;
	XnActualIntProperty m_FirmwareTecDebugPrint;
	XnActualIntProperty m_ReadAllEndpoints;
	XnGeneralProperty m_I2C;
	XnIntProperty m_DeleteFile;
	XnIntProperty m_TecSetPoint;
	XnGeneralProperty m_TecStatus;
	XnGeneralProperty m_TecFastConvergenceStatus;
	XnIntProperty m_EmitterSetPoint;
	XnGeneralProperty m_EmitterStatus;
	XnGeneralProperty m_FileAttributes;
	XnGeneralProperty m_FlashFile;
	XnGeneralProperty m_FirmwareLog;
	XnGeneralProperty m_FlashChunk;
	XnGeneralProperty m_FileList;
	XnGeneralProperty m_BIST;
	XnGeneralProperty m_ProjectorFault;
	XnSensorFirmware m_Firmware;
	XnSensorFPS m_FPS;
	XnCmosInfo m_CmosInfo;
	XnSensorIO m_SensorIO;

	XnSensorObjects m_Objects;

	/** A scheduler to be used for performing periodic tasks. */
	XnScheduler* m_pScheduler;
	XnScheduledTask* m_pLogTask;
	XnScheduledTask* m_pCPUTask;
	XnDumpFile* m_FirmwareLogDump;
	XnDumpFile* m_FrameSyncDump;
	bool m_nFrameSyncEnabled;
	typedef struct
	{
		XnDeviceStream* pStream;
		OniFrame* pFrame;
	} FrameSyncedStream;
	std::vector<FrameSyncedStream> m_FrameSyncedStreams;
	int m_nFrameSyncLastFrameID;
	xnl::CriticalSection m_frameSyncCs;

	bool m_bInitialized;

	XnIntPropertySynchronizer m_PropSynchronizer;

	char m_strGlobalConfigFile[XN_FILE_MAX_PATH];
};

#endif // XNSENSOR_H
