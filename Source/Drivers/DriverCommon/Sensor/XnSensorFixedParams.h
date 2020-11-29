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
#ifndef XNSENSORFIXEDPARAMS_H
#define XNSENSORFIXEDPARAMS_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnStreamParams.h>
#include "XnDeviceSensor.h"

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
class XnSensorFirmware;
struct XnDevicePrivateData;
typedef struct XnDevicePrivateData XnDevicePrivateData;

//---------------------------------------------------------------------------
// XnSensorFixedParams class
//---------------------------------------------------------------------------
class XnSensorFixedParams
{
public:
	XnSensorFixedParams(XnDevicePrivateData* pDevicePrivateData);

	XnStatus Init();

	inline uint16_t GetDepthCmosI2CBus() const { return m_nSensorDepthCMOSI2CBus; }
	inline uint16_t GetDepthCmosI2CSlaveAddress() const { return m_nSensorDepthCMOSI2CSlaveAddress; }
	inline uint16_t GetImageCmosI2CBus() const { return m_nSensorImageCMOSI2CBus; }
	inline uint16_t GetImageCmosI2CSlaveAddress() const { return m_nSensorImageCMOSI2CSlaveAddress; }

	inline OniDepthPixel GetZeroPlaneDistance() const { return m_nZeroPlaneDistance; }
	inline double GetZeroPlanePixelSize() const { return m_dZeroPlanePixelSize; }
	inline double GetEmitterDCmosDistance() const { return m_dEmitterDCmosDistance; }
	inline double GetDCmosRCmosDistance() const { return m_dDCmosRCmosDistance; }

	inline const XnChar* GetSensorSerial() const { return m_strSensorSerial; }

	inline XnImageCMOSType GetImageCmosType() const { return (XnImageCMOSType)m_nImageCmosType; }
	inline XnDepthCMOSType GetDepthCmosType() const { return (XnDepthCMOSType)m_nDepthCmosType; }

	inline const XnChar* GetDeviceName() const { return m_deviceInfo.strDeviceName; }
	inline const XnChar* GetVendorData() const { return m_deviceInfo.strVendorData; }
	inline const XnChar* GetPlatformString() const { return m_strPlatformString; }

private:
	XnDevicePrivateData* m_pDevicePrivateData;

	uint16_t m_nSensorDepthCMOSI2CBus;
	uint16_t m_nSensorDepthCMOSI2CSlaveAddress;
	uint16_t m_nSensorImageCMOSI2CBus;
	uint16_t m_nSensorImageCMOSI2CSlaveAddress;

	OniDepthPixel m_nZeroPlaneDistance;
	double m_dZeroPlanePixelSize;
	double m_dEmitterDCmosDistance;
	double m_dDCmosRCmosDistance;

	uint32_t m_nImageCmosType;
	uint32_t m_nDepthCmosType;

	XnChar m_strSensorSerial[XN_DEVICE_MAX_STRING_LENGTH];
	XnDeviceInformation m_deviceInfo;
	XnChar m_strPlatformString[XN_DEVICE_MAX_STRING_LENGTH];
};

#endif // XNSENSORFIXEDPARAMS_H
