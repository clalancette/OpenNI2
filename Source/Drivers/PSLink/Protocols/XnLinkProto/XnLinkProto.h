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
#ifndef XNLINKPROTO_H
#define XNLINKPROTO_H

#include <XnPlatform.h>
#include "XnLinkDefs.h"

#pragma pack (push, 1)

//-----------------------------------------------------------------------
// Packet Structure
//-----------------------------------------------------------------------
#if XN_PLATFORM_IS_LITTLE_ENDIAN
typedef struct XnLinkPacketHeader
{
	uint16_t m_nMagic;
	uint16_t m_nSize;
	uint16_t m_nMsgType;
	uint16_t m_nCID;
	uint16_t m_nPacketID;
	uint16_t m_nStreamID : 14;
	uint16_t m_nFragmentation : 2; //The two most significant bits of these 16 bits are the fragmentation
} XnLinkPacketHeader;
#else
typedef struct XnLinkPacketHeader
{
	uint16_t m_nMagic;
	uint16_t m_nSize;
	uint16_t m_nMsgType;
	uint16_t m_nCID;
	uint16_t m_nPacketID;
	uint16_t m_nFragmentation : 2; //The two most significant bits of these 16 bits are the fragmentation
	uint16_t m_nStreamID : 14;
} XnLinkPacketHeader;

#endif

typedef struct XnLinkPacket
{
	XnLinkPacketHeader m_packetHeader;
	uint8_t m_data[1];
} XnLinkPacket;

typedef struct XnLinkDataHeader
{
	uint32_t m_nTimestampLo;
	uint32_t m_nTimestampHi;
} XnLinkDataHeader;


//-----------------------------------------------------------------------
// Log Header Structure
//-----------------------------------------------------------------------
typedef struct XnLinkLogParam
{
	uint8_t m_ID;       // 0 for normal log, 1,2... for other logType
	uint8_t command;    // from XnLinkLogCommand.
						// 0- write data to file  with m_ID,
						// 1- open the file with m_ID and Name logFileName
						// 2- for close file with m_ID and Name logFileName
	uint16_t size;		//size of all the message (included the Header and data)
} XnLinkLogParam;


typedef struct XnLinkLogFileParam
{
	uint8_t logFileName[XN_LINK_MAX_LOG_FILE_NAME_LENGTH];
}XnLinkLogFileParam;

//-----------------------------------------------------------------------
// Data Elements
//-----------------------------------------------------------------------
typedef struct XnLinkVideoMode
{
	uint16_t m_nXRes;
	uint16_t m_nYRes;
	uint16_t m_nFPS;
	uint8_t m_nPixelFormat; // from XnLinkPixelFormat
	uint8_t m_nCompression; // from XnLinkCompressionType
} XnLinkVideoMode;

typedef struct
{
	uint32_t m_nNumModes;
	XnLinkVideoMode m_supportedVideoModes[1];
} XnLinkSupportedVideoModes;

typedef struct XnLinkShiftToDepthConfig
{
	/** The zero plane distance in depth units. */
	uint16_t nZeroPlaneDistance;
	uint16_t m_nReserved;
	/** The zero plane pixel size */
	float fZeroPlanePixelSize;
	/** The distance between the emitter and the Depth Cmos */
	float fEmitterDCmosDistance;
	/** The maximum possible shift value from this device. */
	uint32_t nDeviceMaxShiftValue;
	/** The maximum possible depth from this device (as opposed to a cut-off). */
	uint32_t nDeviceMaxDepthValue;

	uint32_t nConstShift;
	uint32_t nPixelSizeFactor;
	uint32_t nParamCoeff;
	uint32_t nShiftScale;
	uint16_t nDepthMinCutOff;
	uint16_t nDepthMaxCutOff;

} XnLinkShiftToDepthConfig;

typedef struct XnLinkSerialNumber
{
	XnChar m_strSerialNumber[XN_LINK_SERIAL_NUMBER_SIZE];
} XnLinkSerialNumber;

