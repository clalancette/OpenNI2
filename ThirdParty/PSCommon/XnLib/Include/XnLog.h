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
#ifndef _XN_LOG_H_
#define _XN_LOG_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnLib.h"
#include "XnLogTypes.h"
#include "XnDump.h"

//---------------------------------------------------------------------------
// Exported Function Declaration
//---------------------------------------------------------------------------

/**
 * This function initializes the log from an INI file.
 *
 * @param	csINIFile		[in]	The name of the INI file.
 * @param	csSectionName	[in]	The name of the section to read values from.
 */
XN_C_API XnStatus XN_C_DECL xnLogInitFromINIFile(const char* csINIFile, const char* csSectionName);

/**
 * This function closes the log.
 */
XN_C_API XnStatus XN_C_DECL xnLogClose();

// @}

/**
 * @name Filtering Log Messages
 * Functions for managing filters on the log system.
 * @{
 */

/**
 * Sets the minimum severity to be output from a specific mask (logger name).
 *
 * @param	strMask		[in]	Name of the logger.
 * @param	minSeverity	[in]	Minimum severity to be output. Use @ref XN_LOG_SEVERITY_NONE to output nothing.
 */
XN_C_API XnStatus XN_C_DECL xnLogSetMaskMinSeverity(const char* strMask, XnLogSeverity minSeverity);

/**
 * Gets the minimum severity to be output from a specific mask (logger name).
 *
 * @param	strMask		[in]	Name of the logger.
 *
 * @returns The minimum severity, or @ref XN_LOG_SEVERITY_NONE is mask will output nothing.
 */
XN_C_API XnLogSeverity XN_C_DECL xnLogGetMaskMinSeverity(const char* strMask);

// @}

/**
 * @name Log Writers
 * Functions for managing which writers are active in the log system (i.e. which outputs
 * will be generated).
 * @{
 */

/**
 * Registers a new Log Writer to receive log entries.
 *
 * @param	pWriter			[in]	The writer to register
 */
XN_C_API XnStatus XN_C_DECL xnLogRegisterLogWriter(XnLogWriter* pWriter);

/**
 * Unregisters a Log Writer from receiving log entries.
 *
 * @param	pWriter			[in]	The writer to unregister
 */
XN_C_API void XN_C_DECL xnLogUnregisterLogWriter(XnLogWriter* pWriter);

/**
* Configures if log entries will be printed to console.
*
* @param	bConsoleOutput	[in]	true to print log entries to console, false otherwise.
*/
XN_C_API XnStatus XN_C_DECL xnLogSetConsoleOutput(bool bConsoleOutput);

/**
* Configures if log entries will be printed to a log file.
*
* @param	bFileOutput	[in]	true to print log entries to the file, false otherwise.
*/
XN_C_API XnStatus XN_C_DECL xnLogSetFileOutput(bool bFileOutput);

// @}

/**
 * @name File Output
 * Functions for configuring how files are created.
 * @{
 */

/**
 * This function closes current log file, and starts a new one (if file writer is currently active)
 */
XN_C_API XnStatus XN_C_DECL xnLogStartNewFile();

/**
 * Configures if log entries in file will include the file and line that caused them.
 *
 * @param	bLineInfo	[in]	true to print file and line, false otherwise
 */
XN_C_API XnStatus XN_C_DECL xnLogSetLineInfo(bool bLineInfo);

/**
 * Configures the folder under which logs will be written.
 *
 * @param	strOutputFolder	[in]	Folder to write to
 */
XN_C_API XnStatus XN_C_DECL xnLogSetOutputFolder(const char* strOutputFolder);

/**
 * Gets current log file name
 *
 * @param	strFileName		[in]	A buffer to be filled
 * @param	nBufferSize		[in]	The size of the buffer
 */
XN_C_API XnStatus XN_C_DECL xnLogGetFileName(char* strFileName, uint32_t nBufferSize);

// @}

/**
 * @name Logger API
 * Functions for writing entries to the log (used mainly by middleware developers)
 * @{
 */

/**
 * Opens a logger for writing.
 *
 * @param	strMask		[in]	Name of the logger to open.
 */
XN_C_API XnLogger* XN_C_DECL xnLoggerOpen(const char* strMask);

