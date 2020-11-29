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
#ifndef XN16ZEMBTABLESCODEC_H
#define XN16ZEMBTABLESCODEC_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnJpeg.h>
#include "XnStreamCompression.h"
#include "XnCodecBase.h"
#include "XnCodecIDs.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class Xn16zEmbTablesCodec final : public XnCodecBase
{
public:
	Xn16zEmbTablesCodec(uint16_t nMaxValue) : m_nMaxValue(nMaxValue) {}

	XnCodecID GetCodecID() const override { return XN_CODEC_16Z_EMB_TABLES; }
	XnCompressionFormats GetCompressionFormat() const override { return XN_COMPRESSION_16Z_EMB_TABLE; }

	float GetWorseCompressionRatio() const override { return XN_STREAM_COMPRESSION_DEPTH16Z_WORSE_RATIO; }
	uint32_t GetOverheadSize() const override { return m_nMaxValue * sizeof(uint16_t); }

private:
	XnStatus CompressImpl(const unsigned char* pData, uint32_t nDataSize, unsigned char* pCompressedData, uint32_t* pnCompressedDataSize)
	{
		return XnStreamCompressDepth16ZWithEmbTable((uint16_t*)pData, nDataSize, pCompressedData, pnCompressedDataSize, m_nMaxValue);
	}

	XnStatus DecompressImpl(const unsigned char* pCompressedData, uint32_t nCompressedDataSize, unsigned char* pData, uint32_t* pnDataSize)
	{
		return XnStreamUncompressDepth16ZWithEmbTable(pCompressedData, nCompressedDataSize, (uint16_t*)pData, pnDataSize);
	}

	uint16_t m_nMaxValue;
};

#endif // XN16ZEMBTABLESCODEC_H
