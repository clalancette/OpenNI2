/*****************************************************************************
*                                                                            *
*  PrimeSense PSCommon Library                                               *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PSCommon.                                            *
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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <algorithm>
#include <list>

#include <XnUSB.h>

#if defined(XN_PLATFORM_MACOSX_XCODE) || defined(XN_PLATFORM_IOS)
#include <libusb.h>
#else
#include <libusb-1.0/libusb.h>
#endif

#include "XnLinuxUSB.h"
#include "../XnUSBInternal.h"
#include <XnOS.h>
#include <XnLog.h>
#include <XnOSCpp.h>

#if (XN_PLATFORM == XN_PLATFORM_LINUX_X86 || XN_PLATFORM == XN_PLATFORM_LINUX_ARM)
#include <libudev.h>
#define XN_USE_UDEV
#endif

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnUSBEventCallback
{
	XnUSBDeviceCallbackFunctionPtr pFunc;
	void* pCookie;

	// What kind of device are we hooking on
	uint16_t nVendorID;
	uint16_t nProductID;
} XnUSBEventCallback;

static std::list<XnUSBEventCallback*> g_connectivityEvent;

#ifdef XN_USE_UDEV
typedef struct XnUSBConnectedDevice
{
	uint16_t nVendorID;
	uint16_t nProductID;

	uint8_t nBusNum;
	uint8_t nDevNum;

	// /dev/bus/usb/001/016
	char strNode[XN_FILE_MAX_PATH + 1];
	// id27/0601@1/16
	char strDevicePath[XN_FILE_MAX_PATH + 1];
} XnUSBConnectedDevice;

static std::list<XnUSBConnectedDevice*> g_connectedDevices;

XN_THREAD_HANDLE g_hUDEVThread = NULL;
bool g_bShouldRunUDEVThread = false;
#endif

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define MAX_DEVPATH_LENGTH 256
#define USB_TYPE_STANDARD		(0x00 << 5)
#define USB_TYPE_CLASS			(0x01 << 5)
#define USB_TYPE_VENDOR			(0x02 << 5)
#define USB_ENDPOINT_IN			0x80

#define XN_MASK_USB "xnUSB"

#define XN_USB_HANDLE_EVENTS_TIMEOUT 500

#define XN_VALIDATE_DEVICE_HANDLE(x)				\
	if (x == NULL)									\
		return (XN_STATUS_USB_DEVICE_NOT_VALID);

#define XN_VALIDATE_EP_HANDLE(x)					\
	if (x == NULL)									\
		return (XN_STATUS_USB_ENDPOINT_NOT_VALID);

struct xnUSBInitData
{
	libusb_context* pContext;
	XN_THREAD_HANDLE hThread;
	bool bShouldThreadRun;
	uint32_t nOpenDevices;
	XN_CRITICAL_SECTION_HANDLE hLock;
} g_InitData = {NULL, NULL, false, 0, NULL};

XnStatus xnUSBPlatformSpecificShutdown();

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
#ifdef XN_USE_UDEV
void xnUSBDeviceConnected(struct udev_device *dev)
{
	XnUSBConnectedDevice *pConnected;
	pConnected = XN_NEW(XnUSBConnectedDevice);

	pConnected->nVendorID  = strtoul(udev_device_get_sysattr_value(dev,"idVendor"),  NULL, 16);
	pConnected->nProductID = strtoul(udev_device_get_sysattr_value(dev,"idProduct"), NULL, 16);
	pConnected->nBusNum    = strtoul(udev_device_get_sysattr_value(dev,"busnum"),    NULL, 10);
	pConnected->nDevNum    = strtoul(udev_device_get_sysattr_value(dev,"devnum"),    NULL, 10);

	// copy the device node path aside, to be used upon removal
	xnOSStrCopy(pConnected->strNode, udev_device_get_devnode(dev), XN_FILE_MAX_PATH);

	// generate our unique URI
	snprintf(pConnected->strDevicePath, XN_FILE_MAX_PATH,
				"%04hx/%04hx@%hhu/%hhu",
				pConnected->nVendorID,
				pConnected->nProductID,
				pConnected->nBusNum,
				pConnected->nDevNum);

	// add the device to the connectedDevices List
	g_connectedDevices.push_back(pConnected);

	// notify the proper events of the connection
	for (std::list<XnUSBEventCallback*>::iterator it = g_connectivityEvent.begin(); it != g_connectivityEvent.end(); ++it)
	{
		XnUSBEventCallback* pCallback = *it;

		if (pCallback->nVendorID == pConnected->nVendorID && pCallback->nProductID == pConnected->nProductID)
		{
			XnUSBEventArgs args;
			args.strDevicePath = pConnected->strDevicePath;
			args.eventType = XN_USB_EVENT_DEVICE_CONNECT;
			pCallback->pFunc(&args, pCallback->pCookie);
		}
	}
}

void xnUSBDeviceDisconnected(struct udev_device *dev)
{
	// find dev in the connected devices' list
	XnUSBConnectedDevice *pConnected = NULL;
	for (std::list<XnUSBConnectedDevice*>::iterator it = g_connectedDevices.begin(); it != g_connectedDevices.end(); ++it)
	{
		if (!xnOSStrCmp(((XnUSBConnectedDevice *)*it)->strNode, udev_device_get_devnode(dev)))
		{
			pConnected = *it;
			break;
		}
	}

	if (!pConnected)
	{
		// got disconnection of an unknown device. not good.
		xnLogWarning(XN_MASK_USB, "Got device disconnection event - for an unknown device!");
		return;
	}

	// notify the proper events of the disconnection
	for (std::list<XnUSBEventCallback*>::iterator it = g_connectivityEvent.begin(); it != g_connectivityEvent.end(); ++it)
	{
		XnUSBEventCallback* pCallback = *it;

		if (pCallback->nVendorID == pConnected->nVendorID && pCallback->nProductID == pConnected->nProductID)
		{
			XnUSBEventArgs args;
			args.strDevicePath = pConnected->strDevicePath;
			args.eventType = XN_USB_EVENT_DEVICE_DISCONNECT;
			pCallback->pFunc(&args, pCallback->pCookie);
		}
	}

	// remove the device from connectedDevices List
	std::list<XnUSBConnectedDevice*>::iterator it = std::find(g_connectedDevices.begin(), g_connectedDevices.end(), pConnected);
	if (it != g_connectedDevices.end())
	{
		g_connectedDevices.erase(it);
	}
	XN_DELETE(pConnected);
}

XN_THREAD_PROC xnUSBUDEVEventsThread(XN_THREAD_PARAM /*pThreadParam*/)
{
	struct udev *udev;
	struct udev_device *dev;
   	struct udev_monitor *mon;
	int fd;

	/* Create the udev object */
	udev = udev_new();
	if (!udev)
	{
		printf("Can't create udev\n");
		exit(1);
	}

	/* This section sets up a monitor which will report events when
	   devices attached to the system change.  Events include "add",
	   "remove", "change", "online", and "offline".

	   This section sets up and starts the monitoring. Events are
	   polled for (and delivered) later on.

	   It is important that the monitor be set up before the call to
	   udev_enumerate_scan_devices() so that events (and devices) are
	   not missed.  For example, if enumeration happened first, there
	   would be no event generated for a device which was attached after
	   enumeration but before monitoring began.

	   Note that a filter is added so that we only get events for
	   "usb/usb_device" devices. */

	/* Set up a monitor to monitor "usb_device" devices */
	mon = udev_monitor_new_from_netlink(udev, "udev");
	udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", "usb_device");
	udev_monitor_enable_receiving(mon);
	/* Get the file descriptor (fd) for the monitor.
	   This fd will get passed to select() */
	fd = udev_monitor_get_fd(mon);

	//////////////////////////////////////////////////////////////////////////

	/* Enumerate the currently connected devices and store them,
	   so we can notify of their disconnection */

	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;


	enumerate = udev_enumerate_new(udev);
	/* Create a list of the devices.
	   Note that it's not possible to match by "devtype="usb_device"",
	   as in monitor filter, but this filter combination seems to do the job... */
	udev_enumerate_add_match_subsystem(enumerate, "usb");
	udev_enumerate_add_match_sysattr(enumerate, "idVendor", NULL);
	udev_enumerate_add_match_sysattr(enumerate, "idProduct", NULL);
	udev_enumerate_add_match_sysattr(enumerate, "busnum", NULL);
	udev_enumerate_add_match_sysattr(enumerate, "devnum", NULL);

	udev_enumerate_scan_devices(enumerate);
	devices = udev_enumerate_get_list_entry(enumerate);
	/* udev_list_entry_foreach is a macro which expands to
	   a loop. The loop will be executed for each member in
	   devices, setting dev_list_entry to a list entry
	   which contains the device's path in /sys. */
	udev_list_entry_foreach(dev_list_entry, devices)
	{
		const char *path;

		/* Get the filename of the /sys entry for the device
		   and create a udev_device object (dev) representing it */
		path = udev_list_entry_get_name(dev_list_entry);
		dev = udev_device_new_from_syspath(udev, path);

		/* Notify as if it was just connected */
		// note - it's better that connectivity events register AFTER this point,
		//        so they don't get notified of already connected devices.
		xnUSBDeviceConnected(dev);

		udev_device_unref(dev);
	}
	/* Free the enumerator object */
	udev_enumerate_unref(enumerate);

	//////////////////////////////////////////////////////////////////////////

	/* Begin polling for udev events. Events occur when devices
	   attached to the system are added, removed, or change state.
	   udev_monitor_receive_device() will return a device
	   object representing the device which changed and what type of
	   change occured.

	   The select() system call is used to ensure that the call to
	   udev_monitor_receive_device() will not block.

	   The monitor was set up earler in this file, and monitoring is
	   already underway.

	   This section will run continuously, calling usleep() at the end
	   of each pass. This is to demonstrate how to use a udev_monitor
	   in a non-blocking way. */
	while (g_bShouldRunUDEVThread)
	{
		/* Set up the call to select(). In this case, select() will
		   only operate on a single file descriptor, the one
		   associated with our udev_monitor. Note that the timeval
		   object is set to 0, which will cause select() to not
		   block. */
		fd_set fds;
		struct timeval tv;
		int ret;

		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 250 * 1000;

		ret = select(fd+1, &fds, NULL, NULL, &tv);

		/* Check if our file descriptor has received data. */
		if (ret > 0 && FD_ISSET(fd, &fds))
		{
			/* Make the call to receive the device.
			   select() ensured that this will not block. */
			dev = udev_monitor_receive_device(mon);
			if (dev)
			{
				const char *action = udev_device_get_action(dev);

				if (!xnOSStrCmp(action, "add"))
				{
					xnUSBDeviceConnected(dev);
				}
				else if (!xnOSStrCmp(action, "remove"))
				{
					xnUSBDeviceDisconnected(dev);
				}
				//note - handle the other events? "change" event might be useful...

				// now release dev
				udev_device_unref(dev);
			}
			else
			{
				xnLogWarning(XN_MASK_USB, "No Device from udev_monitor_receive_device(). An error occured.");
			}
		}
	}
	udev_monitor_unref(mon);
	udev_unref(udev);

	for (std::list<XnUSBConnectedDevice*>::iterator it = g_connectedDevices.begin(); it != g_connectedDevices.end(); ++it)
	{
		XN_DELETE(*it);
	}

	XN_THREAD_PROC_RETURN(XN_STATUS_OK);
}
#endif

