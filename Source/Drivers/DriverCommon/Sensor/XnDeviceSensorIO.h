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
#ifndef XNDEVICESENSORIO_H
#define XNDEVICESENSORIO_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnUSB.h>
#include <XnStreamParams.h>
#include <XnDevice.h>
#include "XnFirmwareInfo.h"
#include <OniCTypes.h>
#include <XnStringsHash.h>

//---------------------------------------------------------------------------
// Structures & Enums
//---------------------------------------------------------------------------
typedef struct XnUsbConnection
{
	XN_USB_EP_HANDLE UsbEp;

	bool bIsOpen;
	uint8_t* pUSBBuffer;
	uint32_t nUSBBufferReadOffset;
	uint32_t nUSBBufferWriteOffset;
	uint32_t nMaxPacketSize;
} XnUsbConnection;

typedef struct XnUsbControlConnection
{
	/* When true, control connection is implemented using bulk end points. */
	bool bIsBulk;
	XN_USB_EP_HANDLE ControlOutConnectionEp;
	XN_USB_EP_HANDLE ControlInConnectionEp;
} XnUsbControlConnection;

typedef struct XN_SENSOR_HANDLE
{
	XN_USB_DEV_HANDLE USBDevice;

	XnUsbControlConnection ControlConnection;
	XnUsbConnection DepthConnection;
	XnUsbConnection ImageConnection;
	XnUsbConnection MiscConnection;
	uint8_t nBoardVer;
} XN_SENSOR_HANDLE;

//---------------------------------------------------------------------------
// Functions Declaration
//---------------------------------------------------------------------------
class XnSensorIO final
{
public:
	XnSensorIO(XN_SENSOR_HANDLE* pSensorHandle);
	~XnSensorIO();

	XnStatus OpenDevice(const char* strPath);

	XnStatus OpenDataEndPoints(XnSensorUsbInterface nInterface, const XnFirmwareInfo& fwInfo);

	XnSensorUsbInterface GetCurrentInterface(const XnFirmwareInfo& fwInfo) const;

	XnStatus CloseDevice();

	inline bool IsMiscEndpointSupported() const { return m_bMiscSupported; }
	inline bool IsLowBandwidth() const { return m_bIsLowBandwidth; }

	const char* GetDevicePath();

private:
	XN_SENSOR_HANDLE* m_pSensorHandle;
	bool m_bMiscSupported;
	char m_strDeviceName[XN_DEVICE_MAX_STRING_LENGTH];
	bool m_bIsLowBandwidth;
};

#endif // XNDEVICESENSORIO_H
