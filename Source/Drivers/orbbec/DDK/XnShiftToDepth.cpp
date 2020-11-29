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
#include "XnShiftToDepth.h"
#include <XnOS.h>
#include "XnDDK.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnShiftToDepthInit(XnShiftToDepthTables* pShiftToDepth, const XnShiftToDepthConfig* pConfig)
{
	XN_VALIDATE_INPUT_PTR(pShiftToDepth);
	XN_VALIDATE_INPUT_PTR(pConfig);

	XN_VALIDATE_ALIGNED_CALLOC(pShiftToDepth->pShiftToDepthTable, OniDepthPixel, pConfig->nDeviceMaxShiftValue+1, XN_DEFAULT_MEM_ALIGN);
	XN_VALIDATE_ALIGNED_CALLOC(pShiftToDepth->pDepthToShiftTable, uint16_t, pConfig->nDeviceMaxDepthValue+1, XN_DEFAULT_MEM_ALIGN);
	pShiftToDepth->bIsInitialized = TRUE;

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
		return XN_STATUS_DEVICE_INVALID_MAX_SHIFT;
	}

	// check max depth wasn't changed (if so, memory should be re-allocated)
	if (pConfig->nDeviceMaxDepthValue > pShiftToDepth->nDepthsCount)
	{
		return XN_STATUS_DEVICE_INVALID_MAX_DEPTH;
	}

	enum OpticsRelationShip OpticsRelation;
	uint32_t nIndex = 0;
	XnInt16  nShiftValue = 0;
	double dFixedRefX = 0;
	double dMetric = 0;
	double dDepth = 0;
	int32_t nConstShift;
	double dPlanePixelSize = pConfig->fZeroPlanePixelSize;
	double dPlaneDsr = pConfig->nZeroPlaneDistance;
	double dPlaneDcl = pConfig->fEmitterDCmosDistance;
	if (pConfig->nConstShift == 201)
	{
		OpticsRelation = ProjectRightOfCMOS;
		nConstShift = pConfig->nParamCoeff * (pConfig->nConstShift-1);
	}
	else
	{
		OpticsRelation = ProjectLeftOfCMOS;
		nConstShift = pConfig->nParamCoeff * pConfig->nConstShift;
	}

	dPlanePixelSize *= pConfig->nPixelSizeFactor;
	nConstShift /= pConfig->nPixelSizeFactor;

	OniDepthPixel* pShiftToDepthTable = pShiftToDepth->pShiftToDepthTable;
	uint16_t* pDepthToShiftTable = pShiftToDepth->pDepthToShiftTable;

	xnOSMemSet(pShiftToDepthTable, 0, pShiftToDepth->nShiftsCount * sizeof(OniDepthPixel));
	xnOSMemSet(pDepthToShiftTable, 0, pShiftToDepth->nDepthsCount * sizeof(uint16_t));

	uint16_t nLastDepth = 0;
	uint16_t nLastIndex = 0;

	uint32_t nMaxDepth = XN_MIN(pConfig->nDeviceMaxDepthValue, pConfig->nDepthMaxCutOff);

	for (nIndex = 1; nIndex < pConfig->nDeviceMaxShiftValue; nIndex++)
	{
		nShiftValue = (XnInt16)nIndex;
		if (OpticsRelation == ProjectRightOfCMOS)
		{
			 nShiftValue = 1603 - nShiftValue;
		}

		dFixedRefX = (double)(nShiftValue - nConstShift) / (double)pConfig->nParamCoeff;
		dFixedRefX -= 0.375;
		dMetric = dFixedRefX * dPlanePixelSize;
		dDepth = pConfig->nShiftScale * ((dMetric * dPlaneDsr / (dPlaneDcl - dMetric)) + dPlaneDsr);

		// check cut-offs
		if ((dDepth > pConfig->nDepthMinCutOff) && (dDepth < nMaxDepth))
		{
			pShiftToDepthTable[nIndex] = (uint16_t)dDepth;

			for (uint16_t i = nLastDepth; i < dDepth; i++)
			{
				pDepthToShiftTable[i] = nLastIndex;
			}

			nLastIndex = (uint16_t)nIndex;
			nLastDepth = (uint16_t)dDepth;
		}
	}

	for (uint16_t i = nLastDepth; i <= pConfig->nDeviceMaxDepthValue; i++)
	{
		pDepthToShiftTable[i] = nLastIndex;
	}

	return XN_STATUS_OK;
}

XnStatus XnShiftToDepthConvert(XnShiftToDepthTables* pShiftToDepth, uint16_t* pInput, uint32_t nInputSize, OniDepthPixel* pOutput)
{
	XN_VALIDATE_INPUT_PTR(pShiftToDepth);
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);

	uint16_t* pInputEnd = pInput + nInputSize;
	OniDepthPixel* pShiftToDepthTable = pShiftToDepth->pShiftToDepthTable;

	while (pInput != pInputEnd)
	{
		pOutput[0] = pShiftToDepthTable[pInput[0]];

		pInput++;
		pOutput++;
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
		pShiftToDepth->bIsInitialized = FALSE;
	}

	return XN_STATUS_OK;
}
