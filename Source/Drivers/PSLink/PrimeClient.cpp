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
#include <vector>

#include "PrimeClient.h"
#include "XnLinkInputStreamsMgr.h"
#include <PSLink.h>
#include "IConnectionFactory.h"
#include "XnPsVersion.h"

#include <XnLog.h>
#include "XnOSCpp.h"

#define XN_MASK_PRIME_CLIENT "PrimeClient"

namespace xn
{

const uint32_t PrimeClient::MAX_COMMAND_SIZE = 0x40000;
const uint32_t PrimeClient::CONT_STREAM_PREDEFINED_BUFFER_SIZE = 0x40000;

PrimeClient::PrimeClient()
{
	m_pConnectionFactory = NULL;
	m_bInitialized = false;
	m_bConnected = false;

	/* Global properties */
	xnOSMemSet(&m_fwVersion, 0, sizeof(m_fwVersion));
	xnOSMemSet(&m_protocolVersion, 0, sizeof(m_protocolVersion));
	m_nHWVersion = 0;
	xnOSMemSet(&m_strSerialNumber, 0, sizeof(m_strSerialNumber));
	m_nFWLogStreamID = XN_LINK_STREAM_ID_NONE;
}

PrimeClient::~PrimeClient()
{
	Shutdown();
}

XnStatus PrimeClient::Init(const char* strConnString, XnTransportType transportType)
{
	XnStatus nRetVal = XN_STATUS_OK;
	m_pConnectionFactory = CreateConnectionFactory(transportType);
	XN_VALIDATE_ALLOC_PTR(m_pConnectionFactory);
	nRetVal = m_pConnectionFactory->Init(strConnString);
	XN_IS_STATUS_OK_LOG_ERROR("Init connection factory", nRetVal);
	xnOSStrCopy(m_strConnectionString, strConnString, sizeof(m_strConnectionString));

	nRetVal = m_linkInputStreamsMgr.Init();
	XN_IS_STATUS_OK_LOG_ERROR("Init link input streams mgr", nRetVal);
	nRetVal = m_linkOutputStreamsMgr.Init();
	XN_IS_STATUS_OK_LOG_ERROR("Init link output streams mgr", nRetVal);
	nRetVal = m_linkControlEndpoint.Init(MAX_COMMAND_SIZE, m_pConnectionFactory);
	XN_IS_STATUS_OK_LOG_ERROR("Init link control endpoint", nRetVal);

	m_inputDataEndpoints.resize(m_pConnectionFactory->GetNumInputDataConnections());

	m_bInitialized = true;
	return XN_STATUS_OK;
}

void PrimeClient::Shutdown()
{
	if (m_bInitialized)
	{
		for (uint32_t nEndpointID = 0; nEndpointID < m_inputDataEndpoints.size(); nEndpointID++)
		{
			m_inputDataEndpoints[nEndpointID].Shutdown();
		}
		m_outputDataEndpoint.Shutdown();

		//First shutdown stream managers before control endpoint, because they might need to send a StopStreaming command
		m_linkOutputStreamsMgr.Shutdown();
		m_linkInputStreamsMgr.Shutdown();
		m_linkControlEndpoint.Shutdown();
		xnOSSleep(200); //TODO: Get rid of this once we have a disconnection command
		m_pConnectionFactory->Shutdown();
		XN_DELETE(m_pConnectionFactory);
		m_pConnectionFactory = NULL;
		m_bInitialized = false;
	}
}

bool PrimeClient::IsInitialized() const
{
	return m_bInitialized;
}

XnStatus PrimeClient::Connect()
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (!m_bConnected)
	{
		nRetVal = m_linkControlEndpoint.Connect();
		XN_IS_STATUS_OK_LOG_ERROR("Connect link control endpoint", nRetVal);

		// Connect output data endpoint (if any)
		//TODO: Connect output data endpoint only on the first time we send anything to device.
		nRetVal = ConnectOutputDataEndpoint();
		XN_IS_STATUS_OK_LOG_ERROR("Connect output data endpoint", nRetVal);

		nRetVal = m_linkControlEndpoint.GetSupportedProperties(m_supportedProps);
		XN_IS_STATUS_OK_LOG_ERROR("Get supported properties", nRetVal);

		// Get some versions
		XnLinkDetailedVersion fwVersion;
		nRetVal = m_linkControlEndpoint.GetFWVersion(fwVersion);
		XN_IS_STATUS_OK_LOG_ERROR("Get FW version", nRetVal);
		m_fwVersion.m_nMajor = fwVersion.m_nMajor;
		m_fwVersion.m_nMinor = fwVersion.m_nMinor;
		m_fwVersion.m_nMaintenance = fwVersion.m_nMaintenance;
		m_fwVersion.m_nBuild = fwVersion.m_nBuild;
		xnOSStrCopy(m_fwVersion.m_strModifier, fwVersion.m_strModifier, sizeof(m_fwVersion.m_strModifier));

		nRetVal = m_linkControlEndpoint.GetProtocolVersion(m_protocolVersion);
		XN_IS_STATUS_OK_LOG_ERROR("Get protocol version", nRetVal);

		nRetVal = m_linkControlEndpoint.GetHardwareVersion(m_nHWVersion);
		XN_IS_STATUS_OK_LOG_ERROR("Get hardware version", nRetVal);

		nRetVal = m_linkControlEndpoint.GetSerialNumber(m_strSerialNumber, sizeof(m_strSerialNumber));
		XN_IS_STATUS_OK_LOG_ERROR("Get serial number", nRetVal);

		xnLogInfo(XN_MASK_PRIME_CLIENT, "Prime Client is now connected.");
		LogVersions();
		m_bConnected = true;
	}

