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
#ifndef XNLINKLOGPARSER_H
#define XNLINKLOGPARSER_H

#include "XnLinkMsgParser.h"
#include <XnHash.h>

namespace xn
{

class LinkLogParser : public LinkMsgParser
{
public:
	LinkLogParser();
	virtual ~LinkLogParser();

	void GenerateOutputBuffer(bool toCreate);

protected:
	virtual XnStatus ParsePacketImpl(XnLinkFragmentation fragmentation,
					const uint8_t* pSrc,
	                                const uint8_t* pSrcEnd,
					uint8_t*& pDst,
					const uint8_t* pDstEnd);

	XnStatus WriteToLogFile(uint8_t fileID, const void* pData, uint32_t dataLength);
	XnStatus CloseLogFile(uint8_t fileID);
	XnStatus OpenLogFile(uint8_t fileID, const char* fileName);
private:
	xnl::Hash<uint8_t, XnDumpFile*> m_activeLogs;
	bool m_copyDataToOutput;
};

}

#endif // XNLINKLOGPARSER_H
