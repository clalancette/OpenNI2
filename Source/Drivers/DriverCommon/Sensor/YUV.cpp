/*****************************************************************************
*									     *
*  OpenNI 2.x Alpha							     *
*  Copyright (C) 2012 PrimeSense Ltd.					     *
*									     *
*  This file is part of OpenNI. 					     *
*									     *
*  Licensed under the Apache License, Version 2.0 (the "License");	     *
*  you may not use this file except in compliance with the License.	     *
*  You may obtain a copy of the License at				     *
*									     *
*      http://www.apache.org/licenses/LICENSE-2.0			     *
*									     *
*  Unless required by applicable law or agreed to in writing, software	     *
*  distributed under the License is distributed on an "AS IS" BASIS,	     *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and	     *
*  limitations under the License.					     *
*									     *
*****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "YUV.h"
#include <math.h>

#if (XN_PLATFORM == XN_PLATFORM_WIN32)
	#ifdef __INTEL_COMPILER
		#include <ia32intrin.h>
	#else
		#include <emmintrin.h>
	#endif
#endif

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
void YUV444ToRGB888(uint8_t cY, uint8_t cU, uint8_t cV,
					uint8_t& cR, uint8_t& cG, uint8_t& cB)
{
	int32_t nC = cY - 16;
	int16_t nD = cU - 128;
	int16_t nE = cV - 128;

	nC = nC * 298 + 128;

	cR = (uint8_t)XN_MIN(XN_MAX((nC 	   + 409 * nE) >> 8, 0), 255);
	cG = (uint8_t)XN_MIN(XN_MAX((nC - 100 * nD - 208 * nE) >> 8, 0), 255);
	cB = (uint8_t)XN_MIN(XN_MAX((nC + 516 * nD	     ) >> 8, 0), 255);
}

#if (XN_PLATFORM == XN_PLATFORM_WIN32)

void YUV422ToRGB888(const uint8_t* pYUVImage, uint8_t* pRGBImage, uint32_t nYUVSize, uint32_t* pnActualRead, uint32_t* pnRGBSize)
{
	const uint8_t* pYUVLast = pYUVImage + nYUVSize - 8;
	const uint8_t* pYUVOrig = pYUVImage;
	const uint8_t* pRGBOrig = pRGBImage;
	const uint8_t* pRGBLast = pRGBImage + *pnRGBSize - 12;

	const __m128 minus128 = _mm_set_ps1(-128);
	const __m128 plus113983 = _mm_set_ps1(1.13983F);
	const __m128 minus039466 = _mm_set_ps1(-0.39466F);
	const __m128 minus058060 = _mm_set_ps1(-0.58060F);
	const __m128 plus203211 = _mm_set_ps1(2.03211F);
	const __m128 zero = _mm_set_ps1(0);
	const __m128 plus255 = _mm_set_ps1(255);

	// define YUV floats
	__m128 y;
	__m128 u;
	__m128 v;

	__m128 temp;

	// define RGB floats
	__m128 r;
	__m128 g;
	__m128 b;

	// define RGB integers
	__m128i iR;
	__m128i iG;
	__m128i iB;

	uint32_t* piR = (uint32_t*)&iR;
	uint32_t* piG = (uint32_t*)&iG;
	uint32_t* piB = (uint32_t*)&iB;

	while (pYUVImage <= pYUVLast && pRGBImage <= pRGBLast)
	{
		// process 4 pixels at once (values should be ordered backwards)
		y = _mm_set_ps(pYUVImage[YUV422_Y2 + YUV422_BPP], pYUVImage[YUV422_Y1 + YUV422_BPP], pYUVImage[YUV422_Y2], pYUVImage[YUV422_Y1]);
		u = _mm_set_ps(pYUVImage[YUV422_U + YUV422_BPP],  pYUVImage[YUV422_U + YUV422_BPP],  pYUVImage[YUV422_U],  pYUVImage[YUV422_U]);
		v = _mm_set_ps(pYUVImage[YUV422_V + YUV422_BPP],  pYUVImage[YUV422_V + YUV422_BPP],  pYUVImage[YUV422_V],  pYUVImage[YUV422_V]);

		u = _mm_add_ps(u, minus128); // u -= 128
		v = _mm_add_ps(v, minus128); // v -= 128

		/*

		http://en.wikipedia.org/wiki/YUV

		From YUV to RGB:
		R =	Y + 1.13983 V
		G =	Y - 0.39466 U - 0.58060 V
		B =	Y + 2.03211 U

		*/

		temp = _mm_mul_ps(plus113983, v);
		r = _mm_add_ps(y, temp);

		temp = _mm_mul_ps(minus039466, u);
		g = _mm_add_ps(y, temp);
		temp = _mm_mul_ps(minus058060, v);
		g = _mm_add_ps(g, temp);

		temp = _mm_mul_ps(plus203211, u);
		b = _mm_add_ps(y, temp);

		// make sure no value is smaller than 0
		r = _mm_max_ps(r, zero);
		g = _mm_max_ps(g, zero);
		b = _mm_max_ps(b, zero);

		// make sure no value is bigger than 255
		r = _mm_min_ps(r, plus255);
		g = _mm_min_ps(g, plus255);
		b = _mm_min_ps(b, plus255);

		// convert floats to int16 (there is no conversion to uint8, just to int8).
		iR = _mm_cvtps_epi32(r);
		iG = _mm_cvtps_epi32(g);
		iB = _mm_cvtps_epi32(b);

		// extract the 4 pixels RGB values.
		// because we made sure values are between 0 and 255, we can just take the lower byte
		// of each INT16
		pRGBImage[0] = (uint8_t)piR[0];
		pRGBImage[1] = (uint8_t)piG[0];
		pRGBImage[2] = (uint8_t)piB[0];

		pRGBImage[3] = (uint8_t)piR[1];
		pRGBImage[4] = (uint8_t)piG[1];
		pRGBImage[5] = (uint8_t)piB[1];

		pRGBImage[6] = (uint8_t)piR[2];
		pRGBImage[7] = (uint8_t)piG[2];
		pRGBImage[8] = (uint8_t)piB[2];

		pRGBImage[9] = (uint8_t)piR[3];
		pRGBImage[10] = (uint8_t)piG[3];
		pRGBImage[11] = (uint8_t)piB[3];

		// advance the streams
		pYUVImage += 8;
		pRGBImage += 12;
	}

	*pnActualRead = (uint32_t)(pYUVImage - pYUVOrig);
	*pnRGBSize = (uint32_t)(pRGBImage - pRGBOrig);
}

