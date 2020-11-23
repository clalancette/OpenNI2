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
#include <stdio.h>
#include "OpenNI.h"

#include "OniSampleUtilities.h"

void analyzeFrame(const openni::VideoFrameRef& frame)
{
	openni::DepthPixel* pDepth;
	openni::RGB888Pixel* pColor;

	int middleIndex = (frame.getHeight()+1)*frame.getWidth()/2;

	switch (frame.getVideoMode().getPixelFormat())
	{
	case openni::PIXEL_FORMAT_DEPTH_1_MM:
	case openni::PIXEL_FORMAT_DEPTH_100_UM:
		pDepth = (openni::DepthPixel*)frame.getData();
		printf("[%08llu] %8d\n", (long long)frame.getTimestamp(),
			pDepth[middleIndex]);
		break;
	case openni::PIXEL_FORMAT_RGB888:
		pColor = (openni::RGB888Pixel*)frame.getData();
		printf("[%08llu] 0x%02x%02x%02x\n", (long long)frame.getTimestamp(),
			pColor[middleIndex].r&0xff,
			pColor[middleIndex].g&0xff,
			pColor[middleIndex].b&0xff);
		break;
	default:
		printf("Unknown format\n");
	}
}

class PrintCallback final : public openni::VideoStream::NewFrameListener
{
public:
	void onNewFrame(openni::VideoStream& stream) override
	{
		stream.readFrame(&m_frame);

		analyzeFrame(m_frame);
	}
private:
	openni::VideoFrameRef m_frame;
};

class OpenNIDeviceListener final : public openni::OpenNI::DeviceConnectedListener,
									public openni::OpenNI::DeviceDisconnectedListener,
									public openni::OpenNI::DeviceStateChangedListener
{
public:
	void onDeviceStateChanged(const openni::DeviceInfo* pInfo, openni::DeviceState state) override
	{
		printf("Device \"%s\" error state changed to %d\n", pInfo->getUri(), state);
	}

	void onDeviceConnected(const openni::DeviceInfo* pInfo) override
	{
		printf("Device \"%s\" connected\n", pInfo->getUri());
	}

	void onDeviceDisconnected(const openni::DeviceInfo* pInfo) override
	{
		printf("Device \"%s\" disconnected\n", pInfo->getUri());
	}
};

int main()
{
	openni::Status rc = openni::OpenNI::initialize();
	if (rc != openni::STATUS_OK)
	{
		printf("Initialize failed\n%s\n", openni::OpenNI::getExtendedError());
		return 1;
	}

	OpenNIDeviceListener devicePrinter;

	openni::OpenNI::addDeviceConnectedListener(&devicePrinter);
	openni::OpenNI::addDeviceDisconnectedListener(&devicePrinter);
	openni::OpenNI::addDeviceStateChangedListener(&devicePrinter);

	openni::Array<openni::DeviceInfo> deviceList;
	openni::OpenNI::enumerateDevices(&deviceList);
	for (int i = 0; i < deviceList.getSize(); ++i)
	{
		printf("Device \"%s\" already connected\n", deviceList[i].getUri());
	}

	openni::Device device;
	rc = device.open(openni::ANY_DEVICE);
	if (rc != openni::STATUS_OK)
	{
		printf("Couldn't open device\n%s\n", openni::OpenNI::getExtendedError());
		return 2;
	}

	openni::VideoStream depth;

	if (device.getSensorInfo(openni::SENSOR_DEPTH) != NULL)
	{
		rc = depth.create(device, openni::SENSOR_DEPTH);
		if (rc != openni::STATUS_OK)
		{
			printf("Couldn't create depth stream\n%s\n", openni::OpenNI::getExtendedError());
		}
	}
	rc = depth.start();
	if (rc != openni::STATUS_OK)
	{
		printf("Couldn't start the depth stream\n%s\n", openni::OpenNI::getExtendedError());
	}

	PrintCallback depthPrinter;

	// Register to new frame
	depth.addNewFrameListener(&depthPrinter);

	// Wait while we're getting frames through the printer
	while (!wasKeyboardHit())
	{
		Sleep(100);
	}

	depth.removeNewFrameListener(&depthPrinter);


	depth.stop();
	depth.destroy();
	device.close();

	openni::OpenNI::removeDeviceStateChangedListener(&devicePrinter);
	openni::OpenNI::removeDeviceDisconnectedListener(&devicePrinter);
	openni::OpenNI::removeDeviceConnectedListener(&devicePrinter);
	openni::OpenNI::shutdown();

	return 0;
}
