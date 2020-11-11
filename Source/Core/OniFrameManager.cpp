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
#include "OniFrameManager.h"
#include <XnOSCpp.h>

ONI_NAMESPACE_IMPLEMENTATION_BEGIN

FrameManager::FrameManager()
{
}

FrameManager::~FrameManager()
{
}

OniFrameInternal* FrameManager::acquireFrame()
{
	OniFrameInternal* pFrame = m_frames.Acquire();

	xnOSMemSet(pFrame, 0, sizeof(OniFrameInternal));
	pFrame->refCount = 1;

	return pFrame;
}

void FrameManager::addRef(OniFrame* pFrame)
{
	OniFrameInternal* pInternal = (OniFrameInternal*)pFrame;
	m_frames.Lock();
	++pInternal->refCount;
	m_frames.Unlock();
}

void FrameManager::release(OniFrame* pFrame)
{
	OniFrameInternal* pInternal = (OniFrameInternal*)pFrame;
	m_frames.Lock();
	if (--pInternal->refCount == 0)
	{
		// notify frame is back to pool
        if (pInternal->backToPoolFunc != NULL)
        {
            pInternal->backToPoolFunc(pInternal, pInternal->backToPoolFuncCookie);
        }
        
		// and return frame to pool
		m_frames.Release(pInternal);
	}
	m_frames.Unlock();
}

ONI_NAMESPACE_IMPLEMENTATION_END
