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
#ifndef XNPLAYERTYPES_H
#define XNPLAYERTYPES_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnStatus.h>
#include <XnOS.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------

/** The maximum length of names of objects in OpenNI **/
#define XN_MAX_NAME_LENGTH					80

/** The name of the OpenNI recording format. **/
#define XN_FORMAT_NAME_ONI	"oni"

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * Type of the production node.
 */
typedef int32_t XnProductionNodeType;

/**
 * Predefined types of production nodes.
 */
typedef enum XnPredefinedProductionNodeType
{
	/** An invalid node type **/
	XN_NODE_TYPE_INVALID = -1,

	/** A device node **/
	XN_NODE_TYPE_DEVICE = 1,

	/** A depth generator **/
	XN_NODE_TYPE_DEPTH = 2,

	/** An image generator **/
	XN_NODE_TYPE_IMAGE = 3,

	/** An audio generator **/
	XN_NODE_TYPE_AUDIO = 4,

	/** An IR generator **/
	XN_NODE_TYPE_IR = 5,

	/** A user generator **/
	XN_NODE_TYPE_USER = 6,

	/** A recorder **/
	XN_NODE_TYPE_RECORDER = 7,

	/** A player **/
	XN_NODE_TYPE_PLAYER = 8,

	/** A gesture generator **/
	XN_NODE_TYPE_GESTURE = 9,

	/** A scene analyzer **/
	XN_NODE_TYPE_SCENE = 10,

	/** A hands generator **/
	XN_NODE_TYPE_HANDS = 11,

	/** A Codec **/
	XN_NODE_TYPE_CODEC = 12,

	/** Abstract types **/
	XN_NODE_TYPE_PRODUCTION_NODE = 13,
	XN_NODE_TYPE_GENERATOR = 14,
	XN_NODE_TYPE_MAP_GENERATOR = 15,
	XN_NODE_TYPE_SCRIPT = 16,

	XN_NODE_TYPE_FIRST_EXTENSION,

} XnPredefinedProductionNodeType;

/**
 * A Version.
 */
typedef struct XnVersion
{
	uint8_t nMajor;
	uint8_t nMinor;
	uint16_t nMaintenance;
	uint32_t nBuild;
} XnVersion;

//---------------------------------------------------------------------------
// 3D Vision Types
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Generators Capabilities
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Generators API Structs
//---------------------------------------------------------------------------

/**
 * The output mode of a map generator.
 */
typedef struct XnMapOutputMode
{
	/** Number of elements in the X-axis. */
	uint32_t nXRes;
	/** Number of elements in the Y-axis. */
	uint32_t nYRes;
	/** Number of frames per second. */
	uint32_t nFPS;
} XnMapOutputMode;

/**
 * Cropping configuration
 */
typedef struct XnCropping
{
	/** true if cropping is turned on, false otherwise. */
	bool bEnabled;
	/** Offset in the X-axis, in pixels. */
	uint16_t nXOffset;
	/** Offset in the Y-axis, in pixels. */
	uint16_t nYOffset;
	/** Number of pixels in the X-axis. */
	uint16_t nXSize;
	/** Number of pixels in the Y-axis. */
	uint16_t nYSize;
} XnCropping;

/**
 * Field-Of-View
 */
typedef struct XnFieldOfView
{
	/** Horizontal Field Of View, in radians. */
	double fHFOV;
	/** Vertical Field Of View, in radians. */
	double fVFOV;
} XnFieldOfView;

typedef enum XnPixelFormat
{
	XN_PIXEL_FORMAT_RGB24 = 1,
	XN_PIXEL_FORMAT_YUV422 = 2,
	XN_PIXEL_FORMAT_GRAYSCALE_8_BIT = 3,
	XN_PIXEL_FORMAT_GRAYSCALE_16_BIT = 4,
	XN_PIXEL_FORMAT_MJPEG = 5,
} XnPixelFormat;

typedef enum XnPlayerSeekOrigin
{
	XN_PLAYER_SEEK_SET = 0,
	XN_PLAYER_SEEK_CUR = 1,
	XN_PLAYER_SEEK_END = 2,
} XnPlayerSeekOrigin;

//---------------------------------------------------------------------------
// Recorder Types
//---------------------------------------------------------------------------

/** An ID of a codec. See @ref xnCreateCodec. **/
typedef uint32_t XnCodecID;

/** Define a Codec ID by 4 characters, e.g. XN_CODEC_ID('J','P','E','G') **/
#define XN_CODEC_ID(c1, c2, c3, c4) (XnCodecID)((c4 << 24) | (c3 << 16) | (c2 << 8) | c1)

/**
 * An interface used for communication between OpenNI and a player module. This interface is used by a player
 * module to receive recorded data from OpenNI, which knows where to get them according to one of the values of
 * @ref XnRecordMedium.
 **/