XN_THREAD_PROC xnUSBHandleEventsThread(XN_THREAD_PARAM /*pThreadParam*/)
{
	// init timeout
	struct timeval timeout;
	timeout.tv_sec = XN_USB_HANDLE_EVENTS_TIMEOUT / 1000;
	timeout.tv_usec = XN_USB_HANDLE_EVENTS_TIMEOUT % 1000;

	while (g_InitData.bShouldThreadRun)
	{
		// let libusb process its asynchronous events
		libusb_handle_events_timeout(g_InitData.pContext, &timeout);
	}

	XN_THREAD_PROC_RETURN(XN_STATUS_OK);
}

XnStatus xnUSBPlatformSpecificInit()
{
	xnLogVerbose(XN_MASK_USB, "Initializing USB...");

	// initialize the library
	int rc = libusb_init(&g_InitData.pContext);
	if (rc != 0)
	{
		return (XN_STATUS_USB_INIT_FAILED);
	}

	XnStatus nRetVal = xnOSCreateCriticalSection(&g_InitData.hLock);
	XN_IS_STATUS_OK(nRetVal);

#ifdef XN_USE_UDEV
	// initialize the UDEV Events thread
	g_bShouldRunUDEVThread = true;
	nRetVal = xnOSCreateThread(xnUSBUDEVEventsThread, NULL, &g_hUDEVThread);
	if (nRetVal != XN_STATUS_OK)
	{
		g_hUDEVThread = NULL;
		g_bShouldRunUDEVThread = false;
		// clean-up
		xnUSBPlatformSpecificShutdown();
		return nRetVal;
	}
#endif

	xnLogInfo(XN_MASK_USB, "USB is initialized.");
	return (XN_STATUS_OK);
}

XnStatus xnUSBAsynchThreadAddRef()
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnl::AutoCSLocker locker(g_InitData.hLock);

	++g_InitData.nOpenDevices;

	if (g_InitData.hThread == NULL)
	{
		xnLogVerbose(XN_MASK_USB, "Starting libusb asynch thread...");

		// mark thread should run
		g_InitData.bShouldThreadRun = true;

		// and start thread
		nRetVal = xnOSCreateThread(xnUSBHandleEventsThread, NULL, &g_InitData.hThread);
		if (nRetVal != XN_STATUS_OK)
		{
			// clean-up
			xnUSBPlatformSpecificShutdown();
			return nRetVal;
		}

		// set thread priority to critical
		nRetVal = xnOSSetThreadPriority(g_InitData.hThread, XN_PRIORITY_CRITICAL);
		if (nRetVal != 0)
		{
			xnLogWarning(XN_MASK_USB, "USB events thread: Failed to set thread priority to critical. This might cause loss of data...");
			printf("Warning: USB events thread - failed to set priority. This might cause loss of data...\n");
		}
	}

	return (XN_STATUS_OK);
}

