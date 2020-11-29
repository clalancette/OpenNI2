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
#ifndef XNLINKPROTOUTILS_H
#define XNLINKPROTOUTILS_H

#include <vector>

#include "XnLinkDefs.h"
#include "XnLinkProto.h"
#include "XnLinkStatusCodes.h"
#include "XnShiftToDepth.h"
#include <XnStatus.h>
#include <XnLog.h>
#include <XnBitSet.h>
#include <XnBox3D.h>
#include <XnVector3D.h>
#include <PSLink.h>

#define XN_MASK_LINK "xnLink"

namespace xn
{

class LinkPacketHeader : private XnLinkPacketHeader
{
public:
	XnStatus Validate(uint32_t nBytesToRead) const;
	XnBool IsMagicValid() const { return (m_nMagic == XN_LINK_MAGIC); }
	uint16_t GetSize() const { return m_nSize; }
	uint16_t GetDataSize() const { return (m_nSize - sizeof(XnLinkPacketHeader)); }
	uint16_t GetMsgType() const { return m_nMsgType; }
	XnLinkFragmentation GetFragmentationFlags() const { return XnLinkFragmentation(m_nFragmentation); }
	uint16_t GetStreamID() const { return m_nStreamID; }
	uint16_t GetPacketID() const { return m_nPacketID; }
	uint16_t GetCID() const { return m_nCID; }
	const uint8_t* GetPacketData() const {return (reinterpret_cast<const uint8_t*>(this) + sizeof(XnLinkPacketHeader)); }
	uint8_t* GetPacketData() {return (reinterpret_cast<uint8_t*>(this) + sizeof(XnLinkPacketHeader)); }

	void SetMagic() { m_nMagic = XN_LINK_MAGIC; }
	void SetSize(uint16_t nSize) { m_nSize = nSize; }
	void SetMsgType(uint16_t nMsgType) { m_nMsgType = nMsgType; }
	void SetFragmentationFlags(XnLinkFragmentation flags) { m_nFragmentation = flags; }
	void SetStreamID(uint16_t nStreamID) { m_nStreamID = nStreamID; }
	void SetPacketID(uint16_t nPacketID) { m_nPacketID = nPacketID; }
	void SetCID(uint16_t nCID) { m_nCID = nCID; }
};

static_assert(sizeof(xn::LinkPacketHeader) == sizeof(XnLinkPacketHeader));

}

XnStatus xnLinkResponseCodeToStatus(uint16_t nResponseCode);
const XnChar* xnLinkResponseCodeToStr(uint16_t nResponseCode);
const XnChar* xnFragmentationFlagsToStr(XnLinkFragmentation fragmentation);

const XnChar* xnLinkStreamTypeToString(XnStreamType streamType);
XnStreamType xnLinkStreamTypeFromString(const XnChar* strType);

const XnChar* xnLinkGestureTypeToName(uint32_t gestureType);
uint32_t xnLinkGestureNameToType(const XnChar* strGesture);

const XnChar* xnLinkPixelFormatToName(XnFwPixelFormat pixelFormat);
XnFwPixelFormat xnLinkPixelFormatFromName(const XnChar* name);
const XnChar* xnLinkCompressionToName(XnFwCompressionType compression);
XnFwCompressionType xnLinkCompressionFromName(const XnChar* name);

const XnChar* xnLinkPoseTypeToName(uint32_t poseType);
uint32_t xnLinkPoseNameToType(const XnChar* strPose);
XnStatus xnLinkPosesToNames(uint32_t nPoses, std::vector<const XnChar*>& aPosesNames);

xnl::Point3D xnLinkPoint3DToPoint3D(const XnLinkPoint3D& point);
XnLinkPoint3D XnPoint3DToLinkPoint3D(const xnl::Point3D& point);

XnLinkBoundingBox3D xnBoundingBox3DToLinkBoundingBox3D(const xnl::Box3D& box);
xnl::Box3D xnLinkBoundingBox3DToBoundingBox3D(const XnLinkBoundingBox3D& box);

XnStatus xnLinkGetStreamDumpName(uint16_t nStreamID, XnChar* strDumpName, uint32_t nDumpNameSize);
XnStatus xnLinkGetEPDumpName(uint16_t nEPID, XnChar* strDumpName, uint32_t nDumpNameSize);

XnStatus xnLinkParseIDSet(std::vector<xnl::BitSet>& idSet, const void* pIDSet, uint32_t nSize);

/*pnEncodedSize is max size on input, actual size on output. pIDs is an array of uint16 values that must be grouped by interface ID.*/
XnStatus xnLinkEncodeIDSet(void* pIDSet, uint32_t *pnEncodedSize, const uint16_t* pIDs, uint32_t nNumIDs);

XnStatus xnLinkParseFrameSyncStreamIDs(std::vector<uint16_t>& frameSyncStreamIDs, const void* pFrameSyncStreamIDs, uint32_t nBufferSize);
//nBufferSize is max size on input, actual size on output
XnStatus xnLinkEncodeFrameSyncStreamIDs(void* pFrameSyncStreamIDs, uint32_t& nBufferSize, const std::vector<uint16_t>& frameSyncStreamIDs);
XnStatus xnLinkParseComponentVersionsList(std::vector<XnComponentVersion>& componentVersions, const XnLinkComponentVersionsList* pLinkList, uint32_t nBufferSize);

