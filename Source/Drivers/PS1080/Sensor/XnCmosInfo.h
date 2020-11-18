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
#ifndef XNCMOSINFO_H
#define XNCMOSINFO_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <list>

#include "XnSensorFirmware.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
typedef struct
{
	XnCmosBlankingInformation BlankingInfo;
	XnResolutions nRes;
	XnUInt32 nFPS;
} XnCmosBlankingData;

class XnCmosInfo
{
public:
	XnCmosInfo(XnSensorFirmware* pFirmware, XnDevicePrivateData* pDevicePrivateData);
	~XnCmosInfo();

	XnStatus SetCmosConfig(XnCMOSType nCmos, XnResolutions nResolution, XnUInt32 nFPS);

	inline const XnCmosBlankingCoefficients* GetBlankingCoefficients(XnCMOSType nCmos) const { return m_pCurrCmosBlankingInfo[nCmos]; }

private:
	XnSensorFirmware* m_pFirmware;
	XnDevicePrivateData* m_pDevicePrivateData;

	std::list<XnCmosBlankingData> m_CmosBlankingInfo;
	XnCmosBlankingCoefficients* m_pCurrCmosBlankingInfo[XN_CMOS_COUNT];
};

#endif // XNCMOSINFO_H
