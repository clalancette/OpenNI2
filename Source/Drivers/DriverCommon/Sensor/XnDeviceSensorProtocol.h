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
#ifndef XNDEVICESENSORPROTOCOL_H
#define XNDEVICESENSORPROTOCOL_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceSensor.h"
#include "XnHostProtocol.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_SENSOR_PROTOCOL_SENSOR_CLOCK_LENGTH	2
#define XN_SENSOR_PROTOCOL_SENSOR_VER_LENGTH 1

#define XN_SENSOR_PROTOCOL_RESPONSE_DEPTH_START 					0x7100
#define XN_SENSOR_PROTOCOL_RESPONSE_DEPTH_BUFFER					0x7200
#define XN_SENSOR_PROTOCOL_RESPONSE_DEPTH_END						0x7500

#define XN_SENSOR_PROTOCOL_RESPONSE_IMAGE_START 					0x8100
#define XN_SENSOR_PROTOCOL_RESPONSE_IMAGE_BUFFER					0x8200
#define XN_SENSOR_PROTOCOL_RESPONSE_IMAGE_END						0x8500

#define XN_SENSOR_PROTOCOL_RESPONSE_AUDIO_BUFFER					0x9200

#define XN_SENSOR_PROTOCOL_RESPONSE_GMC 							0xa200

#define XN_SENSOR_PROTOCOL_RESPONSE_GMC_DEBUG						0xb200
#define XN_SENSOR_PROTOCOL_RESPONSE_GMC_DEBUG_END					0xb500

#define XN_SENSOR_PROTOCOL_RESPONSE_WAVELENGTH_CORRECTION_DEBUG 	0xc200

#define XN_SENSOR_PROTOCOL_RESPONSE_TEC_DEBUG						0xd200

#define XN_SENSOR_PROTOCOL_RESPONSE_NESA_DEBUG						0xd201

#define XN_SENSOR_PROTOCOL_RESPONSE_PROJECTOR_FAULT_EVENT			0xdead

#define XN_SENSOR_PROTOCOL_RESPONSE_OVERHEAT						0xf31f

#define XN_SENSOR_PROTOCOL_DEBUG_DATA_EP1							0xdb01
#define XN_SENSOR_PROTOCOL_DEBUG_DATA_EP2							0xdb02

#define XN_SENSOR_PROTOCOL_MAX_BUFFER_SIZE 1024*1024
#define XN_SENSOR_PROTOCOL_USB_BUFFER_SIZE 4*1024*1024
#define XN_SENSOR_PROTOCOL_USB_MAX_ZERO_READ_COUNTER 5
#define XN_SENSOR_PROTOCOL_READ_SLEEP 100
#define XN_SENSOR_PROTOCOL_READ_MAX_TRIES 30

#define XN_SENSOR_PROTOCOL_MAX_RECONFIG_TRIES 10

#define XN_SENSOR_PROTOCOL_START_CAPTURE_TRIES 5
#define XN_SENSOR_PROTOCOL_START_CAPTURE_SLEEP 1000

#define XN_SENSOR_PROTOCOL_GMC_MAX_POINTS_IN_PACKET 100

/** the number of points to accumulate before processing takes place. */
#define XN_GMC_MIN_COUNT_FOR_RUNNING	1000

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------
#pragma pack (push, 1)

typedef struct XnSensorProtocolResponseHeader
{
	uint16_t nMagic;
	uint16_t nType;
	uint16_t nPacketID;
	uint16_t nBufSize;
	uint32_t nTimeStamp;
} XnSensorProtocolResponseHeader;
#pragma pack (pop) // Undo the pack change...

typedef enum
{
	XN_WAITING_FOR_CONFIGURATION,
	XN_IGNORING_GARBAGE,
	XN_LOOKING_FOR_MAGIC,
	XN_PACKET_HEADER,
	XN_PACKET_DATA
} XnMiniPacketState;

typedef struct XnSpecificUsbDeviceState
{
	XnMiniPacketState State;
	XnSensorProtocolResponseHeader CurrHeader;
	uint32_t nMissingBytesInState;
} XnSpecificUsbDeviceState;

typedef struct XnSpecificUsbDevice
{
	XnDevicePrivateData* pDevicePrivateData;
	XnUsbConnection* pUsbConnection;
	uint32_t nIgnoreBytes;
	uint32_t nChunkReadBytes;
	uint32_t nNumberOfBuffers;
	XnSpecificUsbDeviceState CurrState;
	uint32_t nTimeout;
} XnSpecificUsbDevice;


//---------------------------------------------------------------------------
// Functions Declaration
//---------------------------------------------------------------------------
bool XN_CALLBACK_TYPE XnDeviceSensorProtocolUsbEpCb(unsigned char* pBuffer, uint32_t nBufferSize, void* pCallbackData);

XnStatus XnCalculateExpectedImageSize(XnDevicePrivateData* pDevicePrivateData, uint32_t* pnExpectedSize);
void XnProcessUncompressedDepthPacket(XnSensorProtocolResponseHeader* pCurrHeader, unsigned char* pData, uint32_t nDataSize, bool bEOP, XnSpecificUsbDevice* pSpecificDevice);
XnStatus XnDeviceSensorProtocolUpdateImageProcessor(XnDevicePrivateData* pDevicePrivateData);

XN_THREAD_PROC XnDeviceSensorProtocolScriptThread(XN_THREAD_PARAM pThreadParam);

#endif // XNDEVICESENSORPROTOCOL_H
