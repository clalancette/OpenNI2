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
#ifndef XNLINK11BITS2DPARSER_H
#define XNLINK11BITS2DPARSER_H

#include "XnLinkMsgParser.h"
#include "XnShiftToDepth.h"

namespace xn
{

class Link11BitS2DParser : public LinkMsgParser
{
public:
	Link11BitS2DParser(const XnShiftToDepthTables& shiftToDepthTables);
	virtual ~Link11BitS2DParser();

protected:
	virtual XnStatus ParsePacketImpl(XnLinkFragmentation fragmentation,
						const uint8_t* pSrc,
						const uint8_t* pSrcEnd,
						uint8_t*& pDst,
						const uint8_t* pDstEnd);

private:
	uint32_t m_nState;
	uint16_t m_nShift;
	const OniDepthPixel* m_pShiftToDepth;
};

}

#endif // XNLINK11BITS2DPARSER_H
