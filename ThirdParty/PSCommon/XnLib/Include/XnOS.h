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
#ifndef _XN_OS_H_
#define _XN_OS_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnPlatform.h"
#include "XnMacros.h"
#include "XnStatusCodes.h"
#include "XnOSStrings.h"
#include "XnMemory.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_MASK_OS "xnOS"

// uncomment next line to activate memory profiling
//#define XN_MEM_PROFILING

//---------------------------------------------------------------------------
// OS Identifier
//---------------------------------------------------------------------------
#if (XN_PLATFORM == XN_PLATFORM_WIN32)
	#include "Win32/XnOSWin32.h"
#elif (XN_PLATFORM == XN_PLATFORM_LINUX_X86 || XN_PLATFORM == XN_PLATFORM_LINUX_ARM)
	#include "Linux-x86/XnOSLinux-x86.h"
#elif (XN_PLATFORM == XN_PLATFORM_MACOSX)
        #include "MacOSX/XnOSMacOSX.h"
	#error OpenNI OS Abstraction Layer - Unsupported Platform!
#endif

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
#define XN_MAX_OS_NAME_LENGTH 255

typedef bool (XN_CALLBACK_TYPE* XnConditionFunc)(void* pConditionData);

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
/** The time since Xiron Core was initialized */
extern XnOSTimer g_xnOSHighResGlobalTimer;

//---------------------------------------------------------------------------
// Files
//---------------------------------------------------------------------------
// File open modes
/** Open the file for reading. */
#define XN_OS_FILE_READ				0x01
/** Open the file for writing. */
#define XN_OS_FILE_WRITE			0x02
/** Create a new file (but only if it doesn't exist). */
#define XN_OS_FILE_CREATE_NEW_ONLY	0x04
/** Truncate the file if it already exists. */
#define XN_OS_FILE_TRUNCATE			0x08
/** File be opened in append mode */
#define XN_OS_FILE_APPEND			0x10
/** All writes will be immediately written to disk */
#define XN_OS_FILE_AUTO_FLUSH		0x20

// Seek types
/** The seek type enum list. */
typedef enum {
	/** Absolute seek from the file beginning. */
	XN_OS_SEEK_SET = 0,
	/** Relative seek from the current location. */
	XN_OS_SEEK_CUR,
	/** Absolute seek from the file ending. */
	XN_OS_SEEK_END
} XnOSSeekType;

//---------------------------------------------------------------------------
// Network
//---------------------------------------------------------------------------
// Network socket type
/** The network socket type. */
typedef enum {
	/** UDP socket. */
	XN_OS_UDP_SOCKET = 0,
	/** TCP socket. */
	XN_OS_TCP_SOCKET
} XnOSSocketType;

#define XN_OS_NETWORK_LOCAL_HOST	"127.0.0.1"

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
// Memory
/** Validate that the input pointer X is not NULL. */
#define XN_VALIDATE_INPUT_PTR(x) XN_VALIDATE_PTR(x, XN_STATUS_NULL_INPUT_PTR)

/** Validate that the output pointer X is not NULL. */
#define XN_VALIDATE_OUTPUT_PTR(x) XN_VALIDATE_PTR(x, XN_STATUS_NULL_OUTPUT_PTR)
/** Validate that a X pointer after a memory allocation call is not NULL. */
#define XN_VALIDATE_ALLOC_PTR(x) XN_VALIDATE_PTR(x, XN_STATUS_ALLOC_FAILED)

/** Allocate Y bytes into the X pointer and make sure NULL wasn't returned. */
#define XN_VALIDATE_ALLOC(x,y)			\
		x = (y*)xnOSMalloc(sizeof(y));	\
		XN_VALIDATE_ALLOC_PTR(x);

/** Allocate Z elements of Y type into the X pointer and make sure NULL wasn't returned. */
#define XN_VALIDATE_CALLOC(x,y,z)			\
		x = (y*)xnOSCalloc(z, sizeof(y));	\
		XN_VALIDATE_ALLOC_PTR(x);

/** Allocate Y aligned bytes into the X pointer and make sure NULL wasn't returned. */
#define XN_VALIDATE_ALIGNED_ALLOC(x,y,w)			\
		x = (y*)xnOSMallocAligned(sizeof(y), w);	\
		XN_VALIDATE_ALLOC_PTR(x);

