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
#ifndef ONISTREAMFRAMEHOLDER_H
#define ONISTREAMFRAMEHOLDER_H

#include "OniCommon.h"
#include "OniFrameHolder.h"

ONI_NAMESPACE_IMPLEMENTATION_BEGIN

class VideoStream;

class StreamFrameHolder final : public FrameHolder
{
public:

	// Constructor.
	StreamFrameHolder(FrameManager& frameManager, VideoStream* pStream);

	// Destructor.
	~StreamFrameHolder();

	// Get the next frame belonging to a stream.
	OniStatus readFrame(VideoStream* pStream, OniFrame** pFrame) override;

	// Process a newly received frame.
	OniStatus processNewFrame(VideoStream* pStream, OniFrame* pFrame) override;

	// Peek at next frame.
	OniFrame* peekFrame(VideoStream* pStream) override;

	// Clear all the frame in the holder.
	void clear() override;

	// Return list of streams which are members of the stream group.
	void getStreams(VideoStream** ppStreams, int* pNumStreams) override;

	// Return number of streams which are members of the stream group.
	int getNumStreams() override;

	void setStreamEnabled(VideoStream* /*pStream*/, bool /*enabled*/) override;

private:

	VideoStream* m_pStream;

	OniFrame* m_pLastFrame;
};

ONI_NAMESPACE_IMPLEMENTATION_END

#endif // ONISTREAMFRAMEHOLDER_H