typedef struct XnLinkPoint3D
{
	float m_fX;
	float m_fY;
	float m_fZ;
} XnLinkPoint3D;

typedef struct XnLinkBoundingBox3D
{
	XnLinkPoint3D leftBottomNear;
	XnLinkPoint3D rightTopFar;
} XnLinkBoundingBox3D;

typedef struct XnLinkBitSet
{
	uint32_t m_nSize; //Size in bytes of encoded data
	uint8_t m_aData[1];
} XnLinkBitSet;

typedef struct XnLinkStreamInfo
{
	uint32_t m_nStreamType;
	XnChar m_strCreationInfo[XN_LINK_MAX_CREATION_INFO_LENGTH];
} XnLinkStreamInfo;

typedef struct XnLinkHandData
{
	uint32_t m_nHandID;
	XnLinkPoint3D m_position;
	uint32_t m_touchingFOVEdge; // XnDirection values, ILLEGAL means not touching FOV edge
} XnLinkHandData;

typedef struct XnLinkHandsData
{
	uint32_t m_nHandsCount;
	XnLinkHandData m_aHands[1];
} XnLinkHandsData;

typedef struct XnLinkGestureRecognizedEventArgs
{
	XnLinkPoint3D m_IDPosition;
	XnLinkPoint3D m_EndPosition;
} XnLinkGestureRecognizedEventArgs;

typedef struct XnLinkGestureProgressEventArgs
{
	XnLinkPoint3D m_position;
	float m_fProgress;
} XnLinkGestureProgressEventArgs;

typedef struct XnLinkGestureIntermediateStageEventArgs
{
	XnLinkPoint3D m_position;
} XnLinkGestureIntermediateStageEventArgs;

typedef struct XnLinkGestureEventHeader
{
	uint32_t m_nGesture;// Taken from XnLinkGestureType
	uint32_t m_nGestureEventType;//Taken from XnLinkGestureEventType
} XnLinkGestureEventHeader;

typedef struct XnLinkGestureDataHeader
{
	uint32_t m_nEventsCount;
} XnLinkGestureDataHeader;

typedef struct XnLinkUserPoseDetectionElement
{
	uint32_t m_nPoseID; // Flags taken from XnLinkPoseType
	uint32_t m_nDetectionStatus; // XnPoseDetectionStatus
} XnLinkUserPoseDetectionElement;

typedef struct XnLinkUserInPoseElement
{
	uint32_t m_nPoseID; // Flags taken from XnLinkPoseType
} XnLinkUserPoseElement;

typedef struct XnLinkUserCalibrationElement
{
	uint32_t m_nCalibrationStatus; // taken from XnCalibrationStatus
} XnLinkUserCalibrationElement;

typedef struct XnLinkUserJointData
{
	uint32_t m_nJointID;
	XnLinkPoint3D m_position;
	float m_fPositionConfidence;
	float m_afOrientation[9];
	float m_fOrientationConfidence;
} XnLinkUserJointData;

typedef struct XnLinkUserTrackingElement
{
	XnLinkUserJointData m_aJoints[1];
} XnLinkUserTrackingElement;

typedef struct XnLinkUserDataElementHeader
{
	uint32_t m_nElementType;
	uint32_t m_nElementSize;
} XnLinkUserDataElementHeader;

typedef struct XnLinkUserDataElement
{
	XnLinkUserDataElementHeader m_header;
	uint8_t m_elementData[1];
} XnLinkUserDataElement;

typedef struct XnLinkUserDataHeader
{
	uint32_t m_nSize;
	uint32_t m_nUserID;
	uint32_t m_nUserStatus; // Flags taken from XnLinkUserStatus
	XnLinkPoint3D m_centerOfMass;
	uint32_t m_nDataElements;
} XnLinkUserDataHeader;

typedef struct XnLinkUserData
{
	XnLinkUserDataHeader m_header;
	XnLinkUserDataElement m_aElements[1];
} XnLinkUserData;

