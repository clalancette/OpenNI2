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
#ifndef PRIMECLIENT_H
#define PRIMECLIENT_H

#include <vector>

#include "XnLinkProtoLibDefs.h"
#include "XnLinkControlEndpoint.h"
#include "XnLinkInputDataEndpoint.h"
#include "XnLinkOutputDataEndpoint.h"
#include "XnLinkInputStreamsMgr.h"
#include "XnLinkOutputStreamsMgr.h"
#include "PrimeClientDefs.h"
#include "XnShiftToDepth.h"
#include <PSLink.h>
#include <XnPlatform.h>
#include <XnStatus.h>

namespace xn
{

class ISyncIOConnection;
class IOutputConnection;
class IConnectionFactory;

class PrimeClient :
	virtual public ILinkDataEndpointNotifications
{
public:
	PrimeClient();
	virtual ~PrimeClient();

	/* Initialization and shutdown */
	virtual XnStatus Init(const XnChar* strConnString, XnTransportType transportType);
	virtual void Shutdown();
	virtual bool IsInitialized() const;
	virtual XnStatus Connect();
	virtual bool IsConnected() const;
	virtual void Disconnect();

	/* Global Properties */
	virtual const XnDetailedVersion& GetFWVersion() const;
	virtual uint32_t GetHWVersion() const;
	virtual const XnLeanVersion& GetDeviceProtocolVersion() const;
	virtual const XnChar* GetSerialNumber() const;
	virtual XnStatus GetComponentsVersions(std::vector<XnComponentVersion>& componentVersions);
	const XnChar* GetConnectionString() const { return m_strConnectionString; }

	/* Global Device Commands */
	virtual	XnStatus GetBootStatus(XnBootStatus& bootStatus);
	virtual	XnStatus GetSupportedI2CDevices(std::vector<XnLinkI2CDevice>& supportedDevices);
	virtual XnStatus WriteI2C(uint8_t nDeviceID, uint8_t nAddressSize, uint32_t nAddress, uint8_t nValueSize, uint32_t nValue, uint32_t nMask);
	virtual XnStatus ReadI2C(uint8_t nDeviceID, uint8_t nAddressSize, uint32_t nAddress, uint8_t nValueSize, uint32_t& nValue);
	virtual XnStatus WriteAHB(uint32_t nAddress, uint32_t nValue, uint8_t nBitOffset, uint8_t nBitWidth);
	virtual XnStatus ReadAHB(uint32_t nAddress, uint8_t nBitOffset, uint8_t nBitWidth, uint32_t& nValue);
	virtual XnStatus SoftReset();
	virtual XnStatus HardReset();
	virtual XnStatus ReadDebugData(XnCommandDebugData& commandDebugData);
	virtual XnStatus SetProjectorActive(bool bActive);
	virtual XnStatus SetAccActive(bool bActive);
	virtual XnStatus GetAccActive(bool& bActive);
	virtual XnStatus SetVDDActive(bool bActive);
	virtual XnStatus GetVDDActive(bool& bActive);
	virtual XnStatus SetPeriodicBistActive(bool bActive);
	virtual XnStatus GetPeriodicBistActive(bool& bActive);
	virtual XnStatus StartFWLog();
	virtual XnStatus StopFWLog();
	virtual XnStatus OpenFWLogFile(uint8_t logID);
	virtual	XnStatus CloseFWLogFile(uint8_t logID);
	virtual XnStatus GetSupportedLogFiles(std::vector<XnLinkLogFile>& supportedFiles);

	virtual XnStatus RunPresetFile(const XnChar* strFileName);
	virtual XnStatus GetSupportedBistTests(std::vector<XnBistInfo>& supportedTests);
	virtual XnStatus GetSupportedTempList(std::vector<XnTempInfo>& supportedTempList);
	virtual XnStatus GetTemperature(XnCommandTemperatureResponse& temp);
	virtual XnStatus ExecuteBist(uint32_t nID, uint32_t& errorCode, uint32_t& extraDataSize, uint8_t* extraData);
	virtual XnStatus FormatZone(uint8_t nZone);
	//TODO: Implement Get emitter active

	/* Stream Management */
	virtual XnStatus EnumerateStreams(std::vector<XnFwStreamInfo>& aStreamInfos);
	virtual XnStatus EnumerateStreams(XnStreamType streamType, std::vector<XnFwStreamInfo>& aStreamInfos);
	virtual XnStatus CreateInputStream(XnStreamType nodeType, const XnChar* strCreationInfo, uint16_t& nStreamID);

	virtual XnStatus DestroyInputStream(uint16_t nStreamID);
	virtual LinkInputStream* GetInputStream(uint16_t nStreamID);
	virtual const LinkInputStream* GetInputStream(uint16_t nStreamID) const;

	virtual XnStatus InitOutputStream(uint16_t nStreamID,
		uint32_t nMaxMsgSize,
		uint16_t nMaxPacketSize,
		XnLinkCompressionType compression,
		XnStreamFragLevel streamFragLevel);

	virtual void ShutdownOutputStream(uint16_t nStreamID);

	virtual	XnStatus BeginUploadFileOnControlEP();
	virtual	XnStatus EndUploadFileOnControlEP();
	virtual XnStatus UploadFileOnControlEP(const XnChar* strFileName, bool bOverrideFactorySettings);
	virtual XnStatus GetFileList(std::vector<XnFwFileEntry>& files);
	virtual XnStatus DownloadFile(uint16_t zone, const XnChar* strFirmwareFileName, const XnChar* strTargetFile);

	virtual XnStatus EnableProjectorPulse(float delay, float width, float cycle);
	virtual XnStatus DisableProjectorPulse();
	virtual XnStatus GetProjectorPulse(bool& enabled, float& delay, float& width, float& framesToskip);
	virtual XnStatus SetProjectorPower(uint16_t power);
	virtual XnStatus GetProjectorPower(uint16_t& power);

	virtual void HandleLinkDataEndpointDisconnection(uint16_t nEndpointID);

protected:
	virtual XnStatus ConnectOutputDataEndpoint();
	virtual IConnectionFactory* CreateConnectionFactory(XnTransportType transportType) = 0;
	void LogVersions();
	XnStatus CreateInputStreamImpl(XnLinkStreamType streamType, const XnChar* strCreationInfo, uint16_t& nStreamID, uint16_t& nEndpointID);
	bool IsPropertySupported(uint16_t propID);
	LinkControlEndpoint m_linkControlEndpoint;
	LinkOutputDataEndpoint m_outputDataEndpoint;
	IConnectionFactory* m_pConnectionFactory;
	LinkInputStreamsMgr m_linkInputStreamsMgr;
	LinkOutputStreamsMgr m_linkOutputStreamsMgr;

private:
	static const uint32_t MAX_COMMAND_SIZE;
	static const uint32_t CONT_STREAM_PREDEFINED_BUFFER_SIZE;

	bool m_bInitialized;
	volatile bool m_bConnected;
	std::vector<LinkInputDataEndpoint> m_inputDataEndpoints;
	uint16_t m_nFWLogStreamID;
	XnChar m_strConnectionString[XN_FILE_MAX_PATH];

	std::vector<xnl::BitSet> m_supportedProps;
	XnDetailedVersion m_fwVersion;
	XnLeanVersion m_protocolVersion;
	uint32_t m_nHWVersion;
	XnChar m_strSerialNumber[XN_SERIAL_NUMBER_SIZE];
};

}

#endif // PRIMECLIENT_H
