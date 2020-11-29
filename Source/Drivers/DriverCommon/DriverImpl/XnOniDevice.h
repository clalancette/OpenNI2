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
#ifndef XNONIDEVICE_H
#define XNONIDEVICE_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <Driver/OniDriverAPI.h>
#include <XnLib.h>
#include "XnOniDepthStream.h"
#include "XnOniColorStream.h"
#include "XnOniIRStream.h"
#include "../Sensor/XnSensor.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

class XnOniStream;
class XnOniDriver;

class XnOniDevice final :
	public oni::driver::DeviceBase
{
public:
	XnOniDevice(const char* uri, oni::driver::DriverServices& driverServices, XnOniDriver* pDriver);
	~XnOniDevice();

	XnStatus Init(const char* mode);

	OniDeviceInfo* GetInfo() { return &m_info; }

	OniStatus getSensorInfoList(OniSensorInfo** pSensors, int* numSensors) override;

	oni::driver::StreamBase* createStream(OniSensorType sensorType) override;
	void destroyStream(oni::driver::StreamBase* pStream) override;

	OniStatus getProperty(int propertyId, void* data, int* pDataSize) override;
	OniStatus setProperty(int propertyId, const void* data, int dataSize) override;
	OniBool isPropertySupported(int propertyId) override;
	void notifyAllProperties() override;

	OniStatus EnableFrameSync(XnOniStream** pStreams, int streamCount);
	void DisableFrameSync();

	OniBool isImageRegistrationModeSupported(OniImageRegistrationMode mode) override;

	XnSensor* GetSensor()
	{
		return &m_sensor;
	}

	XnOniDriver* GetDriver()
	{
		return m_pDriver;
	}

private:
	XnStatus FillSupportedVideoModes();

	OniDeviceInfo m_info;
	int m_numSensors;
	OniSensorInfo m_sensors[10];
	oni::driver::DriverServices& m_driverServices;
	XnSensor m_sensor;
	XnOniDriver* m_pDriver;
};

#endif // XNONIDEVICE_H