void xnUSBAsynchThreadStop()
{
	if (g_InitData.hThread != NULL)
	{
		// mark for thread to exit
		g_InitData.bShouldThreadRun = false;

		// wait for it to exit
		xnLogVerbose(XN_MASK_USB, "Shutting down USB events thread...");
		XnStatus nRetVal = xnOSWaitForThreadExit(g_InitData.hThread, XN_USB_HANDLE_EVENTS_TIMEOUT * 2);
		if (nRetVal != XN_STATUS_OK)
		{
			xnLogWarning(XN_MASK_USB, "USB events thread didn't shutdown. Terminating it...");
			xnOSTerminateThread(&g_InitData.hThread);
		}
		else
		{
			xnOSCloseThread(&g_InitData.hThread);
		}

		g_InitData.hThread = NULL;
	}
}

void xnUSBAsynchThreadRelease()
{
	xnl::AutoCSLocker locker(g_InitData.hLock);

	--g_InitData.nOpenDevices;

	if (g_InitData.nOpenDevices == 0)
	{
		xnUSBAsynchThreadStop();
	}
}

XnStatus xnUSBPlatformSpecificShutdown()
{
	xnUSBAsynchThreadStop();
#ifdef XN_USE_UDEV
	g_bShouldRunUDEVThread = false;
	xnOSWaitAndTerminateThread(&g_hUDEVThread, 2 * 1000);
	g_hUDEVThread = NULL;
#endif

	if (g_InitData.hLock != NULL)
	{
		xnOSCloseCriticalSection(&g_InitData.hLock);
		g_InitData.hLock = NULL;
	}

	if (g_InitData.pContext != NULL)
	{
		// close the library
		libusb_exit(g_InitData.pContext);
		g_InitData.pContext = NULL;
	}

	return (XN_STATUS_OK);
}

/*
* Finds a USB device.
* the returned device must be unreferenced when it is no longer needed using libusb_unref_device.
*/
XnStatus FindDevice(uint16_t nVendorID, uint16_t nProductID, void* /*pExtraParam*/, libusb_device** ppDevice)
{
	*ppDevice = NULL;

	// get device list
	libusb_device** ppDevices;
	ssize_t nDeviceCount = libusb_get_device_list(g_InitData.pContext, &ppDevices);

	// check for error
	if (nDeviceCount < 0)
	{
		return (XN_STATUS_USB_ENUMERATE_FAILED);
	}

	// enumerate over the devices
	for (ssize_t i = 0; i < nDeviceCount; ++i)
	{
		libusb_device* pDevice = ppDevices[i];

		// get device descriptor
		libusb_device_descriptor desc;
		int rc = libusb_get_device_descriptor(pDevice, &desc);
		if (rc != 0)
		{
			return (XN_STATUS_USB_ENUMERATE_FAILED);
		}

		// check if this is the requested device
		if (desc.idVendor == nVendorID && desc.idProduct == nProductID)
		{
			// add a reference to the device (so it won't be destroyed when list is freed)
			libusb_ref_device(pDevice);
			*ppDevice = pDevice;
			break;
		}
	}

	// free the list (also dereference each device)
	libusb_free_device_list(ppDevices, 1);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBIsDevicePresent(uint16_t nVendorID, uint16_t nProductID, void* pExtraParam, bool* pbDevicePresent)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// make sure library was initialized
	XN_VALIDATE_USB_INIT();

	// Validate parameters
	XN_VALIDATE_OUTPUT_PTR(pbDevicePresent);

	*pbDevicePresent = false;

	libusb_device* pDevice;
	nRetVal = FindDevice(nVendorID, nProductID, pExtraParam, &pDevice);
	XN_IS_STATUS_OK(nRetVal);

	if (pDevice != NULL)
	{
		*pbDevicePresent = true;

		// unref device
		libusb_unref_device(pDevice);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBEnumerateDevices(uint16_t nVendorID, uint16_t nProductID, const XnUSBConnectionString** pastrDevicePaths, uint32_t* pnCount)
{
	// get device list
	libusb_device** ppDevices;
	ssize_t nDeviceCount = libusb_get_device_list(g_InitData.pContext, &ppDevices);

	// first enumeration - count
	uint32_t nCount = 0;

	for (ssize_t i = 0; i < nDeviceCount; ++i)
	{
		libusb_device* pDevice = ppDevices[i];

		// get device descriptor
		libusb_device_descriptor desc;
		int rc = libusb_get_device_descriptor(pDevice, &desc);
		if (rc != 0)
		{
			libusb_free_device_list(ppDevices, 1);
			return (XN_STATUS_USB_ENUMERATE_FAILED);
		}

		// check if this is the requested device
		if (desc.idVendor == nVendorID && desc.idProduct == nProductID)
		{
			++nCount;
		}
	}

	// allocate array
	XnUSBConnectionString* aResult = (XnUSBConnectionString*)xnOSCalloc(nCount, sizeof(XnUSBConnectionString));
	if (aResult == NULL)
	{
		libusb_free_device_list(ppDevices, 1);
		return XN_STATUS_ALLOC_FAILED;
	}

	// second enumeration - fill
	uint32_t nCurrent = 0;
	for (ssize_t i = 0; i < nDeviceCount; ++i)
	{
		libusb_device* pDevice = ppDevices[i];

		// get device descriptor
		libusb_device_descriptor desc;
		int rc = libusb_get_device_descriptor(pDevice, &desc);
		if (rc != 0)
		{
			xnOSFree(aResult);
			libusb_free_device_list(ppDevices, 1);
			return (XN_STATUS_USB_ENUMERATE_FAILED);
		}

		// check if this is the requested device
		if (desc.idVendor == nVendorID && desc.idProduct == nProductID)
		{
			sprintf(aResult[nCurrent], "%04hx/%04hx@%hhu/%hhu", nVendorID, nProductID, libusb_get_bus_number(pDevice), libusb_get_device_address(pDevice));
			nCurrent++;
		}
	}

	*pastrDevicePaths = aResult;
	*pnCount = nCount;

	// free the list (also dereference each device)
	libusb_free_device_list(ppDevices, 1);

	return XN_STATUS_OK;
}

XN_C_API void xnUSBFreeDevicesList(const XnUSBConnectionString* astrDevicePaths)
{
	xnOSFree(astrDevicePaths);
}

XN_C_API XnStatus xnUSBOpenDeviceImpl(libusb_device* pDevice, XN_USB_DEV_HANDLE* pDevHandlePtr)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (pDevice == NULL)
	{
		return (XN_STATUS_USB_DEVICE_NOT_FOUND);
	}

	// allocate device handle
	libusb_device_handle* handle;

	// open device
	int rc = libusb_open(pDevice, &handle);

	// in any case, unref the device (we don't need it anymore)
	libusb_unref_device(pDevice);
	pDevice = NULL;

	// now check if open failed
	if (rc != 0)
	{
		return (XN_STATUS_USB_DEVICE_OPEN_FAILED);
	}

	// claim the interface (you cannot open any end point before claiming the interface)
	rc = libusb_claim_interface(handle, 0);
	if (rc != 0)
	{
		libusb_close(handle);
		return (XN_STATUS_USB_SET_INTERFACE_FAILED);
	}

	XN_VALIDATE_ALLOC(*pDevHandlePtr, XnUSBDeviceHandle);
	XN_USB_DEV_HANDLE pDevHandle = *pDevHandlePtr;
	pDevHandle->hDevice = handle;
	pDevHandle->nInterface = 0;
	pDevHandle->nAltSetting = 0;

	// mark the device is of high-speed
	pDevHandle->nDevSpeed = XN_USB_DEVICE_HIGH_SPEED;

	nRetVal = xnUSBAsynchThreadAddRef();
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(*pDevHandlePtr);
		return (nRetVal);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBOpenDevice(uint16_t nVendorID, uint16_t nProductID, void* pExtraParam, void* /*pExtraParam2*/, XN_USB_DEV_HANDLE* pDevHandlePtr)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// make sure library was initialized
	XN_VALIDATE_USB_INIT();

	// Validate parameters
	XN_VALIDATE_OUTPUT_PTR(pDevHandlePtr);

	libusb_device* pDevice;
	nRetVal = FindDevice(nVendorID, nProductID, pExtraParam, &pDevice);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnUSBOpenDeviceImpl(pDevice, pDevHandlePtr);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBOpenDeviceByPath(const XnUSBConnectionString strDevicePath, XN_USB_DEV_HANDLE* pDevHandlePtr)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// parse connection string
	uint16_t nVendorID = 0;
	uint16_t nProductID = 0;
	uint8_t nBus = 0;
	uint8_t nAddress = 0;
	sscanf(strDevicePath, "%hx/%hx@%hhu/%hhu", &nVendorID, &nProductID, &nBus, &nAddress);

	if (nVendorID == 0 || nProductID == 0 || nBus == 0 || nAddress == 0)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_USB_DEVICE_OPEN_FAILED, "Invalid connection string: %s", strDevicePath);
	}

	// find device
	libusb_device** ppDevices;
	ssize_t nDeviceCount = libusb_get_device_list(g_InitData.pContext, &ppDevices);

	libusb_device* pRequestedDevice = NULL;

	for (ssize_t i = 0; i < nDeviceCount; ++i)
	{
		libusb_device* pDevice = ppDevices[i];

		// get device descriptor
		libusb_device_descriptor desc;
		int rc = libusb_get_device_descriptor(pDevice, &desc);
		if (rc != 0)
		{
			libusb_free_device_list(ppDevices, 1);
			return (XN_STATUS_USB_ENUMERATE_FAILED);
		}

		// check if this is the requested device
		if (desc.idVendor == nVendorID && desc.idProduct == nProductID && libusb_get_bus_number(pDevice) == nBus && libusb_get_device_address(pDevice) == nAddress)
		{
			// add a reference to the device (so it won't be destroyed when list is freed)
			libusb_ref_device(pDevice);
			pRequestedDevice = pDevice;
			break;
		}
	}

	libusb_free_device_list(ppDevices, 1);

	nRetVal = xnUSBOpenDeviceImpl(pRequestedDevice, pDevHandlePtr);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBCloseDevice(XN_USB_DEV_HANDLE pDevHandle)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_DEVICE_HANDLE(pDevHandle);

	int rc = libusb_release_interface(pDevHandle->hDevice, pDevHandle->nInterface);
	if (0 != rc)
	{
		return (XN_STATUS_USB_DEVICE_CLOSE_FAILED);
	}

	libusb_close(pDevHandle->hDevice);

	XN_FREE_AND_NULL(pDevHandle);

	xnUSBAsynchThreadRelease();

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBGetDeviceSpeed(XN_USB_DEV_HANDLE pDevHandle, XnUSBDeviceSpeed* pDevSpeed)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_DEVICE_HANDLE(pDevHandle);
	XN_VALIDATE_OUTPUT_PTR(pDevSpeed);

	*pDevSpeed = pDevHandle->nDevSpeed;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBSetConfig(XN_USB_DEV_HANDLE /*pDevHandle*/, uint8_t /*nConfig*/)
{
	return (XN_STATUS_OS_UNSUPPORTED_FUNCTION);
}

