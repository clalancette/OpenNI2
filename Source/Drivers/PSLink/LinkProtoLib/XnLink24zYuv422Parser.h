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
#ifndef XNLINK24ZYUV422PARSER_H
#define XNLINK24ZYUV422PARSER_H

#include "XnLinkMsgParser.h"

namespace xn
{

class Link24zYuv422Parser : public LinkMsgParser
{
public:
	Link24zYuv422Parser(uint32_t xRes, uint32_t yRes, XnBool transformToRGB);
	virtual ~Link24zYuv422Parser();

	virtual XnStatus Init();

protected:
	virtual XnStatus ParsePacketImpl(
		XnLinkFragmentation fragmentation,
		const XnUInt8* pSrc,
		const XnUInt8* pSrcEnd,
		XnUInt8*& pDst,
		const XnUInt8* pDstEnd);

private:
	XnStatus Uncompress24z(
		const XnUInt8* pInput, XnSizeT nInputSize,
		XnUInt8* pOutput, XnSizeT* pnOutputSize, uint32_t nLineSize,
		XnSizeT* pnActualRead, XnBool bLastPart);

	XnUInt8* m_dataFromPrevPacket;
	XnSizeT m_dataFromPrevPacketBytes;
	uint32_t m_lineWidthBytes;
	uint32_t m_rgbFrameSize;
	uint32_t m_expectedFrameSize;
	XnBool m_transformToRGB;
	XnUInt8* m_tempYuvImage; // hold Yuv Image, when transform is required
	uint32_t m_tempYuvImageBytes;
};

}


#endif // XNLINK24ZYUV422PARSER_H
