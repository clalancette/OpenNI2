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

#ifndef _DEPTH_UTILS_IMPL_H_
#define _DEPTH_UTILS_IMPL_H_

#include <XnLib.h>
#include "DepthUtils.h"

#define MAX_Z 65535

class DepthUtilsImpl final
{
public:
	DepthUtilsImpl();
	~DepthUtilsImpl();

	XnStatus Initialize(DepthUtilsSensorCalibrationInfo* pBlob);
	XnStatus Free();

	XnStatus Apply(unsigned short* pOutput);

	XnStatus SetDepthConfiguration(int xres, int yres, OniPixelFormat format, bool isMirrored);

	XnStatus SetColorResolution(int xres, int yres);

	XnStatus TranslateSinglePixel(uint32_t x, uint32_t y, unsigned short z, uint32_t& imageX, uint32_t& imageY);
private:
	void BuildDepthToShiftTable(XnUInt16* pRGBRegDepthToShiftTable, int xres);
	XnStatus BuildRegistrationTable(XnUInt16* pRegTable, RegistrationInfo* pRegInfo, XnUInt16** pDepthToShiftTable, int xres, int yres);

	void CreateDXDYTables (double* RegXTable, double* RegYTable,
		uint32_t resX, uint32_t resY,
		int64_t AX6, int64_t BX6, int64_t CX2, int64_t DX2,
		uint32_t deltaBetaX,
		int64_t AY6, int64_t BY6, int64_t CY2, int64_t DY2,
		uint32_t deltaBetaY,
		int64_t dX0, int64_t dY0,
		int64_t dXdX0, int64_t dXdY0, int64_t dYdX0, int64_t dYdY0,
		int64_t dXdXdX0, int64_t dYdXdX0, int64_t dYdXdY0, int64_t dXdXdY0,
		int64_t dYdYdX0, int64_t dYdYdY0,
		uint32_t startingBetaX, uint32_t startingBetaY);
	void CreateDXDYTablesInternal(double* RegXTable, double* RegYTable,
		int32_t resX, int32_t resY,
		int64_t AX6, int64_t BX6, int64_t CX2, int64_t DX2,
		int32_t deltaBetaX,
		int64_t AY6, int64_t BY6, int64_t CY2, int64_t DY2,
		int32_t deltaBetaY,
		int64_t dX0, int64_t dY0,
		int64_t dXdX0, int64_t dXdY0, int64_t dYdX0, int64_t dYdY0,
		int64_t dXdXdX0, int64_t dYdXdX0, int64_t dYdXdY0, int64_t dXdXdY0,
		int64_t dYdYdX0, int64_t dYdYdY0,
		int32_t betaX, int32_t betaY);

	DepthUtilsSensorCalibrationInfo m_blob;


	XnUInt16* m_pDepthToShiftTable_QQVGA;
	XnUInt16* m_pDepthToShiftTable_QVGA;
	XnUInt16* m_pDepthToShiftTable_VGA;

	XnUInt16* m_pDepthToShiftTable_SXGA;  //xice

	XnUInt16* m_pRegistrationTable_QQVGA;
	XnUInt16* m_pRegistrationTable_QVGA;
	XnUInt16* m_pRegistrationTable_VGA;

	XnUInt16* m_pRegistrationTable_SXGA;  //xice

	PadInfo* m_pPadInfo;
	RegistrationInfo* m_pRegistrationInfo;

	XnUInt16* m_pRegTable;
	XnUInt16* m_pDepth2ShiftTable;

	bool m_bD2SAlloc;
	bool m_bInitialized;
	bool m_isMirrored;
	struct
	{
		int x, y;
	} m_depthResolution, m_colorResolution;
};


#endif