	return XN_STATUS_OK;
}

void PrimeClient::Disconnect()
{
	for (uint16_t i = 0; i < m_inputDataEndpoints.size(); i++)
	{
		m_inputDataEndpoints[i].Disconnect();
	}
	m_linkControlEndpoint.Disconnect();
}


bool PrimeClient::IsConnected() const
{
	return m_bConnected;
}

const LinkInputStream* PrimeClient::GetInputStream(uint16_t nStreamID) const
{
	return m_linkInputStreamsMgr.GetInputStream(nStreamID);
}

LinkInputStream* PrimeClient::GetInputStream(uint16_t nStreamID)
{
	return m_linkInputStreamsMgr.GetInputStream(nStreamID);
}

const XnDetailedVersion& PrimeClient::GetFWVersion() const
{
	return m_fwVersion;
}

uint32_t PrimeClient::GetHWVersion() const
{
	return m_nHWVersion;
}

const XnLeanVersion& PrimeClient::GetDeviceProtocolVersion() const
{
	return m_protocolVersion;
}

const char* PrimeClient::GetSerialNumber() const
{
	return m_strSerialNumber;
}

XnStatus PrimeClient::GetComponentsVersions(std::vector<XnComponentVersion>& componentVersions)
{
	return m_linkControlEndpoint.GetComponentsVersions(componentVersions);
}

XnStatus PrimeClient::InitOutputStream(uint16_t nStreamID,
									   uint32_t nMaxMsgSize,
									   uint16_t nMaxPacketSize,
									   XnLinkCompressionType compression,
									   XnStreamFragLevel streamFragLevel)
{
	return m_linkOutputStreamsMgr.InitOutputStream(nStreamID, nMaxMsgSize, nMaxPacketSize, compression,
		streamFragLevel, &m_outputDataEndpoint);
}

void PrimeClient::ShutdownOutputStream(uint16_t nStreamID)
{
	m_linkOutputStreamsMgr.ShutdownOutputStream(nStreamID);
}

void PrimeClient::HandleLinkDataEndpointDisconnection(uint16_t /*nEndpointID*/)
{
}

XnStatus PrimeClient::SoftReset()
{
	return m_linkControlEndpoint.SoftReset();
}

XnStatus PrimeClient::HardReset()
{
	return m_linkControlEndpoint.HardReset();
}

XnStatus PrimeClient::ReadDebugData(XnCommandDebugData& commandDebugData)
{
	return m_linkControlEndpoint.ReadDebugData(commandDebugData);
}

