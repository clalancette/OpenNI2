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
#include "XnStreamCompression.h"
#include <XnStatus.h>
#include <XnLog.h>

#define XN_MASK_STREAM_COMPRESSION "xnStreamCompression"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus XnStreamCompressDepth16ZWithEmbTable(const uint16_t* pInput, const uint32_t nInputSize, XnUInt8* pOutput, uint32_t* pnOutputSize, uint16_t nMaxValue)
{
	// Local function variables
	const uint16_t* pInputEnd = pInput + (nInputSize / sizeof(uint16_t));
	const uint16_t* pOrigInput = pInput;
	const XnUInt8* pOrigOutput = pOutput;
	uint16_t nCurrValue = 0;
	uint16_t nLastValue = 0;
	uint16_t nAbsDiffValue = 0;
	XnInt16 nDiffValue = 0;
	XnUInt8 cOutStage = 0;
	XnUInt8 cOutChar = 0;
	XnUInt8 cZeroCounter = 0;
	static uint16_t nEmbTable[XN_MAX_UINT16];
	uint16_t nEmbTableIdx=0;

	// Note: this function does not make sure it stay within the output memory boundaries!

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);
	XN_VALIDATE_INPUT_PTR(pnOutputSize);

	// Create the embedded value translation table...
	pOutput+=2;
	xnOSMemSet(&nEmbTable[0], 0, nMaxValue*sizeof(uint16_t));

	while (pInput != pInputEnd)
	{
		nEmbTable[*pInput] = 1;
		pInput++;
	}

	for (uint32_t i=0; i<nMaxValue; i++)
	{
		if (nEmbTable[i] == 1)
		{
			nEmbTable[i] = nEmbTableIdx;
			nEmbTableIdx++;
			*(uint16_t*)pOutput = XN_PREPARE_VAR16_IN_BUFFER(uint16_t(i));
			pOutput+=2;
		}
	}

	*(uint16_t*)(pOrigOutput) = XN_PREPARE_VAR16_IN_BUFFER(nEmbTableIdx);

	// Encode the data...
	pInput = pOrigInput;
	nLastValue = nEmbTable[*pInput];
	*(uint16_t*)pOutput = XN_PREPARE_VAR16_IN_BUFFER(nLastValue);
	pInput++;
	pOutput+=2;

	while (pInput < pInputEnd)
	{
		nCurrValue = nEmbTable[*pInput];

		nDiffValue = (nLastValue - nCurrValue);
		nAbsDiffValue = (uint16_t)abs(nDiffValue);

		if (nAbsDiffValue <= 6)
		{
			nDiffValue += 6;

			if (cOutStage == 0)
			{
				cOutChar = (XnUInt8)(nDiffValue << 4);

				cOutStage = 1;
			}
			else
			{
				cOutChar += (XnUInt8)nDiffValue;

				if (cOutChar == 0x66)
				{
					cZeroCounter++;

					if (cZeroCounter == 15)
					{
						*pOutput = 0xEF;
						pOutput++;

						cZeroCounter = 0;
					}
				}
				else
				{
					if (cZeroCounter != 0)
					{
						*pOutput = 0xE0 + cZeroCounter;
						pOutput++;

						cZeroCounter = 0;
					}

					*pOutput = cOutChar;
					pOutput++;
				}

				cOutStage = 0;
			}
		}
		else
		{
			if (cZeroCounter != 0)
			{
				*pOutput = 0xE0 + cZeroCounter;
				pOutput++;

				cZeroCounter = 0;
			}

			if (cOutStage == 0)
			{
				cOutChar = 0xFF;
			}
			else
			{
				cOutChar += 0x0F;
				cOutStage = 0;
			}

			*pOutput = cOutChar;
			pOutput++;

			if (nAbsDiffValue <= 63)
			{
				nDiffValue += 192;

				*pOutput = (XnUInt8)nDiffValue;
				pOutput++;
			}
			else
			{
				*(uint16_t*)pOutput = XN_PREPARE_VAR16_IN_BUFFER((nCurrValue << 8) + (nCurrValue >> 8));
				pOutput+=2;
			}
		}

		nLastValue = nCurrValue;
		pInput++;
	}

	if (cOutStage != 0)
	{
		*pOutput = cOutChar + 0x0D;
		pOutput++;
	}

	if (cZeroCounter != 0)
	{
		*pOutput = 0xE0 + cZeroCounter;
		pOutput++;
	}

	*pnOutputSize = (uint32_t)(pOutput - pOrigOutput);

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamUncompressDepth16ZWithEmbTable(const XnUInt8* pInput, const uint32_t nInputSize, uint16_t* pOutput, uint32_t* pnOutputSize)
{
	// Local function variables
	const XnUInt8* pInputEnd = pInput + nInputSize;
	uint16_t* pOutputEnd = 0;
	uint16_t* pOrigOutput = pOutput;
	uint16_t nLastFullValue = 0;
	XnUInt8 cInput = 0;
	XnUInt8 cZeroCounter = 0;
	XnInt8 cInData1 = 0;
	XnInt8 cInData2 = 0;
	XnUInt8 cInData3 = 0;
	uint16_t* pEmbTable = NULL;
	uint16_t nEmbTableIdx = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);
	XN_VALIDATE_INPUT_PTR(pnOutputSize);

	if (nInputSize < sizeof(uint16_t))
	{
		xnLogError(XN_MASK_STREAM_COMPRESSION, "Input size too small");
		return (XN_STATUS_BAD_PARAM);
	}

	nEmbTableIdx = XN_PREPARE_VAR16_IN_BUFFER(*(uint16_t*)pInput);
	pInput+=2;
	pEmbTable = (uint16_t*)pInput;
	pInput+=nEmbTableIdx * 2;
	for (uint32_t i = 0; i < nEmbTableIdx; i++)
		pEmbTable[i] = XN_PREPARE_VAR16_IN_BUFFER(pEmbTable[i]);

	pOutputEnd = pOutput + (*pnOutputSize / sizeof(uint16_t));

	// Decode the data...
	nLastFullValue = XN_PREPARE_VAR16_IN_BUFFER(*(uint16_t*)pInput);
	*pOutput = pEmbTable[nLastFullValue];
	pInput+=2;
	pOutput++;

	while (pInput != pInputEnd)
	{
		cInput = *pInput;

		if (cInput < 0xE0)
		{
			cInData1 = cInput >> 4;
			cInData2 = (cInput & 0x0f);

			nLastFullValue -= (cInData1 - 6);
			XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
			*pOutput =  pEmbTable[nLastFullValue];
			pOutput++;

			if (cInData2 != 0x0f)
			{
				if (cInData2 != 0x0d)
				{
					nLastFullValue -= (cInData2 - 6);
					XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
					*pOutput =  pEmbTable[nLastFullValue];
					pOutput++;
				}

				pInput++;
			}
			else
			{
				pInput++;

				cInData3 = *pInput;
				if (cInData3 & 0x80)
				{
					nLastFullValue -= (cInData3 - 192);

					XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
					*pOutput =  pEmbTable[nLastFullValue];

					pOutput++;
					pInput++;
				}
				else
				{
					nLastFullValue = cInData3 << 8;
					pInput++;
					nLastFullValue += *pInput;

					XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
					*pOutput =  pEmbTable[nLastFullValue];

					pOutput++;
					pInput++;
				}
			}
		}
		else if (cInput == 0xFF)
		{
			pInput++;

			cInData3 = *pInput;

			if (cInData3 & 0x80)
			{
				nLastFullValue -= (cInData3 - 192);

				XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
				*pOutput =  pEmbTable[nLastFullValue];

				pInput++;
				pOutput++;
			}
			else
			{
				nLastFullValue = cInData3 << 8;
				pInput++;
				nLastFullValue += *pInput;

				XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
				*pOutput =  pEmbTable[nLastFullValue];

				pInput++;
				pOutput++;
			}
		}
		else //It must be 0xE?
		{
			cZeroCounter = cInput - 0xE0;

			while (cZeroCounter != 0)
			{
				XN_CHECK_OUTPUT_OVERFLOW(pOutput+1, pOutputEnd);
				*pOutput =  pEmbTable[nLastFullValue];
				pOutput++;

				*pOutput =  pEmbTable[nLastFullValue];
				pOutput++;

				cZeroCounter--;
			}

			pInput++;
		}
	}

	*pnOutputSize = (uint32_t)((pOutput - pOrigOutput) * sizeof(uint16_t));

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamCompressDepth16Z(const uint16_t* pInput, const uint32_t nInputSize, XnUInt8* pOutput, uint32_t* pnOutputSize)
{
	// Local function variables
	const uint16_t* pInputEnd = pInput + (nInputSize / sizeof(uint16_t));
	XnUInt8* pOrigOutput = pOutput;
	uint16_t nCurrValue = 0;
	uint16_t nLastValue = 0;
	uint16_t nAbsDiffValue = 0;
	XnInt16 nDiffValue = 0;
	XnUInt8 cOutStage = 0;
	XnUInt8 cOutChar = 0;
	XnUInt8 cZeroCounter = 0;

	// Note: this function does not make sure it stay within the output memory boundaries!

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);
	XN_VALIDATE_INPUT_PTR(pnOutputSize);

	if (nInputSize == 0)
	{
		*pnOutputSize = 0;
		return XN_STATUS_OK;
	}

	// Encode the data...
	nLastValue = *pInput;
	*(uint16_t*)pOutput = nLastValue;
	pInput++;
	pOutput+=2;

	while (pInput != pInputEnd)
	{
		nCurrValue = *pInput;

		nDiffValue = (nLastValue - nCurrValue);
		nAbsDiffValue = (uint16_t)abs(nDiffValue);

		if (nAbsDiffValue <= 6)
		{
			nDiffValue += 6;

			if (cOutStage == 0)
			{
				cOutChar = (XnUInt8)(nDiffValue << 4);

				cOutStage = 1;
			}
			else
			{
				cOutChar += (XnUInt8)nDiffValue;

				if (cOutChar == 0x66)
				{
					cZeroCounter++;

					if (cZeroCounter == 15)
					{
						*pOutput = 0xEF;
						pOutput++;

						cZeroCounter = 0;
					}
				}
				else
				{
					if (cZeroCounter != 0)
					{
						*pOutput = 0xE0 + cZeroCounter;
						pOutput++;

						cZeroCounter = 0;
					}

					*pOutput = cOutChar;
					pOutput++;
				}

				cOutStage = 0;
			}
		}
		else
		{
			if (cZeroCounter != 0)
			{
				*pOutput = 0xE0 + cZeroCounter;
				pOutput++;

				cZeroCounter = 0;
			}

			if (cOutStage == 0)
			{
				cOutChar = 0xFF;
			}
			else
			{
				cOutChar += 0x0F;
				cOutStage = 0;
			}

			*pOutput = cOutChar;
			pOutput++;

			if (nAbsDiffValue <= 63)
			{
				nDiffValue += 192;

				*pOutput = (XnUInt8)nDiffValue;
				pOutput++;
			}
			else
			{
				*(uint16_t*)pOutput = (nCurrValue << 8) + (nCurrValue >> 8);
				pOutput+=2;
			}
		}

		nLastValue = nCurrValue;
		pInput++;
	}

	if (cOutStage != 0)
	{
		*pOutput = cOutChar + 0x0D;
		pOutput++;
	}

	if (cZeroCounter != 0)
	{
		*pOutput = 0xE0 + cZeroCounter;
		pOutput++;
	}

	*pnOutputSize = (uint32_t)(pOutput - pOrigOutput);

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamUncompressDepth16Z(const XnUInt8* pInput, const uint32_t nInputSize, uint16_t* pOutput, uint32_t* pnOutputSize)
{
	// Local function variables
	const XnUInt8* pInputEnd = pInput + nInputSize;
	uint16_t* pOutputEnd = 0;
	const uint16_t* pOrigOutput = pOutput;
	uint16_t nLastFullValue = 0;
	XnUInt8 cInput = 0;
	XnUInt8 cZeroCounter = 0;
	XnInt8 cInData1 = 0;
	XnInt8 cInData2 = 0;
	XnUInt8 cInData3 = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);
	XN_VALIDATE_INPUT_PTR(pnOutputSize);

	if (nInputSize < sizeof(uint16_t))
	{
		xnLogError(XN_MASK_STREAM_COMPRESSION, "Input size too small");
		return (XN_STATUS_BAD_PARAM);
	}

	pOutputEnd = pOutput + (*pnOutputSize / sizeof(uint16_t));

	// Decode the data...
	nLastFullValue = *(uint16_t*)pInput;
	*pOutput = nLastFullValue;
	pInput+=2;
	pOutput++;

	while (pInput != pInputEnd)
	{
		cInput = *pInput;

		if (cInput < 0xE0)
		{
			cInData1 = cInput >> 4;
			cInData2 = (cInput & 0x0f);

			nLastFullValue -= (cInData1 - 6);
			XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
			*pOutput = nLastFullValue;
			pOutput++;

			if (cInData2 != 0x0f)
			{
				if (cInData2 != 0x0d)
				{
					nLastFullValue -= (cInData2 - 6);
					XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
					*pOutput = nLastFullValue;
					pOutput++;
				}

				pInput++;
			}
			else
			{
				pInput++;

				cInData3 = *pInput;
				if (cInData3 & 0x80)
				{
					nLastFullValue -= (cInData3 - 192);

					XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
					*pOutput = nLastFullValue;

					pOutput++;
					pInput++;
				}
				else
				{
					nLastFullValue = cInData3 << 8;
					pInput++;
					nLastFullValue += *pInput;

					XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
					*pOutput = nLastFullValue;

					pOutput++;
					pInput++;
				}
			}
		}
		else if (cInput == 0xFF)
		{
			pInput++;

			cInData3 = *pInput;

			if (cInData3 & 0x80)
			{
				nLastFullValue -= (cInData3 - 192);

				XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
				*pOutput = nLastFullValue;

				pInput++;
				pOutput++;
			}
			else
			{
				nLastFullValue = cInData3 << 8;
				pInput++;
				nLastFullValue += *pInput;

				XN_CHECK_OUTPUT_OVERFLOW(pOutput, pOutputEnd);
				*pOutput = nLastFullValue;

				pInput++;
				pOutput++;
			}
		}
		else //It must be 0xE?
		{
			cZeroCounter = cInput - 0xE0;

			while (cZeroCounter != 0)
			{
				XN_CHECK_OUTPUT_OVERFLOW(pOutput+1, pOutputEnd);
				*pOutput = nLastFullValue;
				pOutput++;

				*pOutput = nLastFullValue;
				pOutput++;

				cZeroCounter--;
			}

			pInput++;
		}
	}

	*pnOutputSize = (uint32_t)((pOutput - pOrigOutput) * sizeof(uint16_t));

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamCompressImage8Z(const XnUInt8* pInput, const uint32_t nInputSize, XnUInt8* pOutput, uint32_t* pnOutputSize)
{
	// Local function variables
	const XnUInt8* pInputEnd = pInput + nInputSize;
	const XnUInt8* pOrigOutput = pOutput;
	XnUInt8 nCurrValue = 0;
	XnUInt8 nLastValue = 0;
	XnUInt8 nAbsDiffValue = 0;
	XnInt8 nDiffValue = 0;
	XnUInt8 cOutStage = 0;
	XnUInt8 cOutChar = 0;
	XnUInt8 cZeroCounter = 0;
	XnBool bFlag = FALSE;

	// Note: this function does not make sure it stay within the output memory boundaries!

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);
	XN_VALIDATE_INPUT_PTR(pnOutputSize);

	// Encode the data...
	nLastValue = *pInput;
	*pOutput = nLastValue;
	pInput++;
	pOutput++;

	while (pInput != pInputEnd)
	{
		nCurrValue = *pInput;

		nDiffValue = (nLastValue - nCurrValue);
		nAbsDiffValue = (XnUInt8)abs(nDiffValue);

		if (nAbsDiffValue <= 6)
		{
			nDiffValue += 6;

			if (cOutStage == 0)
			{
				cOutChar = nDiffValue << 4;

				cOutStage = 1;
			}
			else
			{
				cOutChar += nDiffValue;

				if ((cOutChar == 0x66) && (bFlag == FALSE))
				{
					cZeroCounter++;

					if (cZeroCounter == 15)
					{
						*pOutput = 0xEF;
						pOutput++;

						cZeroCounter = 0;
					}
				}
				else
				{
					if (cZeroCounter != 0)
					{
						*pOutput = 0xE0 + cZeroCounter;
						pOutput++;

						cZeroCounter = 0;
					}

					*pOutput = cOutChar;
					pOutput++;

					bFlag = FALSE;
				}

				cOutStage = 0;
			}
		}
		else
		{
			if (cZeroCounter != 0)
			{
				*pOutput = 0xE0 + cZeroCounter;
				pOutput++;

				cZeroCounter = 0;
			}

			if (cOutStage == 0)
			{
				cOutChar = 0xF0;
				cOutChar += nCurrValue >> 4;

				*pOutput = cOutChar;
				pOutput++;

				cOutChar = (nCurrValue & 0xF) << 4;
				cOutStage = 1;

				bFlag = TRUE;
			}
			else
			{
				cOutChar += 0x0F;
				cOutStage = 0;

				*pOutput = cOutChar;
				pOutput++;

				*pOutput = nCurrValue;
				pOutput++;
			}
		}

		nLastValue = nCurrValue;
		pInput++;
	}

	if (cOutStage != 0)
	{
		*pOutput = cOutChar + 0x0D;
		pOutput++;
	}

	if (cZeroCounter != 0)
	{
		*pOutput = 0xE0 + cZeroCounter;
		pOutput++;
	}

	*pnOutputSize = (uint32_t)(pOutput - pOrigOutput);

	// All is good...
	return (XN_STATUS_OK);
}

