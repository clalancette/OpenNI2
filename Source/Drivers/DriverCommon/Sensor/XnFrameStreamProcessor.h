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
#ifndef XNFRAMESTREAMPROCESSOR_H
#define XNFRAMESTREAMPROCESSOR_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnStreamProcessor.h"
#include <DDK/XnFrameStream.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

/*
* A processor for streams that are frame-based.
*/
class XnFrameStreamProcessor : public XnStreamProcessor
{
public:
	/*
	* Initializes a new frame-stream-processor.
	*
	* @param	pDevicePrivateData	[in]	A pointer to the device.
	* @param	csName				[in]	Name of this stream.
	* @param	nTypeSOF			[in]	The packet type that signifies start-of-frame.
	* @param	nTypeEOF			[in]	The packet type that signifies end-of-frame.
	*/
	XnFrameStreamProcessor(XnFrameStream* pStream, XnSensorStreamHelper* pHelper, XnFrameBufferManager* pBufferManager, uint16_t nTypeSOF, uint16_t nTypeEOF);

	/**
	* Destroys a frame-based stream processor
	*/
	virtual ~XnFrameStreamProcessor();

protected:
	//---------------------------------------------------------------------------
	// Overridden Functions
	//---------------------------------------------------------------------------
	virtual void ProcessPacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, uint32_t nDataOffset, uint32_t nDataSize);
	virtual void OnPacketLost();

	//---------------------------------------------------------------------------
	// New Virtual Functions
	//---------------------------------------------------------------------------

	/*
	* Called when a frame starts.
	*
	* @param	pHeader 	[in]	Header for current packet.
	*/
	virtual void OnStartOfFrame(const XnSensorProtocolResponseHeader* pHeader);

	/*
	* Called for every chunk received
	*
	* @param	pHeader 	[in]	A pointer to current packet header.
	* @param	pData		[in]	A pointer to the data.
	* @param	nDataOffset	[in]	The offset of this data chunk inside current packet.
	* @param	nDataSize	[in]	Size of the data in bytes.
	*/
	virtual void ProcessFramePacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, uint32_t nDataOffset, uint32_t nDataSize) = 0;

	/*
	* Called when a frame ends.
	*
	* @param	pHeader 	[in]	Header for current packet.
	*/
	virtual void OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader);

	/*
	* Called when a frame is ready for reading.
	*
	* @param	nFrameID	[in]	ID of this frame.
	* @param	nFrameTS	[in]	Timestamp of this frame.
	*/
	virtual void OnFrameReady(uint32_t /*nFrameID*/, uint64_t /*nFrameTS*/) {}

	//---------------------------------------------------------------------------
	// Utility Functions
	//---------------------------------------------------------------------------

	inline XnFrameStream* GetStream()
	{
		return (XnFrameStream*)XnStreamProcessor::GetStream();
	}

	/*
	* Gets the expected output size.
	*/
	inline uint32_t GetExpectedOutputSize()
	{
		return GetStream()->GetRequiredDataSize();
	}

	/*
	* Gets current write buffer.
	*/
	inline XnBuffer* GetWriteBuffer()
	{
		return m_pTripleBuffer->GetWriteBuffer();
	}

	inline OniFrame* GetWriteFrame()
	{
		return m_pTripleBuffer->GetWriteFrame();
	}

	/*
	* Gets current frame ID (for logging purposes mainly).
	*/
	inline uint32_t GetCurrentFrameID()
	{
		return m_pTripleBuffer->GetLastFrameID();
	}

	/*
	* Notifies that write buffer has overflowed, logging a warning and reseting it.
	*/
	void WriteBufferOverflowed();

	/*
	* Checks if write buffer has overflowed, if so, a log will be issued and buffer will reset.
	*/
	inline bool CheckWriteBufferForOverflow(uint32_t nWriteSize)
	{
		if (GetWriteBuffer()->GetFreeSpaceInBuffer() < nWriteSize)
		{
			WriteBufferOverflowed();
			return false;
		}

		return true;
	}

	/*
	* Marks current frame as corrupted.
	*/
	void FrameIsCorrupted();

	void SetAllowDoubleSOFPackets(bool bAllow) { m_bAllowDoubleSOF = bAllow; }

private:
	//---------------------------------------------------------------------------
	// Class Members
	//---------------------------------------------------------------------------
	/* The type of start-of-frame packet. */
	uint16_t m_nTypeSOF;
	/* The type of end-of-frame packet. */
	uint16_t m_nTypeEOF;
	/* A pointer to the triple frame buffer of this stream. */
	XnFrameBufferManager* m_pTripleBuffer;

	char m_csInDumpMask[XN_DEVICE_MAX_STRING_LENGTH + 2];
	char m_csInternalDumpMask[XN_DEVICE_MAX_STRING_LENGTH + 8];
	XnDumpFile* m_InDump;
	XnDumpFile* m_InternalDump;
	bool m_bFrameCorrupted;
	bool m_bAllowDoubleSOF;
	uint16_t m_nLastSOFPacketID;
	uint64_t m_nFirstPacketTimestamp;
};

#endif // XNFRAMESTREAMPROCESSOR_H