XN_C_API XnStatus xnUSBGetConfig(XN_USB_DEV_HANDLE /*pDevHandle*/, uint8_t* /*pnConfig*/)
{
	return (XN_STATUS_OS_UNSUPPORTED_FUNCTION);
}

XN_C_API XnStatus xnUSBSetInterface(XN_USB_DEV_HANDLE pDevHandle, uint8_t nInterface, uint8_t nAltInterface)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_DEVICE_HANDLE(pDevHandle);

	int rc = libusb_set_interface_alt_setting(pDevHandle->hDevice, nInterface, nAltInterface);
	if (rc != 0)
	{
		return (XN_STATUS_USB_SET_INTERFACE_FAILED);
	}

	pDevHandle->nInterface = nInterface;
	pDevHandle->nAltSetting = nAltInterface;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBGetInterface(XN_USB_DEV_HANDLE pDevHandle, uint8_t* pnInterface, uint8_t* pnAltInterface)
{
	uint8_t nAltInterface;
	int rc = libusb_control_transfer(pDevHandle->hDevice,
		LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_STANDARD | LIBUSB_RECIPIENT_INTERFACE,
		LIBUSB_REQUEST_GET_INTERFACE, 0, 0, &nAltInterface, 1, 1000);
	if (rc != 1)
	{
		return (XN_STATUS_USB_GET_INTERFACE_FAILED);
	}

	*pnInterface = 0;
	*pnAltInterface = nAltInterface;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBOpenEndPoint(XN_USB_DEV_HANDLE pDevHandle, uint16_t nEndPointID, XnUSBEndPointType nEPType, XnUSBDirectionType nDirType, XN_USB_EP_HANDLE* pEPHandlePtr)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_DEVICE_HANDLE(pDevHandle);
	XN_VALIDATE_OUTPUT_PTR(pEPHandlePtr);

	// get the device from the handle
	libusb_device* pDevice = libusb_get_device(pDevHandle->hDevice);

	// get the configuration descriptor
	libusb_config_descriptor* pConfig;
	int rc = libusb_get_active_config_descriptor(pDevice, &pConfig);
	if (rc != 0)
	{
		return (XN_STATUS_USB_CONFIG_QUERY_FAILED);
	}

	// make sure configuration contains the interface we need
	if (pConfig->bNumInterfaces <= pDevHandle->nInterface)
	{
		libusb_free_config_descriptor(pConfig);
		return (XN_STATUS_USB_INTERFACE_QUERY_FAILED);
	}

	// take that interface
	const libusb_interface* pInterface = &pConfig->interface[pDevHandle->nInterface];

	// make sure interface contains the alternate setting we work with
	if (pInterface->num_altsetting <= pDevHandle->nAltSetting)
	{
		libusb_free_config_descriptor(pConfig);
		return (XN_STATUS_USB_INTERFACE_QUERY_FAILED);
	}

	// take that setting
	const libusb_interface_descriptor* pInterfaceDesc = &pInterface->altsetting[pDevHandle->nAltSetting];

	// search for the requested endpoint
	const libusb_endpoint_descriptor* pEndpointDesc = NULL;

	for (uint8_t i = 0; i < pInterfaceDesc->bNumEndpoints; ++i)
	{
		if (pInterfaceDesc->endpoint[i].bEndpointAddress == nEndPointID)
		{
			pEndpointDesc = &pInterfaceDesc->endpoint[i];
			break;
		}
	}

	if (pEndpointDesc == NULL)
	{
		libusb_free_config_descriptor(pConfig);
		return (XN_STATUS_USB_ENDPOINT_NOT_FOUND);
	}

	libusb_transfer_type transfer_type = (libusb_transfer_type)(pEndpointDesc->bmAttributes & 0x3); // lower 2-bits

	// calculate max packet size
	// NOTE: we do not use libusb functions (libusb_get_max_packet_size/libusb_get_max_iso_packet_size) because
	// they have a bug and do not consider alternative interfaces
	uint32_t nMaxPacketSize = 0;

	if (transfer_type == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS)
	{
		uint32_t wMaxPacketSize = pEndpointDesc->wMaxPacketSize;
		// bits 11 and 12 mark the number of additional transactions, bits 0-10 mark the size
		uint32_t nAdditionalTransactions = wMaxPacketSize >> 11;
		uint32_t nPacketSize = wMaxPacketSize & 0x7FF;
		nMaxPacketSize = (nAdditionalTransactions + 1) * (nPacketSize);
	}
	else
	{
		nMaxPacketSize = pEndpointDesc->wMaxPacketSize;
	}

	// free the configuration descriptor. no need of it anymore
	libusb_free_config_descriptor(pConfig);
	pConfig = NULL;

	// Make sure the endpoint matches the required endpoint type
	if (nEPType == XN_USB_EP_BULK)
	{
		if (transfer_type != LIBUSB_TRANSFER_TYPE_BULK)
		{
			return (XN_STATUS_USB_WRONG_ENDPOINT_TYPE);
		}
	}
	else if (nEPType == XN_USB_EP_INTERRUPT)
	{
		if (transfer_type != LIBUSB_TRANSFER_TYPE_INTERRUPT)
		{
			return (XN_STATUS_USB_WRONG_ENDPOINT_TYPE);
		}
	}
	else if (nEPType == XN_USB_EP_ISOCHRONOUS)
	{
		if (transfer_type != LIBUSB_TRANSFER_TYPE_ISOCHRONOUS)
		{
			return (XN_STATUS_USB_WRONG_ENDPOINT_TYPE);
		}
	}
	else
	{
		return (XN_STATUS_USB_UNKNOWN_ENDPOINT_TYPE);
	}

	// Make sure the endpoint matches the required direction
	libusb_endpoint_direction direction = (libusb_endpoint_direction)(nEndPointID & 0x80); // 8th bit
	if (nDirType == XN_USB_DIRECTION_IN)
	{
		if (direction != LIBUSB_ENDPOINT_IN)
		{
			return (XN_STATUS_USB_WRONG_ENDPOINT_DIRECTION);
		}
	}
	else if (nDirType == XN_USB_DIRECTION_OUT)
	{
		if (direction != LIBUSB_ENDPOINT_OUT)
		{
			return (XN_STATUS_USB_WRONG_ENDPOINT_DIRECTION);
		}
	}
	else
	{
		return (XN_STATUS_USB_UNKNOWN_ENDPOINT_DIRECTION);
	}

	// allocate handle
	XN_VALIDATE_ALIGNED_CALLOC(*pEPHandlePtr, XnUSBEPHandle, 1, XN_DEFAULT_MEM_ALIGN);
	XN_USB_EP_HANDLE pHandle = *pEPHandlePtr;
	pHandle->hDevice = pDevHandle->hDevice;
	pHandle->nAddress = nEndPointID;
	pHandle->nType = nEPType;
	pHandle->nDirection = nDirType;
	pHandle->nMaxPacketSize = nMaxPacketSize;

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnUSBCloseEndPoint(XN_USB_EP_HANDLE pEPHandle)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_EP_HANDLE(pEPHandle);

	XN_ALIGNED_FREE_AND_NULL(pEPHandle);

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnUSBGetEndPointMaxPacketSize(XN_USB_EP_HANDLE pEPHandle, uint32_t* pnMaxPacketSize)
{
	// Validate xnUSB
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_EP_HANDLE(pEPHandle);

	// Validate the input/output pointers
	XN_VALIDATE_INPUT_PTR(pEPHandle);
	XN_VALIDATE_OUTPUT_PTR(pnMaxPacketSize);

	*pnMaxPacketSize = pEPHandle->nMaxPacketSize;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBAbortEndPoint(XN_USB_EP_HANDLE /*pEPHandle*/)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

XN_C_API XnStatus xnUSBFlushEndPoint(XN_USB_EP_HANDLE /*pEPHandle*/)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

XN_C_API XnStatus xnUSBResetEndPoint(XN_USB_EP_HANDLE /*pEPHandle*/)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

XN_C_API XnStatus xnUSBSendControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, uint8_t nRequest, uint16_t nValue, uint16_t nIndex, unsigned char* pBuffer, uint32_t nBufferSize, uint32_t nTimeOut)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_DEVICE_HANDLE(pDevHandle);

	if (nBufferSize != 0)
	{
		XN_VALIDATE_INPUT_PTR(pBuffer);
	}

	uint8_t bmRequestType;

	if (nType == XN_USB_CONTROL_TYPE_VENDOR )
	{
		bmRequestType = LIBUSB_REQUEST_TYPE_VENDOR;
	}
	else if (nType == XN_USB_CONTROL_TYPE_CLASS)
	{
		bmRequestType = LIBUSB_REQUEST_TYPE_CLASS;
	}
	else if (nType == XN_USB_CONTROL_TYPE_STANDARD)
	{
		bmRequestType = LIBUSB_REQUEST_TYPE_STANDARD;
	}
	else
	{
		return (XN_STATUS_USB_WRONG_CONTROL_TYPE);
	}

	bmRequestType |= LIBUSB_ENDPOINT_OUT;

	// send
	int nBytesSent = libusb_control_transfer(pDevHandle->hDevice, bmRequestType, nRequest, nValue, nIndex, pBuffer, nBufferSize, nTimeOut);

	// check everything went OK
	if (nBytesSent == LIBUSB_ERROR_TIMEOUT)
	{
		return (XN_STATUS_USB_TRANSFER_TIMEOUT);
	}
	else if (nBytesSent < 0)
	{
		return (XN_STATUS_USB_CONTROL_SEND_FAILED);
	}

	if ((uint32_t)nBytesSent != nBufferSize)
	{
		return (XN_STATUS_USB_GOT_UNEXPECTED_BYTES);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBReceiveControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, uint8_t nRequest, uint16_t nValue, uint16_t nIndex, unsigned char* pBuffer, uint32_t nBufferSize, uint32_t* pnBytesReceived, uint32_t nTimeOut)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_DEVICE_HANDLE(pDevHandle);
	XN_VALIDATE_OUTPUT_PTR(pBuffer);
	XN_VALIDATE_OUTPUT_PTR(pnBytesReceived);

	if (nBufferSize == 0)
	{
		return (XN_STATUS_USB_BUFFER_TOO_SMALL);
	}

	*pnBytesReceived = 0;

	uint8_t bmRequestType;

	if (nType == XN_USB_CONTROL_TYPE_VENDOR )
	{
		bmRequestType = LIBUSB_REQUEST_TYPE_VENDOR;
	}
	else if (nType == XN_USB_CONTROL_TYPE_CLASS)
	{
		bmRequestType = LIBUSB_REQUEST_TYPE_CLASS;
	}
	else if (nType == XN_USB_CONTROL_TYPE_STANDARD)
	{
		bmRequestType = LIBUSB_REQUEST_TYPE_STANDARD;
	}
	else
	{
		return (XN_STATUS_USB_WRONG_CONTROL_TYPE);
	}

	bmRequestType |= LIBUSB_ENDPOINT_IN;

	// send
	int nBytesReceived = libusb_control_transfer(pDevHandle->hDevice, bmRequestType, nRequest, nValue, nIndex, pBuffer, nBufferSize, nTimeOut);

	// check everything went OK
	if (nBytesReceived == LIBUSB_ERROR_TIMEOUT)
	{
		return (XN_STATUS_USB_TRANSFER_TIMEOUT);
	}
	else if (nBytesReceived < 0) // error
	{
		xnLogWarning(XN_MASK_USB, "Failed to receive from USB control endpoint (%d)", nBytesReceived);
		return (XN_STATUS_USB_CONTROL_RECV_FAILED);
	}
	else if (nBytesReceived == 0) // nothing received
	{
		// received empty message
		return (XN_STATUS_USB_NOT_ENOUGH_DATA);
	}
	else if ((uint32_t)nBytesReceived > nBufferSize) // too much
	{
		xnLogWarning(XN_MASK_USB, "Too many bytes!!!");
		return (XN_STATUS_USB_TOO_MUCH_DATA);
	}

	// return number of bytes received
	*pnBytesReceived = nBytesReceived;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBWriteEndPoint(XN_USB_EP_HANDLE pEPHandle, unsigned char* pBuffer, uint32_t nBufferSize, uint32_t nTimeOut)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_EP_HANDLE(pEPHandle);
	XN_VALIDATE_INPUT_PTR(pBuffer);

	if (pEPHandle->nDirection != XN_USB_DIRECTION_OUT)
	{
		return (XN_STATUS_USB_WRONG_ENDPOINT_DIRECTION);
	}

	if (nBufferSize == 0)
	{
		return (XN_STATUS_USB_BUFFER_TOO_SMALL);
	}

	// send (according to EP type)
	int nBytesSent = 0;
	int rc = 0;

	if (pEPHandle->nType == XN_USB_EP_BULK)
	{
		rc = libusb_bulk_transfer(pEPHandle->hDevice, pEPHandle->nAddress, pBuffer, nBufferSize, &nBytesSent, nTimeOut);
	}
	else if (pEPHandle->nType == XN_USB_EP_INTERRUPT)
	{
		rc = libusb_interrupt_transfer(pEPHandle->hDevice, pEPHandle->nAddress, pBuffer, nBufferSize, &nBytesSent, nTimeOut);
	}
	else
	{
		return (XN_STATUS_USB_UNSUPPORTED_ENDPOINT_TYPE);
	}

	// check result
	if (rc == LIBUSB_ERROR_TIMEOUT)
	{
		return (XN_STATUS_USB_TRANSFER_TIMEOUT);
	}
	else if (rc != 0)
	{
		return (XN_STATUS_USB_ENDPOINT_WRITE_FAILED);
	}

	if ((uint32_t)nBytesSent != nBufferSize)
	{
		return (XN_STATUS_USB_GOT_UNEXPECTED_BYTES);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBReadEndPoint(XN_USB_EP_HANDLE pEPHandle, unsigned char* pBuffer, uint32_t nBufferSize, uint32_t* pnBytesReceived, uint32_t nTimeOut)
{
	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_EP_HANDLE(pEPHandle);
	XN_VALIDATE_OUTPUT_PTR(pBuffer);
	XN_VALIDATE_OUTPUT_PTR(pnBytesReceived);

	if (pEPHandle->nDirection != XN_USB_DIRECTION_IN)
	{
		return (XN_STATUS_USB_WRONG_ENDPOINT_DIRECTION);
	}

	if (nBufferSize == 0)
	{
		return (XN_STATUS_USB_BUFFER_TOO_SMALL);
	}

	// receive (according to EP type)
	*pnBytesReceived = 0;

	int nBytesReceived = 0;
	int rc = 0;

	if (pEPHandle->nType == XN_USB_EP_BULK)
	{
		rc = libusb_bulk_transfer(pEPHandle->hDevice, pEPHandle->nAddress, pBuffer, nBufferSize, &nBytesReceived, nTimeOut);
	}
	else if (pEPHandle->nType == XN_USB_EP_INTERRUPT)
	{
		rc = libusb_interrupt_transfer(pEPHandle->hDevice, pEPHandle->nAddress, pBuffer, nBufferSize, &nBytesReceived, nTimeOut);
	}
	else
	{
		return (XN_STATUS_USB_UNSUPPORTED_ENDPOINT_TYPE);
	}

	// check result
	if (rc == LIBUSB_ERROR_TIMEOUT)
	{
		return (XN_STATUS_USB_TRANSFER_TIMEOUT);
	}
	else if (rc != 0)
	{
		return (XN_STATUS_USB_ENDPOINT_WRITE_FAILED);
	}

	if (nBytesReceived == 0)
	{
		return (XN_STATUS_USB_NOT_ENOUGH_DATA);
	}

	*pnBytesReceived = nBytesReceived;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBQueueReadEndPoint(XN_USB_EP_HANDLE /*pEPHandle*/, unsigned char* /*pBuffer*/, uint32_t /*nBufferSize*/, uint32_t /*nTimeOut*/)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

XN_C_API XnStatus xnUSBFinishReadEndPoint(XN_USB_EP_HANDLE /*pEPHandle*/, uint32_t* /*pnBytesReceived*/, uint32_t /*nTimeOut*/)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

void xnCleanupThreadData(XnUSBReadThreadData* pThreadData)
{
	for (uint32_t i = 0; i < pThreadData->nNumBuffers; ++i)
	{
		if (pThreadData->pBuffersInfo[i].transfer != NULL)
		{
			XN_ALIGNED_FREE_AND_NULL(pThreadData->pBuffersInfo[i].transfer->buffer);
			libusb_free_transfer(pThreadData->pBuffersInfo[i].transfer);
			pThreadData->pBuffersInfo[i].transfer = NULL;
			xnOSCloseEvent(&pThreadData->pBuffersInfo[i].hEvent);
		}
	}

	XN_ALIGNED_FREE_AND_NULL(pThreadData->pBuffersInfo);
}

/** Checks if any transfer of the thread is queued. */
bool xnIsAnyTransferQueued(XnUSBReadThreadData* pThreadData)
{
	for (uint32_t i = 0; i < pThreadData->nNumBuffers; ++i)
	{
		if (pThreadData->pBuffersInfo[i].bIsQueued)
			return (true);
	}

	return (false);
}

XN_THREAD_PROC xnUSBReadThreadMain(XN_THREAD_PARAM pThreadParam)
{
	XnUSBReadThreadData* pThreadData = (XnUSBReadThreadData*)pThreadParam;

	// set thread priority to critical
	XnStatus nRetVal = xnOSSetThreadPriority(pThreadData->hReadThread, XN_PRIORITY_CRITICAL);
	if (nRetVal != 0)
	{
		xnLogWarning(XN_MASK_USB, "Failed to set thread priority to critical. This might cause loss of data...");
	}

	// first of all, submit all transfers
	for (uint32_t i = 0; i < pThreadData->nNumBuffers; ++i)
	{
		XnUSBBuffersInfo* pBufferInfo = &pThreadData->pBuffersInfo[i];
		libusb_transfer* pTransfer = pBufferInfo->transfer;

		// submit request
		pBufferInfo->bIsQueued = true;
		int rc = libusb_submit_transfer(pTransfer);
		if (rc != 0)
		{
			xnLogError(XN_MASK_USB, "Endpoint 0x%x, Buffer %d: Failed to submit asynch I/O transfer (err=%d)!", pTransfer->endpoint, pBufferInfo->nBufferID, rc);
		}
	}

	// now let libusb process asynchornous I/O

	while (true)
	{
		for (uint32_t i = 0; i < pThreadData->nNumBuffers; ++i)
		{
			// check if thread can exit (only after kill was requested, and all transfers returned)
			if (pThreadData->bKillReadThread && !xnIsAnyTransferQueued(pThreadData))
			{
				XN_THREAD_PROC_RETURN(XN_STATUS_OK);
			}

			XnUSBBuffersInfo* pBufferInfo = &pThreadData->pBuffersInfo[i];
			libusb_transfer* pTransfer = pBufferInfo->transfer;

			// wait for the next transfer to be completed, and process it
			nRetVal = xnOSWaitEvent(pBufferInfo->hEvent, pThreadData->bKillReadThread ? 0 : pThreadData->nTimeOut);
			if (nRetVal == XN_STATUS_OS_EVENT_TIMEOUT)
			{
//				xnLogWarning(XN_MASK_USB, "Endpoint 0x%x, Buffer %d: timeout. cancelling transfer...", pTransfer->endpoint, pBufferInfo->nBufferID);

				// cancel it
				int rc = libusb_cancel_transfer(pBufferInfo->transfer);
				if (rc != 0)
				{
					// If we get LIBUSB_ERROR_NOT_FOUND it means that the transfer was already cancaled/completed which is a very common thing during normal shutdown so there's no need to print it.
					if (rc != LIBUSB_ERROR_NOT_FOUND)
					{
						if (rc == LIBUSB_ERROR_NO_DEVICE)
						{
							goto disconnect;
						}
						else
						{
							xnLogError(XN_MASK_USB, "Endpoint 0x%x, Buffer %d: Failed to cancel asynch I/O transfer (err=%d)!", pTransfer->endpoint, pBufferInfo->nBufferID, rc);
						}
					}
				}

				// wait for it to cancel
				nRetVal = xnOSWaitEvent(pBufferInfo->hEvent, pThreadData->nTimeOut);
			}

			if (nRetVal != XN_STATUS_OK)
			{
				// not much we can do
				xnLogWarning(XN_MASK_USB, "Endpoint 0x%x, Buffer %d: Failed waiting on asynch transfer event: %s", pTransfer->endpoint, pBufferInfo->nBufferID, xnGetStatusString(nRetVal));
			}

			// check the result of the transfer
			if (pBufferInfo->bIsQueued)
			{
				xnLogWarning(XN_MASK_USB, "Endpoint 0x%x, Buffer %d: Transfer is still queued though event was raised!", pTransfer->endpoint, pBufferInfo->nBufferID);
				// TODO: cancel it?
			}
			else // transfer done
			{
				// check for unexpected disconnects
				if (pTransfer->status == LIBUSB_TRANSFER_NO_DEVICE)
				{
					goto disconnect;
				}

				if (pBufferInfo->nLastStatus == LIBUSB_TRANSFER_COMPLETED || // read succeeded
					pBufferInfo->nLastStatus == LIBUSB_TRANSFER_CANCELLED)   // cancelled, but maybe some data arrived
				{
					if (pTransfer->type == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS)
					{
						unsigned char* pBuffer = NULL;
						uint32_t nTotalBytes = 0;
						bool bCompletePacket;

						// some packets may return empty or partial, aggregate as many consequent packets as possible, and then send them to processing
						for (int32_t i = 0; i < pTransfer->num_iso_packets; ++i)
						{
							struct libusb_iso_packet_descriptor* pPacket = &pTransfer->iso_packet_desc[i];

							// continue aggregating
							if (pPacket->status == LIBUSB_TRANSFER_COMPLETED)
							{
								if (pBuffer == NULL)
								{
									pBuffer = libusb_get_iso_packet_buffer_simple(pTransfer, i);
								}

								nTotalBytes += pPacket->actual_length;

								bCompletePacket = true;
							}
							else
							{
								if (pPacket->status != LIBUSB_TRANSFER_ERROR) //Skip printing uninformative common errors
								{
									xnLogWarning(XN_MASK_USB, "Endpoint 0x%x, Buffer %d, packet %d Asynch transfer failed (status: %d)", pTransfer->endpoint, pBufferInfo->nBufferID, i, pPacket->status);
								}

								bCompletePacket = false;
							}

							// stop condition for aggregating
							if (!bCompletePacket || // failed packet
								pPacket->actual_length != pPacket->length || // partial packet
								i == pTransfer->num_iso_packets - 1) // last packet
							{
								if (nTotalBytes != 0)
								{
									pBufferInfo->pThreadData->pCallbackFunction(pBuffer, nTotalBytes, pBufferInfo->pThreadData->pCallbackData);
								}
								pBuffer = NULL;
								nTotalBytes = 0;
							}
						} // packets loop
					}
					else
					{
						// call callback method
						pBufferInfo->pThreadData->pCallbackFunction(pTransfer->buffer, pTransfer->actual_length, pBufferInfo->pThreadData->pCallbackData);
					}
				}
				else if (pBufferInfo->nLastStatus == LIBUSB_TRANSFER_TIMED_OUT)
				{
					// no need to do anything.
				}
				else
				{
					xnLogWarning(XN_MASK_USB, "Endpoint 0x%x, Buffer %d: Asynch transfer failed (status: %d)", pTransfer->endpoint, pBufferInfo->nBufferID, pTransfer->status);
				}

				// as long as running should continue, resubmit transfer
				if (!pBufferInfo->pThreadData->bKillReadThread)
				{
					pBufferInfo->bIsQueued = true;
					int rc = libusb_submit_transfer(pTransfer);
					if (rc != 0)
					{
						if (rc == LIBUSB_ERROR_NO_DEVICE)
						{
							goto disconnect;
						}

						xnLogError(XN_MASK_USB, "Endpoint 0x%x, Buffer %d: Failed to re-submit asynch I/O transfer (err=%d)!", pTransfer->endpoint, pBufferInfo->nBufferID, rc);
					}
				}
			}
		}
	}

	XN_THREAD_PROC_RETURN(XN_STATUS_OK);

disconnect:
	xnLogError(XN_MASK_USB, "Unexpected device disconnect, aborting the read thread!");

	// close the thread, the device is gone...
	XN_THREAD_PROC_RETURN(XN_STATUS_OK);
}

/* This function is called whenever transfer is done (successfully or with an error). */
void xnTransferCallback(libusb_transfer *pTransfer)
{
	XnUSBBuffersInfo* pBufferInfo = (XnUSBBuffersInfo*)pTransfer->user_data;

	// mark that buffer is done
	pBufferInfo->bIsQueued = false;

	// keep the status (according to libusb documentation, this field is invalid outside the callback method)
	pBufferInfo->nLastStatus = pTransfer->status;

	// notify endpoint thread this buffer is done
	XnStatus nRetVal = xnOSSetEvent(pBufferInfo->hEvent);
	if (nRetVal != XN_STATUS_OK)
	{
		// we don't have much to do if set event failed, log a warning
		xnLogWarning(XN_MASK_USB, "Failed to set event for buffer: %s", xnGetStatusString(nRetVal));
	}
}

XN_C_API XnStatus xnUSBInitReadThread(XN_USB_EP_HANDLE pEPHandle, uint32_t nBufferSize, uint32_t nNumBuffers, uint32_t nTimeOut, XnUSBReadCallbackFunctionPtr pCallbackFunction, void* pCallbackData)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validate parameters
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_EP_HANDLE(pEPHandle);
	XN_VALIDATE_INPUT_PTR(pCallbackFunction);

	xnLogVerbose(XN_MASK_USB, "Starting a USB read thread...");

	XnUSBReadThreadData* pThreadData = &pEPHandle->ThreadData;

	if (pThreadData->bIsRunning == true)
	{
		return (XN_STATUS_USB_READTHREAD_ALREADY_INIT);
	}

	memset(pThreadData, 0, sizeof(XnUSBReadThreadData));
	pThreadData->nNumBuffers = nNumBuffers;
	pThreadData->pCallbackFunction = pCallbackFunction;
	pThreadData->pCallbackData = pCallbackData;
	pThreadData->bKillReadThread = false;
	pThreadData->nTimeOut = nTimeOut;

	// allocate buffers
	pThreadData->pBuffersInfo = (XnUSBBuffersInfo*)xnOSCallocAligned(nNumBuffers, sizeof(XnUSBBuffersInfo), XN_DEFAULT_MEM_ALIGN);
	if (pThreadData->pBuffersInfo == NULL)
	{
		xnCleanupThreadData(pThreadData);
		return XN_STATUS_ALLOC_FAILED;
	}

	int nNumIsoPackets = 0;
	int nMaxPacketSize = 0;

	if (pEPHandle->nType == XN_USB_EP_ISOCHRONOUS)
	{
		// calculate how many packets can be set in this buffer
		nMaxPacketSize = pEPHandle->nMaxPacketSize;
		nNumIsoPackets = nBufferSize / nMaxPacketSize;
	}

	for (uint32_t i = 0; i < nNumBuffers; i++)
	{
		XnUSBBuffersInfo* pBufferInfo = &pThreadData->pBuffersInfo[i];
		pBufferInfo->nBufferID = i;
		pBufferInfo->pThreadData = pThreadData;

		// allocate transfer
		pBufferInfo->transfer = libusb_alloc_transfer(nNumIsoPackets);

		libusb_transfer* pTransfer = pBufferInfo->transfer;

		if (pTransfer == NULL)
		{
			xnCleanupThreadData(pThreadData);
			return (XN_STATUS_ALLOC_FAILED);
		}

		// allocate buffer
		unsigned char* pBuffer = (unsigned char*)xnOSCallocAligned(nBufferSize, sizeof(unsigned char), XN_DEFAULT_MEM_ALIGN);
		if (pBuffer == NULL)
		{
			xnCleanupThreadData(pThreadData);
			return (XN_STATUS_ALLOC_FAILED);
		}

		// fill transfer params
		if (pEPHandle->nType == XN_USB_EP_BULK)
		{
			libusb_fill_bulk_transfer(pTransfer, pEPHandle->hDevice, pEPHandle->nAddress, pBuffer, nBufferSize, xnTransferCallback, pBufferInfo, 0);
		}
		else if (pEPHandle->nType == XN_USB_EP_INTERRUPT)
		{
			libusb_fill_interrupt_transfer(pTransfer, pEPHandle->hDevice, pEPHandle->nAddress, pBuffer, nBufferSize, xnTransferCallback, pBufferInfo, 0);
		}
		else if (pEPHandle->nType == XN_USB_EP_ISOCHRONOUS)
		{
			libusb_fill_iso_transfer(pTransfer, pEPHandle->hDevice, pEPHandle->nAddress, pBuffer, nBufferSize, nNumIsoPackets, xnTransferCallback, pBufferInfo, 0);
			libusb_set_iso_packet_lengths(pTransfer, nMaxPacketSize);
		}
		else
		{
			return (XN_STATUS_USB_UNSUPPORTED_ENDPOINT_TYPE);
		}

		// create event
		nRetVal = xnOSCreateEvent(&pBufferInfo->hEvent,false);
		if (nRetVal != XN_STATUS_OK)
		{
			xnCleanupThreadData(pThreadData);
			return (nRetVal);
		}
	}

	// create a thread to perform the asynchronous read operations
	nRetVal = xnOSCreateThread(xnUSBReadThreadMain, &pEPHandle->ThreadData, &pThreadData->hReadThread);
	if (nRetVal != XN_STATUS_OK)
	{
		xnCleanupThreadData(pThreadData);
		return (nRetVal);
	}

	pThreadData->bIsRunning = true;

	xnLogInfo(XN_MASK_USB, "USB read thread was started.");

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBShutdownReadThread(XN_USB_EP_HANDLE pEPHandle)
{
	XN_VALIDATE_USB_INIT();
	XN_VALIDATE_EP_HANDLE(pEPHandle);

	XnUSBReadThreadData* pThreadData = &pEPHandle->ThreadData;

	if (pThreadData->bIsRunning == false)
	{
		return (XN_STATUS_USB_READTHREAD_NOT_INIT);
	}

	if (pThreadData->hReadThread != NULL)
	{
		// mark thread should be killed
		pThreadData->bKillReadThread = true;

		// PATCH: we don't cancel the requests, because there is a bug causing segmentation fault.

		// now wait for thread to exit (we wait the timeout of all buffers + an extra second)
		XnStatus nRetVal = xnOSWaitForThreadExit(pThreadData->hReadThread, pThreadData->nTimeOut * pThreadData->nNumBuffers + 1000);
		if (nRetVal != XN_STATUS_OK)
		{
			xnOSTerminateThread(&pThreadData->hReadThread);
		}
		else
		{
			xnOSCloseThread(&pThreadData->hReadThread);
		}
	}

	xnCleanupThreadData(pThreadData);

	pThreadData->bIsRunning = false;

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUSBSetCallbackHandler(uint16_t /*nVendorID*/, uint16_t /*nProductID*/, void* /*pExtraParam*/, XnUSBEventCallbackFunctionPtr /*pCallbackFunction*/, void* /*pCallbackData*/)
{
	return (XN_STATUS_OS_UNSUPPORTED_FUNCTION);
}

XN_C_API XnStatus XN_C_DECL xnUSBRegisterToConnectivityEvents(uint16_t nVendorID, uint16_t nProductID, XnUSBDeviceCallbackFunctionPtr pFunc, void* pCookie, XnRegistrationHandle* phRegistration)
{
	XN_VALIDATE_INPUT_PTR(pFunc);
	XN_VALIDATE_OUTPUT_PTR(phRegistration);

	XnUSBEventCallback* pCallback;
	XN_VALIDATE_NEW(pCallback, XnUSBEventCallback);
	pCallback->pFunc = pFunc;
	pCallback->pCookie = pCookie;
	pCallback->nVendorID  = nVendorID;
	pCallback->nProductID = nProductID;

	g_connectivityEvent.push_back(pCallback);

	*phRegistration = (XnRegistrationHandle)pCallback;

	return XN_STATUS_OK;
}

XN_C_API void XN_C_DECL xnUSBUnregisterFromConnectivityEvents(XnRegistrationHandle hRegistration)
{
	XnUSBEventCallback* pCallback = reinterpret_cast<XnUSBEventCallback*>(hRegistration);
	std::list<XnUSBEventCallback*>::iterator it = std::find(g_connectivityEvent.begin(), g_connectivityEvent.end(), pCallback);
	if (it != g_connectivityEvent.end())
	{
		g_connectivityEvent.erase(it);
		XN_DELETE(pCallback);
	}
}