typedef struct XnLinkUserFrameHeader
{
	uint32_t	m_nUsersCount;

	uint32_t	m_nUsersPixelBLOBFormat;
	uint32_t	m_nUsersPixelBLOBSize;

	uint16_t	m_nUsersPixelXRes;
	uint16_t	m_nUsersPixelYRes;
} XnLinkUserFrameHeader;

typedef struct XnLinkUserFrame
{
	XnLinkUserFrameHeader m_header;

	// Using 2 unfixed sized fields, Commented out to avoid direct usage
	//XnUChar			m_UsersPixelsBLOB[1];
	//XnLinkUserData	m_aUsers[1];
} XnLinkUserFrame;

typedef struct XnLinkEESectionHeader
{
	uint32_t m_nMagic; //Should be "LOAD"
	uint32_t m_nSize;
} XnLinkEESectionHeader;

typedef struct XnLinkIDSetHeader
{
	uint16_t m_nFormat;						//Values come from XnLinkIDSetFormat. Currently must be XN_LINK_IDS_LIST_FORMAT_BITSET.
	uint16_t m_nNumGroups;
} XnLinkIDSetHeader;

typedef struct XnLinkIDSetGroupHeader
{
	uint8_t m_nGroupID;
	uint8_t m_nSize;
} XnLinkIDSetGroupHeader;

typedef struct XnLinkIDSetGroup
{
	XnLinkIDSetGroupHeader m_header;
	uint8_t m_idsBitmap[1]; //Contains a bit of 1 for every id in the set, 0 for id not in the set
} XnLinkIDSetGroup;

typedef struct XnLinkCropping
{
	/** TRUE if cropping is turned on, FALSE otherwise. */
	uint8_t m_bEnabled;

	uint8_t m_nReserved1;
	uint8_t m_nReserved2;
	uint8_t m_nReserved3;

	/** Offset in the X-axis, in pixels. */
	uint16_t m_nXOffset;
	/** Offset in the Y-axis, in pixels. */
	uint16_t m_nYOffset;
	/** Number of pixels in the X-axis. */
	uint16_t m_nXSize;
	/** Number of pixels in the Y-axis. */
	uint16_t m_nYSize;
} XnLinkCropping;

typedef struct XnLinkStreamIDsList
{
	uint16_t m_nNumStreamIDs;
	uint16_t m_anStreamIDs[1];
} XnLinkStreamIDsList;

typedef struct XnLinkStreamIDsList XnLinkFrameSyncStreamIDs;

typedef struct XnLinkPropValHeader
{
	uint16_t m_nPropType;	//Values come from XnLinkPropType
	uint16_t m_nPropID;		//Values come from XnLinkInternalPropID
	uint32_t m_nValueSize;
} XnLinkPropValHeader;

typedef struct XnLinkPropVal
{
	XnLinkPropValHeader m_header;
	uint8_t m_value[1];
} XnLinkPropVal;

typedef struct XnLinkPropSet
{
	uint32_t m_nNumProps;
	//Followed by a number of XnLinkPropVal's
	uint8_t m_aData[1];
} XnLinkPropSet;

typedef struct XnLinkBistTest
{
	uint32_t m_nID;
	XnChar m_strName[XN_LINK_MAX_BIST_NAME_LENGTH];
} XnLinkBistTest;

typedef struct XnLinkSupportedBistTests
{
	uint32_t m_nCount;
	XnLinkBistTest m_aTests[1];
} XnLinkSupportedBistTests;

typedef struct XnLinkUploadFileHeader
{
	XnBool m_bOverrideFactorySettings;
} XnLinkUploadFileHeader;

typedef struct XnLinkLeanVersion
{
	uint8_t m_nMajor;
	uint8_t m_nMinor;
	uint16_t m_nReserved;
} XnLinkLeanVersion;

