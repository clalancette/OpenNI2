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
#ifndef XNLINKCONTROLENDPOINT_H
#define XNLINKCONTROLENDPOINT_H

#include <vector>

#include "ISyncIOConnection.h"
#include "XnLinkMsgEncoder.h"
#include "XnLinkResponseMsgParser.h"
#include "XnLinkDefs.h"
#include "XnLinkProtoLibDefs.h"
#include <XnBitSet.h>
#include <PrimeSense.h>

struct XnShiftToDepthConfig;

namespace xn
{

class IConnectionFactory;
struct BaseStreamProps;

class LinkControlEndpoint
{
public:
	static const uint32_t MUTEX_TIMEOUT;

	LinkControlEndpoint();
	virtual ~LinkControlEndpoint();

	XnStatus Init(uint32_t nMaxOutMsgSize, IConnectionFactory* pConnectionFactory);
	void Shutdown();

	XnStatus Connect();
	void Disconnect();
	XnBool IsConnected() const;

#ifdef DATA_ON_CONTROL
	void SetStreamID(XnUInt16 nStreamID);
#endif

	//nResponseSize is max size on input, actual size on output
	//pIsLast - optional. If provided, command will not automatically continue response, and the out value is whether this is the last packet. If NULL, all data is fetched automatically.
	XnStatus ExecuteCommand(XnUInt16 nMsgType, XnUInt16 nStreamID, const void* pCmdData, uint32_t nCmdSize, void* pResponseData, uint32_t& nResponseSize, XnBool* pIsLast = NULL);
	XnStatus SendData(XnUInt16 nMsgType, const void* pCmdData, uint32_t nCmdSize, void* pResponseData, uint32_t& nResponseSize);
	XnUInt16 GetPacketID() const;
	XN_MUTEX_HANDLE GetMutex() const;
	XnBool IsMsgTypeSupported(XnUInt16 nMsgType);
	XnBool IsPropertySupported(XnUInt16 nPropID);

	/* Specific commands */
	XnStatus GetFWVersion(XnLinkDetailedVersion& version);
	XnStatus GetProtocolVersion(XnLeanVersion& version);
	XnStatus GetHardwareVersion(uint32_t& version);
	XnStatus GetSerialNumber(XnChar* strSerialNumber, uint32_t nSize);
	XnStatus GetComponentsVersions(std::vector<XnComponentVersion>& components);
	XnStatus GetSupportedMsgTypes(std::vector<xnl::BitSet>& supportedMsgTypes);
	XnStatus GetSupportedProperties(std::vector<xnl::BitSet>& supportedProperties);
	XnStatus GetSupportedInterfaces(XnUInt16 nStreamID, xnl::BitSet& supportedInterfaces);
	XnStatus GetBootStatus(XnBootStatus& bootStatus);
	XnStatus UploadFile(const XnChar* strFileName, XnBool bOverrideFactorySettings);
	XnStatus GetFileList(std::vector<XnFwFileEntry>& files);
	XnStatus DownloadFile(XnUInt16 zone, const XnChar* fwFileName, const XnChar* targetFile);
	XnStatus StartStreaming(XnUInt16 nStreamID);
	XnStatus StopStreaming(XnUInt16 nStreamID);
	XnStatus SoftReset();
	XnStatus HardReset();
	XnStatus ReadDebugData(XnCommandDebugData& commandDebugData);
	XnStatus GetSupportedBistTests(std::vector<XnBistInfo>& supportedTests);
	XnStatus GetSupportedTempList(std::vector<XnTempInfo>& supportedTests);
	XnStatus GetTemperature(XnCommandTemperatureResponse& temp);
	XnStatus ExecuteBistTests(uint32_t nID, uint32_t& errorCode, uint32_t& extraDataSize, uint8_t* extraData);
	XnStatus StartUsbTest();
	XnStatus StopUsbTest();
	XnStatus GetSupportedI2CDevices(std::vector<XnLinkI2CDevice>& supporteddevices);
	XnStatus WriteI2C(XnUInt8 nDeviceID, XnUInt8 nAddressSize, uint32_t nAddress, XnUInt8 nValueSize, uint32_t nValue, uint32_t nMask);
	XnStatus ReadI2C(XnUInt8 nDeviceID, XnUInt8 nAddressSize, uint32_t nAddress, XnUInt8 nValueSize, uint32_t& nValue);
	XnStatus WriteAHB(uint32_t nAddress, uint32_t nValue, XnUInt8 nBitOffset, XnUInt8 nBitWidth);
	XnStatus ReadAHB(uint32_t nAddress, XnUInt8 nBitOffset, XnUInt8 nBitWidth, uint32_t& nValue);
	XnStatus GetShiftToDepthConfig(XnUInt16 nStreamID, XnShiftToDepthConfig& shiftToDepthConfig);
	XnStatus SetVideoMode(XnUInt16 nStreamID, const XnFwStreamVideoMode& videoMode);
	XnStatus GetVideoMode(XnUInt16 nStreamID, XnFwStreamVideoMode& videoMode);
	XnStatus GetSupportedVideoModes(XnUInt16 nStreamID, std::vector<XnFwStreamVideoMode>& supportedVideoModes);
	XnStatus EnumerateStreams(std::vector<XnFwStreamInfo>& aStreamInfos);
	XnStatus CreateInputStream(XnStreamType streamType, const XnChar* strCreationInfo, XnUInt16& nStreamID, XnUInt16& nEndpointID);
	XnStatus DestroyInputStream(XnUInt16 nStreamID);
	XnStatus SetCropping(XnUInt16 nStreamID, const OniCropping& cropping);
	XnStatus GetCropping(XnUInt16 nStreamID, OniCropping& cropping);
	XnStatus SetProjectorActive(XnBool bActive);
	XnStatus SetAccActive(XnBool bActive);
	XnStatus GetAccActive(XnBool& bActive);
	XnStatus SetVDDActive(XnBool bActive);
	XnStatus GetVDDActive(XnBool& bActive);
	XnStatus SetPeriodicBistActive(XnBool bActive);
	XnStatus GetPeriodicBistActive(XnBool& bActive);
	XnStatus GetSupportedLogFiles(std::vector<XnLinkLogFile>& supportedFiles);
	XnStatus OpenFWLogFile(XnUInt8 logID, XnUInt16 nLogStreamID);
	XnStatus CloseFWLogFile(XnUInt8 logID, XnUInt16 nLogStreamID);
	XnStatus SetProjectorPulse(XnBool enabled, float delay, float width, float cycle);
	XnStatus GetProjectorPulse(XnBool& enabled, float& delay, float& width, float& framesToskip);
	XnStatus SetProjectorPower(XnUInt16 power);
	XnStatus GetProjectorPower(XnUInt16& power);
	XnStatus SetGain(XnUInt16 streamID, XnUInt16 gain);
	XnStatus GetGain(XnUInt16 streamID, XnUInt16& gain);
	//TODO: Implement Get emitter active

