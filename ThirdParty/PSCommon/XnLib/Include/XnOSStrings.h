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
#ifndef _XN_OS_STRINGS_H_
#define _XN_OS_STRINGS_H_

#include "XnPlatform.h"
#include <stdarg.h>

// Strings
XN_C_API XnStatus XN_C_DECL xnOSStrPrefix(const char* cpPrefixString, char* cpDestString, const uint32_t nDestLength);
XN_C_API XnStatus XN_C_DECL xnOSStrAppend(char* cpDestString, const char* cpSrcString, const uint32_t nDestLength);
XN_C_API XnStatus XN_C_DECL xnOSStrCopy(char* cpDestString, const char* cpSrcString, const uint32_t nDestLength);
XN_C_API uint32_t XN_C_DECL xnOSStrLen(const char* cpStr);
XN_C_API XnStatus XN_C_DECL xnOSStrNCopy(char* cpDestString, const char* cpSrcString, const uint32_t nCopyLength, const uint32_t nDestLength);
XN_C_API XnStatus XN_C_DECL xnOSStrCRC32(const char* cpString, uint32_t* nCRC32);
XN_C_API XnStatus XN_C_DECL xnOSStrNCRC32(XnUChar* cpBuffer, uint32_t nBufferSize, uint32_t* nCRC32);
XN_C_API XnStatus XN_C_DECL xnOSStrFormat(char* cpDestString, const uint32_t nDestLength, uint32_t* pnCharsWritten, const char* cpFormat, ...);
XN_C_API XnStatus XN_C_DECL xnOSStrFormatV(char* cpDestString, const uint32_t nDestLength, uint32_t* pnCharsWritten, const char* cpFormat, va_list args);
XN_C_API int32_t  XN_C_DECL xnOSStrCmp(const char* cpFirstString, const char* cpSecondString);
XN_C_API int32_t  XN_C_DECL xnOSStrCaseCmp(const char* cpFirstString, const char* cpSecondString);
XN_C_API void     XN_C_DECL xnOSItoA(int32_t nValue, char* cpStr, int32_t nBase);
/** Should be freed using @ref xnOSFree() */
XN_C_API char* XN_C_DECL xnOSStrDup(const char* strSource);
XN_C_API XnStatus XN_C_DECL xnOSGetEnvironmentVariable(const char* strEnv, char* strDest, uint32_t nDestSize);
XN_C_API XnStatus XN_C_DECL xnOSExpandEnvironmentStrings(const char* strSrc, char* strDest, uint32_t nDestSize);


#endif // _XN_OS_STRINGS_H_
