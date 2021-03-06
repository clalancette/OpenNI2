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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include "XnShiftToDepth.h"
#include "XnLinkStatusCodes.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnShiftToDepthInit(XnShiftToDepthTables* pShiftToDepth, const XnShiftToDepthConfig* pConfig)
{
	XN_VALIDATE_INPUT_PTR(pShiftToDepth);
	XN_VALIDATE_INPUT_PTR(pConfig);

	XN_VALIDATE_ALIGNED_CALLOC(pShiftToDepth->pShiftToDepthTable, OniDepthPixel, pConfig->nDeviceMaxShiftValue+1, XN_DEFAULT_MEM_ALIGN);
	XN_VALIDATE_ALIGNED_CALLOC(pShiftToDepth->pDepthToShiftTable, uint16_t, pConfig->nDeviceMaxDepthValue+1, XN_DEFAULT_MEM_ALIGN);
	pShiftToDepth->bIsInitialized = true;

	// store allocation sizes
	pShiftToDepth->nShiftsCount = pConfig->nDeviceMaxShiftValue + 1;
	pShiftToDepth->nDepthsCount = pConfig->nDeviceMaxDepthValue + 1;

	return XnShiftToDepthUpdate(pShiftToDepth, pConfig);
}

XnStatus XnShiftToDepthUpdate(XnShiftToDepthTables* pShiftToDepth, const XnShiftToDepthConfig* pConfig)
{
	XN_VALIDATE_INPUT_PTR(pShiftToDepth);
	XN_VALIDATE_INPUT_PTR(pConfig);

	// check max shift wasn't changed (if so, memory should be re-allocated)
	if (pConfig->nDeviceMaxShiftValue > pShiftToDepth->nShiftsCount)
	{
		return XN_STATUS_LINK_INVALID_MAX_SHIFT;
	}

	// check max depth wasn't changed (if so, memory should be re-allocated)
	if (pConfig->nDeviceMaxDepthValue > pShiftToDepth->nDepthsCount)
	{
		return XN_STATUS_LINK_INVALID_MAX_DEPTH;
	}

	uint16_t nIndex = 0;
	int16_t  nShiftValue = 0;
	double dFixedRefX = 0;
	double dMetric = 0;
	double dDepth = 0;
	double dPlanePixelSize = pConfig->fZeroPlanePixelSize;
	double dPlaneDsr = pConfig->nZeroPlaneDistance;
	double dPlaneDcl = pConfig->fEmitterDCmosDistance;
	int32_t nConstShift = pConfig->nParamCoeff * pConfig->nConstShift;
	double dDepthScale = (pConfig->dDepthScale == 0) ? 1.0 : pConfig->dDepthScale;

	dPlanePixelSize *= pConfig->nPixelSizeFactor;
	if (pConfig->nPixelSizeFactor == 0)
	{
		return XN_STATUS_ERROR;
	}
	nConstShift /= pConfig->nPixelSizeFactor;

	OniDepthPixel* pShiftToDepthTable = pShiftToDepth->pShiftToDepthTable;
	uint16_t* pDepthToShiftTable = pShiftToDepth->pDepthToShiftTable;

	xnOSMemSet(pShiftToDepthTable, 0, pShiftToDepth->nShiftsCount * sizeof(OniDepthPixel));
	xnOSMemSet(pDepthToShiftTable, 0, pShiftToDepth->nDepthsCount * sizeof(uint16_t));

	uint16_t nLastDepth = 0;
	uint16_t nLastIndex = 0;

	for (nIndex = 1; nIndex < pConfig->nDeviceMaxShiftValue; nIndex++)
	{
		nShiftValue = nIndex;

		dFixedRefX = (double)(nShiftValue - nConstShift) / (double)pConfig->nParamCoeff;
		dMetric = dFixedRefX * dPlanePixelSize;
		dDepth = pConfig->nShiftScale * ((dMetric * dPlaneDsr / (dPlaneDcl - dMetric)) + dPlaneDsr) * dDepthScale;

		// check cut-offs
		if ((dDepth > pConfig->nDepthMinCutOff) && (dDepth < pConfig->nDepthMaxCutOff))
		{
			pShiftToDepthTable[nIndex] = (uint16_t)dDepth;

			for (uint16_t i = nLastDepth; i < dDepth; i++)
				pDepthToShiftTable[i] = nLastIndex;

			nLastIndex = nIndex;
			nLastDepth = (uint16_t)dDepth;
		}
	}

	for (uint32_t i = nLastDepth; i <= pConfig->nDeviceMaxDepthValue; i++)
	{
		pDepthToShiftTable[i] = nLastIndex;
	}

	return XN_STATUS_OK;
}

XnStatus XnShiftToDepthConvert(const XnShiftToDepthTables* pShiftToDepth,
							   const uint16_t* pInput,
							   uint32_t nInputSize,
							   OniDepthPixel* pOutput)
{
	XN_VALIDATE_INPUT_PTR(pShiftToDepth);
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);

	const uint16_t* pInputEnd = pInput + nInputSize;
	OniDepthPixel* pShiftToDepthTable = pShiftToDepth->pShiftToDepthTable;

	while (pInput != pInputEnd)
	{
		if (*pInput >= pShiftToDepth->nShiftsCount)
		{
			*pOutput = 0;
		}
		else
		{
			*pOutput = pShiftToDepthTable[*pInput];
		}
		pOutput++;
		pInput++;
	}

	return XN_STATUS_OK;
}

XnStatus XnShiftToDepthFree(XnShiftToDepthTables* pShiftToDepth)
{
	XN_VALIDATE_INPUT_PTR(pShiftToDepth);

	if (pShiftToDepth->bIsInitialized)
	{
		XN_ALIGNED_FREE_AND_NULL(pShiftToDepth->pDepthToShiftTable);
		XN_ALIGNED_FREE_AND_NULL(pShiftToDepth->pShiftToDepthTable);
		pShiftToDepth->bIsInitialized = false;
	}

	return XN_STATUS_OK;
}