/** Allocate Z aligned elements of Y type into the X pointer and make sure NULL wasn't returned. */
#define XN_VALIDATE_ALIGNED_CALLOC(x,y,z,w)			\
		x = (y*)xnOSCallocAligned(z, sizeof(y), w);	\
		XN_VALIDATE_ALLOC_PTR(x);

/** Validate that the memory free request succeeded and set X to NULL. */
#define XN_FREE_AND_NULL(x)		\
		if (x != NULL)			\
		{						\
			xnOSFree(x);		\
			x = NULL;			\
		}
/** Validate that the aligned memory free request succeeded and set X to NULL. */
#define XN_ALIGNED_FREE_AND_NULL(x)		\
		if (x != NULL)			\
		{						\
			xnOSFreeAligned(x);	\
			x = NULL;			\
		}


/** Creates a new type object and validates that allocation succeeded. */
#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	#define XN_VALIDATE_NEW(ptr, type, ...)						\
		{														\
			(ptr) = XN_NEW(type, __VA_ARGS__);					\
			if ((ptr) == NULL)									\
			{													\
				return (XN_STATUS_ALLOC_FAILED);				\
			}													\
		}

#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define XN_VALIDATE_NEW(ptr, type, ...)						\
		{														\
			(ptr) = XN_NEW(type, ##__VA_ARGS__);				\
			if ((ptr) == NULL)									\
			{													\
				return (XN_STATUS_ALLOC_FAILED);				\
			}													\
		}
#else
	#define XN_VALIDATE_NEW(ptr, type)							\
		{														\
			(ptr) = XN_NEW(type);								\
			if ((ptr) == NULL)									\
			{													\
				return (XN_STATUS_ALLOC_FAILED);				\
			}													\
		}
#endif

/** Creates a new type object, validates that allocation succeeded, and initializes the object (type must have an Init function). */
#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	#define XN_VALIDATE_NEW_AND_INIT(ptr, type, ...)			\
		{														\
			XN_VALIDATE_NEW(ptr, type, __VA_ARGS__);			\
			XnStatus rc = (ptr)->Init();						\
			if (rc != XN_STATUS_OK)								\
			{													\
				XN_DELETE(ptr);									\
				return (rc);									\
			}													\
		}
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define XN_VALIDATE_NEW_AND_INIT(ptr, type, ...)			\
		{														\
			XN_VALIDATE_NEW(ptr, type, ##__VA_ARGS__);			\
			XnStatus rc = (ptr)->Init();						\
			if (rc != XN_STATUS_OK)								\
			{													\
				XN_DELETE(ptr);									\
				return (rc);									\
			}													\
		}
#else
	#define XN_VALIDATE_NEW_AND_INIT(ptr, type)					\
		{														\
			XN_VALIDATE_NEW(ptr, type);							\
			XnStatus rc = (ptr)->Init();						\
			if (rc != XN_STATUS_OK)								\
			{													\
				XN_DELETE(ptr);									\
				return (rc);									\
			}													\
		}
#endif

// Strings
/** Append x into y (with z as the max size) and check the status via z. */
#define XN_VALIDATE_STR_APPEND(w,x,y,z)	\
	z = xnOSStrAppend(w, x, y);			\
	XN_IS_STATUS_OK(z);

/** Prefix x into y (with z as the max size) and check the status via z. */
#define XN_VALIDATE_STR_PREFIX(w,x,y,z)	\
	z = xnOSStrPrefix(w, x, y);			\
	XN_IS_STATUS_OK(z);

#define XN_VALIDATE_STR_COPY(w,x,y,z)	\
	z = xnOSStrCopy(w, x, y);			\
	XN_IS_STATUS_OK(z);

#define XN_VALIDATE_STRN_COPY(v,w,x,y,z)	\
	z = xnOSStrNCopy(v, w, x, y);			\
	XN_IS_STATUS_OK(z);

// INI
/** Read a string from the INI file and check the status via z. */
#define XN_VALIDATE_READ_INI_STR(u,v,w,x,y,z)		\
		z = xnOSReadStringFromINI(u, v, w, x, y);	\
		XN_IS_STATUS_OK(z);

/** Read an int from the INI file and check the status via z. */
#define XN_VALIDATE_READ_INI_INT(v,w,x,y,z)		\
		z = xnOSReadIntFromINI(v, w, x, y);		\
		XN_IS_STATUS_OK(z);

/** Read a float from the INI file and check the status via z. */
#define XN_VALIDATE_READ_INI_FLOAT(v,w,x,y,z)	\
		z = xnOSReadFloatFromINI(v, w, x, y);	\
		XN_IS_STATUS_OK(z);

/** Read a double from the INI file and check the status via z. */
#define XN_VALIDATE_READ_INI_DOUBLE(v,w,x,y,z)	\
		z = xnOSReadDoubleFromINI(v, w, x, y);	\
		XN_IS_STATUS_OK(z);

// Mutex
/** Lock the mutex x for a y period of time and check the status via z. */
#define XN_VALIDATE_LOCK_MUTEX(x,y,z)	\
		z = xnOSLockMutex(x, y);		\
		XN_IS_STATUS_OK(z);

/** UnLock the mutex x and check the status via z. */
#define XN_VALIDATE_UNLOCK_MUTEX(x,z)	\
		z = xnOSUnLockMutex(x);			\
		XN_IS_STATUS_OK(z);

// Files
/** Returns XN_STATUS_OS_FILE_NOT_FOUND if the file x doesn't exists. */
#define XN_VALIDATE_FILE_EXISTS_RET(x,y,z,w)	\
		y = xnOSDoesFileExist(x, &z);				\
		XN_IS_STATUS_OK(y);						\
		if (z == false)							\
		{										\
			return (w);							\
		}
#define XN_VALIDATE_FILE_EXISTS(x,y,z)	\
		XN_VALIDATE_FILE_EXISTS_RET(x,y,z,XN_STATUS_OS_FILE_NOT_FOUND)

//---------------------------------------------------------------------------
// Exported Function Declaration
//---------------------------------------------------------------------------
// Common
XN_C_API XnStatus XN_C_DECL xnOSGetInfo(xnOSInfo* pOSInfo);


#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	#define XN_NEW(type, ...)		new type(__VA_ARGS__)
#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define XN_NEW(type, ...)		new type(__VA_ARGS__)
#else
	#define XN_NEW(type, arg)		new type(arg)
#endif

#define XN_NEW_ARR(type, count)		new type[count]
#define XN_DELETE(p)				delete (p)
#define XN_DELETE_ARR(p)			delete[] (p)

typedef enum
{
	XN_ALLOCATION_MALLOC,
	XN_ALLOCATION_MALLOC_ALIGNED,
	XN_ALLOCATION_CALLOC,
	XN_ALLOCATION_CALLOC_ALIGNED,
	XN_ALLOCATION_NEW,
	XN_ALLOCATION_NEW_ARRAY
} XnAllocationType;

/**
* Memory Profiling - Logs an allocation of memory.
*/
XN_C_API void* XN_C_DECL xnOSLogMemAlloc(void* pMemBlock, XnAllocationType nAllocType, uint32_t nBytes, const char* csFunction, const char* csFile, uint32_t nLine, const char* csAdditional);

/**
* Memory Profiling - Logs freeing of memory.
*/
XN_C_API void XN_C_DECL xnOSLogMemFree(const void* pMemBlock);

/**
* Memory Profiling - Prints a current memory report to requested file.
*/
XN_C_API void XN_C_DECL xnOSWriteMemoryReport(const char* csFileName);

// for memory profiling, replace all malloc/calloc/free/new/delete calls
#if (defined XN_MEM_PROFILING) && (!defined(XN_OS_IMPL))
	#ifdef _MSC_VER
		#pragma message("Compiling with Memory Profiling!")
	#elif defined(__INTEL_COMPILER)
		#warning "Compiling with Memory Profiling!"
	//TODO: Add warning for linux compiler(s)
	#endif

	#ifdef __cplusplus
		#include <new>
		static void* operator new(size_t size)
		{
			void* p = xnOSMalloc(size);
			return xnOSLogMemAlloc(p, XN_ALLOCATION_NEW, size, "", "", 0, "");
		}
		static void* operator new[](size_t size)
		{
			void* p = xnOSMalloc(size);
			return xnOSLogMemAlloc(p, XN_ALLOCATION_NEW, size, "", "", 0, "");
		}
		static void* operator new(size_t size, const char* csFunction, const char* csFile, uint32_t nLine, const char* csAdditional)
		{
			void* p = xnOSMalloc(size);
			return xnOSLogMemAlloc(p, XN_ALLOCATION_NEW, size, csFunction, csFile, nLine, csAdditional);
		}

		// called only if ctor threw exception
		static void operator delete(void* p, const char* /*csFunction*/, const char* /*csFile*/, uint32_t /*nLine*/, const char* /*csAdditional*/)
		{
			xnOSLogMemFree(p);
			xnOSFree(p);
		}

		static void operator delete(void* p)
		{
			xnOSLogMemFree(p);
			xnOSFree(p);
		}

		static void* operator new[](size_t size, const char* csFunction, const char* csFile, uint32_t nLine, const char* csAdditional)
		{
			void* p = xnOSMalloc(size);
			return xnOSLogMemAlloc(p, XN_ALLOCATION_NEW_ARRAY, size, csFunction, csFile, nLine, csAdditional);
		}

		// called only if ctor threw exception
		static void operator delete[](void* p, const char* /*csFunction*/, const char* /*csFile*/, uint32_t /*nLine*/, const char* /*csAdditional*/)
		{
			xnOSLogMemFree(p);
			xnOSFree(p);
		}

		static void operator delete[](void* p)
		{
			xnOSLogMemFree(p);
			xnOSFree(p);
		}

		#define xnOSMalloc(nAllocSize)									xnOSLogMemAlloc(xnOSMalloc(nAllocSize), XN_ALLOCATION_MALLOC, nAllocSize, __FUNCTION__, __FILE__, __LINE__, NULL)
		#define xnOSMallocAligned(nAllocSize, nAlignment)				xnOSLogMemAlloc(xnOSMallocAligned(nAllocSize, nAlignment), XN_ALLOCATION_MALLOC_ALIGNED, nAllocSize, __FUNCTION__, __FILE__, __LINE__, "Aligned to " XN_STRINGIFY(nAlignment))
		#define xnOSCalloc(nAllocNum, nAllocSize)						xnOSLogMemAlloc(xnOSCalloc(nAllocNum, nAllocSize), XN_ALLOCATION_CALLOC, nAllocNum*nAllocSize, __FUNCTION__, __FILE__, __LINE__, NULL)
		#define xnOSCallocAligned(nAllocNum, nAllocSize, nAlignment)	xnOSLogMemAlloc(xnOSCallocAligned(nAllocNum, nAllocSize, nAlignment), XN_ALLOCATION_CALLOC_ALIGNED, nAllocNum*nAllocSize, __FUNCTION__, __FILE__, __LINE__, "Aligned to " XN_STRINGIFY(nAlignment))
		#define xnOSFree(pMemBlock)										{ xnOSLogMemFree(pMemBlock); xnOSFree(pMemBlock); }
		#define xnOSFreeAligned(pMemBlock)								{ xnOSLogMemFree(pMemBlock); xnOSFreeAligned(pMemBlock); }

		#undef XN_NEW
		#undef XN_NEW_ARR

		#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
			#define XN_NEW(type, ...)		new (__FUNCTION__, __FILE__, __LINE__, XN_STRINGIFY(type)) type(__VA_ARGS__)
		#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
			#define XN_NEW(type, arg...)	new (__FUNCTION__, __FILE__, __LINE__, XN_STRINGIFY(type)) type(arg)
		#else
			#define XN_NEW(type, arg)		new (__FUNCTION__, __FILE__, __LINE__, XN_STRINGIFY(type)) type(arg)
		#endif

		#define XN_NEW_ARR(type, count)		new (__FUNCTION__, __FILE__, __LINE__, XN_STRINGIFY(count) " " XN_STRINGIFY(type)) type[count]

	#endif
#endif

// Files
XN_C_API XnStatus XN_C_DECL xnOSCountFiles(const char* cpSearchPattern, int32_t* pnFoundFiles);
XN_C_API XnStatus XN_C_DECL xnOSGetFileList(const char* cpSearchPattern, const char* cpPrefixPath, char cpFileList[][XN_FILE_MAX_PATH], const int32_t nMaxFiles, int32_t* pnFoundFiles);
XN_C_API XnStatus XN_C_DECL xnOSOpenFile(const char* cpFileName, const uint32_t nFlags, XN_FILE_HANDLE* pFile);
XN_C_API XnStatus XN_C_DECL xnOSCloseFile(XN_FILE_HANDLE* pFile);
XN_C_API XnStatus XN_C_DECL xnOSReadFile(const XN_FILE_HANDLE File, void* pBuffer, uint32_t* pnBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSWriteFile(const XN_FILE_HANDLE File, const void* pBuffer, const uint32_t nBufferSize);
XN_C_API XnStatus XN_API_DEPRECATED("Use xnOSSeekFile64() instead") XN_C_DECL
			    xnOSSeekFile  (const XN_FILE_HANDLE File, const XnOSSeekType SeekType, const int32_t nOffset);
XN_C_API XnStatus XN_C_DECL xnOSSeekFile64(const XN_FILE_HANDLE File, const XnOSSeekType SeekType, const int64_t nOffset);
XN_C_API XnStatus XN_API_DEPRECATED("Use xnOSTellFile64() instead") XN_C_DECL
			    xnOSTellFile  (const XN_FILE_HANDLE File, uint32_t* nFilePos);
XN_C_API XnStatus XN_C_DECL xnOSTellFile64(const XN_FILE_HANDLE File, uint64_t* nFilePos);
XN_C_API XnStatus XN_C_DECL xnOSTruncateFile64(const XN_FILE_HANDLE File, uint64_t nFilePos);
XN_C_API XnStatus XN_C_DECL xnOSFlushFile(const XN_FILE_HANDLE File);
XN_C_API XnStatus XN_C_DECL xnOSDoesFileExist(const char* cpFileName, bool* pbResult);
XN_C_API XnStatus XN_C_DECL xnOSDoesDirectoryExist(const char* cpDirName, bool* pbResult);
XN_C_API XnStatus XN_C_DECL xnOSLoadFile(const char* cpFileName, void* pBuffer, const uint32_t nBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSSaveFile(const char* cpFileName, const void* pBuffer, const uint32_t nBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSAppendFile(const char* cpFileName, const void* pBuffer, const uint32_t nBufferSize);
XN_C_API XnStatus XN_API_DEPRECATED("Use xnOSGetFileSize64() instead") XN_C_DECL
			    xnOSGetFileSize  (const char* cpFileName, uint32_t* pnFileSize);
XN_C_API XnStatus XN_C_DECL xnOSGetFileSize64(const char* cpFileName, uint64_t* pnFileSize);
XN_C_API XnStatus XN_C_DECL xnOSCreateDirectory(const char* cpDirName);
XN_C_API XnStatus XN_C_DECL xnOSGetDirName(const char* cpFilePath, char* cpDirName, const uint32_t nBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSGetFileName(const char* cpFilePath, char* cpFileName, const uint32_t nBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSGetFullPathName(const char* strFilePath, char* strFullPath, uint32_t nBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSGetCurrentDir(char* cpDirName, const uint32_t nBufferSize);
XN_C_API XnStatus XN_C_DECL xnOSSetCurrentDir(const char* cpDirName);

/**
 * Strips the directory separator at the end of the specified path by directly modifying the given string.
 * Always returns XN_STATUS_OK.
 */
XN_C_API XnStatus XN_C_DECL xnOSStripDirSep(char* strDirName);
/**
 * Checks if the specified character works as a directory separator.
 */
XN_C_API bool XN_C_DECL xnOSIsDirSep(char c);
/**
 * Appends the specified path component(s) to the specified path buffer.
 * Directory separator is applied if necessary.
 * If the path component to append is an absolute path, the resulted path is completely altered with it.
 *
 * @param	strDestPath					[in]	Buffer that stores the original path to be operated.
 * @param	strPathComponentToAppend	[in]	Path component(s) to append. Can be absolute or relative.
 * @param	nBufferSize					[in]	Size of strDestPath.
 */
XN_C_API XnStatus XN_C_DECL xnOSAppendFilePath(char* strDestPath, const char* strPathComponentToAppend, const uint32_t nBufferSize);
/**
 * Returns true if the specified path is absolute.
 */
XN_C_API bool XN_C_DECL xnOSIsAbsoluteFilePath(const char* strFilePath);
XN_C_API XnStatus XN_C_DECL xnOSDeleteFile(const char* cpFileName);
XN_C_API XnStatus XN_C_DECL xnOSDeleteEmptyDirectory(const char* strDirName);
XN_C_API XnStatus XN_C_DECL xnOSDeleteDirectoryTree(const char* strDirName);

// INI
XN_C_API XnStatus XN_C_DECL xnOSReadStringFromINI(const char* cpINIFile, const char* cpSection, const char* cpKey, char* cpDest, const uint32_t nDestLength);
XN_C_API XnStatus XN_C_DECL xnOSReadFloatFromINI(const char* cpINIFile, const char* cpSection, const char* cpKey, float* fDest);
XN_C_API XnStatus XN_C_DECL xnOSReadDoubleFromINI(const char* cpINIFile, const char* cpSection, const char* cpKey, double* fDest);
XN_C_API XnStatus XN_C_DECL xnOSReadIntFromINI(const char* cpINIFile, const char* cpSection, const char* cpKey, int32_t* nDest);
XN_C_API XnStatus XN_C_DECL xnOSWriteStringToINI(const char* cpINIFile, const char* cpSection, const char* cpKey, const char* cpSrc);
XN_C_API XnStatus XN_C_DECL xnOSWriteFloatToINI(const char* cpINIFile, const char* cpSection, const char* cpKey, const float fSrc);
XN_C_API XnStatus XN_C_DECL xnOSWriteDoubleToINI(const char* cpINIFile, const char* cpSection, const char* cpKey, const double fSrc);
XN_C_API XnStatus XN_C_DECL xnOSWriteIntToINI(const char* cpINIFile, const char* cpSection, const char* cpKey, const int32_t nSrc);

// Shared libraries
XN_C_API XnStatus XN_C_DECL xnOSLoadLibrary(const char* cpFileName, XN_LIB_HANDLE* pLibHandle);
XN_C_API XnStatus XN_C_DECL xnOSFreeLibrary(const XN_LIB_HANDLE LibHandle);
XN_C_API XnStatus XN_C_DECL xnOSGetProcAddress(const XN_LIB_HANDLE LibHandle, const char* cpProcName, XnFarProc* pProcAddr);

/**
 * Returns the absolute path of the module that includes the specified proc address.
 *
 * @param	procAddr		[in]	Proc address contained by the target module.
 * @param	strModulePath	[in]	Buffer to receive the absolute path of the module. Must have the size of XN_FILE_MAX_PATH at least.
 */
XN_C_API XnStatus XN_C_DECL xnOSGetModulePathForProcAddress(void* procAddr, char *strModulePath);

struct timespec;

// Time
XN_C_API XnStatus XN_C_DECL xnOSGetEpochTime(uint32_t* nEpochTime);
XN_C_API XnStatus XN_C_DECL xnOSGetTimeStamp(uint64_t* nTimeStamp);
XN_C_API XnStatus XN_C_DECL xnOSGetHighResTimeStamp(uint64_t* nTimeStamp);
XN_C_API XnStatus XN_C_DECL xnOSSleep(uint32_t nMilliseconds);
XN_C_API XnStatus XN_C_DECL xnOSStartTimer(XnOSTimer* pTimer);
XN_C_API XnStatus XN_C_DECL xnOSStartHighResTimer(XnOSTimer* pTimer);
XN_C_API XnStatus XN_C_DECL xnOSQueryTimer(XnOSTimer Timer, uint64_t* pnTimeSinceStart);
XN_C_API XnStatus XN_C_DECL xnOSStopTimer(XnOSTimer* pTimer);
XN_C_API XnStatus XN_C_DECL xnOSGetMonoTime(struct timespec* pTime);
XN_C_API XnStatus XN_C_DECL xnOSGetTimeout(struct timespec* pTime, uint32_t nMilliseconds);
XN_C_API XnStatus XN_C_DECL xnOSGetAbsTimeout(struct timespec* pTime, uint32_t nMilliseconds);

// Threads
typedef enum XnThreadPriority
{
	XN_PRIORITY_LOW,
	XN_PRIORITY_NORMAL,
	XN_PRIORITY_HIGH,
	XN_PRIORITY_CRITICAL
} XnThreadPriority;

XN_C_API XnStatus XN_C_DECL xnOSCreateThread(XN_THREAD_PROC_PROTO pThreadProc, const XN_THREAD_PARAM pThreadParam, XN_THREAD_HANDLE* pThreadHandle);
XN_C_API XnStatus XN_C_DECL xnOSTerminateThread(XN_THREAD_HANDLE* pThreadHandle);
XN_C_API XnStatus XN_C_DECL xnOSCloseThread(XN_THREAD_HANDLE* pThreadHandle);
XN_C_API XnStatus XN_C_DECL xnOSWaitForThreadExit(XN_THREAD_HANDLE ThreadHandle, uint32_t nMilliseconds);
XN_C_API XnStatus XN_C_DECL xnOSSetThreadPriority(XN_THREAD_HANDLE ThreadHandle, XnThreadPriority nPriority);
XN_C_API XnStatus XN_C_DECL xnOSGetCurrentThreadID(XN_THREAD_ID* pThreadID);
XN_C_API XnStatus XN_C_DECL xnOSWaitAndTerminateThread(XN_THREAD_HANDLE* pThreadHandle, uint32_t nMilliseconds);
XN_C_API bool XN_C_DECL xnOSDoesThreadExistByID(XN_THREAD_ID threadId);

// Processes
XN_C_API XnStatus XN_C_DECL xnOSGetCurrentProcessID(XN_PROCESS_ID* pProcID);
XN_C_API XnStatus XN_C_DECL xnOSCreateProcess(const char* strExecutable, uint32_t nArgs, const char** pstrArgs, XN_PROCESS_ID* pProcID);

// Mutex
XN_C_API XnStatus XN_C_DECL xnOSCreateMutex(XN_MUTEX_HANDLE* pMutexHandle);
XN_C_API XnStatus XN_C_DECL xnOSCreateNamedMutex(XN_MUTEX_HANDLE* pMutexHandle, const char* cpMutexName);
XN_C_API XnStatus XN_C_DECL xnOSCreateNamedMutexEx(XN_MUTEX_HANDLE* pMutexHandle, const char* cpMutexName, bool bAllowOtherUsers);
XN_C_API XnStatus XN_C_DECL xnOSCloseMutex(XN_MUTEX_HANDLE* pMutexHandle);
XN_C_API XnStatus XN_C_DECL xnOSLockMutex(const XN_MUTEX_HANDLE MutexHandle, uint32_t nMilliseconds);
XN_C_API XnStatus XN_C_DECL xnOSUnLockMutex(const XN_MUTEX_HANDLE MutexHandle);

// Critical Sections
XN_C_API XnStatus XN_C_DECL xnOSCreateCriticalSection(XN_CRITICAL_SECTION_HANDLE* pCriticalSectionHandle);
XN_C_API XnStatus XN_C_DECL xnOSCloseCriticalSection(XN_CRITICAL_SECTION_HANDLE* pCriticalSectionHandle);
XN_C_API XnStatus XN_C_DECL xnOSEnterCriticalSection(XN_CRITICAL_SECTION_HANDLE* pCriticalSectionHandle);
XN_C_API XnStatus XN_C_DECL xnOSLeaveCriticalSection(XN_CRITICAL_SECTION_HANDLE* pCriticalSectionHandle);

// Events
XN_C_API XnStatus XN_C_DECL xnOSCreateEvent(XN_EVENT_HANDLE* pEventHandle, bool bManualReset);
XN_C_API XnStatus XN_C_DECL xnOSCreateNamedEvent(XN_EVENT_HANDLE* pEventHandle, const char* cpEventName, bool bManualReset);
XN_C_API XnStatus XN_C_DECL xnOSCreateNamedEventEx(XN_EVENT_HANDLE* pEventHandle, const char* cpEventName, bool bManualReset, bool bAllowOtherUsers);
XN_C_API XnStatus XN_C_DECL xnOSOpenNamedEvent(XN_EVENT_HANDLE* pEventHandle, const char* cpEventName);
XN_C_API XnStatus XN_C_DECL xnOSOpenNamedEventEx(XN_EVENT_HANDLE* pEventHandle, const char* cpEventName, bool bAllowOtherUsers);
XN_C_API XnStatus XN_C_DECL xnOSCloseEvent(XN_EVENT_HANDLE* pEventHandle);
XN_C_API XnStatus XN_C_DECL xnOSSetEvent(const XN_EVENT_HANDLE EventHandle);
XN_C_API XnStatus XN_C_DECL xnOSResetEvent(const XN_EVENT_HANDLE EventHandle);
XN_C_API XnStatus XN_C_DECL xnOSWaitEvent(const XN_EVENT_HANDLE EventHandle, uint32_t nMilliseconds);
XN_C_API bool XN_C_DECL xnOSIsEventSet(const XN_EVENT_HANDLE EventHandle);

// Semaphores
XN_C_API XnStatus XN_C_DECL xnOSCreateSemaphore(XN_SEMAPHORE_HANDLE* pSemaphoreHandle, uint32_t nInitialCount);
XN_C_API XnStatus XN_C_DECL xnOSLockSemaphore(XN_SEMAPHORE_HANDLE hSemaphore, uint32_t nMilliseconds);
XN_C_API XnStatus XN_C_DECL xnOSUnlockSemaphore(XN_SEMAPHORE_HANDLE hSemaphore);
XN_C_API XnStatus XN_C_DECL xnOSCloseSemaphore(XN_SEMAPHORE_HANDLE* pSemaphoreHandle);

/**
* Waits for a condition to be met. The condition is evaluated every time an event is set.
*
* @param	EventHandle		[in]	The Event handle.
* @param	nMilliseconds	[in]	A timeout in milliseconds to wait.
* @param	pConditionFunc	[in]	A function that should be called to evaluate condition.
* @param	pConditionData	[in]	A cookie to be passed to the condition functions.
*/
XN_C_API XnStatus XN_C_DECL xnOSWaitForCondition(const XN_EVENT_HANDLE EventHandle, uint32_t nMilliseconds, XnConditionFunc pConditionFunc, void* pConditionData);

// Shared Memory
typedef struct XnOSSharedMemory XnOSSharedMemory, *XN_SHARED_MEMORY_HANDLE;

/**
 * Creates a shared memory block and maps it to the process memory.
 *
 * @param	strName			[in]	A machine-unique name that will be used by other processes to open this block.
 * @param	nSize			[in]	The size of the buffer.
 * @param	nAccessFlags	[in]	Creation flags. Can contain XN_OS_FILE_READ, XN_OS_FILE_WRITE or both.
 * @param	phSharedMem		[out]	A handle to the shared-memory block.
 */
XN_C_API XnStatus XN_C_DECL xnOSCreateSharedMemory(const char* strName, uint32_t nSize, uint32_t nAccessFlags, XN_SHARED_MEMORY_HANDLE* phSharedMem);

XN_C_API XnStatus XN_C_DECL xnOSCreateSharedMemoryEx(const char* strName, uint32_t nSize, uint32_t nAccessFlags, bool bAllowOtherUsers, XN_SHARED_MEMORY_HANDLE* phSharedMem);

/**
 * Opens a shared memory block, and returns the address in which it was mapped to the process' memory.
 *
 * @param	strName			[in]	A machine-unique name that will be used by other processes to open this block.
 * @param	nAccessFlags	[in]	Creation flags. Must contain XN_OS_FILE_READ, and optionally XN_OS_FILE_WRITE.
 * @param	phSharedMem		[out]	A handle to the shared-memory block.
 */
XN_C_API XnStatus XN_C_DECL xnOSOpenSharedMemory(const char* strName, uint32_t nAccessFlags, XN_SHARED_MEMORY_HANDLE* phSharedMem);

XN_C_API XnStatus XN_C_DECL xnOSOpenSharedMemoryEx(const char* strName, uint32_t nAccessFlags, bool bAllowOtherUsers, XN_SHARED_MEMORY_HANDLE* phSharedMem);

/**
 * Closes a shared memory block.
 *
 * @param	hSharedMem		[in]	A handle to the block to be closed.
 */
XN_C_API XnStatus XN_C_DECL xnOSCloseSharedMemory(XN_SHARED_MEMORY_HANDLE hSharedMem);

/**
 * Gets the address in which the shared-memory block is mapped in this process.
 *
 * @param	hSharedMem		[in]	A handle to the shared memory block.
 * @param	ppAddress		[out]	The address.
 */
XN_C_API XnStatus XN_C_DECL xnOSSharedMemoryGetAddress(XN_SHARED_MEMORY_HANDLE hSharedMem, void** ppAddress);

// Keyboard
XN_C_API bool XN_C_DECL xnOSWasKeyboardHit();
XN_C_API char XN_C_DECL xnOSReadCharFromInput();

// Debug Utilities
XN_C_API XnStatus XN_C_DECL xnOSGetCurrentCallStack(int32_t nFramesToSkip, char** astrFrames, uint32_t nMaxNameLength, int32_t* pnFrames);
XN_C_API XnStatus XN_C_DECL xnOSPrintCurrentCallstack();


#endif // _XN_OS_H_
