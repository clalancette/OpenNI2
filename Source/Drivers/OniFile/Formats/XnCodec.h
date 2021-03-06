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
#ifndef XNCODEC_H
#define XNCODEC_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnStatus.h"
#include "XnStreamFormats.h"

typedef uint32_t XnCodecID;

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnCodec
{
public:
	XnCodec() {}
	virtual ~XnCodec() {}

	virtual XnCodecID GetCodecID() const = 0;

	virtual XnStatus Init() { return XN_STATUS_OK; }

	virtual XnCompressionFormats GetCompressionFormat() const = 0;

	virtual XnStatus Compress(const unsigned char* pData, uint32_t nDataSize, unsigned char* pCompressedData, uint32_t* pnCompressedDataSize) = 0;

	virtual XnStatus Decompress(const unsigned char* pCompressedData, uint32_t nCompressedDataSize, unsigned char* pData, uint32_t* pnDataSize) = 0;

	static XnCompressionFormats GetCompressionFormatFromCodecID(XnCodecID codecID);
	static XnCodecID GetCodecIDFromCompressionFormat(XnCompressionFormats format);
};

#endif // XNCODEC_H
