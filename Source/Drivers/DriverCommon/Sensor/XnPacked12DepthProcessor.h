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
#ifndef XNPACKED12DEPTHPROCESSOR_H
#define XNPACKED12DEPTHPROCESSOR_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDepthProcessor.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

class XnPacked12DepthProcessor : public XnDepthProcessor
{
public:
	XnPacked12DepthProcessor(XnSensorDepthStream* pStream, XnSensorStreamHelper* pHelper, XnFrameBufferManager* pBufferManager);
	virtual ~XnPacked12DepthProcessor();

	XnStatus Init();

protected:
	//---------------------------------------------------------------------------
	// Overridden Functions
	//---------------------------------------------------------------------------
	virtual void ProcessFramePacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, uint32_t nDataOffset, uint32_t nDataSize);
	virtual void OnStartOfFrame(const XnSensorProtocolResponseHeader* pHeader);
	virtual void OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader);

	//---------------------------------------------------------------------------
	// Internal Functions
	//---------------------------------------------------------------------------
	XnStatus Unpack12to16(const uint8_t* pcInput, const uint32_t nInputSize, uint32_t* pnActualRead);

	//---------------------------------------------------------------------------
	// Class Members
	//---------------------------------------------------------------------------
private:
	/* A buffer used for storing some left-over bytes for the next packet. */
	XnBuffer m_ContinuousBuffer;
};

#endif // XNPACKED12DEPTHPROCESSOR_H
