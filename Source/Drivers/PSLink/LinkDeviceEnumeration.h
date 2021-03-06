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
#ifndef LINKDEVICEENUMERATION_H
#define LINKDEVICEENUMERATION_H

#include <vector>

#include <XnStringsHash.h>
#include <XnEvent.h>
#include <OniCTypes.h>
#include <XnUSB.h>

class LinkDeviceEnumeration
{
public:
	typedef xnl::Event1Arg<const OniDeviceInfo&> DeviceConnectivityEvent;

	static XnStatus Initialize();
	static void Shutdown();

	static OniDeviceInfo* GetDeviceInfo(const char* uri);

	static DeviceConnectivityEvent::Interface& ConnectedEvent() { return ms_connectedEvent; }
	static DeviceConnectivityEvent::Interface& DisconnectedEvent() { return ms_disconnectedEvent; }

	static XnStatus EnumerateSensors(OniDeviceInfo* aDevices, uint32_t* pnCount);

private:
	typedef struct XnUsbId
	{
		uint16_t vendorID;
		uint16_t productID;
	} XnUsbId;

	typedef xnl::XnStringsHashT<OniDeviceInfo> DevicesHash;

	static void XN_CALLBACK_TYPE OnConnectivityEventCallback(XnUSBEventArgs* pArgs, void* pCookie);
	static void OnConnectivityEvent(const char* uri, XnUSBEventType eventType, XnUsbId usbId);

	static bool ms_initialized;
	static DeviceConnectivityEvent ms_connectedEvent;
	static DeviceConnectivityEvent ms_disconnectedEvent;

	static XnUsbId ms_supportedProducts[];
	static uint32_t ms_supportedProductsCount;
	static DevicesHash ms_devices;
	static std::vector<XnRegistrationHandle> ms_aRegistrationHandles;
	static XN_CRITICAL_SECTION_HANDLE ms_lock;
};

#endif // LINKDEVICEENUMERATION_H
