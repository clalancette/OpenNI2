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
#include "XnLogConsoleWriter.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
void XnLogConsoleWriter::WriteEntry(const XnLogEntry* pEntry)
{
	printf("%9" PRIu64 " %-10s %s\n", pEntry->nTimestamp, pEntry->strSeverity, pEntry->strMessage);
}

void XnLogConsoleWriter::WriteUnformatted(const char* strMessage)
{
	printf("%s\n", strMessage);
}
