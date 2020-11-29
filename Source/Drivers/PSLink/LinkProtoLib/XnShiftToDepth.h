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

#ifndef XNSHIFTTODEPTH_H
#define XNSHIFTTODEPTH_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <OniCTypes.h>
#include <XnPlatform.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct XnShiftToDepthConfig
{
	/** The zero plane distance in depth units. */
	OniDepthPixel nZeroPlaneDistance;
	/** The zero plane pixel size */
	float fZeroPlanePixelSize;
	/** The distance between the emitter and the Depth Cmos */
	float fEmitterDCmosDistance;
	/** The maximum possible shift value from this device. */
	uint32_t nDeviceMaxShiftValue;
	/** The maximum possible depth from this device (as opposed to a cut-off). */
	uint32_t nDeviceMaxDepthValue;

	uint32_t nConstShift;
	uint32_t nPixelSizeFactor;
	uint32_t nParamCoeff;
	uint32_t nShiftScale;

	double dDepthScale;
	OniDepthPixel nDepthMinCutOff;
	OniDepthPixel nDepthMaxCutOff;

} XnShiftToDepthConfig;

typedef struct XnShiftToDepthTables
{
	XnBool bIsInitialized;
	/** The shift-to-depth table. */
	OniDepthPixel* pShiftToDepthTable;
	/** The number of entries in the shift-to-depth table. */
	uint32_t nShiftsCount;
	/** The depth-to-shift table. */
	XnUInt16* pDepthToShiftTable;
	/** The number of entries in the depth-to-shift table. */
	uint32_t nDepthsCount;
} XnShiftToDepthTables;

//---------------------------------------------------------------------------
// Functions Declaration
//---------------------------------------------------------------------------
XnStatus XnShiftToDepthInit(XnShiftToDepthTables* pShiftToDepth,
							const XnShiftToDepthConfig* pConfig);

XnStatus XnShiftToDepthUpdate(XnShiftToDepthTables* pShiftToDepth,
							  const XnShiftToDepthConfig* pConfig);

XnStatus XnShiftToDepthConvert(const XnShiftToDepthTables* pShiftToDepth,
							   const XnUInt16* pInput,
							   uint32_t nInputSize,
							   OniDepthPixel* pOutput);

XnStatus XnShiftToDepthFree(XnShiftToDepthTables* pShiftToDepth);

#endif // XNSHIFTTODEPTH_H
