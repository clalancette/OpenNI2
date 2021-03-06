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
#include "XnLinkUnpackedDataReductionParser.h"
#include "XnLinkProtoUtils.h"
#include "XnLinkDefs.h"
#include <XnOS.h>
#include <XnLog.h>

namespace xn
{


const uint16_t LinkUnpackedDataReductionParser::FACTOR = 200;


LinkUnpackedDataReductionParser::LinkUnpackedDataReductionParser()
{

}

LinkUnpackedDataReductionParser::~LinkUnpackedDataReductionParser()
{

}

XnStatus LinkUnpackedDataReductionParser::ParsePacketImpl(XnLinkFragmentation /*fragmentation*/,
								const uint8_t* pSrc,
								const uint8_t* pSrcEnd,
								uint8_t*& pDst,
								const uint8_t* pDstEnd)
{
	size_t nPacketDataSize = pSrcEnd - pSrc;

	if ((pDst + nPacketDataSize) > pDstEnd)
	{
		XN_ASSERT(false);
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	////////////////////////////////////////////
	while (pSrc < pSrcEnd)
	{
		*((uint16_t*)pDst) = *((uint16_t*)pSrc) * FACTOR;
		pDst += sizeof(uint16_t);
		pSrc += sizeof(uint16_t);
	}
	////////////////////////////////////////////

	return XN_STATUS_OK;
}

}
