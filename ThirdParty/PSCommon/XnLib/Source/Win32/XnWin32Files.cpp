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
#include <cstdlib>
#include <cerrno>
#include <shellapi.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

XN_C_API XnStatus xnOSCountFiles(const char* cpSearchPattern, int32_t* pnFoundFiles)
{
	// Local function variables
	WIN32_FIND_DATA FindFileData;
	XN_HANDLE hFind = NULL;
	int32_t nFoundFiles = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSearchPattern);
	XN_VALIDATE_OUTPUT_PTR(pnFoundFiles);

	// Reset the number of found files counter
	*pnFoundFiles = 0;

	// Get the first file matching the search pattern
	hFind = FindFirstFile(cpSearchPattern, &FindFileData);

	// Keep looking for files as long as we have enough space in the filelist and as long as we didnt reach the end (represented by Invalid Handle)
	while (hFind != INVALID_HANDLE_VALUE)
	{
		// Increase the temporary number of found files counter
		nFoundFiles++;

		// Get the next file in the list. If there are no more, FindNextFile returns false and the while loop is aborted
		if (!FindNextFile(hFind, &FindFileData))
		{
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
	}

	// Close the find file list
	FindClose(hFind);

	// Write the temporary number of found files counter into the output
	*pnFoundFiles = nFoundFiles;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetFileList(const char* cpSearchPattern, const char* cpPrefixPath, char cpFileList[][XN_FILE_MAX_PATH], const int32_t nMaxFiles, int32_t* pnFoundFiles)
{
	// Local function variables
	WIN32_FIND_DATA FindFileData;
	XN_HANDLE hFind = NULL;
	int32_t nFoundFiles = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpSearchPattern);
	XN_VALIDATE_OUTPUT_PTR(cpFileList);
	XN_VALIDATE_OUTPUT_PTR(pnFoundFiles);

	// Reset the number of found files counter
	*pnFoundFiles = 0;

	// Get the first file matching the search pattern
	hFind = FindFirstFile(cpSearchPattern, &FindFileData);

	// Keep looking for files as long as we have enough space in the filelist and as long as we didnt reach the end (represented by Invalid Handle)
	while ((hFind != INVALID_HANDLE_VALUE) && (nFoundFiles < nMaxFiles))
	{
		// Copy the file string into its place in the file list
		xnOSStrCopy(cpFileList[nFoundFiles], FindFileData.cFileName, XN_FILE_MAX_PATH);

		if (cpPrefixPath != NULL)
		{
			xnOSStrPrefix(cpPrefixPath, cpFileList[nFoundFiles], XN_FILE_MAX_PATH);
		}

		// Increase the temporary number of found files counter
		nFoundFiles++;

		// Get the next file in the list. If there are no more, FindNextFile returns false and the while loop is aborted
		if (!FindNextFile(hFind, &FindFileData))
		{
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
	}

	// Close the find file list
	FindClose(hFind);

	// Return a file not found error if no files were found...
	if (nFoundFiles == 0)
	{
		return (XN_STATUS_OS_FILE_NOT_FOUND);
	}

	// Write the temporary number of found files counter into the output
	*pnFoundFiles = nFoundFiles;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSOpenFile(const char* cpFileName, const uint32_t nFlags, XN_FILE_HANDLE* pFile)
{
	// Local function variables
	uint32_t nOSOpenFlags = 0;
	uint32_t nOSCreateFlags = OPEN_ALWAYS;
	uint32_t nShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
	uint32_t nAttributes = FILE_ATTRIBUTE_NORMAL;
	bool bFileExists = false;
	XnStatus nRetVal = XN_STATUS_OK;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pFile);

	// Update the OS Create and Open flags according the user request
	if (nFlags & XN_OS_FILE_READ)
	{
		// Add the generic read flags and also specify request that the file must exist
		nOSCreateFlags = OPEN_EXISTING;
		nOSOpenFlags |= GENERIC_READ;
	}
	if (nFlags & XN_OS_FILE_WRITE)
	{
		// Add the generic write flags
		nOSOpenFlags |= GENERIC_WRITE;
	}
	if (nFlags & XN_OS_FILE_CREATE_NEW_ONLY)
	{
		// It's OK to create a new file, but fail if the file already exist
		nOSCreateFlags = CREATE_NEW;
	}
	if (nFlags & XN_OS_FILE_TRUNCATE)
	{
		// If the file exists, we need to truncate it to zero
		nRetVal = xnOSDoesFileExist(cpFileName, &bFileExists);
		XN_IS_STATUS_OK(nRetVal);

		if (bFileExists == true)
		{
			nOSCreateFlags = TRUNCATE_EXISTING;
		}
	}
	if ((nFlags & XN_OS_FILE_WRITE) && (nFlags & XN_OS_FILE_AUTO_FLUSH))
	{
		nAttributes	|= FILE_FLAG_NO_BUFFERING;
		nAttributes	|= FILE_FLAG_WRITE_THROUGH;
	}

	// Open the file via the OS
	*pFile = CreateFile(cpFileName, nOSOpenFlags, nShareMode, NULL, nOSCreateFlags, nAttributes, NULL);

	// handle failure...
	if (*pFile == INVALID_HANDLE_VALUE)
	{
		// Reset the user supplied handle
		*pFile = NULL;

		if ((nFlags & XN_OS_FILE_WRITE) && (nFlags & XN_OS_FILE_CREATE_NEW_ONLY))
		{
			// If the file was opened for write and a create new only flag was specified, this probably means the file already exist
			return (XN_STATUS_OS_FILE_ALREDY_EXISTS);
		}
		else if (nFlags & XN_OS_FILE_WRITE)
		{
			// If the file was opened for write but without the create only flag, return a generic file open failure
			return (XN_STATUS_OS_FILE_OPEN_FAILED);
		}
		else if (nFlags & XN_OS_FILE_READ)
		{
			// If the file was opened for read, this probably means the file doesn't exist
			return (XN_STATUS_OS_FILE_NOT_FOUND);
		}
	}

	if ((nFlags & XN_OS_FILE_WRITE) && (nFlags & XN_OS_FILE_APPEND))
	{
		nRetVal = xnOSSeekFile64(*pFile, XN_OS_SEEK_END, 0);
		XN_IS_STATUS_OK(nRetVal);
		// ZZZZ Add real error checking?
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCloseFile(XN_FILE_HANDLE* pFile)
{
	// Local function variables
	bool bRetVal = false;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pFile);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(*pFile, XN_STATUS_OS_INVALID_FILE);

	// Close the file handle via the OS
	bRetVal = CloseHandle(*pFile);

	// Make sure it succeeded (return value is true)
	if (bRetVal == true)
	{
		// Reset the user supplied handle
		*pFile = NULL;

		// All is good...
		return (XN_STATUS_OK);
	}
	else
	{
		// Something went wrong while trying to close the file...
		return (XN_STATUS_OS_FILE_CLOSE_FAILED);
	}
}

XN_C_API XnStatus xnOSReadFile(const XN_FILE_HANDLE File, void* pBuffer, uint32_t* pnBufferSize)
{
	// Note: The buffer will not always be filled to it's requested size. It's up to the caller to decide if this
	//       is a problem or not...

	// Local function variables
	bool bRetVal = false;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(pBuffer);
	XN_VALIDATE_INPUT_PTR(pnBufferSize);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	// Read a buffer from a file handle via the OS
	bRetVal	= ReadFile(File, pBuffer, *pnBufferSize, (DWORD*)pnBufferSize, NULL);

	// Make sure it succeeded (return value is true)
	if (bRetVal != true)
	{
		return (XN_STATUS_OS_FILE_READ_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSWriteFile(const XN_FILE_HANDLE File, const void* pBuffer, const uint32_t nBufferSize)
{
	// Local function variables
	bool bRetVal = false;
	uint32_t nBytesToWrite = nBufferSize;
	DWORD nBytesWritten = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(pBuffer);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	// Write a buffer to a file handle via the OS
	bRetVal	= WriteFile(File, pBuffer, nBytesToWrite, &nBytesWritten, NULL);

	// Make sure it succeeded (return value is true) and that the correct number of bytes were written
	if ((bRetVal != true) || (nBytesToWrite != nBytesWritten))
	{
		return (XN_STATUS_OS_FILE_WRITE_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSeekFile(const XN_FILE_HANDLE File, const XnOSSeekType SeekType, const int32_t nOffset)
{
	// Local function variables
	DWORD nRealSeekType = 0;
	DWORD nRetOffset = 0;

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	// Convert the ni seek type into OS seek type
	switch (SeekType)
	{
		case XN_OS_SEEK_SET:
			// Absolute seek from the file beginning
			nRealSeekType = FILE_BEGIN;
			break;
		case XN_OS_SEEK_CUR:
			// Relative seek from the current location
			nRealSeekType = FILE_CURRENT;
			break;
		case XN_OS_SEEK_END:
			// Absolute seek from the file ending
			nRealSeekType = FILE_END;
			break;
		default:
			return (XN_STATUS_OS_INVALID_SEEK_TYPE);
	}

	// Seek a file handle via the OS
	nRetOffset = SetFilePointer(File, nOffset, NULL, nRealSeekType);

	// Make sure it succeeded (return value is valid) and that we reached the expected file offset
	if (nRetOffset == INVALID_SET_FILE_POINTER)
	{
		return (XN_STATUS_OS_FILE_SEEK_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSeekFile64(const XN_FILE_HANDLE File, const XnOSSeekType SeekType, const int64_t nOffset)
{
	// Local function variables
	DWORD nRealSeekType = 0;
	LARGE_INTEGER liPos;
	BOOL bSucceeded = 0;

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	// Convert the ni seek type into OS seek type
	switch (SeekType)
	{
		case XN_OS_SEEK_SET:
			// Absolute seek from the file beginning
			nRealSeekType = FILE_BEGIN;
			break;
		case XN_OS_SEEK_CUR:
			// Relative seek from the current location
			nRealSeekType = FILE_CURRENT;
			break;
		case XN_OS_SEEK_END:
			// Absolute seek from the file ending
			nRealSeekType = FILE_END;
			break;
		default:
			return (XN_STATUS_OS_INVALID_SEEK_TYPE);
	}

	// Seek a file handle via the OS
	liPos.QuadPart = nOffset;
	bSucceeded = SetFilePointerEx(File, liPos, NULL, nRealSeekType);

	// Make sure it succeeded
	if (! bSucceeded)
	{
		return (XN_STATUS_OS_FILE_SEEK_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSTellFile(const XN_FILE_HANDLE File, uint32_t* nFilePos)
{
	LARGE_INTEGER liPos;
	BOOL bSucceeded = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(nFilePos);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	// Seek a file handle by 0 bytes in order to read the file position
	liPos.QuadPart = 0;
	bSucceeded = SetFilePointerEx(File, liPos, &liPos, FILE_CURRENT);

	// Make sure it succeeded (return value is valid)
	if (! bSucceeded)
	{
		return (XN_STATUS_OS_FILE_TELL_FAILED);
	}

	// Enforce uint32 limitation
	if (liPos.HighPart)
	{
		return XN_STATUS_INTERNAL_BUFFER_TOO_SMALL;
	}

	*nFilePos = liPos.LowPart;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSTellFile64(const XN_FILE_HANDLE File, uint64_t* nFilePos)
{
	LARGE_INTEGER liPos;
	BOOL bSucceeded = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_OUTPUT_PTR(nFilePos);

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	// Seek a file handle by 0 bytes in order to read the file position
	liPos.QuadPart = 0;
	bSucceeded = SetFilePointerEx(File, liPos, &liPos, FILE_CURRENT);

	// Make sure it succeeded (return value is valid)
	if (! bSucceeded)
	{
		return (XN_STATUS_OS_FILE_TELL_FAILED);
	}

	*nFilePos = liPos.QuadPart;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XN_C_DECL xnOSTruncateFile64(const XN_FILE_HANDLE File, uint64_t nFilePos)
{
	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	// Obtain current file position.
	uint64_t oldFilePos;
	XnStatus status = xnOSTellFile64(File, &oldFilePos);
	if (XN_STATUS_OK != status)
	{
		return status;
	}

	// Seek to the needed file position.
	status = xnOSSeekFile64(File, XN_OS_SEEK_SET, nFilePos);
	if (XN_STATUS_OK != status)
	{
		goto failure;
	}

	// Finally, truncate the physical size of the file.
	BOOL retVal = SetEndOfFile(File);
	if (false == retVal)
	{
		goto failure;
	}
	return XN_STATUS_OK;

failure:
	xnOSSeekFile64(File, XN_OS_SEEK_SET, oldFilePos);
	return status;
}

XN_C_API XnStatus xnOSFlushFile(const XN_FILE_HANDLE File)
{
	bool bRetVal = false;

	// Make sure the actual file handle isn't NULL
	XN_RET_IF_NULL(File, XN_STATUS_OS_INVALID_FILE);

	bRetVal = FlushFileBuffers(File);
	if (bRetVal == false)
	{
		return (XN_STATUS_OS_FILE_FLUSH_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSDeleteFile(const char* cpFileName)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);

	if (!DeleteFile(cpFileName))
	{
		return (XN_STATUS_OS_FAILED_TO_DELETE_FILE);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSDoesFileExist(const char* cpFileName, bool* bResult)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(bResult);

	// Reset the output result
	*bResult = false;

	// Check if the file exists and update the result accordingly
	if ((_access(cpFileName, 0)) != -1)
	{
		*bResult = true;
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSDoesDirectoryExist(const char* cpDirName, bool* pbResult)
{
	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpDirName);
	XN_VALIDATE_OUTPUT_PTR(pbResult);

	*pbResult = false;

	DWORD attribs = ::GetFileAttributes(cpDirName);
	if (attribs != INVALID_FILE_ATTRIBUTES &&
		(attribs & FILE_ATTRIBUTE_DIRECTORY) != 0)
	{
		*pbResult = true;
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetFileSize(const char* cpFileName, uint32_t* pnFileSize)
{
	// Local function variables
	XN_FILE_HANDLE FileHandle;
	XnStatus nRetVal = XN_STATUS_OK;
	LARGE_INTEGER liSize;
	BOOL bSucceeded = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pnFileSize);

	nRetVal = xnOSOpenFile(cpFileName, XN_OS_FILE_READ, &FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	bSucceeded = GetFileSizeEx(FileHandle, &liSize);
	if (!bSucceeded)
		return XN_STATUS_OS_FILE_GET_SIZE_FAILED;

	// Enforce uint32 limitation
	if (liSize.HighPart)
	{
		return XN_STATUS_INTERNAL_BUFFER_TOO_SMALL;
	}

	*pnFileSize = liSize.LowPart;

	nRetVal = xnOSCloseFile(&FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetFileSize64(const char* cpFileName, uint64_t* pnFileSize)
{
	// Local function variables
	XN_FILE_HANDLE FileHandle;
	XnStatus nRetVal = XN_STATUS_OK;
	LARGE_INTEGER liSize;
	BOOL bSucceeded = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpFileName);
	XN_VALIDATE_OUTPUT_PTR(pnFileSize);

	nRetVal = xnOSOpenFile(cpFileName, XN_OS_FILE_READ, &FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	bSucceeded = GetFileSizeEx(FileHandle, &liSize);
	if (!bSucceeded)
		return XN_STATUS_OS_FILE_GET_SIZE_FAILED;
	*pnFileSize = liSize.QuadPart;

	nRetVal = xnOSCloseFile(&FileHandle);
	XN_IS_STATUS_OK(nRetVal);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCreateDirectory(const char* cpDirName)
{
	// Local function variables
	bool bRetVal = false;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpDirName);

	bRetVal = CreateDirectory(cpDirName, NULL);
	if (bRetVal == false)
	{
		return (XN_STATUS_OS_FAILED_TO_CREATE_DIR);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus XN_C_DECL xnOSDeleteEmptyDirectory(const char* strDirName)
{
	XN_VALIDATE_INPUT_PTR(strDirName);

	bool bRetVal = RemoveDirectory(strDirName);
	if (!bRetVal)
	{
		return XN_STATUS_OS_FAILED_TO_DELETE_DIR;
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus XN_C_DECL xnOSDeleteDirectoryTree(const char* strDirName)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(strDirName);

	// file name must be double-null terminated
	char strDirNameDoubleNull[MAX_PATH+1];
	xnOSMemSet(strDirNameDoubleNull, 0, sizeof(strDirNameDoubleNull));
	nRetVal = xnOSStrCopy(strDirNameDoubleNull, strDirName, MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);

	SHFILEOPSTRUCT shOp;
	shOp.hwnd = NULL;
	shOp.wFunc = FO_DELETE;
	shOp.pFrom = strDirNameDoubleNull;
	shOp.pTo = NULL;
	shOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	shOp.fAnyOperationsAborted = 0;
	shOp.hNameMappings = NULL;
	shOp.lpszProgressTitle = NULL;

	int ret = SHFileOperation(&shOp);
	if (ret != 0)
	{
		return XN_STATUS_OS_FAILED_TO_DELETE_DIR;
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetCurrentDir(char* cpDirName, const uint32_t nBufferSize)
{
	// check needed length
	DWORD nNeededLength = GetCurrentDirectory(NULL, 0);

	if (nNeededLength > nBufferSize)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	// there is enough room, take it
	if (0 == GetCurrentDirectory(nBufferSize, cpDirName))
	{
		return XN_STATUS_ERROR;
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSetCurrentDir(const char* cpDirName)
{
	if (0 == SetCurrentDirectory(cpDirName))
	{
		return (XN_STATUS_ERROR);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSGetDirName(const char* cpFilePath, char* cpDirName, const uint32_t nBufferSize)
{
	char strFullPath[XN_FILE_MAX_PATH];
	char* pFileName;
	DWORD res = GetFullPathName(cpFilePath, XN_FILE_MAX_PATH, strFullPath, &pFileName);
	if (res == 0)
	{
		return XN_STATUS_ERROR;
	}

	if (uint32_t(pFileName - strFullPath) > nBufferSize)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	pFileName[0] = '\0';
	xnOSStripDirSep(strFullPath);
	strcpy(cpDirName, strFullPath);

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSGetFileName(const char* cpFilePath, char* cpFileName, const uint32_t nBufferSize)
{
	char ext[XN_FILE_MAX_PATH];
	XN_VALIDATE_INPUT_PTR(cpFilePath);
	XN_VALIDATE_OUTPUT_PTR(cpFileName);

	errno_t err = _splitpath_s(cpFilePath, NULL, 0, NULL, 0, cpFileName, nBufferSize, ext, sizeof(ext));
	if (err == ERANGE)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}
	else if (err != 0)
	{
		return XN_STATUS_ERROR;
	}

	XnStatus nRetVal = xnOSStrAppend(cpFileName, ext, nBufferSize);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XN_C_API XnStatus xnOSGetFullPathName(const char* strFilePath, char* strFullPath, uint32_t nBufferSize)
{
	char* pFileName;
	DWORD res = GetFullPathName(strFilePath, nBufferSize, strFullPath, &pFileName);
	if (res > nBufferSize)
	{
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}
	else if (res == 0)
	{
		return XN_STATUS_ERROR;
	}

	return XN_STATUS_OK;
}

XN_C_API bool xnOSIsAbsoluteFilePath(const char* strFilePath)
{
	// If the path starts with <letter><colon><path separator>, it is absolute.
	return xnOSStrLen(strFilePath) >= 3 && isalpha(strFilePath[0]) && strFilePath[1] == ':' && xnOSIsDirSep(strFilePath[2]);
}
