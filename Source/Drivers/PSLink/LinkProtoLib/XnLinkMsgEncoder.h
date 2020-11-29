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
#ifndef XNLINKMSGENCODER_H
#define XNLINKMSGENCODER_H

#include "XnLinkDefs.h"
#include "XnLinkProto.h"
#include "XnLinkProtoUtils.h"
#include <XnStatus.h>

namespace xn
{

class LinkMsgEncoder
{
public:
	LinkMsgEncoder();
	virtual ~LinkMsgEncoder();

	//nMaxMsgSize includes all message headers (not link layer headers). nPacketSize is total link layer packet size, including link layer header.
	virtual XnStatus Init(uint32_t nMaxMsgSize, uint16_t nPacketSize);
	virtual void Shutdown();
	virtual void BeginEncoding(uint16_t nMsgType,
					uint16_t nBasePacketID,
					uint16_t nStreamID,
					XnLinkFragmentation firstPacketFrag = XN_LINK_FRAG_BEGIN,
					uint16_t nCID = 0);

	virtual void EncodeData(const void* pSourceData, uint32_t nSize);
	virtual void EndEncoding(XnLinkFragmentation lastPacketFrag = XN_LINK_FRAG_END);

	virtual const void* GetEncodedData() const;
	virtual uint32_t GetEncodedSize() const;
	virtual uint32_t GetMaxMsgSize() const;
	virtual uint16_t GetMaxPacketSize() const;

	//Returns the packet ID of the current packet. Only valid after encoding.
	virtual uint16_t GetPacketID() const;

	//This sets the packet ID of the CURRENT packet only. Only valid after encoding.
	virtual void SetPacketID(uint16_t nPacketID);

private:
	uint32_t m_nMaxMsgSize;
	uint16_t m_nMaxPacketSize;
	uint32_t m_nMaxNumPackets;
	uint32_t m_nBufferSize;

	/******* This points to the actual encoded data ******/
	XnUInt8* m_pOutputBuffer;
	/*****************************************************/

	union
	{
		XnUInt8* m_pCurrPacketBuffer;
		LinkPacketHeader* m_pCurrPacket;
	};
	LinkPacketHeader m_packetHeader;
	uint32_t m_nEncodedSize;
};

}

#endif // XNLINKMSGENCODER_H