	XnStatus GetStreamFragLevel(XnUInt16 nStreamID, XnStreamFragLevel& streamFragLevel);
	XnStatus GetMirror(XnUInt16 nStreamID, XnBool& bMirror);
	XnStatus SetMirror(XnUInt16 nStreamID, XnBool bMirror);

	XnStatus BeginUpload();
	XnStatus EndUpload();
	XnStatus FormatZone(XnUInt8 nZone);

	/*DepthGenerator commands */
	XnStatus GetCameraIntrinsics(XnUInt16 nStreamID, XnLinkCameraIntrinsics& cameraIntrinsics);

private:
	static const XnUInt16 BASE_PACKET_ID;
	static const XnUInt16 MAX_RESPONSE_NUM_PACKETS; //Max number of packets in response
	static const XnChar MUTEX_NAME[];

	XnStatus GetLogicalMaxPacketSize(XnUInt16& nMaxPacketSize);

	/** The fragmentation parameter in this function indicates the fragmentation of the whole data BLOCK,
	    not an individual packet. So if it's BEGIN, it means this block of data (one or more packets) begins
		the message. If it's MIDDLE, it's the middle of the message, and if it's END, this block ends the
		message.**/
	XnStatus ExecuteImpl(XnUInt16 nMsgType,
				XnUInt16 nStreamID,
				const void* pData,
				uint32_t nSize,
				XnLinkFragmentation fragmentation,
				void* pResponseData,
				uint32_t& nResponseSize,
				XnBool autoContinue,
				XnBool& isLast);

	XnStatus ContinueResponseImpl(XnUInt16 originalMsgType, XnUInt16 streamID, void* pResponseData, uint32_t& nResponseSize, XnBool& outLastPacket);

	XnStatus ValidateResponsePacket(const LinkPacketHeader* pResponsePacket,
					XnUInt16 nExpectedMsgType,
					XnUInt16 nExpectedStreamID,
					uint32_t nBytesToRead);

	/* Properties */
	XnStatus SetIntProperty(XnUInt16 nStreamID, XnLinkPropID propID, uint64_t nValue);
	XnStatus GetIntProperty(XnUInt16 nStreamID, XnLinkPropID propID, uint64_t& nValue);
	XnStatus SetRealProperty(XnUInt16 nStreamID, XnLinkPropID propID, double dValue);
	XnStatus GetRealProperty(XnUInt16 nStreamID, XnLinkPropID propID, double& dValue);
	XnStatus SetStringProperty(XnUInt16 nStreamID, XnLinkPropID propID, const XnChar* strValue);
	XnStatus GetStringProperty(XnUInt16 nStreamID, XnLinkPropID propID, uint32_t nSize, XnChar* strValue);
	XnStatus SetGeneralProperty(XnUInt16 nStreamID, XnLinkPropID propID, uint32_t nSize, const void* pSource);
	//nSize is max size on input, actual size on output
	XnStatus GetGeneralProperty(XnUInt16 nStreamID, XnLinkPropID propID, uint32_t& nSize, void* pDest);
	XnStatus GetBitSetProperty(XnUInt16 nStreamID, XnLinkPropID propID, xnl::BitSet& bitSet);
	XnStatus SetProperty(XnUInt16 nStreamID, XnLinkPropType propType, XnLinkPropID propID, uint32_t nSize, const void* pSource);
	XnStatus GetProperty(XnUInt16 nStreamID, XnLinkPropType propType, XnLinkPropID propID, uint32_t& nSize, void* pDest);

	union
	{
		XnUInt8* m_pIncomingRawPacket; //Holds one packet, used for receiving from connection
		LinkPacketHeader* m_pIncomingPacket;
	};

	uint32_t m_nMaxOutMsgSize;
	ISyncIOConnection* m_pConnection;
	LinkMsgEncoder m_msgEncoder;
	LinkResponseMsgParser m_responseMsgParser;
	XnUInt8* m_pIncomingResponse; //Holds complete parsed response (without link headers)
	uint32_t m_nMaxResponseSize;
	XnBool m_bInitialized;
	XnBool m_bConnected;
	XnUInt16 m_nPacketID;
	XnUInt16 m_nMaxPacketSize;
	XN_MUTEX_HANDLE m_hMutex;
	std::vector<xnl::BitSet> m_supportedMsgTypes; //Array index is msgtype hi byte, position in bit set is msgtype lo byte.
};

}
#endif // XNLINKCONTROLENDPOINT_H