/**
 * Writes a single log entry.
 *
 * @param	pLogger		[in]	Logger to write to
 * @param	severity	[in]	Severity of the log entry
 * @param	strFile		[in]	Name of the source file
 * @param	nLine		[in]	Line in the source file
 * @param	strFormat	[in]	Format string
 *
 * It is advised to use one of the @ref xnLoggerVerbose, @ref xnLoggerInfo, @ref xnLoggerWarning or
 * @ref xnLoggerError macros instead of calling this method directly.
 */
XN_C_API void XN_C_DECL xnLoggerWrite(XnLogger* pLogger, XnLogSeverity severity, const char* strFile, uint32_t nLine, const char* strFormat, ...);

/**
 * Writes to a logger without an entry format (i.e. no timestamp, mask name, etc.)
 *
 * @param	pLogger		[in]	Logger to write to
 * @param	severity	[in]	Severity of the log entry
 * @param	strFormat	[in]	Format string
 */
XN_C_API void XN_C_DECL xnLoggerWriteNoEntry(XnLogger* pLogger, XnLogSeverity severity, const char* strFormat, ...);

/**
 * Writes binary data to a logger, formatting it to readable text.
 *
 * @param	pLogger		[in]	Logger to write to
 * @param	severity	[in]	Severity of the log entry
 * @param	strFile		[in]	Name of the source file
 * @param	nLine		[in]	Line in the source file
 * @param	pBinData	[in]	A pointer to the binary data to be written
 * @param	nDataSize	[in]	The number of bytes to write
 * @param	strFormat	[in]	Format string for the binary data header
 */
XN_C_API void XN_C_DECL xnLoggerWriteBinaryData(XnLogger* pLogger, XnLogSeverity severity, const char* strFile, uint32_t nLine, unsigned char* pBinData, uint32_t nDataSize, const char* strFormat, ...);

/**
 * Checks if a specific severity is enabled for this logger.
 *
 * @param	pLogger		[in]	Logger to check
 * @param	severity	[in]	Severity to check.
 */
XN_C_API bool XN_C_DECL xnLoggerIsEnabled(XnLogger* pLogger, XnLogSeverity severity);

/**
 * Closes a logger previsouly opened using @ref xnLoggerOpen().
 *
 * @param	pLogger		[in]	The logger to be closed.
 */
XN_C_API void XN_C_DECL _xnLoggerClose(XnLogger* pLogger);

/**
 * Closes a logger and NULLs the handle.
 *
 * @param	pLogger		[in]	The logger to be closed.
 */
#define xnLoggerClose(pLogger)			\
	{									\
		_xnLoggerClose(pLogger);		\
		pLogger = NULL;					\
	}

#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	/**
	* Helper macro for logging. Checks inline if logger is enabled and adds source file and line info.
	*/
	#define xnLoggerWriteHelper(pLogger, severity, csFormat, ...)								\
		if (pLogger != NULL && severity >= pLogger->nMinSeverity)								\
		{																						\
			xnLoggerWrite(pLogger, severity, __FILE__, __LINE__, csFormat, __VA_ARGS__);		\
		}

	/**
	* Helper macro for logging. Issues a verbose log entry.
	*/
	#define xnLoggerVerbose(pLogger, csFormat, ...) xnLoggerWriteHelper(pLogger, XN_LOG_VERBOSE, csFormat, __VA_ARGS__)
	/**
	* Helper macro for logging. Issues an info log entry.
	*/
	#define xnLoggerInfo(pLogger, csFormat, ...)    xnLoggerWriteHelper(pLogger, XN_LOG_INFO, csFormat, __VA_ARGS__)
	/**
	* Helper macro for logging. Issues a warning log entry.
	*/
	#define xnLoggerWarning(pLogger, csFormat, ...) xnLoggerWriteHelper(pLogger, XN_LOG_WARNING, csFormat, __VA_ARGS__)
	/**
	* Helper macro for logging. Issues an error log entry.
	*/
	#define xnLoggerError(pLogger, csFormat, ...)   xnLoggerWriteHelper(pLogger, XN_LOG_ERROR, csFormat, __VA_ARGS__)

	/**
	* Helper macro for returning from a function while logging
	*
	* @param	pLogger		[in]	The logger to be closed.
	* @param	nRetVal		[in]	The return value from the function.
	* @param	severity	[in]	Severity of the log entry
	* @param	csFormat	[in]	Format string
	*/
	#define XN_RETURN_WITH_LOG(pLogger, nRetVal, severity, csFormat, ...)					\
		{																					\
			xnLoggerWriteHelper(pLogger, severity, csFormat, __VA_ARGS__);					\
			return (nRetVal);																\
		}

	/**
	* Helper macro for returning from a function while logging a warning
	*
	* @param	pLogger		[in]	The logger to be closed.
	* @param	nRetVal		[in]	The return value from the function.
	* @param	csFormat	[in]	Format string
	*/
	#define XN_RETURN_WITH_WARNING_LOG(pLogger, nRetVal, csFormat, ...)						\
		XN_RETURN_WITH_LOG(pLogger, nRetVal, XN_LOG_WARNING, csFormat, __VA_ARGS__)

	/**
	* Helper macro for returning from a function while logging an error
	*
	* @param	pLogger		[in]	The logger to be closed.
	* @param	nRetVal		[in]	The return value from the function.
	* @param	csFormat	[in]	Format string
	*/
	#define XN_RETURN_WITH_ERROR_LOG(pLogger, nRetVal, csFormat, ...)						\
		XN_RETURN_WITH_LOG(pLogger, nRetVal, XN_LOG_ERROR, csFormat, __VA_ARGS__)

