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
#ifndef XNLINKOUTPUTDATAENDPOINT_H
#define XNLINKOUTPUTDATAENDPOINT_H

#include <XnPlatform.h>
#include <XnStatus.h>

namespace xn
{

class LinkOutputStreamsMgr;
class IConnectionFactory;
class IOutputConnection;

class LinkOutputDataEndpoint
{
public:
	LinkOutputDataEndpoint();
	virtual ~LinkOutputDataEndpoint();

	XnStatus Init(uint16_t nEndpointID,
	              IConnectionFactory* pConnectionFactory);
	bool IsInitialized() const;
	void Shutdown();
	XnStatus Connect();
	void Disconnect();
	bool IsConnected() const;
	uint16_t GetMaxPacketSize() const;

	XnStatus SendData(const void* pData, uint32_t nSize);

private:
	IOutputConnection* m_pConnection;
	bool m_bInitialized;
	bool m_bConnected;
	uint16_t m_nEndpointID;
};

}

#endif // XNLINKOUTPUTDATAENDPOINT_H
