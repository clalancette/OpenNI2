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
#ifndef XN8ZCODEC_H
#define XN8ZCODEC_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnCodecBase.h"
#include "XnStreamCompression.h"
#include "XnCodecIDs.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class Xn8zCodec final : public XnCodecBase
{
public:
	XnCodecID GetCodecID() const override { return XN_CODEC_8Z; }
	XnCompressionFormats GetCompressionFormat() const override { return XN_COMPRESSION_COLOR_8Z; }
	XnFloat GetWorseCompressionRatio() const override { return XN_STREAM_COMPRESSION_IMAGE8Z_WORSE_RATIO; }
	uint32_t GetOverheadSize() const override { return 0; }

protected:
	XnStatus CompressImpl(const XnUChar* pData, uint32_t nDataSize, XnUChar* pCompressedData, uint32_t* pnCompressedDataSize)
	{
		return XnStreamCompressImage8Z(pData, nDataSize, pCompressedData, pnCompressedDataSize);
	}

	XnStatus DecompressImpl(const XnUChar* pCompressedData, uint32_t nCompressedDataSize, XnUChar* pData, uint32_t* pnDataSize)
	{
		return XnStreamUncompressImage8Z(pCompressedData, nCompressedDataSize, pData, pnDataSize);
	}
};

#endif // XN8ZCODEC_H