typedef struct XnLinkDetailedVersion
{
	uint8_t m_nMajor;
	uint8_t m_nMinor;
	uint16_t m_nMaintenance;
	uint32_t m_nBuild;
	XnChar m_strModifier[XN_LINK_MAX_VERSION_MODIFIER_LENGTH];
} XnLinkDetailedVersion;

typedef struct XnLinkFileVersion
{
	uint8_t m_nMajor;
	uint8_t m_nMinor;
	uint8_t m_nMaintenance;
	uint8_t m_nBuild;
} XnLinkFileVersion;

typedef struct XnLinkFileEntry
{
	XnChar m_strName[XN_LINK_MAX_FILE_NAME_LENGTH];
	XnLinkFileVersion m_nVersion;
	uint32_t m_nAddress;
	uint32_t m_nSize;
	uint16_t m_nCRC;
	uint16_t m_nZone;
	uint8_t m_nFlags; // bitmap of values from XnLinkFileFlags
	uint8_t m_nReserved1;
	uint8_t m_nReserved2;
	uint8_t m_nReserved3;
} XnLinkFileEntry;

typedef struct XnLinkComponentVersion
{
	XnChar m_strName[XN_LINK_MAX_COMPONENT_NAME_LENGTH];
	XnChar m_strVersion[XN_LINK_MAX_VERSION_LENGTH];
} XnLinkComponentVersion;

typedef struct XnLinkComponentVersionsList
{
	uint32_t m_nCount;
	XnLinkComponentVersion m_components[1];
} XnLinkComponentVersionsList;


typedef struct XnLinkAccCurentParam
{
	float m_nTemperature;
	uint32_t m_nLutTabLine;
	uint16_t m_nValueDC;
	uint16_t m_nValueDac;
	uint16_t m_nVoltage1;
	uint16_t m_nVoltage2;
} XnLinkAccCurentParam;

typedef struct XnLinkDCParam
{
	uint32_t m_nDCvalue;
} XnLinkDCParam ;

typedef struct XnLinkCameraIntrinsics
{
	uint16_t m_nOpticalCenterX;
	uint16_t m_nOpticalCenterY;
	float m_fEffectiveFocalLengthInPixels;
} XnLinkCameraIntrinsics;

typedef struct XnLinkI2CDevice
{
	uint8_t m_nMasterID;
	uint8_t m_nSlaveID;
	uint16_t m_nReserved;
	uint32_t m_nID;
	XnChar m_strName[XN_LINK_MAX_I2C_DEVICE_NAME_LENGTH];
} XnLinkI2CDevice;

typedef struct XnLinkSupportedI2CDevices
{
	uint32_t m_nCount;
	XnLinkI2CDevice m_aI2CDevices[1];
} XnLinkSupportedI2CDevices;

typedef struct XnLinkLogFile
{
	uint8_t m_nID;
	XnChar m_strName[XN_LINK_MAX_LOG_FILE_NAME_LENGTH];
} XnLinkLogFile;

typedef struct XnLinkSupportedLogFiles
{
	uint32_t m_nCount;
	XnLinkLogFile m_aLogFiles[1];
} XnLinkSupportedLogFiles;

typedef struct XnLinkProjectorPulse
{
	uint16_t m_bEnabled;
	uint16_t m_nReserved;
	float  m_nDelay; // Delay between frame start and the start of pulse, in milliseconds
	float  m_nWidth; // Pulse width, in milliseconds
	float  m_nCycle; // in pulse mode: number of frames to skip between projector pulses, from pulse start to next pulse start. in PWM : Cycle time
} XnLinkProjectorPulse;

typedef struct XnLinkTemperatureSensor
{
	uint32_t m_nID;
	uint8_t m_strName[XN_LINK_MAX_SENSOR_NAME_LENGTH];
} XnLinkTemperatureSensor;

typedef struct XnLinkTemperatureSensorsList{
 	uint32_t m_nCount;
 	XnLinkTemperatureSensor m_aSensors[XN_LINK_MAX_TEMPERATURE_SENSORS];
} XnLinkTemperatureSensorsList;

