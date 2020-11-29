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
#ifndef XNIOFILESTREAM_H
#define XNIOFILESTREAM_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnIOStream.h>
#include <XnOS.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnIOFileStream final : public XnIOStream
{
public:
	XnIOFileStream(const char* pcsFileName, uint32_t nFlags);
	~XnIOFileStream() { Free(); }

	XnStatus WriteData(const unsigned char* pData, uint32_t nDataSize) override;
	XnStatus ReadData(unsigned char* pData, uint32_t nDataSize) override;
	XnStatus Init() override;
	XnStatus Free() override;

	XnStatus Tell(uint64_t* pnOffset);
	XnStatus Seek(uint64_t nOffset);

private:
	const char* m_pcsFileName;
	uint32_t m_nFlags;
	XN_FILE_HANDLE m_hFile;
};

#endif // XNIOFILESTREAM_H