XnStatus XnStreamUncompressImage8Z(const XnUInt8* pInput, const uint32_t nInputSize, XnUInt8* pOutput, uint32_t* pnOutputSize)
{
	const XnUInt8* pInputEnd = pInput + nInputSize;
	const XnUInt8* pOrigOutput = pOutput;
	XnUInt8 nLastFullValue = 0;
	XnUInt8 cInput = 0;
	XnUInt8 cZeroCounter = 0;
	XnInt8 cInData1 = 0;
	XnInt8 cInData2 = 0;

	// Note: this function does not make sure it stay within the output memory boundaries!

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pInput);
	XN_VALIDATE_INPUT_PTR(pOutput);
	XN_VALIDATE_INPUT_PTR(pnOutputSize);

	if (nInputSize < sizeof(XnUInt8))
	{
		xnLogError(XN_MASK_STREAM_COMPRESSION, "Input size too small");
		return (XN_STATUS_BAD_PARAM);
	}

	// Decode the data...
	nLastFullValue = *pInput;
	*pOutput = nLastFullValue;
	pInput++;
	pOutput++;

	while (pInput != pInputEnd)
	{
		cInput = *pInput;

		if (cInput < 0xE0)
		{
			cInData1 = cInput >> 4;
			cInData2 = (cInput & 0x0f);

			nLastFullValue -= (cInData1 - 6);
			*pOutput = nLastFullValue;
			pOutput++;

			if (cInData2 != 0x0f)
			{
				if (cInData2 != 0x0d)
				{
					nLastFullValue -= (cInData2 - 6);
					*pOutput = nLastFullValue;
					pOutput++;
				}
			}
			else
			{
				pInput++;
				nLastFullValue = *pInput;
				*pOutput = nLastFullValue;
				pOutput++;
			}

			pInput++;
		}
		else if (cInput >= 0xF0)
		{
			cInData1 = cInput << 4;

			pInput++;
			cInput = *pInput;

			nLastFullValue = cInData1 + (cInput >> 4);

			*pOutput = nLastFullValue;
			pOutput++;

			cInData2 = cInput & 0xF;

			if (cInData2 == 0x0F)
			{
				pInput++;
				nLastFullValue = *pInput;
				*pOutput = nLastFullValue;
				pOutput++;
				pInput++;
			}
			else
			{
				if (cInData2 != 0x0D)
				{
					nLastFullValue -= (cInData2 - 6);
					*pOutput = nLastFullValue;
					pOutput++;
				}

				pInput++;
			}
		}
		else //It must be 0xE?
		{
			cZeroCounter = cInput - 0xE0;

			while (cZeroCounter != 0)
			{
				*pOutput = nLastFullValue;
				pOutput++;

				*pOutput = nLastFullValue;
				pOutput++;

				cZeroCounter--;
			}

			pInput++;
		}
	}

	*pnOutputSize = (uint32_t)(pOutput - pOrigOutput);

	// All is good...
	return (XN_STATUS_OK);
}
