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

int main(int argc, char* argv[])
{
	openni::Status rc = openni::OpenNI::initialize();
	if (rc != openni::STATUS_OK)
	{
		printf("Initialize failed\n%s\n", openni::OpenNI::getExtendedError());
		return 1;
	}

	openni::Device device;

	if (argc < 2)
	{
		rc = device.open(openni::ANY_DEVICE);
	}
	else
	{
		rc = device.open(argv[1]);
	}

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
			return 3;
		}
	}

	rc = depth.start();
	if (rc != openni::STATUS_OK)
	{
		printf("Couldn't start the depth stream\n%s\n", openni::OpenNI::getExtendedError());
		return 4;
	}

	openni::VideoFrameRef frame;

	while (!wasKeyboardHit())
	{
		int changedStreamDummy;
		openni::VideoStream* pStream = &depth;
		rc = openni::OpenNI::waitForAnyStream(&pStream, 1, &changedStreamDummy, SAMPLE_READ_WAIT_TIMEOUT);
		if (rc != openni::STATUS_OK)
		{
			printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, openni::OpenNI::getExtendedError());
			continue;
		}

		rc = depth.readFrame(&frame);
		if (rc != openni::STATUS_OK)
		{
			printf("Read failed!\n%s\n", openni::OpenNI::getExtendedError());
			continue;
		}

		if (frame.getVideoMode().getPixelFormat() != openni::PIXEL_FORMAT_DEPTH_1_MM && frame.getVideoMode().getPixelFormat() != openni::PIXEL_FORMAT_DEPTH_100_UM)
		{
			printf("Unexpected frame format\n");
			continue;
		}

		openni::DepthPixel* pDepth = (openni::DepthPixel*)frame.getData();

		int middleIndex = (frame.getHeight()+1)*frame.getWidth()/2;

		printf("[%08llu] %8d\n", (long long)frame.getTimestamp(), pDepth[middleIndex]);
	}

	depth.stop();
	depth.destroy();
	device.close();
	openni::OpenNI::shutdown();

	return 0;
}
