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
#ifndef XNLINKINPUTSTREAMSMGR_H
#define XNLINKINPUTSTREAMSMGR_H

#include "XnLinkDefs.h"
#include "XnLinkProtoLibDefs.h"
#include "XnLinkProtoUtils.h"
#include "XnLinkInputStream.h"
#include <XnStatus.h>
#include <XnHash.h>

namespace xn
{

class LinkControlEndpoint;
class IConnection;

class LinkInputStreamsMgr
{

public:
	LinkInputStreamsMgr();
	~LinkInputStreamsMgr();

	XnStatus Init();
	void Shutdown();

	void RegisterStreamOfType(XnStreamType streamType, const XnChar* strCreationInfo, uint16_t nStreamID);
	bool UnregisterStream(uint16_t nStreamID); // returns true if the unregistered stream was the last one
	bool HasStreamOfType(XnStreamType streamType, const XnChar* strCreationInfo, uint16_t& nStreamID);

	XnStatus InitInputStream(LinkControlEndpoint* pLinkControlEndpoint,
					XnStreamType streamType,
					uint16_t nStreamID,
					IConnection* pConnection);

	void ShutdownInputStream(uint16_t nStreamID);
	XnStatus HandleData(const void* pData, uint32_t nSize);
	const LinkInputStream* GetInputStream(uint16_t nStreamID) const;
	LinkInputStream* GetInputStream(uint16_t nStreamID);

	bool HasStreams() const;

private:
	void HandlePacket(const LinkPacketHeader* pLinkPacketHeader);
	int FindStreamByType(XnStreamType streamType, const XnChar* strCreationInfo); //returns found streamId, or -1

	static const uint32_t FRAG_FLAGS_ALLOWED_CHANGES[4][4];
	static const uint16_t INITIAL_PACKET_ID;

	struct StreamInfo
	{
		uint16_t nNextPacketID;
		uint16_t nMsgType;
		XnLinkFragmentation prevFragmentation;
		XnStreamFragLevel streamFragLevel;
		LinkInputStream* pInputStream;
		bool packetLoss;

		XnStreamType streamType;
		const XnChar* strCreationInfo;
		int refCount;
	};

	StreamInfo m_streamInfos[XN_LINK_MAX_STREAMS];
};

}

#endif // XNLINKINPUTSTREAMSMGR_H