#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define xnLoggerWriteHelper(pLogger, severity, csFormat, ...)								\
		if (pLogger != NULL && severity >= pLogger->nMinSeverity)								\
		{																						\
			xnLoggerWrite(pLogger, severity, __FILE__, __LINE__, csFormat, ##__VA_ARGS__);		\
		}

	#define xnLoggerVerbose(pLogger, csFormat, ...) xnLoggerWriteHelper(pLogger, XN_LOG_VERBOSE, csFormat,## __VA_ARGS__)
	#define xnLoggerInfo(pLogger, csFormat, ...)    xnLoggerWriteHelper(pLogger, XN_LOG_INFO, csFormat, ##__VA_ARGS__)
	#define xnLoggerWarning(pLogger, csFormat, ...) xnLoggerWriteHelper(pLogger, XN_LOG_WARNING, csFormat, ##__VA_ARGS__)
	#define xnLoggerError(pLogger, csFormat, ...)   xnLoggerWriteHelper(pLogger, XN_LOG_ERROR, csFormat, ##__VA_ARGS__)

	/* Writes to the log and returns nRetVal */
	#define XN_RETURN_WITH_LOG(pLogger, nRetVal, severity, csFormat, ...)					\
		{																					\
			xnLoggerWriteHelper(pLogger, severity, csFormat, ##__VA_ARGS__);				\
			return (nRetVal);																\
		}

	/* Logs a warning and returns nRetVal */
	#define XN_RETURN_WITH_WARNING_LOG(pLogger, nRetVal, csFormat, ...)						\
		XN_RETURN_WITH_LOG(pLogger, nRetVal, XN_LOG_WARNING, csFormat, ##__VA_ARGS__)

	/* Logs an error and returns nRetVal */
	#define XN_RETURN_WITH_ERROR_LOG(pLogger, nRetVal, csFormat, ...)						\
		XN_RETURN_WITH_LOG(pLogger, nRetVal, XN_LOG_ERROR, csFormat, ##__VA_ARGS__)

#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_NO_VAARGS
	#define xnLoggerWriteHelper(pLogger, severity, csFormat, arg)								\
		if (pLogger != NULL && severity >= pLogger->nMinSeverity)								\
		{																						\
			xnLoggerWrite(pLogger, severity, __FILE__, __LINE__, csFormat, arg);				\
		}

	#define xnLoggerVerbose(pLogger, csFormat, arg) xnLoggerWriteHelper(pLogger, XN_LOG_VERBOSE, csFormat, arg)
	#define xnLoggerInfo(pLogger, csFormat, arg)    xnLoggerWriteHelper(pLogger, XN_LOG_INFO, csFormat, arg)
	#define xnLoggerWarning(pLogger, csFormat, arg) xnLoggerWriteHelper(pLogger, XN_LOG_WARNING, csFormat, arg)
	#define xnLoggerError(pLogger, csFormat, arg)   xnLoggerWriteHelper(pLogger, XN_LOG_ERROR, csFormat, arg)

	/* Writes to the log and returns nRetVal */
	#define XN_RETURN_WITH_LOG(pLogger, nRetVal, severity, csFormat)					\
		{																				\
			xnLoggerWriteHelper(pLogger, severity, csFormat);							\
			return (nRetVal);															\
		}

	/* Logs a warning and returns nRetVal */
	#define XN_RETURN_WITH_WARNING_LOG(pLogger, nRetVal, csFormat)						\
		XN_RETURN_WITH_LOG(pLogger, nRetVal, XN_LOG_WARNING, csFormat)

	/* Logs an error and returns nRetVal */
	#define XN_RETURN_WITH_ERROR_LOG(pLogger, nRetVal, csFormat)						\
		XN_RETURN_WITH_LOG(pLogger, nRetVal, XN_LOG_ERROR, csFormat)

#else
	#error Xiron Log - Unknown VAARGS type!
#endif

// @}

/**
 * @name Misc.
 * Miscellaneous functions regarding the log system.
 * @{
 */

/**
 * Creates a new file under the logs directory.
 *
 * @param	strName			[in]		Name of the file to create
 * @param	bSessionBased	[in]		true for a session-based file, false otherwise. A session based
 *										file also includes the timestamp and process ID of the running
 *										process as a prefix to its name.
 * @param	csFullPath		[in/out]	A buffer to be filled with full path of the created file
 * @param	nPathBufferSize	[in]		The size of the <c>csFullPath</c> buffer
 * @param	phFile			[out]		The file handle
 */
XN_C_API XnStatus XN_C_DECL xnLogCreateNewFile(const char* strName, bool bSessionBased, char* csFullPath, uint32_t nPathBufferSize, XN_FILE_HANDLE* phFile);

// @}

#define XN_MASK_RETVAL_CHECKS "RetValChecks"

XN_C_API XnLogger* XN_LOGGER_RETVAL_CHECKS;

/** Validates return value and writes log message with appropriate status string **/
#define XN_IS_STATUS_OK_LOG_ERROR(what, nRetVal)														\
	if (nRetVal != XN_STATUS_OK)																		\
	{																									\
		xnLoggerError(XN_LOGGER_RETVAL_CHECKS, "Failed to " what ": %s", xnGetStatusString(nRetVal));	\
		XN_ASSERT(false);																				\
		return (nRetVal);																				\
	}
/** The same, but returns a different value **/
#define XN_IS_STATUS_OK_LOG_ERROR_RET(what, xnStatus, retVal)														\
	if (xnStatus != XN_STATUS_OK)																		\
{																									\
	xnLoggerError(XN_LOGGER_RETVAL_CHECKS, "Failed to " what ": %s", xnGetStatusString(xnStatus));	\
	XN_ASSERT(false);																				\
	return (retVal);																				\
}


#ifndef __XN_NO_BC__

XN_C_API XnStatus XN_API_DEPRECATED("Please use xnLogSetMaskMinSeverity() instead") XN_C_DECL xnLogSetMaskState(const char* csMask, bool bEnabled);
XN_C_API XnStatus XN_API_DEPRECATED("Please use xnLogSetMaskMinSeverity() instead") XN_C_DECL xnLogSetSeverityFilter(XnLogSeverity nMinSeverity);
XN_C_API bool XN_C_DECL xnLogIsEnabled(const char* csLogMask, XnLogSeverity nSeverity);
XN_C_API void XN_C_DECL xnLogWrite(const char* csLogMask, XnLogSeverity nSeverity, const char* csFile, uint32_t nLine, const char* csFormat, ...);
XN_C_API void XN_C_DECL xnLogWriteNoEntry(const char* csLogMask, XnLogSeverity nSeverity, const char* csFormat, ...);
XN_C_API void XN_C_DECL xnLogWriteBinaryData(const char* csLogMask, XnLogSeverity nSeverity, const char* csFile, uint32_t nLine, unsigned char* pBinData, uint32_t nDataSize, const char* csFormat, ...);
XN_C_API XnStatus XN_API_DEPRECATED("Use xnLogCreateNewFile() instead") XN_C_DECL xnLogCreateFile(const char* strFileName, XN_FILE_HANDLE* phFile);
XN_C_API XnStatus XN_API_DEPRECATED("Use xnLogCreateNewFile() instead") XN_C_DECL xnLogCreateFileEx(const char* strFileName, bool bSessionBased, XN_FILE_HANDLE* phFile);

#if XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_WIN32_VAARGS_STYLE
	#define xnLogVerbose(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_VERBOSE, __FILE__, __LINE__, csFormat, __VA_ARGS__)
	#define xnLogInfo(csLogMask, csFormat, ...)		xnLogWrite(csLogMask, XN_LOG_INFO, __FILE__, __LINE__, csFormat, __VA_ARGS__)
	#define xnLogWarning(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_WARNING, __FILE__, __LINE__, csFormat, __VA_ARGS__)
	#define xnLogError(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_ERROR, __FILE__, __LINE__, csFormat, __VA_ARGS__)

	/* Writes to the log and returns nRetVal */
	#define XN_LOG_RETURN(nRetVal, nSeverity, csLogMask, csFormat, ...)					\
	{																					\
		xnLogWrite(csLogMask, nSeverity, __FILE__, __LINE__, csFormat, __VA_ARGS__);	\
		return (nRetVal);																\
	}

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_WARNING_RETURN(nRetVal, csLogMask, csFormat, ...)					\
		XN_LOG_RETURN(nRetVal, XN_LOG_WARNING, csLogMask, csFormat, __VA_ARGS__)

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_ERROR_RETURN(nRetVal, csLogMask, csFormat, ...)						\
		XN_LOG_RETURN(nRetVal, XN_LOG_ERROR, csLogMask, csFormat, __VA_ARGS__)

#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_GCC_VAARGS_STYLE
	#define xnLogVerbose(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_VERBOSE, __FILE__, __LINE__, csFormat, ##__VA_ARGS__)
	#define xnLogInfo(csLogMask, csFormat, ...)		xnLogWrite(csLogMask, XN_LOG_INFO, __FILE__, __LINE__, csFormat, ##__VA_ARGS__)
	#define xnLogWarning(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_WARNING, __FILE__, __LINE__, csFormat, ##__VA_ARGS__)
	#define xnLogError(csLogMask, csFormat, ...)	xnLogWrite(csLogMask, XN_LOG_ERROR, __FILE__, __LINE__, csFormat, ##__VA_ARGS__)

	/* Writes to the log and returns nRetVal */
	#define XN_LOG_RETURN(nRetVal, nSeverity, csLogMask, csFormat, ...)					\
	{																					\
		xnLogWrite(csLogMask, nSeverity, __FILE__, __LINE__, csFormat, ##__VA_ARGS__);	\
		return (nRetVal);																\
	}

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_WARNING_RETURN(nRetVal, csLogMask, csFormat, ...)					\
		XN_LOG_RETURN(nRetVal, XN_LOG_WARNING, csLogMask, csFormat, ##__VA_ARGS__)

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_ERROR_RETURN(nRetVal, csLogMask, csFormat, ...)						\
		XN_LOG_RETURN(nRetVal, XN_LOG_ERROR, csLogMask, csFormat, ##__VA_ARGS__)

#elif XN_PLATFORM_VAARGS_TYPE == XN_PLATFORM_USE_NO_VAARGS
	#define xnLogVerbose(csLogMask, csFormat, args)	xnLogWrite(csLogMask, XN_LOG_VERBOSE, __FILE__, __LINE__, csFormat, args)
	#define xnLogInfo(csLogMask, csFormat, args)	xnLogWrite(csLogMask, XN_LOG_INFO, __FILE__, __LINE__, csFormat, args)
	#define xnLogWarning(csLogMask, csFormat, args)	xnLogWrite(csLogMask, XN_LOG_WARNING, __FILE__, __LINE__, csFormat, args)
	#define xnLogError(csLogMask, csFormat, args)	xnLogWrite(csLogMask, XN_LOG_ERROR, __FILE__, __LINE__, csFormat, args)

	/* Writes to the log and returns nRetVal */
	#define XN_LOG_RETURN(nRetVal, nSeverity csLogMask, csFormat, args)					\
	{																					\
		xnLogWrite(csLogMask, nSeverity, __FILE__, __LINE__, csFormat, args);			\
		return (nRetVal);																\
	}

	/* Logs a warning and returns nRetVal */
	#define XN_LOG_WARNING_RETURN(nRetVal, csLogMask, csFormat, args)					\
		XN_LOG_RETURN(nRetVal, XN_LOG_WARNING, csLogMask, csFormat, args)

	/* Logs an error and returns nRetVal */
	#define XN_LOG_ERROR_RETURN(nRetVal, csLogMask, csFormat, args)						\
		XN_LOG_RETURN(nRetVal, XN_LOG_ERROR, csLogMask, csFormat, args)

#else
	#error Xiron Log - Unknown VAARGS type!
#endif

#endif // ifndef __XN_NO_BC__

#endif //_XN_LOG_H_
