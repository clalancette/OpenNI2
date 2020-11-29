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

#include "DepthUtilsImpl.h"

static int32_t GetFieldValueSigned(uint32_t regValue, int32_t fieldWidth, int32_t fieldOffset)
{
	int32_t val = (int)(regValue>>fieldOffset);
	val = (val<<(32-fieldWidth))>>(32-fieldWidth);
	return val;
}

static void incrementalFitting50(int64_t dPrev, int64_t ddPrev, int64_t dddPrev, int64_t coeff, int32_t betaPrev, int32_t dBeta, int64_t &dCurr, int64_t &ddCurr, int64_t &dddCurr, int32_t &betaCurr);
static void incrementalFitting50(int64_t ddPrev, int64_t dddPrev, int64_t coeff, int64_t &ddCurr, int64_t &dddCurr) { int64_t dummy1; int32_t dummy2; incrementalFitting50(0, ddPrev, dddPrev, coeff, 0, 0, dummy1, ddCurr, dddCurr, dummy2); }
static void incrementalFitting50(int64_t dddPrev, int64_t coeff, int64_t &dddCurr) { int64_t dummy1, dummy2; int32_t dummy3; incrementalFitting50(0, 0, dddPrev, coeff, 0, 0, dummy1, dummy2, dddCurr, dummy3); }
static void incrementalFitting50(int64_t dPrev, int64_t ddPrev, int64_t dddPrev, int64_t coeff, int32_t betaPrev, int32_t dBeta, int64_t &dCurr, int64_t &ddCurr, int64_t &dddCurr, int32_t &betaCurr)
{
	dCurr = dPrev+(ddPrev>>6);
	ddCurr = ddPrev+(dddPrev>>8);
	dddCurr = dddPrev+coeff;
	betaCurr = betaPrev+dBeta;
}

DepthUtilsImpl::DepthUtilsImpl() : m_pDepthToShiftTable_QQVGA(NULL), m_pDepthToShiftTable_QVGA(NULL), m_pDepthToShiftTable_VGA(NULL),
									m_pRegistrationTable_QQVGA(NULL), m_pRegistrationTable_QVGA(NULL), m_pRegistrationTable_VGA(NULL), m_bD2SAlloc(false), m_bInitialized(FALSE)
{
}

DepthUtilsImpl::~DepthUtilsImpl()
{
	Free();
}

XnStatus DepthUtilsImpl::Initialize(DepthUtilsSensorCalibrationInfo* pBlob)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (pBlob == NULL)
	{
		return XN_STATUS_BAD_PARAM;
	}

	if (pBlob->magic != ONI_DEPTH_UTILS_CALIBRATION_INFO_MAGIC)
	{
		return XN_STATUS_BAD_PARAM;
	}

	Free();

	xnOSMemCopy(&m_blob, pBlob, sizeof(DepthUtilsSensorCalibrationInfo));

	// allocate table
	XN_VALIDATE_ALIGNED_CALLOC(m_pRegistrationTable_QQVGA, XnUInt16, 160*120*2, XN_DEFAULT_MEM_ALIGN);
	XN_VALIDATE_ALIGNED_CALLOC(m_pRegistrationTable_QVGA, XnUInt16, 320*240*2, XN_DEFAULT_MEM_ALIGN);
	XN_VALIDATE_ALIGNED_CALLOC(m_pRegistrationTable_VGA, XnUInt16, 640*480*2, XN_DEFAULT_MEM_ALIGN);

	nRetVal = BuildRegistrationTable(m_pRegistrationTable_QQVGA, &m_blob.params1080.registrationInfo_QQVGA, &m_pDepthToShiftTable_QQVGA, 160, 120);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = BuildRegistrationTable(m_pRegistrationTable_QVGA, &m_blob.params1080.registrationInfo_QVGA, &m_pDepthToShiftTable_QVGA, 320, 240);
	XN_IS_STATUS_OK(nRetVal);
	nRetVal = BuildRegistrationTable(m_pRegistrationTable_VGA, &m_blob.params1080.registrationInfo_VGA, &m_pDepthToShiftTable_VGA, 640, 480);
	XN_IS_STATUS_OK(nRetVal);

	m_bInitialized = TRUE;

	return XN_STATUS_OK;

}

