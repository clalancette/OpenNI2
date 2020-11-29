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
#ifndef XNCLIENTUSBCONNECTIONFACTORY_H
#define XNCLIENTUSBCONNECTIONFACTORY_H

#include "IConnectionFactory.h"
#include "XnLinkProtoLibDefs.h"
#include "XnClientUSBControlEndpoint.h"
#include <XnStringsHash.h>
#include <XnUSB.h>

struct XnUSBDeviceHandle;
typedef XnUSBDeviceHandle*  XN_USB_DEV_HANDLE;

namespace xn
{

class ClientUSBConnectionFactory : public IConnectionFactory
{
public:
	ClientUSBConnectionFactory(uint16_t nInputConnections,
					uint16_t nOutputConnections,
					uint32_t nPreControlReceiveSleep);

	virtual ~ClientUSBConnectionFactory();
	virtual XnStatus Init(const XnChar* strConnString);
	virtual void Shutdown();
	virtual bool IsInitialized() const;

	XnStatus SetUsbAltInterface(uint8_t interfaceNum);
	XnStatus GetUsbAltInterface(uint8_t* pInterfaceNum) const;

	virtual uint16_t GetNumOutputDataConnections() const;
	virtual uint16_t GetNumInputDataConnections() const;

	virtual XnStatus GetControlConnection(ISyncIOConnection*& pConn);
	virtual XnStatus CreateOutputDataConnection(uint16_t nID, IOutputConnection*& pConn);
	virtual XnStatus CreateInputDataConnection(uint16_t nID, IAsyncInputConnection*& pConn);

	static XnStatus EnumerateConnStrings(uint16_t nProductID, XnConnectionString*& astrConnStrings, uint32_t& nCount);
	static void FreeConnStringsList(XnConnectionString* astrConnStrings);

private:
	uint16_t m_nInputConnections;
	uint16_t m_nOutputConnections;
	uint32_t m_nPreControlReceiveSleep;
	uint8_t m_nAltInterface;

	ClientUSBControlEndpoint m_controlEndpoint;
	static const uint16_t NUM_INPUT_CONNECTIONS;
	XN_USB_DEV_HANDLE m_hUSBDevice;
	bool m_bInitialized;
	bool m_bUsbInitialized;
	bool m_dataOpen;
};

}

#endif // XNCLIENTUSBCONNECTIONFACTORY_H
