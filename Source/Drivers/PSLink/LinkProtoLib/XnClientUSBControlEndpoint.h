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
#ifndef XNCLIENTUSBCONTROLENDPOINT_H
#define XNCLIENTUSBCONTROLENDPOINT_H

#include "ISyncIOConnection.h"

struct XnUSBDeviceHandle;
typedef XnUSBDeviceHandle*  XN_USB_DEV_HANDLE;

namespace xn
{

class ClientUSBControlEndpoint : virtual public ISyncIOConnection
{
public:
	ClientUSBControlEndpoint(uint32_t nPreControlReceiveSleep);
	virtual ~ClientUSBControlEndpoint();
	// Operations
	XnStatus Init(XN_USB_DEV_HANDLE hUSBDevice);
	void Shutdown();


	// ISyncIOConnection implementation
	virtual XnStatus Connect();
	virtual void Disconnect();
	virtual bool IsConnected() const;
	virtual uint16_t GetMaxPacketSize() const;

	//nSize is max size on input, actual size on output
	virtual XnStatus Receive(void* pData, uint32_t& nSize);
	virtual XnStatus Send(const void* pData, uint32_t nSize);

private:
	//Low level usb packet size - not to be confused with our "logical" packet size which is bigger
	static const uint32_t USB_LOW_LEVEL_MAX_PACKET_SIZE;

	static const uint32_t SEND_TIMEOUT;
	static const uint32_t RECEIVE_TIMEOUT;

	XN_USB_DEV_HANDLE m_hUSBDevice;
	uint32_t m_nPreControlReceiveSleep;
};

}

#endif // XNCLIENTUSBCONTROLENDPOINT_H
