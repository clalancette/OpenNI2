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
#include "XnDataProcessor.h"
#include <XnProfiling.h>
#include "XnSensor.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnDataProcessor::XnDataProcessor(XnDevicePrivateData* pDevicePrivateData, const char* csName) :
	m_pDevicePrivateData(pDevicePrivateData),
	m_nBytesReceived(0),
	m_nLastPacketID(0),
	m_csName(csName),
	m_bUseHostTimestamps(false)
{
	m_TimeStampData.csStreamName = csName;
	m_TimeStampData.bFirst = true;
	m_bUseHostTimestamps = pDevicePrivateData->pSensor->ShouldUseHostTimestamps();
}

XnDataProcessor::~XnDataProcessor()
{}

XnStatus XnDataProcessor::Init()
{
	return (XN_STATUS_OK);
}

void XnDataProcessor::ProcessData(const XnSensorProtocolResponseHeader* pHeader, const unsigned char* pData, uint32_t nDataOffset, uint32_t nDataSize)
{
	XN_PROFILING_START_SECTION("XnDataProcessor::ProcessData")

	// count these bytes
	m_nBytesReceived += nDataSize;

	// check if we start a new packet
	if (nDataOffset == 0)
	{
		// make sure no packet was lost
		if (pHeader->nPacketID != m_nLastPacketID+1 && pHeader->nPacketID != 0)
		{
			xnLogWarning(XN_MASK_SENSOR_PROTOCOL, "%s: Expected %x, got %x", m_csName, m_nLastPacketID+1, pHeader->nPacketID);
			OnPacketLost();
		}

		m_nLastPacketID = pHeader->nPacketID;

		// log packet arrival
		uint64_t nNow;
		xnOSGetHighResTimeStamp(&nNow);
		xnDumpFileWriteString(m_pDevicePrivateData->MiniPacketsDump, "%llu,0x%hx,0x%hx,0x%hx,%u\n", nNow, pHeader->nType, pHeader->nPacketID, pHeader->nBufSize, pHeader->nTimeStamp);
	}

	ProcessPacketChunk(pHeader, pData, nDataOffset, nDataSize);

	XN_PROFILING_END_SECTION
}

void XnDataProcessor::OnPacketLost()
{}