XnStatus DepthUtilsImpl::Free()
{
	m_bInitialized = FALSE;

	if (m_pRegistrationTable_QQVGA != NULL)
	{
		xnOSFreeAligned(m_pRegistrationTable_QQVGA);
		m_pRegistrationTable_QQVGA = NULL;
	}
	if (m_pRegistrationTable_QVGA != NULL)
	{
		xnOSFreeAligned(m_pRegistrationTable_QVGA);
		m_pRegistrationTable_QVGA = NULL;
	}
	if (m_pRegistrationTable_VGA != NULL)
	{
		xnOSFreeAligned(m_pRegistrationTable_VGA);
		m_pRegistrationTable_VGA = NULL;
	}


	if (m_bD2SAlloc)
	{
		if (m_pDepthToShiftTable_QQVGA != NULL)
		{
			xnOSFreeAligned(m_pDepthToShiftTable_QQVGA);
			m_pDepthToShiftTable_QQVGA = NULL;
		}
		if (m_pDepthToShiftTable_QVGA != NULL)
		{
			xnOSFreeAligned(m_pDepthToShiftTable_QVGA);
			m_pDepthToShiftTable_QVGA = NULL;
		}
		if (m_pDepthToShiftTable_VGA != NULL)
		{
			xnOSFreeAligned(m_pDepthToShiftTable_VGA);
			m_pDepthToShiftTable_VGA = NULL;
		}

		m_bD2SAlloc = FALSE;
	}

	return (XN_STATUS_OK);
}

XnStatus DepthUtilsImpl::Apply(unsigned short* pOutput)
{
	unsigned short* pTempBuffer;
	pTempBuffer = (unsigned short*)xnOSCallocAligned(m_depthResolution.x*m_depthResolution.y, sizeof(unsigned short), XN_DEFAULT_MEM_ALIGN);

	unsigned short* pInput = pTempBuffer;
	memcpy(pInput, pOutput, m_depthResolution.x*m_depthResolution.y*2);

	XnInt16* pRegTable;
	XnInt16* pRGBRegDepthToShiftTable = (XnInt16*)m_pDepth2ShiftTable;
	unsigned short nValue = 0;
	unsigned short nOutValue = 0;
	uint32_t nNewX = 0;
	uint32_t nNewY = 0;
	uint32_t nArrPos = 0;
	uint32_t nDepthXRes = m_depthResolution.x;
	uint32_t nDepthYRes = m_depthResolution.y;

	memset(pOutput, 0, nDepthXRes*nDepthYRes*sizeof(unsigned short));

	XnBool bMirror = m_isMirrored;

	uint32_t nLinesShift = m_pPadInfo->nCroppingLines - m_pPadInfo->nStartLines;

	for (uint32_t y = 0; y < nDepthYRes; ++y)
	{
		pRegTable = (XnInt16*)&m_pRegTable[ bMirror ? ((y+1) * nDepthXRes - 1) * 2 : y * nDepthXRes * 2 ];
		for (uint32_t x = 0; x < nDepthXRes; ++x)
		{
			nValue = *pInput;

			if (nValue != 0)
			{
				nNewX = (uint32_t)(*pRegTable + pRGBRegDepthToShiftTable[nValue]) / m_blob.params1080.rgbRegXValScale;
				nNewY = *(pRegTable+1);

				if (nNewX < nDepthXRes && nNewY > nLinesShift)
				{
					nNewY -= nLinesShift;
					nArrPos = bMirror ? (nNewY+1)*nDepthXRes - nNewX - 1 : (nNewY*nDepthXRes) + nNewX;

					nOutValue = pOutput[nArrPos];

					if ((nOutValue == 0) || (nOutValue > nValue))
					{
						if ( nNewX > 0 && nNewY > 0 )
						{
							pOutput[nArrPos-nDepthXRes] = nValue;
							pOutput[nArrPos-nDepthXRes-1] = nValue;
							pOutput[nArrPos-1] = nValue;
						}
						else if( nNewY > 0 )
						{
							pOutput[nArrPos-nDepthXRes] = nValue;
						}
						else if( nNewX > 0 )
						{
							pOutput[nArrPos-1] = nValue;
						}

						pOutput[nArrPos] = nValue;
					}
				}
			}

			pInput++;
			bMirror ? pRegTable-=2 : pRegTable+=2;
		}
	}
	xnOSFreeAligned(pTempBuffer);

	return XN_STATUS_OK;
}

