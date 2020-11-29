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
#include "XnLib.h"
#include <XnLog.h>
#include <XnHash.h>
#include <XnStatusRegister.h>
#include <XnStatusCodes.h>

#define XN_MASK_STATUS "XnStatus"
#define XN_OK 0

//---------------------------------------------------------------------------
// Data Types
//---------------------------------------------------------------------------
typedef xnl::Hash<uint16_t, XnErrorCodeData> XnStatusHash;
typedef xnl::Hash<uint16_t, XnStatusHash*> XnGroupsHash;

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------

/** An array holding all groups' errors */
static XnGroupsHash* g_pErrorGroups = NULL;

static XnErrorCodeData s_OK_Data = { XN_STATUS_OK, XN_STRINGIFY(XN_STATUS_OK), "OK" };
static XnStatus s_XN_OK_result = xnRegisterErrorCodeMessages(0, 0, 1, &s_OK_Data);

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XnErrorCodeData* xnGetErrorCodeData(const XnStatus Status)
{
	// search for it
	uint16_t nGroup = XN_STATUS_GROUP(Status);
	uint16_t nCode = XN_STATUS_CODE(Status);

	if (g_pErrorGroups == NULL)
	{
		return NULL;
	}

	XnStatusHash* pStatusHash = NULL;
	if (g_pErrorGroups->Get(nGroup, pStatusHash) != XN_STATUS_OK)
	{
		// unregistered group
		return NULL;
	}

	XnErrorCodeData* pResult = NULL;
	pStatusHash->Get(nCode, pResult);
	return pResult;
}

XN_C_API XnStatus xnRegisterErrorCodeMessages(uint16_t nGroup, uint16_t nFirst, uint16_t nCount, XnErrorCodeData* pErrorCodeData)
{
	XnStatus nRetVal = XN_STATUS_OK;
	XN_VALIDATE_OUTPUT_PTR(pErrorCodeData);

	if (g_pErrorGroups == NULL)
	{
		g_pErrorGroups = XN_NEW(XnGroupsHash);
		XN_VALIDATE_ALLOC_PTR(g_pErrorGroups);
	}

	XnStatusHash* pStatusHash = NULL;
	if (g_pErrorGroups->Get(nGroup, pStatusHash) != XN_STATUS_OK)
	{
		XN_VALIDATE_NEW(pStatusHash, XnStatusHash);
		nRetVal = g_pErrorGroups->Set(nGroup, pStatusHash);
		XN_IS_STATUS_OK(nRetVal);
	}

	for (uint16_t nIndex = 0; nIndex < nCount; ++nIndex)
	{
		XnErrorCodeData data = {0, NULL, NULL};
		data.nCode = nFirst + nIndex;
		data.csMessage = xnOSStrDup(pErrorCodeData[nIndex].csMessage);
		data.csName = xnOSStrDup(pErrorCodeData[nIndex].csName);

		XnErrorCodeData prevData = {0, NULL, NULL};
		pStatusHash->Get((uint16_t)data.nCode, prevData);

		nRetVal = pStatusHash->Set((uint16_t)data.nCode, data);
		XN_IS_STATUS_OK(nRetVal);

		// if prev contained anything, free it
		xnOSFree(prevData.csName);
		xnOSFree(prevData.csMessage);
	}

	return XN_STATUS_OK;
}

XN_C_API const char* xnGetStatusString(const XnStatus Status)
{
	XnErrorCodeData* pErrorData = xnGetErrorCodeData(Status);
	if (pErrorData == NULL)
	{
		// unregistered error
		return "Unknown Xiron Status!";
	}
	else
	{
		return pErrorData->csMessage;
	}
}

XN_C_API const char* xnGetStatusName(const XnStatus Status)
{
	XnErrorCodeData* pErrorData = xnGetErrorCodeData(Status);
	if (pErrorData == NULL)
	{
		// unregistered error
		return "Unknown Xiron Status!";
	}
	else
	{
		return pErrorData->csName;
	}
}

XN_C_API void xnPrintError(const XnStatus Status, const char* csUserMessage)
{
	printf("%s: %s\n", csUserMessage, xnGetStatusString(Status));
}