uint64_t XnDataProcessor::CreateTimestampFromDevice(uint32_t nDeviceTimeStamp)
{
	uint64_t nNow;
	xnOSGetHighResTimeStamp(&nNow);

	// we register the first TS calculated as time-zero. Every stream's TS data will be
	// synchronized with it
	if (m_pDevicePrivateData->nGlobalReferenceTS == 0)
	{
		xnOSEnterCriticalSection(&m_pDevicePrivateData->hEndPointsCS);
		if (m_pDevicePrivateData->nGlobalReferenceTS == 0)
		{
			m_pDevicePrivateData->nGlobalReferenceTS = nDeviceTimeStamp;
			m_pDevicePrivateData->nGlobalReferenceOSTime = nNow;
		}
		xnOSLeaveCriticalSection(&m_pDevicePrivateData->hEndPointsCS);
	}

	const uint64_t nWrapPoint = ((uint64_t)XN_MAX_UINT32) + 1;
	uint64_t nResultInTicks;
	const uint32_t nDumpCommentMaxLength = 200;
	char csDumpComment[nDumpCommentMaxLength] = "";
	bool bCheckSanity = true;

	if (m_TimeStampData.bFirst)
	{
		/*
		This is a bit tricky, as we need to synchronize the first timestamp of different streams.
		We somehow need to translate 32-bit tick counts to 64-bit timestamps. The device timestamps
		wrap-around every ~71.5 seconds (for PS1080 @ 60 MHz).
		Lets assume the first packet of the first stream got timestamp X. Now we get the first packet of another
		stream with a timestamp Y.
		We need to figure out what is the relation between X and Y.
		We do that by analyzing the following scenarios:
		1. Y is after X, in the same period (no wraparound yet).
		2. Y is after X, in a different period (one or more wraparounds occurred).
		3. Y is before X, in the same period (might happen due to race condition).
		4. Y is before X, in a different period (this can happen if X is really small, and Y is almost at wraparound).

		The following code tried to handle all those cases. It uses an OS timer to try and figure out how
		many wraparounds occurred.
		*/

		// estimate the number of wraparound that occurred using OS time
		uint64_t nOSTime = nNow - m_pDevicePrivateData->nGlobalReferenceOSTime;

		// calculate wraparound length
		double fWrapAroundInMicroseconds = nWrapPoint / (double)m_pDevicePrivateData->fDeviceFrequency;

		// perform a rough estimation
		int32_t nWraps = (int32_t)(nOSTime / fWrapAroundInMicroseconds);

		// now fix the estimation by clipping TS to the correct wraparounds
		int64_t nEstimatedTicks =
			nWraps * nWrapPoint + // wraps time
			nDeviceTimeStamp - m_pDevicePrivateData->nGlobalReferenceTS;

		int64_t nEstimatedTime = (int64_t)(nEstimatedTicks / (double)m_pDevicePrivateData->fDeviceFrequency);

		if (nEstimatedTime < nOSTime - 0.5 * fWrapAroundInMicroseconds)
		{
			nWraps++;
		}
		else if (nEstimatedTime > nOSTime + 0.5 * fWrapAroundInMicroseconds)
		{
			nWraps--;
		}

		// handle the two special cases - 3 & 4 in which we get a timestamp which is
		// *before* global TS (meaning before time 0)
		if (nWraps < 0 || // case 4
			(nWraps == 0 && nDeviceTimeStamp < m_pDevicePrivateData->nGlobalReferenceTS)) // case 3
		{
			nDeviceTimeStamp = m_pDevicePrivateData->nGlobalReferenceTS;
			nWraps = 0;
		}

		m_TimeStampData.nReferenceTS = m_pDevicePrivateData->nGlobalReferenceTS;
		m_TimeStampData.nTotalTicksAtReferenceTS = nWrapPoint * nWraps;
		m_TimeStampData.nLastDeviceTS = 0;
		m_TimeStampData.bFirst = false;
		bCheckSanity = false; // no need.
		sprintf(csDumpComment, "Init. Total Ticks in Ref TS: %" PRIu64, m_TimeStampData.nTotalTicksAtReferenceTS);
	}

	if (nDeviceTimeStamp > m_TimeStampData.nLastDeviceTS) // this is the normal case
	{
		nResultInTicks = m_TimeStampData.nTotalTicksAtReferenceTS + nDeviceTimeStamp - m_TimeStampData.nReferenceTS;
	}
	else // wrap around occurred
	{
		// add the passed time to the reference time
		m_TimeStampData.nTotalTicksAtReferenceTS += (nWrapPoint + nDeviceTimeStamp - m_TimeStampData.nReferenceTS);
		// mark reference timestamp
		m_TimeStampData.nReferenceTS = nDeviceTimeStamp;

		sprintf(csDumpComment, "Wrap around. Refernce TS: %u / TotalTicksAtReference: %" PRIu64, m_TimeStampData.nReferenceTS, m_TimeStampData.nTotalTicksAtReferenceTS);

		nResultInTicks = m_TimeStampData.nTotalTicksAtReferenceTS;
	}

	m_TimeStampData.nLastDeviceTS = nDeviceTimeStamp;

	// calculate result in microseconds
	// NOTE: Intel compiler does too much optimization, and we loose up to 5 milliseconds. We perform
	// the entire calculation in double as a workaround
	double dResultTimeMicroSeconds = (double)nResultInTicks / (double)m_pDevicePrivateData->fDeviceFrequency;
	uint64_t nResultTimeMilliSeconds = (uint64_t)(dResultTimeMicroSeconds / 1000.0);

	bool bIsSane = true;

	// perform sanity check
	if (bCheckSanity && (nResultTimeMilliSeconds > (m_TimeStampData.nLastResultTime + XN_SENSOR_TIMESTAMP_SANITY_DIFF*1000)))
	{
		bIsSane = false;
		xnOSStrAppend(csDumpComment, ",Didn't pass sanity. Will try to re-sync.", nDumpCommentMaxLength);
	}

	uint64_t nResult = (uint64_t)dResultTimeMicroSeconds;

	// dump it
	xnDumpFileWriteString(m_pDevicePrivateData->TimestampsDump, "%llu,%s,%u,%llu,%s\n", nNow, m_TimeStampData.csStreamName, nDeviceTimeStamp, nResult, csDumpComment);

	if (bIsSane)
	{
		m_TimeStampData.nLastResultTime = nResultTimeMilliSeconds;
		return (nResult);
	}
	else
	{
		// sanity failed. We lost sync. restart
		m_TimeStampData.bFirst = true;
		return CreateTimestampFromDevice(nDeviceTimeStamp);
	}
}

uint64_t XnDataProcessor::GetHostTimestamp()
{
	uint64_t nNow;
	xnOSGetHighResTimeStamp(&nNow);

	// we register the first TS calculated as time-zero. Every stream's TS data will be
	// synchronized with it
	if (m_pDevicePrivateData->nGlobalReferenceTS == 0)
	{
		xnOSEnterCriticalSection(&m_pDevicePrivateData->hEndPointsCS);
		if (m_pDevicePrivateData->nGlobalReferenceTS == 0)
		{
			m_pDevicePrivateData->nGlobalReferenceTS = (uint32_t)nNow;
			m_pDevicePrivateData->nGlobalReferenceOSTime = nNow;
		}
		xnOSLeaveCriticalSection(&m_pDevicePrivateData->hEndPointsCS);
	}

	uint64_t nResultTimeMicroseconds = nNow - m_pDevicePrivateData->nGlobalReferenceOSTime;
	return nResultTimeMicroseconds;
}
