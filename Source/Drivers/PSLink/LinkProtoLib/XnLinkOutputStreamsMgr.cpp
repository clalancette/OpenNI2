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
#include "XnLinkOutputStreamsMgr.h"
#include "ILinkOutputStream.h"
#include "XnLinkOutputStream.h"
#include <XnLog.h>

namespace xn
{

const uint16_t LinkOutputStreamsMgr::INITIAL_PACKET_ID = 1;

LinkOutputStreamsMgr::LinkOutputStreamsMgr()
{
}

LinkOutputStreamsMgr::~LinkOutputStreamsMgr()
{

}

XnStatus LinkOutputStreamsMgr::Init()
{
	return XN_STATUS_OK;
}

void LinkOutputStreamsMgr::Shutdown()
{
	for (uint16_t nStreamID = 0; nStreamID < m_outputStreams.size(); nStreamID++)
	{
		ShutdownOutputStream(nStreamID);
	}
	m_outputStreams.clear();
}

XnStatus LinkOutputStreamsMgr::InitOutputStream(uint16_t nStreamID,
						uint32_t nMaxMsgSize,
						uint16_t nMaxPacketSize,
						XnLinkCompressionType compression,
						XnStreamFragLevel streamFragLevel,
						LinkOutputDataEndpoint* pOutputDataEndpoint)
{
	XnStatus nRetVal = XN_STATUS_OK;
	ILinkOutputStream* pLinkOutputStream = NULL;
	if (nStreamID < m_outputStreams.size())
	{
		XN_DELETE(m_outputStreams[nStreamID]);
		m_outputStreams[nStreamID] = NULL;
	}

	switch (streamFragLevel)
	{
		case XN_LINK_STREAM_FRAG_LEVEL_FRAMES:
			pLinkOutputStream = XN_NEW(LinkOutputStream);
			break;
		default:
			xnLogError(XN_MASK_LINK, "Bad stream fragmentation level %u", streamFragLevel);
			XN_ASSERT(false);
			return XN_STATUS_ERROR;
	}

	XN_VALIDATE_ALLOC_PTR(pLinkOutputStream);

	nRetVal = pLinkOutputStream->Init(nStreamID,
						nMaxMsgSize,
						nMaxPacketSize,
						compression,
						INITIAL_PACKET_ID,
						pOutputDataEndpoint);
	if (nRetVal != XN_STATUS_OK)
	{
		XN_DELETE(pLinkOutputStream);
		xnLogError(XN_MASK_LINK, "Failed to initialize link output stream %u: %s", nStreamID, xnGetStatusString(nRetVal));
		XN_ASSERT(false);
		return nRetVal;
	}

	if (nStreamID >= m_outputStreams.size())
	{
		m_outputStreams.resize(nStreamID + 1);
	}
	m_outputStreams[nStreamID] = pLinkOutputStream;

	return XN_STATUS_OK;
}

void LinkOutputStreamsMgr::ShutdownOutputStream(uint16_t nStreamID)
{
	if (nStreamID > m_outputStreams.size())
	{
		xnLogWarning(XN_MASK_LINK, "Stream ID %u is not in array", nStreamID);
		XN_ASSERT(false);
		return;
	}

	if (m_outputStreams[nStreamID] != NULL)
	{
		m_outputStreams[nStreamID]->Shutdown();
		XN_DELETE(m_outputStreams[nStreamID]);
		m_outputStreams[nStreamID] = NULL;
	}
}

XnStatus LinkOutputStreamsMgr::SendData(uint16_t nStreamID,
										uint16_t nMsgType,
										uint16_t nCID,
										XnLinkFragmentation fragmentation,
										const void* pData,
										uint32_t nDataSize)
{
	XnStatus nRetVal = XN_STATUS_OK;
	if ((nStreamID >= m_outputStreams.size()) || (m_outputStreams[nStreamID] == NULL) ||
		!m_outputStreams[nStreamID]->IsInitialized())
	{
		xnLogError(XN_MASK_LINK, "Stream %u is not initialized", nStreamID);
		XN_ASSERT(false);
		return XN_STATUS_NOT_INIT;
	}

	nRetVal = m_outputStreams[nStreamID]->SendData(nMsgType, nCID, fragmentation, pData, nDataSize);
	XN_IS_STATUS_OK_LOG_ERROR("Send data on output stream", nRetVal);
	return XN_STATUS_OK;
}

bool LinkOutputStreamsMgr::IsStreamInitialized( uint16_t nStreamID ) const
{
	return (
		nStreamID < m_outputStreams.size() &&
		m_outputStreams[nStreamID] != NULL &&
		m_outputStreams[nStreamID]->IsInitialized());
}

}
