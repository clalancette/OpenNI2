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
#ifndef ILINKOUTPUTSTREAM_H
#define ILINKOUTPUTSTREAM_H

#include "XnLinkProtoLibDefs.h"
#include "XnLinkProtoUtils.h"
#include <XnPlatform.h>
#include <XnStatus.h>

namespace xn
{

class LinkOutputDataEndpoint;

class ILinkOutputStream
{
public:
	virtual ~ILinkOutputStream() {}

	virtual XnStatus Init(uint16_t nStreamID,
				uint32_t nMaxMsgSize,
				uint16_t nMaxPacketSize,
				XnLinkCompressionType compression,
				uint16_t nInitialPacketID,
				LinkOutputDataEndpoint* pOutputDataEndpoint) = 0;

	virtual bool IsInitialized() const = 0;
	virtual void Shutdown() = 0;
	virtual XnLinkCompressionType GetCompression() const = 0;

	virtual XnStatus SendData(uint16_t nMsgType,
					uint16_t nCID,
					XnLinkFragmentation fragmentation,
					const void* pData,
					uint32_t nDataSize) const = 0;

};

}

#endif // ILINKOUTPUTSTREAM_H
