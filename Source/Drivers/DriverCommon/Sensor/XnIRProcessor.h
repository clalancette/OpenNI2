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
#ifndef XNIRPROCESSOR_H
#define XNIRPROCESSOR_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnFrameStreamProcessor.h"
#include "XnSensorIRStream.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

class XnIRProcessor final : public XnFrameStreamProcessor
{
public:
	XnIRProcessor(XnSensorIRStream* pStream, XnSensorStreamHelper* pHelper, XnFrameBufferManager* pBufferManager);
	~XnIRProcessor();

	XnStatus Init();

private:
	//---------------------------------------------------------------------------
	// Overridden Functions
	//---------------------------------------------------------------------------
	void ProcessFramePacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, uint32_t nDataOffset, uint32_t nDataSize) override;
	void OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader) override;
	XnUInt64 CreateTimestampFromDevice(uint32_t nDeviceTimeStamp) override;
	void OnFrameReady(uint32_t nFrameID, XnUInt64 nFrameTS) override;

	//---------------------------------------------------------------------------
	// Internal Functions
	//---------------------------------------------------------------------------
	XnStatus Unpack10to16(const XnUInt8* pcInput, const uint32_t nInputSize, XnUInt16* pnOutput, uint32_t* pnActualRead, uint32_t* pnOutputSize);
	inline XnSensorIRStream* GetStream()
	{
		return (XnSensorIRStream*)XnFrameStreamProcessor::GetStream();
	}

	//---------------------------------------------------------------------------
	// Class Members
	//---------------------------------------------------------------------------
	/* A buffer to store bytes till we have enough to unpack. */
	XnBuffer m_ContinuousBuffer;
	XnBuffer m_UnpackedBuffer;
	XnUInt64 m_nRefTimestamp; // needed for firmware bug workaround
	XnDepthCMOSType m_DepthCMOSType;
};

#endif // XNIRPROCESSOR_H
