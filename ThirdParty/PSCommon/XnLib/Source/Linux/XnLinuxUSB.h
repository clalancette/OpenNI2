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
#ifndef _XN_USBLINUX_X86_H_
#define _XN_USBLINUX_X86_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_USB_DEFAULT_EP_TIMEOUT 1000
#define XN_USB_READ_THREAD_KILL_TIMEOUT 10000

//---------------------------------------------------------------------------
// Structures & Enums
//---------------------------------------------------------------------------
typedef struct XnUSBDeviceHandle
{
//	bool bValid;
	libusb_device_handle* hDevice;
	XnUSBDeviceSpeed nDevSpeed;
	uint8_t nInterface;
	uint8_t nAltSetting;
} XnUSBDevHandle;

struct XnUSBReadThreadData; // Forward declaration

typedef struct XnUSBBuffersInfo
{
	/* A pointer back to the thread data. */
	XnUSBReadThreadData* pThreadData;
	/* transfer object (through which asynch operations take place). */
	libusb_transfer* transfer;
	/* true when transfer is queued. */
	bool bIsQueued;
	/* An event to notify when buffer is ready. */
	XN_EVENT_HANDLE hEvent;
	uint32_t nBufferID;
	/* Holds the last status received. */
	libusb_transfer_status nLastStatus;
} XnUSBBuffersInfo;

/* Information about a thread reading from an endpoint. */
typedef struct XnUSBReadThreadData
{
	/* true when thread is running. */
	bool bIsRunning;
	/* Number of buffers allocated. */
	uint32_t nNumBuffers;
	/* Array of buffers. */
	XnUSBBuffersInfo* pBuffersInfo;
	/* Size of each buffer. */
	uint32_t nBufferSize;
	/* Timeout value. */
	uint32_t nTimeOut;
	/* User callback function. */
	XnUSBReadCallbackFunctionPtr pCallbackFunction;
	/* User callback data. */
	void* pCallbackData;
	/* Handle to the read thread. */
	XN_THREAD_HANDLE hReadThread;
	/* When true, signals the thread to exit. */
	bool bKillReadThread;
} XnUSBReadThreadData;

typedef struct XnUSBEndPointHandle
{
	libusb_device_handle* hDevice;
	unsigned char nAddress;
	XnUSBEndPointType  nType;
	XnUSBDirectionType nDirection;
	XnUSBReadThreadData ThreadData;
	uint32_t nMaxPacketSize;
} XnUSBEPHandle;

#endif //_XN_USBLINUX_X86_H_