XnStatus PrimeClient::WriteI2C(uint8_t nDeviceID, uint8_t nAddressSize, uint32_t nAddress, uint8_t nValueSize, uint32_t nValue, uint32_t nMask)
{
	return m_linkControlEndpoint.WriteI2C(nDeviceID, nAddressSize, nAddress, nValueSize, nValue, nMask);
}

XnStatus PrimeClient::ReadI2C(uint8_t nDeviceID, uint8_t nAddressSize, uint32_t nAddress, uint8_t nValueSize, uint32_t& nValue)
{
	return m_linkControlEndpoint.ReadI2C(nDeviceID, nAddressSize, nAddress, nValueSize, nValue);
}

XnStatus PrimeClient::WriteAHB(uint32_t nAddress, uint32_t nValue, uint8_t nBitOffset, uint8_t nBitWidth)
{
	return m_linkControlEndpoint.WriteAHB(nAddress, nValue, nBitOffset, nBitWidth);
}

XnStatus PrimeClient::ReadAHB(uint32_t nAddress, uint8_t nBitOffset, uint8_t nBitWidth, uint32_t& nValue)
{
	return m_linkControlEndpoint.ReadAHB(nAddress, nBitOffset, nBitWidth, nValue);
}

XnStatus PrimeClient::EnumerateStreams(std::vector<XnFwStreamInfo>& aStreamInfos)
{
	return m_linkControlEndpoint.EnumerateStreams(aStreamInfos);
}

XnStatus PrimeClient::EnumerateStreams(XnStreamType streamType, std::vector<XnFwStreamInfo>& aStreamInfos)
{
	XnStatus nRetVal = XN_STATUS_OK;

	std::vector<XnFwStreamInfo> aAll;
	nRetVal = m_linkControlEndpoint.EnumerateStreams(aAll);
	XN_IS_STATUS_OK(nRetVal);

	for (uint32_t i = 0; i < aAll.size(); ++i)
	{
		if ((uint32_t)aAll[i].type == streamType)
		{
			aStreamInfos.push_back(aAll[i]);
		}
	}

	return XN_STATUS_OK;
}

XnStatus PrimeClient::CreateInputStream(XnStreamType streamType, const char* strCreationInfo, uint16_t& nStreamID)
{
	if (!m_linkInputStreamsMgr.HasStreamOfType(streamType,strCreationInfo, nStreamID))
	{
		// No stream of this type exists. Create a new one
		XnStatus nRetVal = XN_STATUS_OK;
		uint16_t nEndpointID = 0;

		//Send create stream command
		nRetVal = CreateInputStreamImpl((XnLinkStreamType)streamType, strCreationInfo, nStreamID, nEndpointID);
		XN_IS_STATUS_OK_LOG_ERROR("Create stream", nRetVal);

		xnLogInfo(XN_MASK_LINK, "Created input stream %u of type '%s' on endpoint %u",
			nStreamID, xnLinkStreamTypeToString(streamType), nEndpointID);
	}

	// now let the stream manager know that we have another "holder" of the stream
	m_linkInputStreamsMgr.RegisterStreamOfType(streamType, strCreationInfo, nStreamID);

	return XN_STATUS_OK;
}

XnStatus PrimeClient::DestroyInputStream(uint16_t nStreamID)
{
	if (m_linkInputStreamsMgr.UnregisterStream(nStreamID))
	{
		// we were the last ones "holding" the stream
		XnStatus nRetVal = XN_STATUS_OK;
		nRetVal = m_linkControlEndpoint.DestroyInputStream(nStreamID);
		XN_IS_STATUS_OK_LOG_ERROR("Destroy stream", nRetVal);
		m_linkInputStreamsMgr.ShutdownInputStream(nStreamID);
		xnLogInfo(XN_MASK_PRIME_CLIENT, "Input stream %u destroyed.", nStreamID);
	}
	return XN_STATUS_OK;
}

XnStatus PrimeClient::BeginUploadFileOnControlEP()
{
	return m_linkControlEndpoint.BeginUpload();
}

XnStatus PrimeClient::EndUploadFileOnControlEP()
{
	return m_linkControlEndpoint.EndUpload();
}

