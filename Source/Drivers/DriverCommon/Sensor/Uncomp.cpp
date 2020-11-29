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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "Uncomp.h"
#include <XnProfiling.h>
#include <XnFormatsStatus.h>

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
#define XN_CHECK_UNC_IMAGE_OUTPUT(x, y) 				\
		if (x > y)										\
		{												\
			return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);	\
		}

#define XN_IMAGE_OUTPUT(pOutput, pOutputEnd, nValue)	\
	XN_CHECK_UNC_IMAGE_OUTPUT(pOutput, pOutputEnd)		\
	*pOutput = nValue;									\
	++pOutput;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnStreamUncompressYUVImagePS(const uint8_t* pInput, const uint32_t nInputSize,
										  uint8_t* pOutput, uint32_t* pnOutputSize, uint16_t nLineSize,
										  uint32_t* pnActualRead, XnBool bLastPart)
{
	// Input is made of 4-bit elements.
	const uint8_t* pInputOrig = pInput;
	const uint8_t* pInputEnd = pInput + nInputSize;
	uint8_t* pOrigOutput = pOutput;
	uint8_t* pOutputEnd = pOutput + (*pnOutputSize);
	uint8_t nLastFullValue[4] = {0};

	// NOTE: we use variables of type uint32 instead of uint8 as an optimization (better CPU usage)
	uint32_t nTempValue = 0;
	uint32_t cInput = 0;
	XnBool bReadByte = TRUE;

	if (nInputSize < sizeof(uint8_t))
	{
		printf("Buffer too small!\n");
		return (XN_STATUS_IO_COMPRESSED_BUFFER_TOO_SMALL);
	}

	const uint8_t* pInputLastPossibleStop = pInputOrig;
	uint8_t* pOutputLastPossibleStop = pOrigOutput;

	*pnActualRead = 0;
	*pnOutputSize = 0;

	uint32_t nChannel = 0;
	uint32_t nCurLineSize = 0;

	while (pInput < pInputEnd)
	{
		cInput = *pInput;

		if (bReadByte)
		{
			bReadByte = FALSE;

			if (cInput < 0xd0) // 0x0 to 0xc are diffs
			{
				// take high_element only
				// diffs are between -6 and 6 (0x0 to 0xc)
				nLastFullValue[nChannel] += XnInt8((cInput >> 4) - 6);
			}
			else if (cInput < 0xe0) // 0xd is dummy
			{
				// Do nothing
				continue;
			}
			else // 0xe is not used, so this must be 0xf - full
			{
				// take two more elements
				nTempValue = (cInput & 0x0f) << 4;

				if (++pInput == pInputEnd)
					break;

				nTempValue += (*pInput >> 4);
				nLastFullValue[nChannel] = (uint8_t)nTempValue;
			}
		}
		else
		{
			// take low-element
			cInput &= 0x0f;
			bReadByte = TRUE;
			pInput++;

			if (cInput < 0xd) // 0x0 to 0xc are diffs
			{
				// diffs are between -6 and 6 (0x0 to 0xc)
				nLastFullValue[nChannel] += (XnInt8)(cInput - 6);
			}
			else if (cInput < 0xe) // 0xd is dummy
			{
				// Do nothing
				continue;
			}
			else // 0xe is not in use, so this must be 0xf - full
			{
				if (pInput == pInputEnd)
					break;

				// take two more elements
				nLastFullValue[nChannel] = *pInput;
				pInput++;
			}
		}

		// write output
		if (pOutput >= pOutputEnd)
		{
			return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);
		}

		*pOutput = nLastFullValue[nChannel];
		pOutput++;

		nChannel++;
		switch (nChannel)
		{
		case 2:
			nLastFullValue[3] = nLastFullValue[1];
			break;
		case 4:
			nLastFullValue[1] = nLastFullValue[3];
			nChannel = 0;
			break;
		}

		nCurLineSize++;
		if (nCurLineSize == nLineSize)
		{
			pInputLastPossibleStop = pInput;
			pOutputLastPossibleStop = pOutput;

			nLastFullValue[0] = nLastFullValue[1] = nLastFullValue[2] = nLastFullValue[3] = 0;
			nCurLineSize = 0;
		}
	}

	if (bLastPart == TRUE)
	{
		*pnOutputSize = (uint32_t)(pOutput - pOrigOutput) * sizeof(uint8_t);
		*pnActualRead += (uint32_t)(pInput - pInputOrig) * sizeof(uint8_t);
	}
	else if ((pOutputLastPossibleStop != pOrigOutput) && (pInputLastPossibleStop != pInputOrig))
	{
		*pnOutputSize = (uint32_t)(pOutputLastPossibleStop - pOrigOutput) * sizeof(uint8_t);
		*pnActualRead += (uint32_t)(pInputLastPossibleStop - pInputOrig) * sizeof(uint8_t);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamUncompressImageNew(const uint8_t* pInput, const uint32_t nInputSize,
									uint8_t* pOutput, uint32_t* pnOutputSize, uint16_t nLineSize,
									uint32_t* pnActualRead, XnBool bLastPart)
{
	// Input is made of 4-bit elements.
	const uint8_t* pInputOrig = pInput;
	const uint8_t* pInputEnd = pInput + nInputSize;
	uint8_t* pOrigOutput = pOutput;
	uint8_t* pOutputEnd = pOutput + (*pnOutputSize);
	uint8_t nLastFullValue[4] = {0};

	// NOTE: we use variables of type uint32 instead of uint8 as an optimization (better CPU usage)
	uint32_t nTempValue = 0;
	uint32_t cInput = 0;
	XnBool bReadByte = TRUE;

	if (nInputSize < sizeof(uint8_t))
	{
		printf("Buffer too small!\n");
		return (XN_STATUS_IO_COMPRESSED_BUFFER_TOO_SMALL);
	}

	const uint8_t* pInputLastPossibleStop = pInputOrig;
	uint8_t* pOutputLastPossibleStop = pOrigOutput;

	*pnActualRead = 0;
	*pnOutputSize = 0;

	uint32_t nChannel = 0;
	uint32_t nCurLineSize = 0;

	while (pInput < pInputEnd)
	{
		cInput = *pInput;

		if (bReadByte)
		{
			bReadByte = FALSE;

			if (cInput < 0xd0) // 0x0 to 0xc are diffs
			{
				// take high_element only
				// diffs are between -6 and 6 (0x0 to 0xc)
				nLastFullValue[nChannel] += (XnInt8)((cInput >> 4) - 6);
			}
			else if (cInput < 0xe0) // 0xd is dummy
			{
				// Do nothing
				continue;
			}
			else // 0xe is not used, so this must be 0xf - full
			{
				// take two more elements
				nTempValue = (cInput & 0x0f) << 4;

				if (++pInput == pInputEnd)
					break;

				nTempValue += (*pInput >> 4);
				nLastFullValue[nChannel] = (uint8_t)nTempValue;
			}
		}
		else
		{
			// take low-element
			cInput &= 0x0f;
			bReadByte = TRUE;
			pInput++;

			if (cInput < 0xd) // 0x0 to 0xc are diffs
			{
				// diffs are between -6 and 6 (0x0 to 0xc)
				nLastFullValue[nChannel] += (XnInt8)(cInput - 6);
			}
			else if (cInput < 0xe) // 0xd is dummy
			{
				// Do nothing
				continue;
			}
			else // 0xe is not in use, so this must be 0xf - full
			{
				if (pInput == pInputEnd)
					break;

				// take two more elements
				nLastFullValue[nChannel] = *pInput;
				pInput++;
			}
		}

		// write output
		if (pOutput >= pOutputEnd)
		{
			return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);
		}

		*pOutput = nLastFullValue[nChannel];
		pOutput++;

		nChannel++;
		switch (nChannel)
		{
		case 2:
			nChannel = 0;
			break;
		}

		nCurLineSize++;
		if (nCurLineSize == nLineSize)
		{
			pInputLastPossibleStop = pInput;
			pOutputLastPossibleStop = pOutput;

			nLastFullValue[0] = nLastFullValue[1] = nLastFullValue[2] = nLastFullValue[3] = 0;
			nCurLineSize = 0;
		}
	}

	if (bLastPart == TRUE)
	{
		*pnOutputSize = (uint32_t)(pOutput - pOrigOutput) * sizeof(uint8_t);
		*pnActualRead += (uint32_t)(pInput - pInputOrig) * sizeof(uint8_t);
	}
	else if ((pOutputLastPossibleStop != pOrigOutput) && (pInputLastPossibleStop != pInputOrig))
	{
		*pnOutputSize = (uint32_t)(pOutputLastPossibleStop - pOrigOutput) * sizeof(uint8_t);
		*pnActualRead += (uint32_t)(pInputLastPossibleStop - pInputOrig) * sizeof(uint8_t);
	}

	// All is good...
	return (XN_STATUS_OK);
}
