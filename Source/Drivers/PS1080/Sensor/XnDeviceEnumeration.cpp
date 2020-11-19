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
#include <string>
#include <unordered_map>
#include <vector>

#include "XnDeviceEnumeration.h"
#include <XnUSB.h>

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
static XnBool ms_initialized = FALSE;
static XnDeviceEnumeration::DeviceConnectivityEvent ms_connectedEvent;
static XnDeviceEnumeration::DeviceConnectivityEvent ms_disconnectedEvent;
static std::unordered_map<std::string, OniDeviceInfo> ms_devices;
static std::vector<XnRegistrationHandle> ms_aRegistrationHandles;
static XN_CRITICAL_SECTION_HANDLE ms_lock;

typedef struct XnUsbId
{
	XnUInt16 vendorID;
	XnUInt16 productID;
} XnUsbId;


static XnUsbId ms_supportedProducts[] =
{
	{ 0x1D27, 0x0500 },
	{ 0x1D27, 0x0600 },
	{ 0x1D27, 0x0601 },
	{ 0x1D27, 0x0609 },
};

static XnUInt32 ms_supportedProductsCount = sizeof(ms_supportedProducts) / sizeof(ms_supportedProducts[0]);

static void XN_CALLBACK_TYPE OnConnectivityEventCallback(XnUSBEventArgs* pArgs, void* pCookie);
static void OnConnectivityEvent(const XnChar* uri, XnUSBEventType eventType, XnUsbId usbId);

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnDeviceEnumeration::Initialize()
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (ms_initialized)
	{
		return XN_STATUS_OK;
	}

	nRetVal = xnUSBInit();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSCreateCriticalSection(&ms_lock);
	XN_IS_STATUS_OK(nRetVal);

	// check all products
	for (XnUInt32 i = 0; i < ms_supportedProductsCount; ++i)
	{
		const XnUSBConnectionString* astrDevicePaths = NULL;
		XnUInt32 nCount = 0;

		// register for USB events
		XnRegistrationHandle hRegistration = NULL;
		nRetVal = xnUSBRegisterToConnectivityEvents(ms_supportedProducts[i].vendorID, ms_supportedProducts[i].productID, OnConnectivityEventCallback, &ms_supportedProducts[i], &hRegistration);
		XN_IS_STATUS_OK(nRetVal);

		ms_aRegistrationHandles.push_back(hRegistration);

		// and enumerate for existing ones
		nRetVal = xnUSBEnumerateDevices(ms_supportedProducts[i].vendorID, ms_supportedProducts[i].productID, &astrDevicePaths, &nCount);
		XN_IS_STATUS_OK(nRetVal);

		for (XnUInt32 j = 0; j < nCount; ++j)
		{
			OnConnectivityEvent(astrDevicePaths[j], XN_USB_EVENT_DEVICE_CONNECT, ms_supportedProducts[i]);
		}

		xnUSBFreeDevicesList(astrDevicePaths);
	}

	ms_initialized = TRUE;

	return XN_STATUS_OK;
}

void XnDeviceEnumeration::Shutdown()
{
	if (ms_initialized)
	{
		for (XnUInt32 i = 0; i < ms_aRegistrationHandles.size(); ++i)
		{
			xnUSBUnregisterFromConnectivityEvents(ms_aRegistrationHandles[i]);
		}
		ms_aRegistrationHandles.clear();
		ms_connectedEvent.Clear();
		ms_disconnectedEvent.Clear();

		xnOSCloseCriticalSection(&ms_lock);

		xnUSBShutdown();

		ms_devices.clear();

		ms_initialized = FALSE;
	}
}

XnDeviceEnumeration::DeviceConnectivityEvent::Interface& XnDeviceEnumeration::ConnectedEvent()
{
	return ms_connectedEvent;
}

XnDeviceEnumeration::DeviceConnectivityEvent::Interface& XnDeviceEnumeration::DisconnectedEvent()
{
	return ms_disconnectedEvent;
}

static void OnConnectivityEvent(const XnChar* uri, XnUSBEventType eventType, XnUsbId usbId)
{
	xnl::AutoCSLocker lock(ms_lock);

	if (eventType == XN_USB_EVENT_DEVICE_CONNECT)
	{
		if (ms_devices.find(uri) == ms_devices.end())
		{
			OniDeviceInfo deviceInfo;
			deviceInfo.usbVendorId = usbId.vendorID;
			deviceInfo.usbProductId = usbId.productID;
			xnOSStrCopy(deviceInfo.uri, uri, sizeof(deviceInfo.uri));
			xnOSStrCopy(deviceInfo.vendor, "PrimeSense", sizeof(deviceInfo.vendor));
			xnOSStrCopy(deviceInfo.name, "PS1080", sizeof(deviceInfo.name));

			// add it to hash
			ms_devices.insert({uri, deviceInfo});

			// raise event
			ms_connectedEvent.Raise(deviceInfo);
		}
	}
	else if (eventType == XN_USB_EVENT_DEVICE_DISCONNECT)
	{
		std::unordered_map<std::string, OniDeviceInfo>::iterator it = ms_devices.find(uri);
		if (it != ms_devices.end())
		{
			ms_disconnectedEvent.Raise(it->second);
			ms_devices.erase(it);
		}
	}
}

void XN_CALLBACK_TYPE OnConnectivityEventCallback(XnUSBEventArgs* pArgs, void* pCookie)
{
	XnUsbId usbId = *(XnUsbId*)pCookie;
	OnConnectivityEvent(pArgs->strDevicePath, pArgs->eventType, usbId);
}

XnStatus XnDeviceEnumeration::IsSensorLowBandwidth(const XnChar* uri, XnBool* pbIsLowBandwidth)
{
	*pbIsLowBandwidth = FALSE;

#if (XN_PLATFORM == XN_PLATFORM_WIN32)
	XnChar cpMatchString[XN_FILE_MAX_PATH];

	// WAVI Detection:
	//   Normal USB string: \\?\usb#vid_1d27&pid_0600#6&XXXXXXXX&0&2
	//   WAVI USB String:   \\?\usb#vid_1d27&pid_0600#1&1d270600&2&3
	//                                                  ^^^^^^^^ - VID/PID is always repeated here with the WAVI.
	//                                                             Regular USB devices will have the port/hub chain instead.
	if ((xnOSStrCaseCmp(uri, "\\\\?\\usb#vid_") >= 0) && (xnOSStrLen(uri) > 25))
	{
		strncpy(&cpMatchString[0], &uri[12], 4); //VID
		strncpy(&cpMatchString[4], &uri[21], 4); //PID
		cpMatchString[8] = 0;

		if (strstr ((char*)uri,cpMatchString) != 0)
		{
			*pbIsLowBandwidth = TRUE;
		}
	}
#else
	XN_REFERENCE_VARIABLE(uri);
#endif

	return (XN_STATUS_OK);
}

OniDeviceInfo* XnDeviceEnumeration::GetDeviceInfo(const XnChar* uri)
{
	xnl::AutoCSLocker lock(ms_lock);

	std::unordered_map<std::string, OniDeviceInfo>::iterator it = ms_devices.find(uri);
	if (it != ms_devices.end())
	{
		return &(it->second);
	}
	else
	{
		return NULL;
	}
}