XnStatus PrimeClient::UploadFileOnControlEP(const char* strFileName, bool bOverrideFactorySettings)
{
	return m_linkControlEndpoint.UploadFile(strFileName, bOverrideFactorySettings);
}

XnStatus PrimeClient::FormatZone(uint8_t nZone)
{
	return m_linkControlEndpoint.FormatZone(nZone);
}

XnStatus PrimeClient::ConnectOutputDataEndpoint()
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_outputDataEndpoint.IsInitialized())
	{
		nRetVal = m_outputDataEndpoint.Connect();
		XN_IS_STATUS_OK_LOG_ERROR("Connect output data endpoint", nRetVal);
	}

	return XN_STATUS_OK;
}

XnStatus PrimeClient::GetFileList(std::vector<XnFwFileEntry>& files)
{
	return m_linkControlEndpoint.GetFileList(files);
}

XnStatus PrimeClient::DownloadFile(uint16_t zone, const char* strFirmwareFileName, const char* strTargetFile)
{
	return m_linkControlEndpoint.DownloadFile(zone, strFirmwareFileName, strTargetFile);
}

XnStatus PrimeClient::SetProjectorActive(bool bActive)
{
	return m_linkControlEndpoint.SetProjectorActive(bActive);
}

XnStatus PrimeClient::SetAccActive(bool bActive)
{
	return m_linkControlEndpoint.SetAccActive(bActive);
}

XnStatus PrimeClient::GetAccActive(bool& bActive)
{
	return m_linkControlEndpoint.GetAccActive(bActive);
}

XnStatus PrimeClient::SetVDDActive(bool bActive)
{
	return m_linkControlEndpoint.SetVDDActive(bActive);
}

XnStatus PrimeClient::GetVDDActive(bool& bActive)
{
	return m_linkControlEndpoint.GetVDDActive(bActive);
}

XnStatus PrimeClient::SetPeriodicBistActive(bool bActive)
{
	return m_linkControlEndpoint.SetPeriodicBistActive(bActive);
}

XnStatus PrimeClient::GetPeriodicBistActive(bool& bActive)
{
	return m_linkControlEndpoint.GetPeriodicBistActive(bActive);
}
void PrimeClient::LogVersions()
{
	static bool bVersionsLoggedOnce = false;

	if (!bVersionsLoggedOnce)
	{
		xnLogVerbose(XN_MASK_PRIME_CLIENT, "Prime Client version:\t%s", XN_PS_BRIEF_VERSION_STRING);
		xnLogVerbose(XN_MASK_PRIME_CLIENT, "Host protocol version:\t%u.%u", XN_LINK_PROTOCOL_MAJOR_VERSION, XN_LINK_PROTOCOL_MINOR_VERSION);
		xnLogVerbose(XN_MASK_PRIME_CLIENT, "Device Protocol version:\t%u.%u", m_protocolVersion.m_nMajor, m_protocolVersion.m_nMinor);
		xnLogVerbose(XN_MASK_PRIME_CLIENT, "Device FW version:\t\t%u.%u.%u.%u-%s", m_fwVersion.m_nMajor, m_fwVersion.m_nMinor, m_fwVersion.m_nMaintenance, m_fwVersion.m_nBuild, m_fwVersion.m_strModifier);
		xnLogVerbose(XN_MASK_PRIME_CLIENT, "Device HW version:\t\t0x%04X", m_nHWVersion);
		xnLogVerbose(XN_MASK_PRIME_CLIENT, "Device SerialNumber:\t%s", m_strSerialNumber);
		bVersionsLoggedOnce = true;
	}
}

XnStatus PrimeClient::OpenFWLogFile(uint8_t logID)
{
	XnStatus nRetVal = XN_STATUS_OK;

	//If no stream started, start it
	if (m_nFWLogStreamID == XN_LINK_STREAM_ID_NONE)
	{
		nRetVal = StartFWLog();
		XN_IS_STATUS_OK_LOG_ERROR("Start FWLog stream", nRetVal);
	}

	//Get FW Log input stream
	LinkInputStream* pFWLogStream = GetInputStream(m_nFWLogStreamID);
	if (pFWLogStream == NULL)
	{
		xnLogError(XN_MASK_PRIME_CLIENT, "FW log input stream is NULL?!");
		XN_ASSERT(false);
		return XN_STATUS_ERROR;
	}

	//Open the log file
	nRetVal = m_linkControlEndpoint.OpenFWLogFile(logID, pFWLogStream->GetStreamID());

	return nRetVal;
}

