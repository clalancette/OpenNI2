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
#include "XnClientUSBConnectionFactory.h"
#include "XnClientUSBControlEndpoint.h"
#include "XnClientUSBOutDataEndpoint.h"
#include "XnClientUSBInDataEndpoint.h"
#include "XnLinkProtoUtils.h"
#include <XnUSB.h>
#include <XnLog.h>

#define XN_MASK_USB "xnUSB"

namespace xn
{

const uint16_t ClientUSBConnectionFactory::NUM_INPUT_CONNECTIONS = 2;

ClientUSBConnectionFactory::ClientUSBConnectionFactory(uint16_t nInputConnections,
													   uint16_t nOutputConnections,
													   uint32_t nPreControlReceiveSleep) :
	m_nInputConnections(nInputConnections),
	m_nOutputConnections(nOutputConnections),
	m_nPreControlReceiveSleep(nPreControlReceiveSleep),
	m_nAltInterface(0),
	m_controlEndpoint(nPreControlReceiveSleep),
	m_hUSBDevice(NULL),
	m_bInitialized(false),
	m_bUsbInitialized(false),
	m_dataOpen(false)
{
}

ClientUSBConnectionFactory::~ClientUSBConnectionFactory()
{
	Shutdown();
}

XnStatus ClientUSBConnectionFactory::Init(const XnChar* strConnString)
{
	XnStatus nRetVal = xnUSBInit();
	XN_IS_STATUS_OK_LOG_ERROR("Initialize USB", nRetVal);
	m_bUsbInitialized = true;

	nRetVal = xnUSBOpenDeviceByPath(strConnString, &m_hUSBDevice);
	XN_IS_STATUS_OK_LOG_ERROR("Open USB device", nRetVal);

	//TODO: Check speed maybe?
	nRetVal = m_controlEndpoint.Init(m_hUSBDevice);
	XN_IS_STATUS_OK_LOG_ERROR("Init usb control endpoint", nRetVal);

	m_bInitialized = true;
	return XN_STATUS_OK;
}

void ClientUSBConnectionFactory::Shutdown()
{
	m_controlEndpoint.Shutdown();

	//Close USB device
	if (m_hUSBDevice != NULL)
	{
		xnUSBCloseDevice(m_hUSBDevice);
		m_hUSBDevice = NULL;
	}

	if (m_bUsbInitialized)
	{
		xnUSBShutdown();
		m_bUsbInitialized = false;
	}

	m_bInitialized = false;
}

uint16_t ClientUSBConnectionFactory::GetNumInputDataConnections() const
{
	return m_nInputConnections;
}

uint16_t ClientUSBConnectionFactory::GetNumOutputDataConnections() const
{
	return m_nOutputConnections;
}

bool ClientUSBConnectionFactory::IsInitialized() const
{
	return m_bInitialized;
}

XnStatus ClientUSBConnectionFactory::GetControlConnection(ISyncIOConnection*& pConn)
{
	if (!m_bInitialized)
	{
		return XN_STATUS_NOT_INIT;
	}

	pConn = &m_controlEndpoint;
	return XN_STATUS_OK;
}

XnStatus ClientUSBConnectionFactory::CreateOutputDataConnection(uint16_t /*nID*/, IOutputConnection*& pConn)
{
	//nID is ignored because we only support one output connection in the client
	XnStatus nRetVal = XN_STATUS_OK;
	if (!m_bInitialized)
	{
		return XN_STATUS_NOT_INIT;
	}

	ClientUSBOutDataEndpoint* pUSBOutDataEndpoint = XN_NEW(ClientUSBOutDataEndpoint, XN_USB_EP_ISOCHRONOUS);
	XN_VALIDATE_ALLOC_PTR(pUSBOutDataEndpoint);
	nRetVal = pUSBOutDataEndpoint->Init(m_hUSBDevice);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogError(XN_MASK_USB, "Failed to initialize output data endpoint: %s",
			xnGetStatusString(nRetVal));
		XN_ASSERT(false);
		XN_DELETE(pUSBOutDataEndpoint);
		return nRetVal;
	}

	pConn = pUSBOutDataEndpoint;
	return XN_STATUS_OK;
}

XnStatus ClientUSBConnectionFactory::CreateInputDataConnection(uint16_t nID, IAsyncInputConnection*& pConn)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (!m_bInitialized)
	{
		return XN_STATUS_NOT_INIT;
	}

	ClientUSBInDataEndpoint* pUSBInDataEndpoint = XN_NEW(ClientUSBInDataEndpoint);
	XN_VALIDATE_ALLOC_PTR(pUSBInDataEndpoint);
	nRetVal = pUSBInDataEndpoint->Init(m_hUSBDevice, nID);

	if (nRetVal != XN_STATUS_OK)
	{
		xnLogError(XN_MASK_USB, "Failed to initialize input data endpoint %u: %s",
			nID, xnGetStatusString(nRetVal));
		XN_ASSERT(false);
		XN_DELETE(pUSBInDataEndpoint);
		return nRetVal;
	}
	pConn = pUSBInDataEndpoint;
	m_dataOpen = true;

	return XN_STATUS_OK;
}

XnStatus ClientUSBConnectionFactory::EnumerateConnStrings(uint16_t nProductID,
								XnConnectionString*& astrConnStrings,
								uint32_t& nCount)
{
	XnStatus nRetVal = xnUSBInit();
	if (nRetVal == XN_STATUS_USB_ALREADY_INIT)
	{
		nRetVal = XN_STATUS_OK;
	}
	XN_IS_STATUS_OK_LOG_ERROR("Init usb", nRetVal);
	nRetVal = xnUSBEnumerateDevices(XN_VENDOR_ID, nProductID,
		const_cast<const XnUSBConnectionString**>(&astrConnStrings), &nCount);
	xnUSBShutdown(); //decrease the inner ref-counter
	return nRetVal;
}

void ClientUSBConnectionFactory::FreeConnStringsList(XnConnectionString* astrConnStrings)
{
	xnUSBFreeDevicesList(astrConnStrings);
}

XnStatus ClientUSBConnectionFactory::SetUsbAltInterface(uint8_t interfaceNum)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (m_dataOpen)
	{
		xnLogWarning(XN_MASK_LINK, "Can't set interface once streaming started");
		XN_ASSERT(false);
		return XN_STATUS_BAD_PARAM;
	}

	nRetVal = xnUSBSetInterface(m_hUSBDevice, 0, interfaceNum);
	XN_IS_STATUS_OK(nRetVal);

	m_nAltInterface = interfaceNum;

	return (XN_STATUS_OK);
}

XnStatus ClientUSBConnectionFactory::GetUsbAltInterface(uint8_t* pInterfaceNum) const
{
	*pInterfaceNum = m_nAltInterface;
	return XN_STATUS_OK;
}

}