XnStatus DepthUtilsImpl::SetDepthConfiguration(int xres, int yres, OniPixelFormat /*format*/, bool isMirrored)
{
	m_isMirrored = isMirrored;

	if (xres == 160 && yres == 120)
	{
		m_pPadInfo = &m_blob.params1080.padInfo_QQVGA;
		m_pRegTable = m_pRegistrationTable_QQVGA;
		m_pDepth2ShiftTable = (XnUInt16*)m_pDepthToShiftTable_QQVGA;
		m_pRegistrationInfo = &m_blob.params1080.registrationInfo_QQVGA;
	}
	else if (xres == 320 && yres == 240)
	{
		m_pPadInfo = &m_blob.params1080.padInfo_QVGA;
		m_pRegTable = m_pRegistrationTable_QVGA;
		m_pDepth2ShiftTable = (XnUInt16*)m_pDepthToShiftTable_QVGA;
		m_pRegistrationInfo = &m_blob.params1080.registrationInfo_QVGA;
	}
	else if (xres == 640 && yres == 480)
	{
		m_pPadInfo = &m_blob.params1080.padInfo_VGA;
		m_pRegTable = m_pRegistrationTable_VGA;
		m_pDepth2ShiftTable = (XnUInt16*)m_pDepthToShiftTable_VGA;
		m_pRegistrationInfo = &m_blob.params1080.registrationInfo_VGA;
	}
	else if (xres == 1280 && yres == 1024)
	{
		m_pPadInfo = &m_blob.params1080.padInfo_SXGA;
		m_pRegTable = m_pRegistrationTable_SXGA;
		m_pDepth2ShiftTable = (XnUInt16*)m_pDepthToShiftTable_SXGA;
		m_pRegistrationInfo = &m_blob.params1080.registrationInfo_SXGA;
	}
	else
	{
		return XN_STATUS_BAD_PARAM;
	}
	m_depthResolution.x = xres;
	m_depthResolution.y = yres;

	return XN_STATUS_OK;
}

XnStatus DepthUtilsImpl::SetColorResolution(int xres, int yres)
{
	m_colorResolution.x = xres;
	m_colorResolution.y = yres;

	return XN_STATUS_OK;
}

XnStatus DepthUtilsImpl::TranslateSinglePixel(uint32_t x, uint32_t y, unsigned short z, uint32_t& imageX, uint32_t& imageY)
{
	imageX = 0;
	imageY = 0;

	uint32_t nDepthXRes = m_depthResolution.x;
	XnBool bMirror = m_isMirrored;
	uint32_t nIndex = bMirror ? ((y+1)*nDepthXRes - x - 1) * 2 : (y*nDepthXRes + x) * 2;
	XnInt16* pRegTable = (XnInt16*)&m_pRegTable[nIndex];
	XnInt16* pRGBRegDepthToShiftTable = (XnInt16*)m_pDepth2ShiftTable;
	uint32_t nNewX = 0;
	uint32_t nNewY = 0;

	uint32_t nLinesShift = m_pPadInfo->nCroppingLines - m_pPadInfo->nStartLines;

	if (z == 0)
	{
		return XN_STATUS_BAD_PARAM;
	}

	nNewX = (uint32_t)(*pRegTable + pRGBRegDepthToShiftTable[z]) / m_blob.params1080.rgbRegXValScale;
	nNewY = *(pRegTable+1);
	if (nNewX >= nDepthXRes || nNewY < nLinesShift)
	{
		return XN_STATUS_BAD_PARAM;
	}

	imageX = bMirror ? (nDepthXRes - nNewX - 1) : nNewX;
	imageY = nNewY - nLinesShift;

	/////////////////////////////////////

	XnDouble fullXRes = m_colorResolution.x;
	XnDouble fullYRes;
	XnBool bCrop = FALSE;

	if ((9 * m_colorResolution.x / m_colorResolution.y) == 16)
	{
		fullYRes = m_colorResolution.x * 4 / 5;
		bCrop = TRUE;
	}
	else
	{
		fullYRes = m_colorResolution.y;
		bCrop = FALSE;
	}

	// inflate to full res
	imageX = (uint32_t)(fullXRes / m_depthResolution.x * imageX);
	imageY = (uint32_t)(fullYRes / m_depthResolution.y * imageY);

	if (bCrop)
	{
		// crop from center
		imageY -= (uint32_t)(fullYRes - m_colorResolution.y)/2;
		if (imageY > (uint32_t)m_colorResolution.y)
		{
			return XN_STATUS_BAD_PARAM;
		}
	}

	return XN_STATUS_OK;
}

