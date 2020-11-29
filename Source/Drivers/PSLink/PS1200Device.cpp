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
#include "PS1200Device.h"
#include "XnClientUSBConnectionFactory.h"
#include <PSLink.h>
#include <XnLog.h>

#define XN_MASK_PS1200_DEVICE "PS1200Device"

namespace xn
{

const uint32_t PS1200Device::WAIT_FOR_FREE_BUFFER_TIMEOUT_MS = XN_WAIT_INFINITE;
const uint32_t PS1200Device::NUM_OUTPUT_CONNECTIONS = 0;
const uint32_t PS1200Device::NUM_INPUT_CONNECTIONS = 3;
const uint32_t PS1200Device::PRE_CONTROL_RECEIVE_SLEEP = 0;

PS1200Device::PS1200Device()
{
	m_hInputInterruptCallback = NULL;
	m_bInitialized = false;
}

PS1200Device::~PS1200Device()
{
	Shutdown();
}

XnStatus PS1200Device::Init(const XnChar* strConnString, XnTransportType transportType)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (transportType != XN_TRANSPORT_TYPE_USB)
	{
		xnLogError(XN_MASK_LINK, "Transport type not supported: %d", transportType);
		XN_ASSERT(false);
		return XN_STATUS_BAD_PARAM;
	}

	nRetVal = PrimeClient::Init(strConnString, XN_TRANSPORT_TYPE_USB);
	XN_IS_STATUS_OK_LOG_ERROR("Init EE Device", nRetVal);

	m_bInitialized = true;
	return XN_STATUS_OK;
}

void PS1200Device::Shutdown()
{
	PrimeClient::Shutdown();
	m_bInitialized = false;
}

bool PS1200Device::IsInitialized() const
{
	return m_bInitialized;
}

IConnectionFactory* PS1200Device::CreateConnectionFactory(XnTransportType transportType)
{
	if (transportType != XN_TRANSPORT_TYPE_USB)
	{
		XN_ASSERT(false);
		return NULL;
	}

	return XN_NEW(ClientUSBConnectionFactory,
			        NUM_INPUT_CONNECTIONS,
					NUM_OUTPUT_CONNECTIONS,
					PRE_CONTROL_RECEIVE_SLEEP);
}

ClientUSBConnectionFactory* PS1200Device::GetConnectionFactory()
{
	return (ClientUSBConnectionFactory*)m_pConnectionFactory;
}

const ClientUSBConnectionFactory* PS1200Device::GetConnectionFactory() const
{
	return (const ClientUSBConnectionFactory*)m_pConnectionFactory;
}

XnStatus PS1200Device::SetUsbAltInterface(uint8_t altInterface)
{
	return GetConnectionFactory()->SetUsbAltInterface(altInterface);
}

XnStatus PS1200Device::GetUsbAltInterface(uint8_t& altInterface) const
{
	return GetConnectionFactory()->GetUsbAltInterface(&altInterface);
}

class UsbEndpointTester : public IDataDestination
{
public:
	void Reset()
	{
		m_nCounter = 0;
		m_nTotalBytes = 0;
		m_nLostPackets = 0;
	}

	virtual XnStatus IncomingData(const void* pData, uint32_t nSize)
	{
		m_nTotalBytes += nSize;

		const uint8_t* pCurData = (const uint8_t*)pData;
		const uint8_t* pEndData = pCurData + nSize;

		while (pCurData < pEndData)
		{
			// first word is a counter
			const uint32_t* pDWords = (const uint32_t*)pCurData;
			uint32_t nPacketSize = pDWords[0];
			uint32_t nCounter = pDWords[1];

			uint32_t nLostPackets = (nCounter - m_nCounter - 1);

			m_nLostPackets += nLostPackets;
			m_nCounter = nCounter;
			pCurData += nPacketSize;
		}

		return XN_STATUS_OK;
	}

	virtual void HandleDisconnection()
	{
		xnLogWarning(XN_MASK_PS1200_DEVICE, "Endpoint disconnected during USB test!");
	}

	uint32_t m_nEP;
	uint32_t m_nTotalBytes;
	uint32_t m_nLostPackets;

private:
	int m_nCounter;
};

XnStatus PS1200Device::UsbTest(uint32_t nSeconds, uint32_t& endpointsCount, XnUsbTestEndpointResult* endpoints)
{
	XnStatus nRetVal = XN_STATUS_OK;

	xn::ClientUSBConnectionFactory* pConnFactory = GetConnectionFactory();

	if (m_linkInputStreamsMgr.HasStreams())
	{
		xnLogWarning(XN_MASK_PS1200_DEVICE, "Can't start USB test when other streams exists!");
		return XN_STATUS_ERROR;
	}

	uint16_t nNumEndpoints = pConnFactory->GetNumInputDataConnections();
	if (nNumEndpoints > endpointsCount)
	{
		xnLogWarning(XN_MASK_PS1200_DEVICE, "Endpoints array is too small");
		return XN_STATUS_BAD_PARAM;
	}

	xn::IAsyncInputConnection* aEndpoints[20];
	UsbEndpointTester aTesters[20];

	for (int i = 0; i < nNumEndpoints; ++i)
	{
		nRetVal = pConnFactory->CreateInputDataConnection((uint16_t)i, aEndpoints[i]);
		if (nRetVal != XN_STATUS_OK)
		{
			for (int j = 0; j < i; ++j)
			{
				XN_DELETE(aEndpoints[j]);
			}
			return nRetVal;
		}

		aTesters[i].Reset();
		aTesters[i].m_nEP = i;
		aEndpoints[i]->SetDataDestination(&aTesters[i]);
		aEndpoints[i]->Connect();
	}

	nRetVal = m_linkControlEndpoint.StartUsbTest();
	if (nRetVal != XN_STATUS_OK)
	{
		for (int i = 0; i < nNumEndpoints; ++i)
		{
			XN_DELETE(aEndpoints[i]);
		}
		return nRetVal;
	}

	// let the test run
	xnOSSleep(nSeconds*1000);

	nRetVal = m_linkControlEndpoint.StopUsbTest();
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_PS1200_DEVICE, "Failed to stop USB test!");
		XN_ASSERT(false);
	}

	for (int i = 0; i < nNumEndpoints; ++i)
	{
		XN_DELETE(aEndpoints[i]);

		endpoints[i].averageBytesPerSecond = aTesters[i].m_nTotalBytes / (double)nSeconds;
		endpoints[i].lostPackets = aTesters[i].m_nLostPackets;
	}
	endpointsCount = nNumEndpoints;

	return (XN_STATUS_OK);
}

}
