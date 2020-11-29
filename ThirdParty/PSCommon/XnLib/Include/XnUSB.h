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
#ifndef _XN_USB_H_
#define _XN_USB_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnOS.h"

//---------------------------------------------------------------------------
// Structures & Enums
//---------------------------------------------------------------------------
typedef enum {
	XN_USB_DEVICE_LOW_SPEED = 0,
	XN_USB_DEVICE_FULL_SPEED,
	XN_USB_DEVICE_HIGH_SPEED,
} XnUSBDeviceSpeed;

typedef enum {
	XN_USB_EP_BULK = 0,
	XN_USB_EP_ISOCHRONOUS,
	XN_USB_EP_INTERRUPT,
} XnUSBEndPointType;

typedef enum {
	XN_USB_DIRECTION_IN = 0,
	XN_USB_DIRECTION_OUT,
} XnUSBDirectionType;

typedef enum {
	XN_USB_CONTROL_TYPE_STANDARD = 0,
	XN_USB_CONTROL_TYPE_CLASS,
	XN_USB_CONTROL_TYPE_VENDOR,
} XnUSBControlType;

typedef enum {
	XN_USB_EVENT_DEVICE_CONNECT = 0,
	XN_USB_EVENT_DEVICE_DISCONNECT,
} XnUSBEventType;

struct XnUSBDeviceHandle;
struct XnUSBEndPointHandle;

typedef XnChar XnUSBConnectionString[XN_FILE_MAX_PATH];

typedef XnUSBDeviceHandle*  XN_USB_DEV_HANDLE;
typedef XnUSBEndPointHandle* XN_USB_EP_HANDLE;

typedef bool (XN_CALLBACK_TYPE* XnUSBReadCallbackFunctionPtr)(XnUChar* pBuffer, uint32_t nBufferSize, void* pCallbackData);
typedef bool (XN_CALLBACK_TYPE* XnUSBEventCallbackFunctionPtr)(XnUSBEventType USBEventType, XnChar* cpDevPath, void* pCallbackData);

typedef struct XnUSBEventArgs
{
	const XnChar* strDevicePath;
	XnUSBEventType eventType;
} XnUSBEventArgs;

typedef void (XN_CALLBACK_TYPE* XnUSBDeviceCallbackFunctionPtr)(XnUSBEventArgs* pArgs, void* pCookie);

//---------------------------------------------------------------------------
// Exported Function Declaration
//---------------------------------------------------------------------------
XN_C_API XnStatus XN_C_DECL xnUSBInit();
XN_C_API XnStatus XN_C_DECL xnUSBShutdown();

XN_C_API XnStatus XN_API_DEPRECATED("Use xnUSBEnumerateDevices() instead") XN_C_DECL xnUSBIsDevicePresent(uint16_t nVendorID, uint16_t nProductID, void* pExtraParam, bool* pbDevicePresent);

XN_C_API XnStatus XN_C_DECL xnUSBEnumerateDevices(uint16_t nVendorID, uint16_t nProductID, const XnUSBConnectionString** pastrDevicePaths, uint32_t* pnCount);
XN_C_API void XN_C_DECL xnUSBFreeDevicesList(const XnUSBConnectionString* astrDevicePaths);

XN_C_API XnStatus XN_API_DEPRECATED("Use xnUSBOpenDeviceByPath() instead") XN_C_DECL xnUSBOpenDevice(uint16_t nVendorID, uint16_t nProductID, void* pExtraParam, void* pExtraParam2, XN_USB_DEV_HANDLE* pDevHandlePtr);
XN_C_API XnStatus XN_C_DECL xnUSBOpenDeviceByPath(const XnUSBConnectionString strDevicePath, XN_USB_DEV_HANDLE* pDevHandlePtr);
XN_C_API XnStatus XN_C_DECL xnUSBCloseDevice(XN_USB_DEV_HANDLE pDevHandle);

XN_C_API XnStatus XN_C_DECL xnUSBGetDeviceSpeed(XN_USB_DEV_HANDLE pDevHandle, XnUSBDeviceSpeed* pDevSpeed);

