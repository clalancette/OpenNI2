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
#include "XnFirmwareCommands.h"
#include "XnHostProtocol.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnFirmwareCommands::XnFirmwareCommands(XnDevicePrivateData* pDevicePrivateData) :
	m_pDevicePrivateData(pDevicePrivateData)
{
}

XnStatus XnFirmwareCommands::GetFirmwareParam(uint16_t nParam, uint16_t* pnValue)
{
	return XnHostProtocolGetParam(m_pDevicePrivateData, nParam, *pnValue);
}

XnStatus XnFirmwareCommands::SetFirmwareParam(uint16_t nParam, uint16_t nValue)
{
	return XnHostProtocolSetParam(m_pDevicePrivateData, nParam, nValue);
}

XnStatus XnFirmwareCommands::SetMultipleFirmwareParams(XnInnerParamData* aParams, uint32_t nCount)
{
	return XnHostProtocolSetMultipleParams(m_pDevicePrivateData, (uint16_t)nCount, aParams);
}