void DepthUtilsImpl::BuildDepthToShiftTable(XnUInt16* pRGBRegDepthToShiftTable, int xres)
{
	uint32_t nXScale = m_blob.params1080.cmosVGAOutputXRes / xres;
	uint32_t nIndex = 0;
	XnDouble dDepth = 0;

	unsigned short nMaxDepth = MAX_Z;

	XnDouble dPlanePixelSize = m_blob.params1080.zpps;

	uint64_t nPlaneDsr = m_blob.params1080.zpd;
	XnDouble dPlaneDsr = (XnDouble)nPlaneDsr;

	XnDouble dDCRCDist = m_blob.params1080.dcrcdist;

	XnDouble dPelSize = 1.0 / (dPlanePixelSize * nXScale * m_blob.params1080.s2dPelConst);
	XnDouble dPelDCC = dDCRCDist * dPelSize * m_blob.params1080.s2dPelConst;
	XnDouble dPelDSR = dPlaneDsr * dPelSize * m_blob.params1080.s2dPelConst;

	memset(pRGBRegDepthToShiftTable, 0, nMaxDepth * sizeof(XnInt16));

	for (nIndex = 0; nIndex < nMaxDepth; nIndex++)
	{
		dDepth = nIndex * dPelSize;
		pRGBRegDepthToShiftTable[nIndex] = (XnInt16)(((dPelDCC * (dDepth - dPelDSR) / dDepth) + (m_blob.params1080.s2dConstOffset)) * m_blob.params1080.rgbRegXValScale);
	}
}

XnStatus DepthUtilsImpl::BuildRegistrationTable(XnUInt16* pRegTable, RegistrationInfo* pRegInfo, XnUInt16** pDepthToShiftTable, int xres, int yres)
{
	// take needed parameters to perform registration

	XN_VALIDATE_ALIGNED_CALLOC(*pDepthToShiftTable, XnUInt16, MAX_Z+1, XN_DEFAULT_MEM_ALIGN);
	m_bD2SAlloc = TRUE;

	BuildDepthToShiftTable(*pDepthToShiftTable, xres);

	XnDouble* RegXTable = XN_NEW_ARR(XnDouble, m_blob.params1080.rgbRegXRes*m_blob.params1080.rgbRegYRes);
	XnDouble* RegYTable = XN_NEW_ARR(XnDouble, m_blob.params1080.rgbRegXRes*m_blob.params1080.rgbRegYRes);

	XnUInt16 nDepthXRes  = (XnUInt16) xres;
	XnUInt16 nDepthYRes  = (XnUInt16) yres;
	XnDouble* pRegXTable = (XnDouble*)RegXTable;
	XnDouble* pRegYTable = (XnDouble*)RegYTable;
	XnDouble nNewX = 0;
	XnDouble nNewY = 0;

	// Create the dx dy tables
	CreateDXDYTables(RegXTable, RegYTable,
		nDepthXRes,	nDepthYRes,
		GetFieldValueSigned(pRegInfo->nRGS_AX, 32, 0),
		GetFieldValueSigned(pRegInfo->nRGS_BX, 32, 0),
		GetFieldValueSigned(pRegInfo->nRGS_CX, 32, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DX, 32, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DX_BETA_INC, 24, 0),
		GetFieldValueSigned(pRegInfo->nRGS_AY, 32, 0),
		GetFieldValueSigned(pRegInfo->nRGS_BY, 32, 0),
		GetFieldValueSigned(pRegInfo->nRGS_CY, 32, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DY, 32, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DY_BETA_INC, 24, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DX_START, 19, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DY_START, 19, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DXDX_START, 21, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DXDY_START, 21, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DYDX_START, 21, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DYDY_START, 21, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DXDXDX_START, 27, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DYDXDX_START, 27, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DYDXDY_START, 27, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DXDXDY_START, 27, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DYDYDX_START, 27, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DYDYDY_START, 27, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DX_BETA_START, 17, 0),
		GetFieldValueSigned(pRegInfo->nRGS_DY_BETA_START, 17, 0)
		);

	// Pre-process the table, do sanity checks and convert it from double to ints (for better performance)
	for (int32_t nY = 0; nY < nDepthYRes; nY++)
	{
		for (int32_t nX = 0; nX < nDepthXRes; nX++)
		{
			nNewX = (nX + *pRegXTable + m_blob.params1080.sensorWinOffsetX) * m_blob.params1080.rgbRegXValScale;
			nNewY = (nY + *pRegYTable + m_blob.params1080.sensorWinOffsetY);

			if (nNewY < 1)
			{
				nNewY = 1;
				nNewX = ((nDepthXRes*4) * m_blob.params1080.rgbRegXValScale); // set illegal value on purpose
			}

			if (nNewX < 1)
			{
				nNewX = ((nDepthXRes*4) * m_blob.params1080.rgbRegXValScale); // set illegal value on purpose
			}

			if (nNewY > nDepthYRes-2)
			{
				nNewX = ((nDepthXRes*4) * m_blob.params1080.rgbRegXValScale); // set illegal value on purpose
				nNewY = nDepthYRes;
			}

			*pRegTable = (XnInt16)nNewX;
			*(pRegTable+1) = (XnInt16)nNewY;

			pRegXTable++;
			pRegYTable++;
			pRegTable+=2;
		}
	}

	XN_DELETE_ARR(RegXTable);
	XN_DELETE_ARR(RegYTable);

	return (XN_STATUS_OK);
}

