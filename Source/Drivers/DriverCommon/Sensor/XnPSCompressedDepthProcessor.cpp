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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnPSCompressedDepthProcessor.h"
#include <XnProfiling.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnPSCompressedDepthProcessor::XnPSCompressedDepthProcessor(XnSensorDepthStream* pStream, XnSensorStreamHelper* pHelper, XnFrameBufferManager* pBufferManager) :
	XnDepthProcessor(pStream, pHelper, pBufferManager)
{
}

XnStatus XnPSCompressedDepthProcessor::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = XnDepthProcessor::Init();
	XN_IS_STATUS_OK(nRetVal);

	XN_VALIDATE_BUFFER_ALLOCATE(m_RawData, GetExpectedOutputSize());

	return XN_STATUS_OK;
}

XnPSCompressedDepthProcessor::~XnPSCompressedDepthProcessor()
{
}

#define XN_CHECK_UNC_DEPTH_OUTPUT(x, y, z)			\
	if (x >= y)										\
	{												\
		return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);	\
	}												\
	if (z >= XN_DEVICE_SENSOR_MAX_SHIFT_VALUE)		\
	{												\
		z = XN_DEVICE_SENSOR_NO_DEPTH_VALUE;		\
	}

#define XN_DEPTH_OUTPUT(pDepthOutput, pOutputEnd, nValue)					                \
	XN_CHECK_UNC_DEPTH_OUTPUT(pDepthOutput, pOutputEnd, nValue)				                \
	*pDepthOutput = GetOutput(nValue);										                \
	++pDepthOutput;

#define GET_NEXT_INPUT(nInput)								\
	if (__bShouldReadByte)									\
	{														\
		if (__pCurrInput == __pInputEnd)					\
			break;											\
															\
		/* read from input */								\
		__nLastByte = *__pCurrInput;						\
		__bShouldReadByte = false;							\
															\
		/* take high 4-bits */								\
		nInput = __nLastByte >> 4;							\
															\
		__pCurrInput++;										\
	}														\
	else													\
	{														\
		/* byte already read. take its low 4-bits */		\
		nInput = __nLastByte & 0x0F;						\
		__bShouldReadByte = true;							\
	}


#define GET_INPUT_READ_BYTES (__pCurrInput - __pInputOrig);

XnStatus XnPSCompressedDepthProcessor::UncompressDepthPS(const uint8_t* pInput, const uint32_t nInputSize,
								   uint16_t* pDepthOutput, uint32_t* pnOutputSize,
								   uint32_t* pnActualRead, bool bLastPart)
{
	// Input is made of 4-bit elements.
	const uint8_t* __pInputOrig = pInput;
	const uint8_t* __pCurrInput = pInput;
	const uint8_t* __pInputEnd = pInput + nInputSize;
	/** True if input is in a steady state (not in the middle of a byte) */
	bool __bShouldReadByte = true;
	uint32_t __nLastByte = 0;

	uint16_t* pOutputEnd = pDepthOutput + (*pnOutputSize / sizeof(OniDepthPixel));
	uint16_t nLastValue = 0;

	const uint8_t* pInputOrig = pInput;
	uint16_t* pOutputOrig = pDepthOutput;

	const uint8_t* pInputLastPossibleStop = pInputOrig;
	uint16_t* pOutputLastPossibleStop = pOutputOrig;

	// NOTE: we use variables of type uint32 instead of uint8 as an optimization (better CPU usage)
	uint32_t nInput;
	uint32_t nLargeValue;
	bool bCanStop;

	for (;;)
	{
		bCanStop = __bShouldReadByte;
		GET_NEXT_INPUT(nInput);

		switch (nInput)
		{
		case 0xd: // Dummy.
			// Do nothing
			break;
		case 0xe: // RLE
			// read count
			GET_NEXT_INPUT(nInput);

			// should repeat last value (nInput + 1) times
			nInput++;
			while (nInput != 0)
			{
				XN_DEPTH_OUTPUT(pDepthOutput, pOutputEnd, nLastValue);
				--nInput;
			}
			break;

		case 0xf: // Full (or large)
			// read next element
			GET_NEXT_INPUT(nInput);

			// First bit tells us if it's a large diff (turned on) or a full value (turned off)
			if (nInput & 0x8) // large diff (7-bit)
			{
				// turn off high bit, and shift left
				nLargeValue = (nInput - 0x8) << 4;

				// read low 4-bits
				GET_NEXT_INPUT(nInput);

				nLargeValue |= nInput;
				// diff values are from -64 to 63 (0x00 to 0x7f)
				nLastValue += ((int16_t)nLargeValue - 64);
			}
			else // Full value (15-bit)
			{
				if (bCanStop)
				{
					// We can stop here. First input is a full value
					/** Gets a pointer to 1 element before current input */
					pInputLastPossibleStop = __pCurrInput - 1;
					pOutputLastPossibleStop = pDepthOutput;
				}

				nLargeValue = (nInput << 12);

				// read 3 more elements
				GET_NEXT_INPUT(nInput);
				nLargeValue |= nInput << 8;

				GET_NEXT_INPUT(nInput);
				nLargeValue |= nInput << 4;

				GET_NEXT_INPUT(nInput);
				nLastValue = (uint16_t)(nLargeValue | nInput);
			}

			XN_DEPTH_OUTPUT(pDepthOutput, pOutputEnd, nLastValue);

			break;
		default: // all rest (smaller than 0xd) are diffs
			// diff values are from -6 to 6 (0x0 to 0xc)
			nLastValue += ((int16_t)nInput - 6);
			XN_DEPTH_OUTPUT(pDepthOutput, pOutputEnd, nLastValue);
		}
	}

	if (bLastPart == true)
	{
		*pnOutputSize = (uint32_t)(pDepthOutput - pOutputOrig) * sizeof(uint16_t);
		*pnActualRead = (uint32_t)GET_INPUT_READ_BYTES;
	}
	else
	{
		*pnOutputSize = (uint32_t)(pOutputLastPossibleStop - pOutputOrig) * sizeof(uint16_t);
		*pnActualRead = (uint32_t)(pInputLastPossibleStop - pInputOrig) * sizeof(uint8_t);
	}

	// All is good...
	return (XN_STATUS_OK);
}