//-----------------------------------------------------------------------
// Command Parameters
//-----------------------------------------------------------------------
typedef struct XnLinkDownloadFileParams
{
	uint16_t m_nZone = 0;
	uint16_t m_nReserved1 = 0;
	XnChar m_strName[XN_LINK_MAX_FILE_NAME_LENGTH];
} XnLinkDownloadFileParams;

typedef struct XnLinkContinueReponseParams
{
	uint16_t m_nOriginalMsgType;
} XnLinkContinueReponseParams;

typedef struct XnLinkWriteI2CParams
{
	uint8_t m_nDeviceID;
	uint8_t m_nAddressSize;
	uint8_t m_nValueSize;
	uint8_t m_nReserved;
	uint32_t m_nAddress;
	uint32_t m_nValue;
	uint32_t m_nMask;
} XnLinkWriteI2CParams;

typedef struct XnLinkReadI2CParams
{
	uint8_t m_nDeviceID;
	uint8_t m_nAddressSize;
	uint8_t m_nValueSize;
	uint8_t m_nReserved;
	uint32_t m_nAddress;
} XnLinkReadI2CParams;

typedef struct XnLinkWriteAHBParams
{
	uint32_t m_nAddress;
	uint32_t m_nValue;
	uint8_t m_nBitOffset; //Offset in bits of value to write within address
	uint8_t m_nBitWidth; //Width in bits of value to write
	uint16_t m_nReserved;
} XnLinkWriteAHBParams;

typedef struct XnLinkReadAHBParams
{
	uint32_t m_nAddress;
	uint8_t m_nBitOffset; //Offset in bits of value to read within address
	uint8_t m_nBitWidth; //Width in bits of value to read
	uint16_t m_nReserved;
} XnLinkReadAHBParams;

typedef struct XnLinkStreamIDsList XnLinkStartStreamingMultiParams;
typedef struct XnLinkStreamIDsList XnLinkStopStreamingMultiParams;

typedef struct XnLinkSetVideoModeParams
{
	XnLinkVideoMode m_videoMode;
} XnLinkSetVideoModeParams;

typedef struct XnLinkCreateStreamParams
{
	uint32_t m_nStreamType = 0;
	XnChar m_strCreationInfo[XN_LINK_MAX_CREATION_INFO_LENGTH];
} XnLinkCreateStreamParams;

typedef XnLinkPropVal XnLinkSetPropParams;

typedef struct XnLinkGetPropParams
{
	uint16_t m_nPropType;	//Values come from XnLinkPropType
	uint16_t m_nPropID;		//Values come from XnLinkInternalPropID
} XnLinkGetPropParams;

typedef struct XnLinkSetMultiPropsParams
{
	uint32_t m_nNumProps;
	//Followed by a number of XnLinkSetPropParams
	uint8_t m_aData[1];
} XnLinkSetMultiPropsParams;

typedef struct XnLinkStartTrackingHandParams
{
	XnLinkPoint3D m_ptPosition;
} XnLinkStartTrackingHandParams;

typedef struct XnLinkStopTrackingHandParams
{
	uint32_t m_nUserID;
} XnLinkStopTrackingHandParams;

typedef struct XnLinkAddGestureParams
{
	uint32_t m_nGestureType; //Values come from XnLinkGestureType
	XnLinkBoundingBox3D m_boundingBox;
} XnLinkAddGestureParams;

typedef struct XnLinkRemoveGestureParams
{
	uint32_t m_nGestureType; //Values come from XnLinkGestureType
} XnLinkRemoveGestureParams;

typedef struct XnLinkExecuteBistParams
{
	uint32_t m_nID;
} XnLinkExecuteBistParams;

typedef struct XnLinkFormatZoneParams
{
	uint32_t m_nZone; //0 or 1
} XnLinkFormatZoneParams;