XnStatus PrimeClient::CloseFWLogFile(uint8_t logID)
{
	XnStatus nRetVal = XN_STATUS_OK;

	//If no stream started, something is wrong
	if (m_nFWLogStreamID == XN_LINK_STREAM_ID_NONE)
	{
		xnLogError(XN_MASK_PRIME_CLIENT, "No FW log input stream");
		XN_ASSERT(false);
		return XN_STATUS_ERROR;
	}

	//Get FW Log input stream
	LinkInputStream* pFWLogStream = GetInputStream(m_nFWLogStreamID);
	if (pFWLogStream == NULL)
	{
		xnLogError(XN_MASK_PRIME_CLIENT, "FW log input stream is NULL?!");
		XN_ASSERT(false);
		return XN_STATUS_ERROR;
	}

	//Close the log file
	nRetVal = m_linkControlEndpoint.CloseFWLogFile(logID, pFWLogStream->GetStreamID());

	return nRetVal;
}

XnStatus PrimeClient::StartFWLog()
{
	XnStatus nRetVal = XN_STATUS_OK;
	std::vector<XnFwStreamInfo> fwLogStreamInfos;
	uint16_t nEndpointID = 0;

	//Enumerate log streams (there should be exactly one)
	nRetVal = EnumerateStreams(XN_LINK_STREAM_TYPE_LOG, fwLogStreamInfos);
	XN_IS_STATUS_OK_LOG_ERROR("Enumerate log streams", nRetVal);
	if (fwLogStreamInfos.size() == 0)
	{
		xnLogError(XN_MASK_PRIME_CLIENT, "No FW log stream exists in device");
		XN_ASSERT(false);
		return XN_STATUS_ERROR;
	}

	if (fwLogStreamInfos.size() > 1)
	{
		xnLogError(XN_MASK_PRIME_CLIENT, "Only one FW log stream is supported");
		XN_ASSERT(false);
		return XN_STATUS_ERROR;
	}

	//Create log stream (from first enumeration result)
	nRetVal = CreateInputStreamImpl(XN_LINK_STREAM_TYPE_LOG, fwLogStreamInfos[0].creationInfo, m_nFWLogStreamID, nEndpointID);
	XN_IS_STATUS_OK_LOG_ERROR("Create log input stream", nRetVal);
	LinkInputStream* pFWLogStream = GetInputStream(m_nFWLogStreamID);
	if (pFWLogStream == NULL)
	{
		xnLogError(XN_MASK_PRIME_CLIENT, "FW log input stream is NULL?!");
		XN_ASSERT(false);
		return XN_STATUS_ERROR;
	}

	//Start the log stream
	nRetVal = pFWLogStream->Start();
	XN_IS_STATUS_OK_LOG_ERROR("Start FW Log Stream", nRetVal);
	xnLogInfo(XN_MASK_PRIME_CLIENT, "FW Log started on stream %u, endpoint %u", m_nFWLogStreamID, nEndpointID);

	return XN_STATUS_OK;
}

XnStatus PrimeClient::StopFWLog()
{
	XnStatus nRetVal = XN_STATUS_OK;
	if (m_nFWLogStreamID != XN_LINK_STREAM_ID_NONE)
	{
		//Get FW Log input stream
		LinkInputStream* pFWLogStream = GetInputStream(m_nFWLogStreamID);
		if (pFWLogStream == NULL)
		{
			xnLogError(XN_MASK_PRIME_CLIENT, "FW log input stream is NULL?!");
			XN_ASSERT(false);
			return XN_STATUS_ERROR;
		}

		//Stop stream
		nRetVal = pFWLogStream->Stop();
		XN_IS_STATUS_OK_LOG_ERROR("Stop FW log stream", nRetVal);

		//Destroy stream
		nRetVal = DestroyInputStream(m_nFWLogStreamID);
		XN_IS_STATUS_OK_LOG_ERROR("Destroy input stream", nRetVal);
		//Mark stream as unused
		m_nFWLogStreamID = XN_LINK_STREAM_ID_NONE;
	}
	return XN_STATUS_OK;
}

