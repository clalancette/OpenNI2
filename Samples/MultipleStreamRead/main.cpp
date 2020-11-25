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
#include <OpenNI.h>

#include "OniSampleUtilities.h"

#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms

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

int main()
{
	openni::Status rc = openni::OpenNI::initialize();
	if (rc != openni::STATUS_OK)
	{
		printf("Initialize failed\n%s\n", openni::OpenNI::getExtendedError());
		return 1;
	}

	openni::Device device;
	rc = device.open(openni::ANY_DEVICE);
	if (rc != openni::STATUS_OK)
	{
		printf("Couldn't open device\n%s\n", openni::OpenNI::getExtendedError());
		return 2;
	}

	openni::VideoStream depth, color;

	if (device.getSensorInfo(openni::SENSOR_DEPTH) != NULL)
	{
		rc = depth.create(device, openni::SENSOR_DEPTH);
		if (rc == openni::STATUS_OK)
		{
			rc = depth.start();
			if (rc != openni::STATUS_OK)
			{
				printf("Couldn't start the color stream\n%s\n", openni::OpenNI::getExtendedError());
			}
		}
		else
		{
			printf("Couldn't create depth stream\n%s\n", openni::OpenNI::getExtendedError());
		}
	}

	if (device.getSensorInfo(openni::SENSOR_COLOR) != NULL)
	{
		rc = color.create(device, openni::SENSOR_COLOR);
		if (rc == openni::STATUS_OK)
		{
			rc = color.start();
			if (rc != openni::STATUS_OK)
			{
				printf("Couldn't start the color stream\n%s\n", openni::OpenNI::getExtendedError());
			}
		}
		else
		{
			printf("Couldn't create color stream\n%s\n", openni::OpenNI::getExtendedError());
		}
	}

	openni::VideoFrameRef frame;

	openni::VideoStream* streams[] = {&depth, &color};

	while (!wasKeyboardHit())
	{
		int readyStream = -1;
		rc = openni::OpenNI::waitForAnyStream(streams, 2, &readyStream, SAMPLE_READ_WAIT_TIMEOUT);
		if (rc != openni::STATUS_OK)
		{
			printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, openni::OpenNI::getExtendedError());
			break;
		}

		switch (readyStream)
		{
		case 0:
			// Depth
			depth.readFrame(&frame);
			break;
		case 1:
			// Color
			color.readFrame(&frame);
			break;
		default:
			printf("Unxpected stream\n");
		}

		analyzeFrame(frame);

		frame.release();
	}

	depth.stop();
	color.stop();
	depth.destroy();
	color.destroy();
	device.close();
	openni::OpenNI::shutdown();

	return 0;
}
