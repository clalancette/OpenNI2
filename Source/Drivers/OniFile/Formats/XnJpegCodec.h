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
#ifndef XNJPEGCODEC_H
#define XNJPEGCODEC_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnCodecBase.h"
#include "XnJpeg.h"
#include "XnCodecIDs.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnJpegCodec final : public XnCodecBase
{
public:
	XnJpegCodec(XnBool bRGB, uint32_t nXRes, uint32_t nYRes, uint32_t nQuality = XN_STREAM_COMPRESSION_JPEG_DEFAULT_QUALITY) :
		m_bRGB(bRGB), m_nXRes(nXRes), m_nYRes(nYRes), m_nQuality(nQuality), mp_CompJPEGContext(NULL), mp_UncompJPEGContext(NULL)
	{}

	~XnJpegCodec()
	{
		XnStreamFreeCompressImageJ(&mp_CompJPEGContext);
		XnStreamFreeUncompressImageJ(&mp_UncompJPEGContext);
	}

	XnCodecID GetCodecID() const override { return XN_CODEC_JPEG; }

	XnStatus Init() override
	{
		XnStatus nRetVal = XN_STATUS_OK;

		nRetVal = XnStreamInitCompressImageJ(&mp_CompJPEGContext);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = XnStreamInitUncompressImageJ(&mp_UncompJPEGContext);
		if (nRetVal != XN_STATUS_OK)
		{
			XnStreamFreeCompressImageJ(&mp_CompJPEGContext);
			return (nRetVal);
		}

		return (XN_STATUS_OK);
	}

	XnCompressionFormats GetCompressionFormat() const override { return XN_COMPRESSION_JPEG; }
	float GetWorseCompressionRatio() const override { return XN_STREAM_COMPRESSION_IMAGEJ_WORSE_RATIO; }
	uint32_t GetOverheadSize() const override { return 0; }

private:
	XN_DISABLE_COPY_AND_ASSIGN(XnJpegCodec);

	XnStatus CompressImpl(const XnUChar* pData, uint32_t /*nDataSize*/, XnUChar* pCompressedData, uint32_t* pnCompressedDataSize) override
	{
		if (m_bRGB)
		{
			return XnStreamCompressImage24J(&mp_CompJPEGContext, pData, pCompressedData, pnCompressedDataSize, m_nXRes, m_nYRes, m_nQuality);
		}
		else
		{
			return XnStreamCompressImage8J(&mp_CompJPEGContext, pData, pCompressedData, pnCompressedDataSize, m_nXRes, m_nYRes, m_nQuality);
		}
	}

	XnStatus DecompressImpl(const XnUChar* pCompressedData, uint32_t nCompressedDataSize, XnUChar* pData, uint32_t* pnDataSize) override
	{
		return XnStreamUncompressImageJ(&mp_UncompJPEGContext, pCompressedData, nCompressedDataSize, pData, pnDataSize);
	}

	const XnBool m_bRGB;
	const uint32_t m_nXRes;
	const uint32_t m_nYRes;
	const uint32_t m_nQuality;
	XnStreamCompJPEGContext *mp_CompJPEGContext;
	XnStreamUncompJPEGContext *mp_UncompJPEGContext;
};

#endif // XNJPEGCODEC_H
