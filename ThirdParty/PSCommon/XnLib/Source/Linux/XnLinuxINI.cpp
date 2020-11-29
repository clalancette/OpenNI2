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
#include <string.h>

#define XN_READ_IXN_BUFFER_SIZE	10*1024

#define XN_IS_SPACE(p)		(*(p) == ' ' || *(p) == '\t')
#define XN_IS_NEWLINE(p)	(*(p) == '\r' || *(p) == '\n')

#define XN_SKIP_LINE(p)							\
	/* first read till end of line. */			\
	while (*p && *p != '\n' && *p != '\r')		\
		p++;									\
	/* now skip end of line tokens. */			\
	while (*p && (*p == '\n' || *p == '\r'))	\
		p++;

#define XN_READ_TILL(p, endCond, dest, destSize)			\
	{														\
		destSize = 0;										\
		while (*p && !(endCond))							\
		{													\
			if (destSize + 1 > XN_INI_MAX_LEN)				\
				break;										\
															\
			dest[destSize++] = *p;							\
			p++;											\
		}													\
		dest[destSize] = '\0';								\
	}

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
static XnStatus FindEntry(const char* cpINIFile, const char* cpSection, const char* cpKey, char* cpDest)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// get file size
	uint64_t nFileSize;
	nRetVal = xnOSGetFileSize64(cpINIFile, &nFileSize);
	XN_IS_STATUS_OK(nRetVal);

	// read entire file to memory
	char* csFileData = (char*)xnOSMalloc(sizeof(char)*nFileSize + 1);
	XN_VALIDATE_ALLOC_PTR(csFileData);

	nRetVal = xnOSLoadFile(cpINIFile, csFileData, nFileSize);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSFree(csFileData);
		return nRetVal;
	}

	// place NULL at the end
	csFileData[nFileSize] = '\0';

	// now parse file
	char* pCurPos = csFileData;
	bool bIsInRequestedSection = false;

	char csTempString[XN_INI_MAX_LEN];
	uint32_t nTempStringLength = 0;

	while (true)
	{
		// ignore spaces
		while (*pCurPos && XN_IS_SPACE(pCurPos))
		{
			pCurPos++;
		}

		// check we haven't reached the end
		if (!*pCurPos)
		{
			break;
		}

		if (*pCurPos == ';' || *pCurPos == '#') // comment
		{
			XN_SKIP_LINE(pCurPos);
			continue;
		}

		if (*pCurPos == '[') // start of section
		{
			pCurPos++;
			XN_READ_TILL(pCurPos, *pCurPos == ']' || XN_IS_NEWLINE(pCurPos), csTempString, nTempStringLength);

			if (*pCurPos == ']') // valid section name
			{
				if (bIsInRequestedSection)
				{
					// we're leaving the requested section, and string wasn't found
					xnOSFree(csFileData);
					return XN_STATUS_OS_INI_READ_FAILED;
				}

				if (strcmp(csTempString, cpSection) == 0)
				{
					bIsInRequestedSection = true;
				}
			}

			// in any case, move to the next line
			XN_SKIP_LINE(pCurPos);
			continue;
		} // section

		// if we're not in the right section, we don't really care what's written in this line. Just skip it.
		if (!bIsInRequestedSection)
		{
			XN_SKIP_LINE(pCurPos);
			continue;
		}

		// regular line. check if this is a key (look for the '=' sign)
		XN_READ_TILL(pCurPos, *pCurPos == '=' || XN_IS_NEWLINE(pCurPos), csTempString, nTempStringLength);

		if (*pCurPos == '=') // we found a key
		{
			if (strcmp(csTempString, cpKey) == 0)
			{
				// we found our key. The value is the rest of the line
				pCurPos++;
				XN_READ_TILL(pCurPos, XN_IS_NEWLINE(pCurPos), cpDest, nTempStringLength);
				xnOSFree(csFileData);
				return XN_STATUS_OK;
			}
		}

		// if we got here, skip to the next line
		XN_SKIP_LINE(pCurPos);

	} // while loop

	xnOSFree(csFileData);
	return (XN_STATUS_OS_INI_READ_FAILED);
}

XN_C_API XnStatus xnOSReadStringFromINI(const char* cpINIFile, const char* cpSection, const char* cpKey, char* cpDest, const uint32_t nDestLength)
{
	XnStatus nRetVal = XN_STATUS_OK;
	bool bINIFileExists = false;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);
	XN_VALIDATE_OUTPUT_PTR(cpDest);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// find value
	char cpValueString[XN_INI_MAX_LEN];
	nRetVal = FindEntry(cpINIFile, cpSection, cpKey, cpValueString);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSStrCopy(cpDest, cpValueString, nDestLength);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSReadFloatFromINI(const char* cpINIFile, const char* cpSection, const char* cpKey, float* fDest)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(fDest);

	double dTemp;
	XnStatus nRetVal = xnOSReadDoubleFromINI(cpINIFile, cpSection, cpKey, &dTemp);
	XN_IS_STATUS_OK(nRetVal);

	*fDest = (float)dTemp;

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSReadDoubleFromINI(const char* cpINIFile, const char* cpSection, const char* cpKey, double* fDest)
{
	XnStatus nRetVal = XN_STATUS_OK;
	bool bINIFileExists = false;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);
	XN_VALIDATE_OUTPUT_PTR(fDest);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// find value
	char cpValueString[XN_INI_MAX_LEN];
	nRetVal = FindEntry(cpINIFile, cpSection, cpKey, cpValueString);
	XN_IS_STATUS_OK(nRetVal);

	*fDest = atof(cpValueString);

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSReadIntFromINI(const char* cpINIFile, const char* cpSection, const char* cpKey, int32_t* nDest)
{
	XnStatus nRetVal = XN_STATUS_OK;
	bool bINIFileExists = false;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSection);
	XN_VALIDATE_INPUT_PTR(cpKey);
	XN_VALIDATE_INPUT_PTR(cpINIFile);
	XN_VALIDATE_OUTPUT_PTR(nDest);

	// Make sure the INI file exists
	XN_VALIDATE_FILE_EXISTS_RET(cpINIFile, nRetVal, bINIFileExists, XN_STATUS_OS_INI_FILE_NOT_FOUND);

	// find value
	char cpValueString[XN_INI_MAX_LEN];
	nRetVal = FindEntry(cpINIFile, cpSection, cpKey, cpValueString);
	XN_IS_STATUS_OK(nRetVal);

	*nDest = atoi(cpValueString);

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSWriteStringToINI(const char* /*cpINIFile*/, const char* /*cpSection*/, const char* /*cpKey*/, const char* /*cpSrc*/)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

XN_C_API XnStatus xnOSWriteFloatToINI(const char* /*cpINIFile*/, const char* /*cpSection*/, const char* /*cpKey*/, const float /*fSrc*/)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

XN_C_API XnStatus xnOSWriteDoubleToINI(const char* /*cpINIFile*/, const char* /*cpSection*/, const char* /*cpKey*/, const double /*fSrc*/)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}

XN_C_API XnStatus xnOSWriteIntToINI(const char* /*cpINIFile*/, const char* /*cpSection*/, const char* /*cpKey*/, const int32_t /*nSrc*/)
{
	return XN_STATUS_OS_UNSUPPORTED_FUNCTION;
}
