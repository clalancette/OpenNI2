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
#include "XnClientUSBOutDataEndpoint.h"
#include <XnUSB.h>
#include <XnLog.h>

#define XN_MASK_USB "xnUSB"

namespace xn
{

const uint16_t ClientUSBOutDataEndpoint::ENDPOINT_ID = 0x0001;
const uint32_t ClientUSBOutDataEndpoint::SEND_TIMEOUT = 2000;

ClientUSBOutDataEndpoint::ClientUSBOutDataEndpoint(XnUSBEndPointType endpointType)
{
	m_hEndpoint = NULL;
	m_hUSBDevice = NULL;
	m_nMaxPacketSize = 0;
	m_endpointType = endpointType;
	m_bConnected = false;
}

ClientUSBOutDataEndpoint::~ClientUSBOutDataEndpoint()
{
	Shutdown();
}

XnStatus ClientUSBOutDataEndpoint::Init(XN_USB_DEV_HANDLE hUSBDevice)
{
	XN_VALIDATE_INPUT_PTR(hUSBDevice);
	m_hUSBDevice = hUSBDevice;
	return XN_STATUS_OK;
}

void ClientUSBOutDataEndpoint::Shutdown()
{
	Disconnect();
	m_hUSBDevice = NULL;
}

XnStatus ClientUSBOutDataEndpoint::Connect()
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (!m_bConnected)
	{
		nRetVal = xnUSBOpenEndPoint(m_hUSBDevice, ENDPOINT_ID, m_endpointType, XN_USB_DIRECTION_OUT, &m_hEndpoint);
		XN_IS_STATUS_OK_LOG_ERROR("Open USB Out Data Endpoint", nRetVal);
		uint32_t nTempMaxPacketSize = 0;
		nRetVal = xnUSBGetEndPointMaxPacketSize(m_hEndpoint, &nTempMaxPacketSize);
		XN_IS_STATUS_OK_LOG_ERROR("Get USB Out Data endpoint max packet size", nRetVal);
		if (nTempMaxPacketSize > XN_MAX_UINT16)
		{
			xnLogError(XN_MASK_USB, "Max packet size exceeds max uint16 value ?!");
			XN_ASSERT(false);
			return XN_STATUS_ERROR;
		}
		m_nMaxPacketSize = static_cast<uint16_t>(nTempMaxPacketSize);
		m_bConnected = true;
	}
	return XN_STATUS_OK;
}

void ClientUSBOutDataEndpoint::Disconnect()
{
	if (m_bConnected)
	{
		xnUSBCloseEndPoint(m_hEndpoint);
		m_hEndpoint = NULL;
	}
}

XnStatus ClientUSBOutDataEndpoint::Send(const void* pData, uint32_t nSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	/* TEMP TEMP TEMP - Patch to bypass USB driver bug */
	{
		uint32_t nBlockSize = 8 * m_nMaxPacketSize;
		uint32_t nRemainderSize = nSize % nBlockSize;
		if (nRemainderSize > 0)
		{
			xnLogVerbose(XN_MASK_USB, "Temporary USB patch: rounded up size to %u (instead of %u) before sending data", nSize + nBlockSize - nRemainderSize, nSize);
			// memset rest of buffer (otherwise it will contain old headers)
			xnOSMemSet((uint8_t*)pData + nSize, 0, nBlockSize - nRemainderSize);
			nSize += (nBlockSize - nRemainderSize);
		}
	}
	/* TEMP TEMP TEMP - Patch to bypass USB driver bug */

	nRetVal = xnUSBWriteEndPoint(m_hEndpoint, (unsigned char*)pData, nSize, SEND_TIMEOUT);

	XN_IS_STATUS_OK_LOG_ERROR("Write to USB data endpoint", nRetVal);
	return XN_STATUS_OK;
}

uint16_t ClientUSBOutDataEndpoint::GetMaxPacketSize() const
{
	XN_ASSERT(m_hEndpoint != NULL); //Are we even connected?
	return m_nMaxPacketSize;
}

bool ClientUSBOutDataEndpoint::IsConnected() const
{
	return m_bConnected;
}

}
