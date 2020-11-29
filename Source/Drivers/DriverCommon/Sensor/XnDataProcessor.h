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
#ifndef XNDATAPROCESSOR_H
#define XNDATAPROCESSOR_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceSensor.h"
#include "XnDeviceSensorProtocol.h"
#include <XnLog.h>

/**
* Base class for all data processors.
*/
class XnDataProcessor
{
public:
	XnDataProcessor(XnDevicePrivateData* pDevicePrivateData, const XnChar* csName);
	virtual ~XnDataProcessor();

	//---------------------------------------------------------------------------
	// Methods
	//---------------------------------------------------------------------------
	/**
	 * Initializes a Data Processor.
	 *
	 * @param	pDevicePrivateData	[in]	A pointer to the device.
	 * @param	csName				[in]	The name of the stream.
	 */
	virtual XnStatus Init();

	/**
	 * Handles some data from this stream.
	 *
	 * @param	pHeader		[in]	A pointer to current packet header.
	 * @param	pData		[in]	A pointer to the data.
	 * @param	nDataOffset	[in]	The offset of this data chunk inside current packet.
	 * @param	nDataSize	[in]	Size of the data in bytes.
	 */
	void ProcessData(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, uint32_t nDataOffset, uint32_t nDataSize);

	inline XnBool ShouldUseHostTimestamps() { return m_bUseHostTimestamps; }

protected:
	//---------------------------------------------------------------------------
	// Virtual Functions
	//---------------------------------------------------------------------------
	virtual void ProcessPacketChunk(const XnSensorProtocolResponseHeader* pHeader, const XnUChar* pData, uint32_t nDataOffset, uint32_t nDataSize) = 0;
	virtual void OnPacketLost();

	//---------------------------------------------------------------------------
	// Utility Functions
	//---------------------------------------------------------------------------
	/*
	 * Gets a calculated timestamp from the device timestamp.
	 *
	 * @param	nDeviceTimeStamp	[in]	The device TS to translate.
	 */
	virtual uint64_t CreateTimestampFromDevice(uint32_t nDeviceTimeStamp);

	uint64_t GetHostTimestamp();

	//---------------------------------------------------------------------------
	// Class Members
	//---------------------------------------------------------------------------
	XnDevicePrivateData* m_pDevicePrivateData;
	/* The number of bytes received so far (since last time this member was reset). */
	uint32_t m_nBytesReceived;
	/* Stores last packet ID */
	uint16_t m_nLastPacketID;
	/* The name of the stream. */
	const XnChar* m_csName;

private:
	/* Data used for calculating timestamps. */
	XnTimeStampData m_TimeStampData;
	XnBool m_bUseHostTimestamps;
};

#endif // XNDATAPROCESSOR_H