/*
uint8_t xnLinkNICapabilityToInterfaceID(const XnChar* strCapabilityName);
const XnChar* xnLinkInterfaceIDToNICapability(uint8_t nInterfaceID);
XnProductionNodeType xnLinkStreamTypeToNINodeType(XnLinkStreamType streamType);
XnLinkStreamType xnLinkNINodeTypeToStreamType(XnProductionNodeType  nodeType);
*/
void xnLinkParseVideoMode(XnFwStreamVideoMode& videoMode, const XnLinkVideoMode& linkVideoMode);
void xnLinkEncodeVideoMode(XnLinkVideoMode& linkVideoMode, const XnFwStreamVideoMode& videoMode);

const XnChar* xnLinkPropTypeToStr(XnLinkPropType propType);

void xnLinkParseDetailedVersion(XnLinkDetailedVersion& version, const XnLinkDetailedVersion& linkVersion);

void xnLinkParseLeanVersion(XnLeanVersion& version, const XnLinkLeanVersion& linkVersion);
void xnEncodeLeanVersion(XnLinkLeanVersion& linkVersion, const XnLeanVersion& version);

/* nNumModes is max number of modes on input, actual number on output. */
XnStatus xnLinkParseSupportedVideoModes(std::vector<XnFwStreamVideoMode>& aModes,
												const XnLinkSupportedVideoModes* pLinkSupportedModes,
												uint32_t nBufferSize);
XnStatus xnLinkParseBitSet(xnl::BitSet& bitSet, const XnLinkBitSet* pBitSet, uint32_t nBufferSize);
XnStatus xnLinkEncodeBitSet(XnLinkBitSet& linkBitSet, uint32_t& nBufferSize, const xnl::BitSet& bitSet);

void xnLinkParseShiftToDepthConfig(XnShiftToDepthConfig& shiftToDepthConfig, const XnLinkShiftToDepthConfig& linkShiftToDepthConfig);

void xnLinkParseCropping(OniCropping& cropping, const XnLinkCropping& linkCropping);
void xnLinkEncodeCropping(XnLinkCropping& linkCropping, const OniCropping& cropping);

const XnChar* xnLinkGetPropName(XnLinkPropID propID);

XnStatus xnLinkValidateGeneralProp(XnLinkPropType propType, uint32_t nValueSize, uint32_t nMinSize);

template <typename T>
XnStatus xnLinkParseIntProp(XnLinkPropType propType, const void* pValue, uint32_t nValueSize, T& nParsedVal)
{
	if (nValueSize < sizeof(T))
	{
		xnLogError(XN_MASK_LINK, "Property value size should be at least %u bytes, but got only %u bytes.",
			sizeof(T), nValueSize);
		return XN_STATUS_LINK_BAD_PROP_SIZE;
	}

	if (propType != XN_LINK_PROP_TYPE_INT)
	{
		xnLogError(XN_MASK_LINK, "Property type should be %s, but got type %s",
			xnLinkPropTypeToStr(XN_LINK_PROP_TYPE_INT),
			xnLinkPropTypeToStr(propType));
		XN_ASSERT(FALSE);
		return XN_STATUS_LINK_BAD_PROP_TYPE;
	}

	nParsedVal = static_cast<T>(XN_PREPARE_VAR64_IN_BUFFER(*reinterpret_cast<const uint64_t*>(pValue)));
	return XN_STATUS_OK;
}

XnStatus xnLinkParseLeanVersionProp(XnLinkPropType propType, const void* pValue, uint32_t nValueSize, XnLeanVersion& leanVersion);
XnStatus xnLinkParseIDSetProp(XnLinkPropType propType, const void* pValue, uint32_t nValueSize, std::vector<xnl::BitSet>& idSet);
XnStatus xnLinkParseBitSetProp(XnLinkPropType propType, const void* pValue, uint32_t nValueSize, xnl::BitSet& bitSet);
XnStatus xnLinkParseFrameSyncStreamIDsProp(XnLinkPropType propType, const void* pValue, uint32_t nValueSize, std::vector<uint16_t>& streamIDs);
XnStatus xnLinkParseComponentVersionsListProp(XnLinkPropType propType, const void* pValue, uint32_t nValueSize, std::vector<XnComponentVersion>& componentVersions);

XnStatus xnLinkParseSupportedBistTests(const XnLinkSupportedBistTests* pSupportedTests, uint32_t nBufferSize, std::vector<XnBistInfo>& supportedTests);
XnStatus xnLinkParseSupportedTempList(const XnLinkTemperatureSensorsList* pSupportedList, uint32_t nBufferSize, std::vector<XnTempInfo>& supportedTempList);
XnStatus xnLinkParseGetTemperature(const XnLinkTemperatureResponse* tempResponse, uint32_t nBufferSize, XnCommandTemperatureResponse& tempData);
XnStatus xnLinkReadDebugData(XnCommandDebugData& commandDebugData, XnLinkDebugDataResponse* pDebugDataResponse);
XnStatus xnLinkParseSupportedI2CDevices(const XnLinkSupportedI2CDevices* pSupportedTests, uint32_t nBufferSize, std::vector<XnLinkI2CDevice>& supportedDevices);
XnStatus xnLinkParseSupportedLogFiles(const XnLinkSupportedLogFiles* pFilesList, uint32_t nBufferSize, std::vector<XnLinkLogFile>& supportedFiles);

void xnLinkParseBootStatus(XnBootStatus& bootStatus, const XnLinkBootStatus& linkBootStatus);

uint32_t xnLinkGetPixelSizeByStreamType(XnLinkStreamType streamType);

void xnLinkVideoModeToString(XnFwStreamVideoMode videoMode, XnChar* buffer, uint32_t bufferSize);

#endif // XNLINKPROTOUTILS_H
