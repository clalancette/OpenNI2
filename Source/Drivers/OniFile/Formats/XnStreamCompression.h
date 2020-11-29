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
#ifndef XNSTREAMCOMPRESSION_H
#define XNSTREAMCOMPRESSION_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>

//---------------------------------------------------------------------------
// Functions Declaration
//---------------------------------------------------------------------------
XnStatus XnStreamCompressDepth16ZWithEmbTable(const uint16_t* pInput, const uint32_t nInputSize, XnUInt8* pOutput, uint32_t* pnOutputSize, uint16_t nMaxValue);

XnStatus XnStreamUncompressDepth16ZWithEmbTable(const XnUInt8* pInput, const uint32_t nInputSize, uint16_t* pOutput, uint32_t* pnOutputSize);

XnStatus XnStreamCompressDepth16Z(const uint16_t* pInput, const uint32_t nInputSize, XnUInt8* pOutput, uint32_t* pnOutputSize);

XnStatus XnStreamUncompressDepth16Z(const XnUInt8* pInput, const uint32_t nInputSize, uint16_t* pOutput, uint32_t* pnOutputSize);

XnStatus XnStreamCompressImage8Z(const XnUInt8* pInput, const uint32_t nInputSize, XnUInt8* pOutput, uint32_t* pnOutputSize);

XnStatus XnStreamUncompressImage8Z(const XnUInt8* pInput, const uint32_t nInputSize, XnUInt8* pOutput, uint32_t* pnOutputSize);

#endif // XNSTREAMCOMPRESSION_H