#else // not Win32

void YUV422ToRGB888(const uint8_t* pYUVImage, uint8_t* pRGBImage, uint32_t nYUVSize, uint32_t* pnActualRead, uint32_t* pnRGBSize)
{
	const uint8_t* pOrigYUV = pYUVImage;
	const uint8_t* pCurrYUV = pYUVImage;
	const uint8_t* pOrigRGB = pRGBImage;
	uint8_t* pCurrRGB = pRGBImage;
	const uint8_t* pLastYUV = pYUVImage + nYUVSize - YUV422_BPP;
	const uint8_t* pLastRGB = pRGBImage + *pnRGBSize - YUV_RGB_BPP;

	while (pCurrYUV <= pLastYUV && pCurrRGB <= pLastRGB)
	{
		YUV444ToRGB888(pCurrYUV[YUV422_Y1], pCurrYUV[YUV422_U], pCurrYUV[YUV422_V],
						pCurrRGB[YUV_RED], pCurrRGB[YUV_GREEN], pCurrRGB[YUV_BLUE]);
		pCurrRGB += YUV_RGB_BPP;
		YUV444ToRGB888(pCurrYUV[YUV422_Y2], pCurrYUV[YUV422_U], pCurrYUV[YUV422_V],
						pCurrRGB[YUV_RED], pCurrRGB[YUV_GREEN], pCurrRGB[YUV_BLUE]);
		pCurrRGB += YUV_RGB_BPP;
		pCurrYUV += YUV422_BPP;
	}

	*pnActualRead = pCurrYUV - pOrigYUV;
	*pnRGBSize = pCurrRGB - pOrigRGB;
}

