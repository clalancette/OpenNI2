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
#include "XnLinkMsgParser.h"
#include "XnLinkProtoUtils.h"
#include "XnLinkDefs.h"
#include <XnOS.h>
#include <XnLog.h>

namespace xn
{

LinkMsgParser::LinkMsgParser()
{
	m_pDestBuffer = NULL;
	m_pCurrDest = NULL;
	m_pDestEnd = NULL;
}

LinkMsgParser::~LinkMsgParser()
{
	Shutdown();
}

XnStatus LinkMsgParser::Init()
{
	return XN_STATUS_OK;
}

void LinkMsgParser::Shutdown()
{
	m_pDestBuffer = NULL;
	m_pCurrDest = NULL;
	m_pDestEnd = NULL;
}

XnStatus LinkMsgParser::BeginParsing(void* pDestBuffer, uint32_t nDestBufferSize)
{
	XN_VALIDATE_INPUT_PTR(pDestBuffer);
	m_pDestBuffer = reinterpret_cast<uint8_t*>(pDestBuffer);
	m_pCurrDest = m_pDestBuffer;
	m_pDestEnd = m_pDestBuffer + nDestBufferSize;

	return XN_STATUS_OK;
}

XnStatus LinkMsgParser::ParsePacket(const LinkPacketHeader& header, const uint8_t* pData)
{
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = ParsePacketImpl(header.GetFragmentationFlags(), pData, pData + header.GetDataSize(), m_pCurrDest, m_pDestEnd);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

const void* LinkMsgParser::GetParsedData() const
{
	return m_pDestBuffer;
}

uint32_t LinkMsgParser::GetParsedSize() const
{
	return uint32_t(m_pCurrDest - m_pDestBuffer);
}

uint32_t LinkMsgParser::GetBufferSize() const
{
	return uint32_t(m_pDestEnd - m_pDestBuffer);
}

XnStatus LinkMsgParser::ParsePacketImpl(XnLinkFragmentation /*fragmentation*/,
					const uint8_t* pSrc,
					const uint8_t* pSrcEnd,
					uint8_t*& pDst,
					const uint8_t* pDstEnd)
{
	size_t nPacketDataSize = pSrcEnd - pSrc;
	if (pDst + nPacketDataSize > pDstEnd)
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	xnOSMemCopy(pDst, pSrc, nPacketDataSize);
	pDst += nPacketDataSize;

	return XN_STATUS_OK;
}

}
