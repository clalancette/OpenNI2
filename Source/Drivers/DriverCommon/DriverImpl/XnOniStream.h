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
#ifndef XNONISTREAM_H
#define XNONISTREAM_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <Driver/OniDriverAPI.h>
#include <XnLib.h>
#include "../Sensor/XnSensor.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnDeviceStream;
class XnOniDevice;

class XnOniStream :
	public oni::driver::StreamBase
{
public:
	XnOniStream(XnSensor* pSensor, const char* strName, OniSensorType sensorType, XnOniDevice* pDevice);
	~XnOniStream();

	virtual XnStatus Init();

	virtual void setServices(oni::driver::StreamServices* pStreamServices);

	OniStatus start();
	void stop();

	virtual OniStatus getProperty(int propertyId, void* data, int* pDataSize);
	virtual OniStatus setProperty(int propertyId, const void* data, int dataSize);
	virtual bool isPropertySupported(int propertyId);

	virtual int getRequiredFrameSize();

	XnOniDevice* GetDevice() { return m_pDevice; }
	XnDeviceStream* GetDeviceStream() { return m_pDeviceStream; }

protected:
	virtual XnStatus SetPropertyImpl(int propertyId, const void* data, int dataSize);

	OniSensorType m_sensorType;
	XnSensor* m_pSensor;
	const char* m_strType;
	XnDeviceStream* m_pDeviceStream;
	XnOniDevice* m_pDevice;
	XnCallbackHandle m_hNewDataCallback;

private:
	void destroy();
	bool m_started;
	static void XN_CALLBACK_TYPE OnNewStreamDataEventHandler(const XnNewStreamDataEventArgs& args, void* pCookie);
};

#endif // XNONISTREAM_H
