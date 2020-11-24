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
#ifndef XNPACKED11DEPTHPROCESSOR_H
#define XNPACKED11DEPTHPROCESSOR_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDepthProcessor.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

class XnPacked11DepthProcessor final : public XnDepthProcessor
{
public:
	XnPacked11DepthProcessor(XnSensorDepthStream* pStream, XnSensorStreamHelper* pHelper, XnFrameBufferManager* pBufferManager);
	~XnPacked11DepthProcessor();

	XnStatus Init();

private:
	//---------------------------------------------------------------------------
	// Overridden Functions
	//---------------------------------------------------------------------------
	void ProcessFramePacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, XnUInt32 nDataOffset, XnUInt32 nDataSize) override;
	void OnStartOfFrame(const XnSensorProtocolResponseHeader* pHeader) override;
	void OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader) override;

	//---------------------------------------------------------------------------
	// Internal Functions
	//---------------------------------------------------------------------------
	XnStatus Unpack11to16(const XnUInt8* pcInput, const XnUInt32 nInputSize, XnUInt32* pnActualRead);

	//---------------------------------------------------------------------------
	// Class Members
	//---------------------------------------------------------------------------
	/* A buffer used for storing some left-over bytes for the next packet. */
	XnBuffer m_ContinuousBuffer;
};

#endif // XNPACKED11DEPTHPROCESSOR_H