#endif

#if (XN_PLATFORM == XN_PLATFORM_WIN32)

void YUYVToRGB888(const uint8_t* pYUVImage, uint8_t* pRGBImage, uint32_t nYUVSize, uint32_t* pnActualRead, uint32_t* pnRGBSize)
{
	const uint8_t* pYUVLast = pYUVImage + nYUVSize - 8;
	const uint8_t* pYUVOrig = pYUVImage;
	const uint8_t* pRGBOrig = pRGBImage;
	const uint8_t* pRGBLast = pRGBImage + *pnRGBSize - 12;

	const __m128 minus128 = _mm_set_ps1(-128);
	const __m128 plus113983 = _mm_set_ps1(1.13983F);
	const __m128 minus039466 = _mm_set_ps1(-0.39466F);
	const __m128 minus058060 = _mm_set_ps1(-0.58060F);
	const __m128 plus203211 = _mm_set_ps1(2.03211F);
	const __m128 zero = _mm_set_ps1(0);
	const __m128 plus255 = _mm_set_ps1(255);

	// define YUV floats
	__m128 y;
	__m128 u;
	__m128 v;

	__m128 temp;

	// define RGB floats
	__m128 r;
	__m128 g;
	__m128 b;

	// define RGB integers
	__m128i iR;
	__m128i iG;
	__m128i iB;

	uint32_t* piR = (uint32_t*)&iR;
	uint32_t* piG = (uint32_t*)&iG;
	uint32_t* piB = (uint32_t*)&iB;

	while (pYUVImage <= pYUVLast && pRGBImage <= pRGBLast)
	{
		// process 4 pixels at once (values should be ordered backwards)
		y = _mm_set_ps(pYUVImage[YUYV_Y2 + YUYV_BPP], pYUVImage[YUYV_Y1 + YUYV_BPP], pYUVImage[YUYV_Y2], pYUVImage[YUYV_Y1]);
		u = _mm_set_ps(pYUVImage[YUYV_U + YUYV_BPP],  pYUVImage[YUYV_U + YUYV_BPP],  pYUVImage[YUYV_U],  pYUVImage[YUYV_U]);
		v = _mm_set_ps(pYUVImage[YUYV_V + YUYV_BPP],  pYUVImage[YUYV_V + YUYV_BPP],  pYUVImage[YUYV_V],  pYUVImage[YUYV_V]);

		u = _mm_add_ps(u, minus128); // u -= 128
		v = _mm_add_ps(v, minus128); // v -= 128

		/*

		http://en.wikipedia.org/wiki/YUV

		From YUV to RGB:
		R =	Y + 1.13983 V
		G =	Y - 0.39466 U - 0.58060 V
		B =	Y + 2.03211 U

		*/

		temp = _mm_mul_ps(plus113983, v);
		r = _mm_add_ps(y, temp);

		temp = _mm_mul_ps(minus039466, u);
		g = _mm_add_ps(y, temp);
		temp = _mm_mul_ps(minus058060, v);
		g = _mm_add_ps(g, temp);

		temp = _mm_mul_ps(plus203211, u);
		b = _mm_add_ps(y, temp);

		// make sure no value is smaller than 0
		r = _mm_max_ps(r, zero);
		g = _mm_max_ps(g, zero);
		b = _mm_max_ps(b, zero);

		// make sure no value is bigger than 255
		r = _mm_min_ps(r, plus255);
		g = _mm_min_ps(g, plus255);
		b = _mm_min_ps(b, plus255);

		// convert floats to int16 (there is no conversion to uint8, just to int8).
		iR = _mm_cvtps_epi32(r);
		iG = _mm_cvtps_epi32(g);
		iB = _mm_cvtps_epi32(b);

		// extract the 4 pixels RGB values.
		// because we made sure values are between 0 and 255, we can just take the lower byte
		// of each INT16
		pRGBImage[0] = (uint8_t)piR[0];
		pRGBImage[1] = (uint8_t)piG[0];
		pRGBImage[2] = (uint8_t)piB[0];

		pRGBImage[3] = (uint8_t)piR[1];
		pRGBImage[4] = (uint8_t)piG[1];
		pRGBImage[5] = (uint8_t)piB[1];

		pRGBImage[6] = (uint8_t)piR[2];
		pRGBImage[7] = (uint8_t)piG[2];
		pRGBImage[8] = (uint8_t)piB[2];

		pRGBImage[9] = (uint8_t)piR[3];
		pRGBImage[10] = (uint8_t)piG[3];
		pRGBImage[11] = (uint8_t)piB[3];

		// advance the streams
		pYUVImage += 8;
		pRGBImage += 12;
	}

	*pnActualRead = (uint32_t)(pYUVImage - pYUVOrig);
	*pnRGBSize = (uint32_t)(pRGBImage - pRGBOrig);
}