XnStatus PrimeClient::CreateInputStreamImpl(XnLinkStreamType streamType, const char* strCreationInfo, uint16_t& nStreamID, uint16_t& nEndpointID)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_linkControlEndpoint.CreateInputStream(streamType, strCreationInfo, nStreamID, nEndpointID);
	XN_IS_STATUS_OK_LOG_ERROR("Create stream on device", nRetVal);

	if (nEndpointID > m_inputDataEndpoints.size())
	{
		xnLogError(XN_MASK_PRIME_CLIENT, "Stream %u was created on non-existing endpoint %u", nStreamID, nEndpointID);
		XN_ASSERT(false);
		return XN_STATUS_ERROR;
	}

	if (!m_inputDataEndpoints[nEndpointID].IsInitialized())
	{
		xnLogVerbose(XN_MASK_PRIME_CLIENT, "Initializing input data endpoint 0x%X...", nEndpointID);
		nRetVal = m_inputDataEndpoints[nEndpointID].Init(nEndpointID,
			m_pConnectionFactory,
			&m_linkInputStreamsMgr,
			this);
		XN_IS_STATUS_OK_LOG_ERROR("Init input data endpoint", nRetVal);
	}

	//Initialize input stream
	nRetVal = m_linkInputStreamsMgr.InitInputStream(&m_linkControlEndpoint, streamType, nStreamID, &m_inputDataEndpoints[nEndpointID]);
	XN_IS_STATUS_OK_LOG_ERROR("Init input stream", nRetVal);
	return XN_STATUS_OK;
}

#define CHECK_TOKEN(pToken, strLine, pFile)																			\
	if (pToken == NULL)																								\
	{																												\
		xnLogError(XN_MASK_PRIME_CLIENT, "Preset file corrupt: line '%s' is not in the correct format!", strLine);	\
		XN_ASSERT(false);																							\
		fclose(pFile);																								\
		return XN_STATUS_CORRUPT_FILE;																				\
	}

XnStatus PrimeClient::RunPresetFile(const char* strFileName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnLogVerbose(XN_MASK_PRIME_CLIENT, "Executing preset file '%s'...", strFileName);

	// Check that file exists
	bool bFileExists;
	nRetVal = xnOSDoesFileExist(strFileName, &bFileExists);
	XN_IS_STATUS_OK(nRetVal);

	if (!bFileExists)
	{
		xnLogError(XN_MASK_PRIME_CLIENT, "File '%s' does not exist", strFileName);
		return XN_STATUS_OS_FILE_NOT_FOUND;
	}

	FILE* pFile = fopen(strFileName, "r");
	XN_ASSERT(pFile != NULL);

	char strLine[1024];

	// read header
	if (NULL == fgets(strLine, sizeof(strLine), pFile))
	{
		xnLogError(XN_MASK_PRIME_CLIENT, "File '%s' is empty - no header", strFileName);
		return XN_STATUS_ERROR;
	}

	uint32_t nAddress;
	uint32_t nValue;
	uint8_t nBitOffset;
	uint8_t nBitWidth;

	for (;;)
	{
		// read a line
		if (NULL == fgets(strLine, sizeof(strLine), pFile))
		{
			// end of file reached
			break;
		}

		// skip empty lines
		if (xnOSStrCmp(strLine, "\n") == 0 || xnOSStrCmp(strLine, "\r\n") == 0)
		{
			continue;
		}
		// skip comments
		int i;
		int length = (int)strlen(strLine);
		for (i = 0; i < length; i++)
		{
			if (strLine[i] == ' ' || strLine[i] == '\t')
			{
				continue;
			}
		}
		if (i < length && strLine[i] == '#' )
		{
			continue;
		}

		// block name
		char* pToken = strtok(strLine, ",");
		CHECK_TOKEN(pToken, strLine, pFile);

		// reg name
		pToken = strtok(NULL, ",");
		CHECK_TOKEN(pToken, strLine, pFile);

		// Address
		pToken = strtok(NULL, ",");
		CHECK_TOKEN(pToken, strLine, pFile);
		sscanf(pToken, "0x%x", &nAddress);

		// field name
		pToken = strtok(NULL, ",");
		CHECK_TOKEN(pToken, strLine, pFile);

		// bit offset
		pToken = strtok(NULL, ",");
		CHECK_TOKEN(pToken, strLine, pFile);
		nBitOffset = (uint8_t)atoi(pToken);

		// bit width
		pToken = strtok(NULL, ",");
		CHECK_TOKEN(pToken, strLine, pFile);
		nBitWidth = (uint8_t)atoi(pToken);

		// value
		pToken = strtok(NULL, ",");
		CHECK_TOKEN(pToken, strLine, pFile);
		sscanf(pToken, "0x%x", &nValue);

		// execute
		nRetVal = WriteAHB(nAddress, nValue, nBitOffset, nBitWidth);
		if (nRetVal != XN_STATUS_OK)
		{
			fclose(pFile);
			return nRetVal;
		}
	}

	fclose(pFile);

	xnLogInfo(XN_MASK_PRIME_CLIENT, "Preset file '%s' was executed", strFileName);

	return (XN_STATUS_OK);
}