void DepthUtilsImpl::CreateDXDYTables (XnDouble* RegXTable, XnDouble* RegYTable,
	uint32_t resX, uint32_t resY,
	int64_t AX6, int64_t BX6, int64_t CX2, int64_t DX2,
	uint32_t deltaBetaX,
	int64_t AY6, int64_t BY6, int64_t CY2, int64_t DY2,
	uint32_t deltaBetaY,
	int64_t dX0, int64_t dY0,
	int64_t dXdX0, int64_t dXdY0, int64_t dYdX0, int64_t dYdY0,
	int64_t dXdXdX0, int64_t dYdXdX0, int64_t dYdXdY0, int64_t dXdXdY0,
	int64_t dYdYdX0, int64_t dYdYdY0,
	uint32_t startingBetaX, uint32_t startingBetaY)
{
	dX0 <<= 9;
	dY0 <<= 9;
	dXdX0 <<= 8;
	dXdY0 <<= 8;
	dYdX0 <<= 8;
	dYdY0 <<= 8;
	dXdXdX0 <<= 8;
	dYdXdX0 <<= 8;
	dYdXdY0 <<= 8;
	dXdXdY0 <<= 8;
	dYdYdX0 <<= 8;
	dYdYdY0 <<= 8;
	startingBetaX <<= 7;
	startingBetaY <<= 7;

	CreateDXDYTablesInternal(RegXTable, RegYTable, resX, resY, AX6, BX6, CX2, DX2, deltaBetaX, AY6, BY6, CY2, DY2,
		deltaBetaY, dX0, dY0, dXdX0, dXdY0, dYdX0, dYdY0, dXdXdX0, dYdXdX0, dYdXdY0, dXdXdY0,
		dYdYdX0, dYdYdY0, startingBetaX, startingBetaY);
}

void DepthUtilsImpl::CreateDXDYTablesInternal(XnDouble* RegXTable, XnDouble* RegYTable,
	int32_t resX, int32_t resY,
	int64_t AX6, int64_t BX6, int64_t CX2, int64_t DX2,
	int32_t deltaBetaX,
	int64_t AY6, int64_t BY6, int64_t CY2, int64_t DY2,
	int32_t deltaBetaY,
	int64_t dX0, int64_t dY0,
	int64_t dXdX0, int64_t dXdY0, int64_t dYdX0, int64_t dYdY0,
	int64_t dXdXdX0, int64_t dYdXdX0, int64_t dYdXdY0, int64_t dXdXdY0,
	int64_t dYdYdX0, int64_t dYdYdY0,
	int32_t betaX, int32_t betaY)
{
	int32_t tOffs = 0;

	for (int32_t row = 0; row < resY; row++)
	{
		incrementalFitting50(dXdXdX0, CX2, dXdXdX0);
		incrementalFitting50(dXdX0, dYdXdX0, DX2, dXdX0, dYdXdX0);

		incrementalFitting50(dX0, dYdX0, dYdYdX0, BX6, betaX, 0, dX0, dYdX0, dYdYdX0, betaX);

		int64_t coldXdXdX0 = dXdXdX0, coldXdX0 = dXdX0, coldX0 = dX0;
		int32_t colBetaX = betaX;

		incrementalFitting50(dXdXdY0, CY2, dXdXdY0);
		incrementalFitting50(dXdY0, dYdXdY0, DY2, dXdY0, dYdXdY0);

		incrementalFitting50(dY0, dYdY0, dYdYdY0, BY6, betaY, deltaBetaY, dY0, dYdY0, dYdYdY0, betaY);

		int64_t coldXdXdY0 = dXdXdY0, coldXdY0 = dXdY0, coldY0 = dY0;
		int32_t colBetaY = betaY;

		for (int32_t col = 0; col < resX; col++, tOffs++)
		{
			RegXTable[tOffs] = coldX0 * (1.0/(1<<17));
			RegYTable[tOffs] = coldY0 * (1.0/(1<<17));

			incrementalFitting50(coldX0, coldXdX0, coldXdXdX0, AX6, colBetaX, deltaBetaX, coldX0, coldXdX0, coldXdXdX0, colBetaX);
			incrementalFitting50(coldY0, coldXdY0, coldXdXdY0, AY6, colBetaY, 0, coldY0, coldXdY0, coldXdXdY0, colBetaY);
		}
	}
}