typedef struct XnLinkLogOpenCloseParams
{
	uint8_t m_nID;
} XnLinkLogOpenCloseParams;

typedef struct XnLinkGetTemperatureParams
{
	uint32_t m_nID;
} XnLinkGetTemperatureParams;

typedef struct XnLinkGetDebugDataParams
{
	uint32_t m_nID;
} XnLinkGetDebugDataParams;

//-----------------------------------------------------------------------
// Command Response Structures
//-----------------------------------------------------------------------
typedef struct XnLinkTemperatureResponse
{
	uint32_t m_nID;
	float value;
} XnLinkTemperatureResponse;

typedef struct XnLinkResponseInfo
{
	uint16_t m_nResponseCode;
	uint16_t m_nReserverd;
} XnLinkResponseInfo;

typedef struct XnLinkResponseHeader
{
	XnLinkPacketHeader m_header;
	XnLinkResponseInfo m_responseInfo;
} XnLinkResponseHeader;

typedef struct XnLinkResponsePacket
{
	XnLinkResponseHeader m_responseHeader;
	uint8_t m_data[1];
} XnLinkResponsePacket;

typedef struct XnLinkReadI2CResponse
{
	uint32_t m_nValue;
} XnLinkReadI2CResponse;

typedef struct XnLinkReadAHBResponse
{
	uint32_t m_nValue;
} XnLinkReadAHBResponse;

typedef XnLinkSupportedVideoModes XnLinkGetSupportedVideoModesResponse;

typedef struct XnLinkGetVideoModeResponse
{
	XnLinkVideoMode m_videoMode;
} XnLinkGetVideoModeResponse;

typedef struct XnLinkEnumerateStreamsResponse
{
	uint32_t m_nNumStreams;
	XnLinkStreamInfo m_streamInfos[1];
} XnLinkEnumerateStreamsResponse;

typedef struct XnLinkCreateStreamResponse
{
	uint16_t m_nStreamID;
	uint16_t m_nEndpointID;
} XnLinkCreateStreamResponse;

typedef struct XnLinkPropVal XnLinkGetPropResponse;

typedef struct XnLinkPropSet XnLinkGetAllPropsResponse;

typedef struct XnLinkGetShiftToDepthConfigResponse
{
	XnLinkShiftToDepthConfig m_config;
} XnLinkGetShiftToDepthConfigResponse;

typedef struct XnLinkEnumerateAvailableGesturesResponse
{
	uint32_t m_nGestures;
	uint32_t m_nProgressSupported;
	uint32_t m_nCurrentlyActive;
} XnLinkEnumerateAvailableGesturesResponse;

typedef struct XnLinkEnumerateActiveGesturesResponse
{
	uint32_t m_nGestures;
} XnLinkEnumerateActiveGesturesResponse;

typedef struct XnLinkSetSkeletonProfileParams
{
	uint32_t m_nProfile;
} XnLinkSetSkeletonProfileParams;

typedef struct XnLinkSetSkeletonJointStateParams
{
	uint16_t m_nJoint;
	uint16_t m_nState;
} XnLinkSetSkeletonJointStateParams;

typedef struct XnLinkRequestSkeletonCalibrationParams
{
	uint32_t m_nUserID;
	uint32_t m_bForce;
} XnLinkRequestSkeletonCalibrationParams;

typedef struct XnLinkAbortSkeletonCalibrationParams
{
	uint32_t m_nUserID;
} XnLinkAbortSkeletonCalibrationParams;

typedef struct XnLinkStartSkeletonTrackingParams
{
	uint32_t m_nUserID;
} XnLinkStartSkeletonTrackingParams;

typedef struct XnLinkStopSkeletonTrackingParams
{
	uint32_t m_nUserID;
} XnLinkStopSkeletonTrackingParams;

typedef struct XnLinkResetSkeletonTrackingParams
{
	uint32_t m_nUserID;
} XnLinkResetSkeletonTrackingParams;

