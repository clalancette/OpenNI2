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
#ifndef XNIMAGEPROCESSOR_H
#define XNIMAGEPROCESSOR_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnFrameStreamProcessor.h"
#include "XnSensorImageStream.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
class XnImageProcessor : public XnFrameStreamProcessor
{
public:
	XnImageProcessor(XnSensorImageStream* pStream, XnSensorStreamHelper* pHelper, XnFrameBufferManager* pBufferManager, bool bCompressedOutput = false);
	virtual ~XnImageProcessor();

	XnStatus Init();

protected:
	//---------------------------------------------------------------------------
	// Overridden Functions
	//---------------------------------------------------------------------------
	virtual void OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader);
	virtual void OnFrameReady(uint32_t nFrameID, uint64_t nFrameTS);

	//---------------------------------------------------------------------------
	// Helper Functions
	//---------------------------------------------------------------------------
	inline XnSensorImageStream* GetStream()
	{
		return (XnSensorImageStream*)XnFrameStreamProcessor::GetStream();
	}

	uint32_t GetActualXRes() { return m_nActualXRes; }
	uint32_t GetActualYRes() { return m_nActualYRes; }

private:
	uint32_t CalculateExpectedSize();
	void CalcActualRes();
	static XnStatus XN_CALLBACK_TYPE ActualResChangedCallback(const XnProperty* pSender, void* pCookie);

	uint32_t m_nActualXRes;
	uint32_t m_nActualYRes;

	XnCallbackHandle m_hXResCallback;
	XnCallbackHandle m_hYResCallback;
	XnCallbackHandle m_hXCropCallback;
	XnCallbackHandle m_hYCropCallback;
	XnCallbackHandle m_hCropEnabledCallback;

	bool m_bCompressedOutput;
};

#endif // XNIMAGEPROCESSOR_H