typedef struct XnPlayerInputStreamInterface
{
	/**
	 * Opens the stream for reading.
	 *
	 * @param	pCookie [in]	A cookie that was received with this interface.
	 */
	XnStatus (XN_CALLBACK_TYPE* Open)(void* pCookie);

	/**
	 * Reads data from the stream. May read less data than asked, if the stream is near its end. This is not
	 * considered an error.
	 *
	 * @param	pCookie		 [in]	A cookie that was received with this interface.
	 * @param	pBuffer		 [out]	A pointer to the buffer to read into.
	 * @param	nSize		 [in]	Number of bytes to read.
	 * @param	pnBytesRead	 [out]	Optional. Number of bytes actually read.
	 */
	XnStatus (XN_CALLBACK_TYPE* Read)(void* pCookie, void* pBuffer, uint32_t nSize, uint32_t* pnBytesRead);

	/**
	 * Sets the stream's pointer to the specified position.
	 *
	 * @param	pCookie		 [in]	A cookie that was received with this interface.
	 * @param	seekType	 [in]	Specifies how to seek - according to current position, end or beginning.
	 * @param	nOffset		 [in]	Specifies how many bytes to move
	 */
	XnStatus (XN_CALLBACK_TYPE* Seek)(void* pCookie, XnOSSeekType seekType, const int32_t nOffset);

	/**
	 * Tells the current stream position
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 *
	 * @returns (uint32_t)-1 if there was an error in the stream.
	 */
	uint32_t (XN_CALLBACK_TYPE* Tell)(void* pCookie);

	/**
	 * Closes the stream.
	 *
	 * @param	pCookie		 [in]	A cookie that was received with this interface.
	 */
	void (XN_CALLBACK_TYPE* Close)(void* pCookie);

	/**
	 * Sets the stream's pointer to the specified position. (64bit version, for large files)
	 *
	 * @param	pCookie		 [in]	A cookie that was received with this interface.
	 * @param	seekType	 [in]	Specifies how to seek - according to current position, end or beginning.
	 * @param	nOffset		 [in]	Specifies how many bytes to move
	 */
	XnStatus (XN_CALLBACK_TYPE* Seek64)(void* pCookie, XnOSSeekType seekType, const int64_t nOffset);

	/**
	 * Tells the current position in the stream. (64bit version, for large files)
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	pPos		[out]	The position of the stream.
	 *
	 * @returns (uint64_t)-1 on error.
	 */
	uint64_t (XN_CALLBACK_TYPE* Tell64)(void* pCookie);

} XnPlayerInputStreamInterface;

/**
 * An interface that is used for notifications about node events.
 **/
typedef struct XnNodeNotifications
{
	/**
	 * Notifies the object that a production node was added
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The instance name of the added node.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeAdded)
		(void* pCookie, const char* strNodeName, XnProductionNodeType type,
		XnCodecID compression, uint32_t nNumberOfFrames);

	/**
	 * Notifies the object that a production node has been removed
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The instance name of the removed node.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeRemoved)
		(void* pCookie, const char* strNodeName);

	/**
	 * Notifies the object that an integer type property has changed.
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The name of the node whose property changed.
	 * @param	strPropName	[in]	The name of the property that changed.
	 * @param	nValue		[in]	The new value of the property.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeIntPropChanged)
		(void* pCookie, const char* strNodeName,
		const char* strPropName, uint64_t nValue);

	/**
	 * Notifies the object that a real type property has changed.
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The name of the node whose property changed.
	 * @param	strPropName	[in]	The name of the property that changed.
	 * @param	dValue		[in]	The new value of the property.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeRealPropChanged)
		(void* pCookie, const char* strNodeName,
		const char* strPropName, double dValue);

	/**
	 * Notifies the object that a string type property has changed.
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The name of the node whose property changed.
	 * @param	strPropName	[in]	The name of the property that changed.
	 * @param	strValue	[in]	The new value of the property.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeStringPropChanged)
		(void* pCookie, const char* strNodeName,
		const char* strPropName, const char* strValue);

	/**
	 * Notifies the object that a general type property has changed.
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The name of the node whose property changed.
	 * @param	strPropName	[in]	The name of the property that changed.
	 * @param	nBufferSize	[in]	The size of the buffer that holds the new value.
	 * @param	pBuffer		[in]	The buffer that holds the new value of the property.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeGeneralPropChanged)
		(void* pCookie, const char* strNodeName,
		const char* strPropName, uint32_t nBufferSize, const void* pBuffer);

	/**
	 * Notifies the object that a node has finished sending all the initial 'property changed' notifications.
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The name of the node whose state is ready.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeStateReady)
		(void* pCookie, const char* strNodeName);

	/**
	 * Notifies the object that it has received new data.
	 *
	 * @param	pCookie		[in]	A cookie that was received with this interface.
	 * @param	strNodeName	[in]	The name of the node whose property changed.
	 * @param	strName		[in]	The name of the property that changed.
	 * @param	nBufferSize	[in]	The size of the buffer that holds the new value.
	 * @param	pBuffer		[in]	The buffer that holds the new value of the property.
	 */
	XnStatus (XN_CALLBACK_TYPE* OnNodeNewData)
		(void* pCookie, const char* strNodeName,
		uint64_t nTimeStamp, uint32_t nFrame, const void* pData, uint32_t nSize);

} XnNodeNotifications;

#endif // XNPLAYERTYPES_H
