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
#ifndef XNCLIENTUSBINDATAENDPOINT_H
#define XNCLIENTUSBINDATAENDPOINT_H

#include "IAsyncInputConnection.h"
#include <XnUSB.h>

struct XnUSBDeviceHandle;
struct XnUSBEndPointHandle;

typedef XnUSBDeviceHandle*  XN_USB_DEV_HANDLE;
typedef XnUSBEndPointHandle* XN_USB_EP_HANDLE;

namespace xn
{

class ClientUSBInDataEndpoint : virtual public IAsyncInputConnection
{
public:
	ClientUSBInDataEndpoint();

	virtual ~ClientUSBInDataEndpoint();

	virtual XnStatus Init(XN_USB_DEV_HANDLE hUSBDevice, uint16_t nEndpointID);
	virtual void Shutdown();
	virtual XnStatus Connect();
	virtual void Disconnect();
	virtual bool IsConnected() const;
	virtual uint16_t GetMaxPacketSize() const;
	virtual XnStatus SetDataDestination(IDataDestination* pDataDestination);

	inline XnUSBEndPointType GetEndpointType() const { return m_endpointType; }

private:
	static bool XN_CALLBACK_TYPE ReadThreadCallback(unsigned char* pBuffer, uint32_t nBufferSize, void* pCallbackData);

	static const uint32_t READ_THREAD_BUFFER_NUM_PACKETS_ISO;
	static const uint32_t READ_THREAD_NUM_BUFFERS_ISO;
	static const uint32_t READ_THREAD_TIMEOUT_ISO;
	static const uint32_t READ_THREAD_BUFFER_NUM_PACKETS_BULK;
	static const uint32_t READ_THREAD_NUM_BUFFERS_BULK;
	static const uint32_t READ_THREAD_TIMEOUT_BULK;

	static const uint32_t BASE_INPUT_ENDPOINT;

	XnUSBEndPointType m_endpointType;
	XN_USB_EP_HANDLE m_hEndpoint;
	XN_USB_DEV_HANDLE m_hUSBDevice;
	uint16_t m_nEndpointID;
	uint16_t m_nMaxPacketSize;
	IDataDestination* m_pDataDestination;
	bool m_bConnected;

};

}

#endif // XNCLIENTUSBINDATAENDPOINT_H
