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
#ifndef XNDEPTHPROCESSOR_H
#define XNDEPTHPROCESSOR_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnFrameStreamProcessor.h"
#include "XnSensorDepthStream.h"

//---------------------------------------------------------------------------
// Compilation Checks
//---------------------------------------------------------------------------

// Optimization: in order to save branches in the code itself, we create a shift-to-depth
// map which will actually translate shift-to-shift. This optimization relies on the
// fact that both shifts and depths are 16-bit long. If this is not the case,
// this optimization should be re-written.
// Then, any processor can always go through this LUT, no matter what the output format is.
#if (OniDepthPixel != uint16_t)
	#error "Depth and Shift do not have the same size. Need to reconsider optimization!"
#endif

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
class XnDepthProcessor : public XnFrameStreamProcessor
{
public:
	XnDepthProcessor(XnSensorDepthStream* pStream, XnSensorStreamHelper* pHelper, XnFrameBufferManager* pBufferManager);
	virtual ~XnDepthProcessor();

	XnStatus Init();

protected:
	//---------------------------------------------------------------------------
	// Overridden Functions
	//---------------------------------------------------------------------------
	virtual void OnStartOfFrame(const XnSensorProtocolResponseHeader* pHeader);
	virtual void OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader);
	virtual void OnFrameReady(uint32_t nFrameID, uint64_t nFrameTS);

	//---------------------------------------------------------------------------
	// Helper Functions
	//---------------------------------------------------------------------------
	inline XnSensorDepthStream* GetStream()
	{
		return (XnSensorDepthStream*)XnFrameStreamProcessor::GetStream();
	}

	inline OniDepthPixel GetOutput(uint16_t nShift)
	{
		return m_pShiftToDepthTable[nShift];
	}

	inline uint32_t GetExpectedSize()
	{
		return m_nExpectedFrameSize;
	}

private:
	void PadPixels(uint32_t nPixels);
	uint32_t CalculateExpectedSize();

	uint32_t m_nPaddingPixelsOnEnd;
	bool m_applyRegistrationOnEnd;
	uint32_t m_nExpectedFrameSize;
	bool m_bShiftToDepthAllocated;
	OniDepthPixel* m_pShiftToDepthTable;
	OniDepthPixel m_noDepthValue;
};

#endif // XNDEPTHPROCESSOR_H