XnStatus PrimeClient::GetSupportedBistTests(std::vector<XnBistInfo>& supportedTests)
{
	return m_linkControlEndpoint.GetSupportedBistTests(supportedTests);
}
XnStatus PrimeClient::GetSupportedTempList(std::vector<XnTempInfo>& supportedTempList)
{
	return m_linkControlEndpoint.GetSupportedTempList(supportedTempList);
}
XnStatus PrimeClient::GetTemperature(XnCommandTemperatureResponse& temp)
{
	return m_linkControlEndpoint.GetTemperature(temp);
}
XnStatus PrimeClient::GetSupportedI2CDevices(std::vector<XnLinkI2CDevice>& supportedDevices)
{
	return m_linkControlEndpoint.GetSupportedI2CDevices(supportedDevices);
}

XnStatus PrimeClient::GetSupportedLogFiles(std::vector<XnLinkLogFile>& supportedFiles)
{
	return m_linkControlEndpoint.GetSupportedLogFiles(supportedFiles);
}

XnStatus PrimeClient::ExecuteBist(uint32_t nID, uint32_t& errorCode, uint32_t& extraDataSize, uint8_t* extraData)
{
	return m_linkControlEndpoint.ExecuteBistTests(nID, errorCode, extraDataSize, extraData);
}

bool PrimeClient::IsPropertySupported(uint16_t propID)
{
	uint32_t nInterface = (propID >> 8);
	uint32_t nProp = (propID & 0x0F);
	return (nInterface < m_supportedProps.size() && m_supportedProps[nInterface].IsSet(nProp));
}

XnStatus PrimeClient::GetBootStatus(XnBootStatus& bootStatus)
{
	return m_linkControlEndpoint.GetBootStatus(bootStatus);
}

XnStatus PrimeClient::EnableProjectorPulse(float delay, float width, float cycle)
{
	return m_linkControlEndpoint.SetProjectorPulse(true, delay, width, cycle);
}

XnStatus PrimeClient::DisableProjectorPulse()
{
	return m_linkControlEndpoint.SetProjectorPulse(false, 0, 0, 0);
}

XnStatus PrimeClient::GetProjectorPulse(bool& enabled, float& delay, float& width, float& framesToskip)
{
	return m_linkControlEndpoint.GetProjectorPulse(enabled, delay, width, framesToskip);
}

XnStatus PrimeClient::SetProjectorPower(uint16_t power)
{
	return m_linkControlEndpoint.SetProjectorPower(power);
}

XnStatus PrimeClient::GetProjectorPower(uint16_t& power)
{
	return m_linkControlEndpoint.GetProjectorPower(power);
}

}
