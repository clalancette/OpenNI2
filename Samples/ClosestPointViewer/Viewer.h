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
#ifndef VIEWER_H
#define VIEWER_H

#include "MWClosestPoint/MWClosestPoint.h"
#include <OpenNI.h>

#define MAX_DEPTH 10000

class MyMwListener final : public closest_point::ClosestPoint::Listener
{
public:
	MyMwListener() : m_ready(false) {}
	~MyMwListener() {m_frame.release();}
	void readyForNextData(closest_point::ClosestPoint* pClosestPoint) override
	{
		m_frame.release();
		openni::Status rc = pClosestPoint->getNextData(m_closest, m_frame);

		if (rc != openni::STATUS_OK)
		{
			printf("Update failed\n");
		}
		m_ready = true;
	}

	const openni::VideoFrameRef& getFrame() {return m_frame;}
	const closest_point::IntPoint3D& getClosestPoint() {return m_closest;}
	bool isAvailable() const {return m_ready;}
	void setUnavailable() {m_ready = false;}
private:
	openni::VideoFrameRef m_frame;
	closest_point::IntPoint3D m_closest;
	bool m_ready;
};


class SampleViewer final
{
public:
	SampleViewer(const char* strSampleName, const char* deviceUri);
	~SampleViewer();

	openni::Status init(int argc, char **argv);
	openni::Status run();	//Does not return

private:
	void display();
	void displayPostDraw(){};	// Overload to draw over the screen image

	void onKey(unsigned char key, int x, int y);

	openni::Status initOpenGL(int argc, char **argv);
	void initOpenGLHooks();

	void finalize();

	SampleViewer(const SampleViewer&);
	SampleViewer& operator=(SampleViewer&);

	static SampleViewer* ms_self;
	static void glutIdle();
	static void glutDisplay();
	static void glutKeyboard(unsigned char key, int x, int y);

	float				m_pDepthHist[MAX_DEPTH];
	char			m_strSampleName[ONI_MAX_STR];
	openni::RGB888Pixel*m_pTexMap;
	unsigned int		m_nTexMapX;
	unsigned int		m_nTexMapY;

	closest_point::ClosestPoint* m_pClosestPoint;
	MyMwListener* m_pClosestPointListener;
};

#endif // VIEWER_H
