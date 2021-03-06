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
#ifndef XNLINKINPUTDATAENDPOINT_H
#define XNLINKINPUTDATAENDPOINT_H

#include "IAsyncInputConnection.h"
#include "IConnection.h"
#include <XnOSCpp.h>

struct XnDumpFile;

namespace xn
{

class IConnectionFactory;
class LinkInputStreamsMgr;

class ILinkDataEndpointNotifications
{
public:
	virtual ~ILinkDataEndpointNotifications() {}
	virtual void HandleLinkDataEndpointDisconnection(uint16_t nEndpointID) = 0;
};

class LinkInputDataEndpoint : public IDataDestination, public IConnection
{
public:
	LinkInputDataEndpoint();
	virtual ~LinkInputDataEndpoint();
	XnStatus Init(uint16_t nEndpointID,
				IConnectionFactory* pConnectionFactory,
				LinkInputStreamsMgr* pLinkInputStreamsMgr,
				ILinkDataEndpointNotifications* pNotifications);
	bool IsInitialized() const;

	void Shutdown();
	XnStatus Connect();
	void Disconnect();
	bool IsConnected() const;
	uint16_t GetMaxPacketSize() const;

	/* IDataDestination Implementation */
	virtual XnStatus IncomingData(const void* pData, uint32_t nSize);
	virtual void HandleDisconnection();

private:
	uint16_t m_nEndpointID;
	LinkInputStreamsMgr* m_pLinkInputStreamsMgr;
	ILinkDataEndpointNotifications* m_pNotifications;
	IAsyncInputConnection* m_pConnection;
	IConnectionFactory* m_pConnectionFactory;
	bool m_bInitialized;
	volatile uint32_t m_nConnected;
	XN_CRITICAL_SECTION_HANDLE m_hCriticalSection;
	XnDumpFile* m_pDumpFile;
};

}

#endif // XNLINKINPUTDATAENDPOINT_H
