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
#ifndef XNFIRMWARECOMMANDS_H
#define XNFIRMWARECOMMANDS_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceSensor.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
* Implements the various firmware opcodes.
* TODO: all opcodes should move here from XnHostProtocol.cpp.
*/
class XnFirmwareCommands final
{
public:
	XnFirmwareCommands(XnDevicePrivateData* pDevicePrivateData);

	XnStatus GetFirmwareParam(uint16_t nParam, uint16_t* pnValue);
	XnStatus SetFirmwareParam(uint16_t nParam, uint16_t nValue);
	XnStatus SetMultipleFirmwareParams(XnInnerParamData* aParams, uint32_t nCount);

private:
	XnDevicePrivateData* m_pDevicePrivateData;
};

#endif // XNFIRMWARECOMMANDS_H
