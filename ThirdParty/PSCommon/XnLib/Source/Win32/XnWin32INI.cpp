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
#include "XnLib.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSReadStringFromINI(const char* cpINIFile, const char* cpSection, const char* cpKey, char* cpDest, const uint32_t nDestLength)
{
	// Local function variables
	uint32_t nReadBytes = 0;
	bool bINIFileExists = false;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);
	XN_VALIDATE_OUTPUT_PTR(cpDest);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// Read the string from the INI file via the OS
	nReadBytes = GetPrivateProfileString (cpSection, cpKey, NULL, cpDest, nDestLength, cpINIFile);

	// Make sure the value was read properly
	if (nReadBytes == 0)
	{
		return (XN_STATUS_OS_INI_READ_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSReadFloatFromINI(const char* cpINIFile, const char* cpSection, const char* cpKey, float* fDest)
{
	// Local function variables
	char cpTempBuffer[XN_INI_MAX_LEN];
	uint32_t nReadBytes = 0;
	bool bINIFileExists = false;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);
	XN_VALIDATE_OUTPUT_PTR(fDest);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// Read the string from the INI file via the OS
	nReadBytes = GetPrivateProfileString (cpSection, cpKey, NULL, cpTempBuffer, XN_INI_MAX_LEN, cpINIFile);

	// Make sure the value was read properly
	if (nReadBytes == 0)
	{
		return (XN_STATUS_OS_INI_READ_FAILED);
	}

	// Convert the string into float
	*fDest = (float)atof(cpTempBuffer);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSReadDoubleFromINI(const char* cpINIFile, const char* cpSection, const char* cpKey, double* fDest)
{
	// Local function variables
	char cpTempBuffer[XN_INI_MAX_LEN];
	uint32_t nReadBytes = 0;
	bool bINIFileExists = false;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);
	XN_VALIDATE_OUTPUT_PTR(fDest);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// Read the string from the INI file via the OS
	nReadBytes = GetPrivateProfileString (cpSection, cpKey, NULL, cpTempBuffer, XN_INI_MAX_LEN, cpINIFile);

	// Make sure the value was read properly
	if (nReadBytes == 0)
	{
		return (XN_STATUS_OS_INI_READ_FAILED);
	}

	// Convert the string into double
	*fDest = atof(cpTempBuffer);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSReadIntFromINI(const char* cpINIFile, const char* cpSection, const char* cpKey, int32_t* nDest)
{
	// Local function variables
	char cpTempBuffer[XN_INI_MAX_LEN];
	uint32_t nReadBytes = 0;
	bool bINIFileExists = false;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);
	XN_VALIDATE_OUTPUT_PTR(nDest);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// Read the string from the INI file via the OS
	nReadBytes = GetPrivateProfileString (cpSection, cpKey, NULL, cpTempBuffer, XN_INI_MAX_LEN, cpINIFile);

	// Make sure the value was read properly
	if (nReadBytes == 0)
	{
		return (XN_STATUS_OS_INI_READ_FAILED);
	}

	// Convert the string into float
	*nDest = atoi(cpTempBuffer);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWriteStringToINI(const char* cpINIFile, const char* cpSection, const char* cpKey, const char* cpSrc)
{
	// Local function variables
	bool bRetVal = false;
	bool bINIFileExists = false;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpSrc);
	XN_VALIDATE_INPUT_PTR(cpINIFile);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// Write the string to the INI file via the OS
	bRetVal = WritePrivateProfileString (cpSection, cpKey, cpSrc, cpINIFile);

	// Make sure the value was written properly
	if (bRetVal == false)
	{
		return (XN_STATUS_OS_INI_WRITE_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWriteFloatToINI(const char* cpINIFile, const char* cpSection, const char* cpKey, const float fSrc)
{
	// Local function variables
	char cpTempBuffer[XN_INI_MAX_LEN];
	bool bRetVal = false;
	bool bINIFileExists = false;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// Convert the float into a string
	sprintf(cpTempBuffer, "%f", fSrc);

	// Write the string to the INI file via the OS
	bRetVal = WritePrivateProfileString (cpSection, cpKey, cpTempBuffer, cpINIFile);

	// Make sure the value was written properly
	if (bRetVal == false)
	{
		return (XN_STATUS_OS_INI_WRITE_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWriteDoubleToINI(const char* cpINIFile, const char* cpSection, const char* cpKey, const double fSrc)
{
	// Local function variables
	char cpTempBuffer[XN_INI_MAX_LEN];
	bool bRetVal = false;
	bool bINIFileExists = false;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// Convert the double into a string
	sprintf(cpTempBuffer, "%f", fSrc);

	// Write the string to the INI file via the OS
	bRetVal = WritePrivateProfileString (cpSection, cpKey, cpTempBuffer, cpINIFile);

	// Make sure the value was written properly
	if (bRetVal == false)
	{
		return (XN_STATUS_OS_INI_WRITE_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWriteIntToINI(const char* cpINIFile, const char* cpSection, const char* cpKey, const int32_t nSrc)
{
	// Local function variables
	char cpTempBuffer[XN_INI_MAX_LEN];
	bool bRetVal = false;
	bool bINIFileExists = false;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// Convert the integer into a string
	_itoa(nSrc, cpTempBuffer, 10);

	// Write the string to the INI file via the OS
	bRetVal = WritePrivateProfileString (cpSection, cpKey, cpTempBuffer, cpINIFile);

	// Make sure the value was written properly
	if (bRetVal == false)
	{
		return (XN_STATUS_OS_INI_WRITE_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}
