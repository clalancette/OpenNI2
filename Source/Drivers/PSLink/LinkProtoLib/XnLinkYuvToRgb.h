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
#ifndef XNLINKYUVTORGB_H
#define XNLINKYUVTORGB_H

namespace xn
{

class LinkYuvToRgb
{
public:
	enum {
		YUV_422_BYTES_PER_PIXEL = 2,
		RGB_888_BYTES_PER_PIXEL = 3
	};

	static XnStatus Yuv422ToRgb888(const uint8_t* pSrc, size_t srcBytes, uint8_t* pDst, size_t& dstSize);
};

}

#endif // XNLINKYUVTORGB_H
