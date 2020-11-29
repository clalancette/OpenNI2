/*****************************************************************************
*                                                                            *
*  PrimeSense PSCommon Library                                               *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PSCommon.                                            *
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
#include <XnOS.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus xnOSWaitForCondition(const XN_EVENT_HANDLE EventHandle, uint32_t nMilliseconds, XnConditionFunc pConditionFunc, void* pConditionData)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// take read start time (for timeout purposes)
	uint64_t nStarted;
	nRetVal = xnOSGetTimeStamp(&nStarted);
	XN_IS_STATUS_OK(nRetVal);

	bool bTimeout = false;

	// as long as condition isn't met
	while (!pConditionFunc(pConditionData))
	{
		// check if timeout occurred
		uint64_t nNow;
		nRetVal = xnOSGetTimeStamp(&nNow);
		XN_IS_STATUS_OK(nRetVal);

		if (nNow - nStarted > nMilliseconds)
		{
			bTimeout = true;
		}
		else
		{
			// not yet. Wait for event to be set
			nRetVal = xnOSWaitEvent(EventHandle, (uint32_t)(nMilliseconds - (nNow - nStarted)));
			if (nRetVal == XN_STATUS_OS_EVENT_TIMEOUT)
			{
				bTimeout = true;
			}
			else if (nRetVal != XN_STATUS_OK)
			{
				xnLogWarning(XN_MASK_OS, "Failed waiting on event for condition...");
			}
		}

		if (bTimeout)
		{
			return (XN_STATUS_OS_EVENT_TIMEOUT);
		}
	}

	// condition was met
	return (XN_STATUS_OK);
}