XN_C_API XnStatus XN_C_DECL xnUSBSetConfig(XN_USB_DEV_HANDLE pDevHandle, uint8_t nConfig);
XN_C_API XnStatus XN_C_DECL xnUSBGetConfig(XN_USB_DEV_HANDLE pDevHandle, uint8_t* pnConfig);

XN_C_API XnStatus XN_C_DECL xnUSBSetInterface(XN_USB_DEV_HANDLE pDevHandle, uint8_t nInterface, uint8_t nAltInterface);
XN_C_API XnStatus XN_C_DECL xnUSBGetInterface(XN_USB_DEV_HANDLE pDevHandle, uint8_t* pnInterface, uint8_t* pnAltInterface);

XN_C_API XnStatus XN_C_DECL xnUSBOpenEndPoint(XN_USB_DEV_HANDLE pDevHandle, uint16_t nEndPointID, XnUSBEndPointType nEPType, XnUSBDirectionType nDirType, XN_USB_EP_HANDLE* pEPHandlePtr);
XN_C_API XnStatus XN_C_DECL xnUSBCloseEndPoint(XN_USB_EP_HANDLE pEPHandle);
XN_C_API XnStatus XN_C_DECL xnUSBGetEndPointMaxPacketSize(XN_USB_EP_HANDLE pEPHandle, uint32_t* pnMaxPacketSize);

XN_C_API XnStatus XN_C_DECL xnUSBAbortEndPoint(XN_USB_EP_HANDLE pEPHandle);
XN_C_API XnStatus XN_C_DECL xnUSBFlushEndPoint(XN_USB_EP_HANDLE pEPHandle);
XN_C_API XnStatus XN_C_DECL xnUSBResetEndPoint(XN_USB_EP_HANDLE pEPHandle);

XN_C_API XnStatus XN_C_DECL xnUSBSendControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, uint8_t nRequest, uint16_t nValue, uint16_t nIndex, XnUChar* pBuffer, uint32_t nBufferSize, uint32_t nTimeOut);
XN_C_API XnStatus XN_C_DECL xnUSBReceiveControl(XN_USB_DEV_HANDLE pDevHandle, XnUSBControlType nType, uint8_t nRequest, uint16_t nValue, uint16_t nIndex, XnUChar* pBuffer, uint32_t nBufferSize, uint32_t* pnBytesReceived, uint32_t nTimeOut);

XN_C_API XnStatus XN_C_DECL xnUSBReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, uint32_t nBufferSize, uint32_t* pnBytesReceived, uint32_t nTimeOut);
XN_C_API XnStatus XN_C_DECL xnUSBWriteEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, uint32_t nBufferSize, uint32_t nTimeOut);

XN_C_API XnStatus XN_C_DECL xnUSBQueueReadEndPoint(XN_USB_EP_HANDLE pEPHandle, XnUChar* pBuffer, uint32_t nBufferSize, uint32_t nTimeOut);
XN_C_API XnStatus XN_C_DECL xnUSBFinishReadEndPoint(XN_USB_EP_HANDLE pEPHandle, uint32_t* pnBytesReceived, uint32_t nTimeOut);

XN_C_API XnStatus XN_C_DECL xnUSBInitReadThread(XN_USB_EP_HANDLE pEPHandle, uint32_t nBufferSize, uint32_t nNumBuffers, uint32_t nTimeOut, XnUSBReadCallbackFunctionPtr pCallbackFunction, void* pCallbackData);
XN_C_API XnStatus XN_C_DECL xnUSBShutdownReadThread(XN_USB_EP_HANDLE pEPHandle);

XN_C_API XnStatus XN_API_DEPRECATED("Use xnUSBRegisterToConnectivityEvents() instead") XN_C_DECL xnUSBSetCallbackHandler(uint16_t nVendorID, uint16_t nProductID, void* pExtraParam, XnUSBEventCallbackFunctionPtr pCallbackFunction, void* pCallbackData);

XN_C_API XnStatus XN_C_DECL xnUSBRegisterToConnectivityEvents(uint16_t nVendorID, uint16_t nProductID, XnUSBDeviceCallbackFunctionPtr pFunc, void* pCookie, XnRegistrationHandle* phRegistration);
XN_C_API void XN_C_DECL xnUSBUnregisterFromConnectivityEvents(XnRegistrationHandle hRegistration);

#endif //_XN_USB_H_