typedef struct XnLinkStartPoseDetectionParams
{
	uint32_t m_nUserID;
	uint32_t m_nPose;
} XnLinkStartPoseDetectionParams;

typedef struct XnLinkStopPoseDetectionParams
{
	uint32_t m_nUserID;
} XnLinkStopPoseDetectionParams;

typedef struct XnLinkSaveSkeletonCalibrationDataParams
{
	uint32_t m_nUserID;
	uint32_t m_nSlot;
} XnLinkSaveSkeletonCalibrationDataParams;

typedef struct XnLinkLoadSkeletonCalibrationDataParams
{
	uint32_t m_nUserID;
	uint32_t m_nSlot;
} XnLinkLoadSkeletonCalibrationDataParams;

typedef struct XnLinkIsSkeletonCalibrationSlotTakenParams
{
	uint32_t m_nSlot;
} XnLinkIsSkeletonCalibrationSlotTakenParams;

typedef struct XnLinkIsSkeletonCalibrationSlotTakenResponse
{
	uint32_t m_nTaken;
} XnLinkIsSkeletonCalibrationSlotTakenResponse;

typedef struct XnLinkClearSkeletonCalibrationSlotParams
{
	uint32_t m_nSlot;
} XnLinkClearSkeletonCalibrationSlotParams;

typedef struct XnLinkGetSkeletonCalibrationDataParams
{
	uint32_t m_nUserID;
} XnLinkGetSkeletonCalibrationDataParams;

typedef struct XnLinkSetSkeletonCalibrationDataParamsHeader
{
	uint32_t m_nUserID;
	uint32_t m_nDataSize;
} XnLinkSetSkeletonCalibrationDataParamsHeader;

typedef struct XnLinkSetSkeletonCalibrationDataParams
{
	XnLinkSetSkeletonCalibrationDataParamsHeader m_header;
	uint8_t m_aData[1];
} XnLinkSetSkeletonCalibrationDataParams;

typedef struct XnLinkSetLogMaskSeverityParams
{
	XnChar m_strMask[XN_LINK_MAX_LOG_MASK_LENGTH];
	uint32_t m_nMinSeverity; // values from XnLogSeverity
} XnLinkSetLogMaskSeverityParams;

typedef struct XnLinkGetLogMaskSeverityParams
{
	XnChar m_strMask[XN_LINK_MAX_LOG_MASK_LENGTH];
} XnLinkGetLogMaskSeverityParams;

typedef struct XnLinkGetLogMaskSeverityResponse
{
	uint32_t m_nMinSeverity; // values from XnLogSeverity
} XnLinkGetLogMaskSeverityResponse;

typedef struct XnLinkExecuteBistResponse // Entire data in this struct is system- and test-specific
{
	uint32_t m_nErrorCode; // 0 for success
	uint32_t m_nExtraDataSize;
	XnUChar m_ExtraData[1];
} XnLinkExecuteBistResponse;

typedef struct XnLinkGetFileListResponse
{
	uint32_t m_nCount;
	XnLinkFileEntry m_aFileEntries[1];
} XnLinkGetFileListResponse;

typedef struct XnLinkDebugDataResponseHeader
{
	uint16_t m_nDataID;		//Values come from XnLinkInternalPropID
	uint16_t m_nValueSize;
} XnLinkDebugDataResponseHeader;

typedef struct XnLinkDebugDataResponse
{
	XnLinkDebugDataResponseHeader m_header;
	uint8_t m_data[1];
} XnLinkDebugDataResponse;

typedef struct XnLinkBootStatus
{
	uint8_t  m_nZone;		    //Values come from XnLinkBootZone
	uint8_t  m_nErrorCode;	    //Values come from XnLinkBootErrorCode
	uint16_t m_nReserved;
} XnLinkBootStatus;

//-----------------------------------------------------------------------
// Device Notifications
//-----------------------------------------------------------------------

#pragma pack (pop)

#endif // XNLINKPROTO_H