void XnPSCompressedDepthProcessor::ProcessFramePacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, uint32_t nDataOffset, uint32_t nDataSize)
{
	XN_PROFILING_START_SECTION("XnPSCompressedDepthProcessor::ProcessFramePacketChunk")

	XnBuffer* pWriteBuffer = GetWriteBuffer();

	const XnUChar* pBuf = NULL;
	uint32_t nBufSize = 0;

	// check if we have bytes stored from previous calls
	if (m_RawData.GetSize() > 0)
	{
		// we have no choice. We need to append current buffer to previous bytes
		if (m_RawData.GetFreeSpaceInBuffer() < nDataSize)
		{
			xnLogWarning(XN_MASK_SENSOR_PROTOCOL_DEPTH, "Bad overflow depth! %d", m_RawData.GetSize());
			FrameIsCorrupted();
		}
		else
		{
			m_RawData.UnsafeWrite(pData, nDataSize);
		}

		pBuf = m_RawData.GetData();
		nBufSize = m_RawData.GetSize();
	}
	else
	{
		// we can process the data directly
		pBuf = pData;
		nBufSize = nDataSize;
	}

	uint32_t nOutputSize = pWriteBuffer->GetFreeSpaceInBuffer();
	uint32_t nWrittenOutput = nOutputSize;
	uint32_t nActualRead = 0;
	bool bLastPart = pHeader->nType == XN_SENSOR_PROTOCOL_RESPONSE_DEPTH_END && (nDataOffset + nDataSize) == pHeader->nBufSize;
	XnStatus nRetVal = UncompressDepthPS(pBuf, nBufSize, (uint16_t*)pWriteBuffer->GetUnsafeWritePointer(),
			&nWrittenOutput, &nActualRead, bLastPart);

	if (nRetVal != XN_STATUS_OK)
	{
		FrameIsCorrupted();

		static uint64_t nLastPrinted = 0;

		uint64_t nCurrTime;
		xnOSGetTimeStamp(&nCurrTime);

		if (nOutputSize != 0 || (nCurrTime - nLastPrinted) > 1000)
		{
			xnLogWarning(XN_MASK_SENSOR_PROTOCOL_DEPTH, "Uncompress depth failed: %s. Input Size: %u, Output Space: %u, Last Part: %d.", xnGetStatusString(nRetVal), nBufSize, nOutputSize, bLastPart);

			xnOSGetTimeStamp(&nLastPrinted);
		}
	}

	pWriteBuffer->UnsafeUpdateSize(nWrittenOutput);

	nBufSize -= nActualRead;
	m_RawData.Reset();

	// if we have any bytes left, keep them for next time
	if (nBufSize > 0)
	{
		pBuf += nActualRead;
		m_RawData.UnsafeWrite(pBuf, nBufSize);
	}

	XN_PROFILING_END_SECTION
}

void XnPSCompressedDepthProcessor::OnStartOfFrame(const XnSensorProtocolResponseHeader* pHeader)
{
	XnDepthProcessor::OnStartOfFrame(pHeader);
	m_RawData.Reset();
}

void XnPSCompressedDepthProcessor::OnEndOfFrame(const XnSensorProtocolResponseHeader* pHeader)
{
	XnDepthProcessor::OnEndOfFrame(pHeader);
	m_RawData.Reset();
}