#else // not Win32

void YUYVToRGB888(const uint8_t* pYUVImage, uint8_t* pRGBImage, uint32_t nYUVSize, uint32_t* pnActualRead, uint32_t* pnRGBSize)
{
	const uint8_t* pOrigYUV = pYUVImage;
	const uint8_t* pCurrYUV = pYUVImage;
	const uint8_t* pOrigRGB = pRGBImage;
	uint8_t* pCurrRGB = pRGBImage;
	const uint8_t* pLastYUV = pYUVImage + nYUVSize - YUYV_BPP;
	const uint8_t* pLastRGB = pRGBImage + *pnRGBSize - YUV_RGB_BPP;

	while (pCurrYUV <= pLastYUV && pCurrRGB <= pLastRGB)
	{
		YUV444ToRGB888(pCurrYUV[YUYV_Y1], pCurrYUV[YUYV_U], pCurrYUV[YUYV_V],
						pCurrRGB[YUV_RED], pCurrRGB[YUV_GREEN], pCurrRGB[YUV_BLUE]);
		pCurrRGB += YUV_RGB_BPP;
		YUV444ToRGB888(pCurrYUV[YUYV_Y2], pCurrYUV[YUYV_U], pCurrYUV[YUYV_V],
						pCurrRGB[YUV_RED], pCurrRGB[YUV_GREEN], pCurrRGB[YUV_BLUE]);
		pCurrRGB += YUV_RGB_BPP;
		pCurrYUV += YUYV_BPP;
	}

	*pnActualRead = pCurrYUV - pOrigYUV;
	*pnRGBSize = pCurrRGB - pOrigRGB;
}

#endif

void YUV420ToRGB888(const uint8_t* pYUVImage, uint8_t* pRGBImage, uint32_t nYUVSize, uint32_t /*nRGBSize*/)
{
	const uint8_t* pLastYUV = pYUVImage + nYUVSize - YUV420_BPP;

	while (pYUVImage < pLastYUV && pRGBImage < pYUVImage)
	{
		YUV444ToRGB888(pYUVImage[YUV420_Y1], pYUVImage[YUV420_U], pYUVImage[YUV420_V],
			pRGBImage[YUV_RED], pRGBImage[YUV_GREEN], pRGBImage[YUV_BLUE]);
		pRGBImage += YUV_RGB_BPP;

		YUV444ToRGB888(pYUVImage[YUV420_Y2], pYUVImage[YUV420_U], pYUVImage[YUV420_V],
			pRGBImage[YUV_RED], pRGBImage[YUV_GREEN], pRGBImage[YUV_BLUE]);
		pRGBImage += YUV_RGB_BPP;

		YUV444ToRGB888(pYUVImage[YUV420_Y3], pYUVImage[YUV420_U], pYUVImage[YUV420_V],
			pRGBImage[YUV_RED], pRGBImage[YUV_GREEN], pRGBImage[YUV_BLUE]);
		pRGBImage += YUV_RGB_BPP;

		YUV444ToRGB888(pYUVImage[YUV420_Y4], pYUVImage[YUV420_U], pYUVImage[YUV420_V],
			pRGBImage[YUV_RED], pRGBImage[YUV_GREEN], pRGBImage[YUV_BLUE]);
		pRGBImage += YUV_RGB_BPP;

		pYUVImage += YUV420_BPP;
	}
}
