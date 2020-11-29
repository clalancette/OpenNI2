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
#ifndef _XN_DUMP_H_
#define _XN_DUMP_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnPlatform.h"
#include "XnStatus.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
struct XnDumpFile;
typedef struct XnDumpFile XnDumpFile;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * Configures if a specific dump mask is enabled.
 *
 * @param	strMask		[in]	The mask to set.
 * @param	bEnabled	[in]	true to enable this dump, false otherwise.
 */
XN_C_API XnStatus XN_C_DECL xnDumpSetMaskState(const char* strMask, bool bEnabled);

/**
 * This function checks if a dump mask is enabled
 *
 * @param	strDumpMask	[in]	The mask that should be checked.
 */
XN_C_API bool XN_C_DECL xnLogIsDumpMaskEnabled(const char* strDumpMask);

/**
 * Opens a file for writing dump.
 *
 * @param	strDumpName		[in]	Name of the dump mask this file belongs to.
 * @param	strNameFormat	[in]	A format string for the name of the file.
 *
 * @returns a file handle for writing data. The file should be closed using @ref xnDumpFileClose().
 */
XN_C_API XnDumpFile* XN_C_DECL xnDumpFileOpen(const char* strDumpName, const char* strNameFormat, ...);

/**
 * Opens a file for writing dump using some advanced options.
 *
 * You would usually prefer to use @ref xnDumpFileOpen().
 *
 * @param	strDumpName		[in]	Name of the dump mask this file belongs to.
 * @param	bForce			[in]	When true, file will be created even if dump is currently off.
 * @param	bSessionDump	[in]	When true, file will be created with current session timestamp as a prefix to its name.
 * @param	strNameFormat	[in]	A format string for the name of the file.
 *
 * @returns a file handle for writing data. The file should be closed using @ref xnDumpFileClose().
 */
XN_C_API XnDumpFile* XN_C_DECL xnDumpFileOpenEx(const char* strDumpName, bool bForce, bool bSessionDump, const char* strNameFormat, ...);

/**
 * Writes a buffer to a dump file.
 *
 * @param	pFile			[in]	Dump file to write to. A pointer retrieved from @ref xnDumpFileOpen.
 * @param	pBuffer			[in]	Data to be written to file.
 * @param	nBufferSize		[in]	Size of the buffer.
 */
XN_C_API void XN_C_DECL _xnDumpFileWriteBuffer(XnDumpFile* pFile, const void* pBuffer, uint32_t nBufferSize);

/**
 * Writes a formatted string to a dump file.
 *
 * @param	pFile		[in]	Dump file to write to. A pointer retrieved from @ref xnDumpFileOpen.
 * @param	strFormat	[in]	Format string.
 *
 * NOTE: the total length of the string must not exceed 8 KB. If it does, it will be truncated.
 */
XN_C_API void XN_C_DECL _xnDumpFileWriteString(XnDumpFile* pFile, const char* strFormat, ...);

/**
 * Closes a dump file.
 *
 * @param	hFile		[in]	Dump file to close. A pointer retrieved from @ref xnDumpFileOpen.
 */
XN_C_API void XN_C_DECL _xnDumpFileClose(XnDumpFile* pFile);

#define xnDumpFileWriteBuffer(pFile, pBuffer, nBufferSize)		\
	if ((pFile) != NULL)										\
	{															\
		_xnDumpFileWriteBuffer(pFile, pBuffer, nBufferSize);	\
	}															\

#define xnDumpFileClose(pFile)									\
	if ((pFile) != NULL)										\
	{															\
		_xnDumpFileClose(pFile);								\
		pFile = NULL;											\
	}															\

#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	#define xnDumpFileWriteString(pFile, strFormat, ...)			\
		if ((pFile) != NULL)										\
		{															\
			_xnDumpFileWriteString(pFile, strFormat, __VA_ARGS__);	\
		}
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define xnDumpFileWriteString(pFile, strFormat, ...)			\
		if ((pFile) != NULL)										\
		{															\
			_xnDumpFileWriteString(pFile, strFormat, ##__VA_ARGS__);\
		}
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_NO_VAARGS
	#define xnDumpFileWriteString(pFile, strFormat, arg)			\
		if ((pFile) != NULL)										\
		{															\
			_xnDumpFileWriteString(pFile, strFormat,arg);			\
		}
#else
	#error Xiron Log - Unknown VAARGS type!
#endif


//---------------------------------------------------------------------------
// Backwards Compatibility Stuff
//---------------------------------------------------------------------------

#ifndef __XN_NO_BC__

#include "XnOS.h"

typedef struct XnDump
{
	XN_FILE_HANDLE hFile;
} XnDump;

const XnDump XN_DUMP_CLOSED = { XN_INVALID_FILE_HANDLE };

XN_C_API void XN_API_DEPRECATED("Use xnDumpFileX methods instead") XN_C_DECL xnDumpInit(XnDump* pDump, const char* csDumpMask, const char* csHeader, const char* csFileNameFormat, ...);
XN_C_API void XN_API_DEPRECATED("Use xnDumpFileX methods instead") XN_C_DECL xnDumpForceInit(XnDump* pDump, const char* csHeader, const char* csFileNameFormat, ...);
XN_C_API void XN_API_DEPRECATED("Use xnDumpFileX methods instead") XN_C_DECL xnDumpClose(XnDump* pDump);
XN_C_API void XN_API_DEPRECATED("Use xnDumpFileX methods instead") XN_C_DECL xnDumpWriteBufferImpl(XnDump dump, const void* pBuffer, uint32_t nBufferSize);
XN_C_API void XN_API_DEPRECATED("Use xnDumpFileX methods instead") XN_C_DECL xnDumpWriteStringImpl(XnDump dump, const char* csFormat, ...);
XN_C_API void XN_API_DEPRECATED("Use xnDumpFileX methods instead") XN_C_DECL xnDumpFlush(XnDump dump);

#define xnDumpWriteBuffer(dump, pBuffer, nBufferSize)		\
	if (dump.hFile != XN_INVALID_FILE_HANDLE)				\
	{														\
		xnDumpWriteBufferImpl(dump, pBuffer, nBufferSize);	\
	}

#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	#define xnDumpWriteString(dump, csFormat, ...)						\
		if ((dump).hFile != XN_INVALID_FILE_HANDLE) {					\
			xnDumpWriteStringImpl((dump), csFormat, __VA_ARGS__);		\
		}
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define xnDumpWriteString(dump, csFormat, ...)						\
		if ((dump).hFile != XN_INVALID_FILE_HANDLE) {					\
			xnDumpWriteStringImpl((dump), csFormat, ##__VA_ARGS__);		\
		}
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_NO_VAARGS
	#define xnDumpWriteString(dump, csFormat, arg)						\
		if ((dump).hFile != XN_INVALID_FILE_HANDLE) {					\
			xnDumpWriteStringImpl((dump), csFormat, arg);				\
		}
#else
	#error Xiron Log - Unknown VAARGS type!
#endif

#endif // #ifndef __XN_NO_BC__

#endif // _XN_DUMP_H_
