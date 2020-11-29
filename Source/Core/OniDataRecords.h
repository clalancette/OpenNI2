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
#ifndef ONIDATARECORDS_H
#define ONIDATARECORDS_H 1

#include <list>

#include "XnOS.h"

#include "OniCommon.h"
#include "OniCTypes.h"

ONI_NAMESPACE_IMPLEMENTATION_BEGIN
#pragma pack(push, 1)

#define ONI_CODEC_ID(c1, c2, c3, c4) uint32_t((c4 << 24) | (c3 << 16) | (c2 << 8) | c1)

#define ONI_CODEC_UNCOMPRESSED      ONI_CODEC_ID('N', 'O', 'N', 'E')
#define ONI_CODEC_16Z_EMB_TABLES    ONI_CODEC_ID('1', '6', 'z', 'T')
#define ONI_CODEC_JPEG              ONI_CODEC_ID('J', 'P', 'E', 'G')

static const size_t IDENTITY_SIZE = 4;

/// The structure of ONI file's file header.
struct FileHeaderData
{
	uint8_t identity[IDENTITY_SIZE];
	struct Version
	{
		uint8_t  major;
		uint8_t  minor;
		uint16_t maintenance;
		uint32_t build;
	} version;
	uint64_t maxTimeStamp;
	uint32_t maxNodeId;
};

/// The structure of ONI file's record header.
struct RecordHeaderData
{
	uint32_t magic;
	uint32_t recordType;
	uint32_t nodeId;
	uint32_t fieldsSize;
	uint32_t payloadSize;
	uint64_t undoRecordPos;
};

struct VideoModeData
{
	uint32_t width;
	uint32_t height;
	uint32_t fps;
};

/// An entry for a frame in the SeekTable
typedef struct DataIndexEntry
{
	uint64_t nTimestamp;
	uint32_t nConfigurationID;
	uint64_t nSeekPos;
} DataIndexEntry;

/// Enumerates known record types.
enum RecordType
{
	RECORD_NODE_ADDED_1_0_0_4		= 0x02,
	RECORD_INT_PROPERTY			= 0x03,
	RECORD_REAL_PROPERTY			= 0x04,
	RECORD_STRING_PROPERTY			= 0x05,
	RECORD_GENERAL_PROPERTY			= 0x06,
	RECORD_NODE_REMOVED			= 0x07,
	RECORD_NODE_DATA_BEGIN			= 0x08,
	RECORD_NODE_STATE_READY			= 0x09,
	RECORD_NEW_DATA				= 0x0A,
	RECORD_END				= 0x0B,
	RECORD_NODE_ADDED_1_0_0_5		= 0x0C,
	RECORD_NODE_ADDED			= 0x0D,
	RECORD_SEEK_TABLE			= 0x0E,
};

/// Enumerates known node types.
enum NodeType
{
	NODE_TYPE_INVALID = -1,
	NODE_TYPE_DEVICE  =  1,
	NODE_TYPE_DEPTH   =  2,
	NODE_TYPE_IMAGE   =  3,
	NODE_TYPE_IR      =  5,
};

/// Return a NodeType that matches the given OniSourceType.
static inline NodeType AsNodeType(OniSensorType sensorType)
{
	NodeType res = NODE_TYPE_INVALID;
	switch (sensorType)
	{
	case ONI_SENSOR_COLOR:
		res = NODE_TYPE_IMAGE;
		break;
	case ONI_SENSOR_DEPTH:
		res = NODE_TYPE_DEPTH;
		break;
	case ONI_SENSOR_IR:
		res = NODE_TYPE_IR;
		break;
	default:
		;
	}
	return res;
}

class RecordAssembler final
{
public:
	RecordAssembler();

	~RecordAssembler();

	void initialize();

	OniStatus serialize(XN_FILE_HANDLE file);

	OniStatus emit_RECORD_NODE_ADDED_1_0_0_5(
		uint32_t nodeType,
		uint32_t nodeId,
		uint32_t codecId,
		uint32_t numberOfFrames,
		uint64_t minTimeStamp,
		uint64_t maxTimeStamp);

	OniStatus emit_RECORD_NODE_ADDED(
		uint32_t nodeType,
		uint32_t nodeId,
		uint32_t codecId,
		uint32_t numberOfFrames,
		uint64_t minTimeStamp,
		uint64_t maxTimeStamp,
		uint64_t seekTablePosition);

	OniStatus emit_RECORD_NODE_STATE_READY(uint32_t nodeId);

	OniStatus emit_RECORD_NODE_REMOVED(uint32_t nodeId, uint64_t nodeAddedPos);

	OniStatus emit_RECORD_SEEK_TABLE(
		uint32_t nodeId,
		uint32_t numFrames,
		const std::list<DataIndexEntry>& dataIndexEntryList);

	OniStatus emit_RECORD_END();

	OniStatus emit_RECORD_NODE_DATA_BEGIN(
		uint32_t nodeId,
		uint32_t framesCount,
		uint64_t maxTimeStamp);

	OniStatus emit_RECORD_NEW_DATA(
		uint32_t    nodeId,
		uint64_t    undoRecordPos,
		uint64_t    timeStamp,
		uint32_t    frameId,
		const void* data,
	size_t     dataSize_bytes);

	OniStatus emit_RECORD_GENERAL_PROPERTY(
		uint32_t    nodeId,
		uint64_t    undoRecordPos,
		const char* propertyName,
		const void* data,
		size_t     dataSize_bytes);

	OniStatus emit_RECORD_INT_PROPERTY(
		uint32_t    nodeId,
		uint64_t    undoRecordPos,
		const char* propertyName,
		uint64_t    data);
	OniStatus emit_RECORD_REAL_PROPERTY(
		uint32_t    nodeId,
		uint64_t    undoRecordPos,
		const char* propertyName,
		double    data);

private:
	void emitCommonHeader(uint32_t recordType, uint32_t nodeId, uint64_t undoRecordPos);
	OniStatus emitString(const char* pStr, size_t& totalFieldsSize_bytes);
	OniStatus emitData(const void* pData, size_t dataSize_bytes);

	template<typename T>
	OniStatus emit(const T& field, size_t& totalFieldsSize_bytes);

	// Union of a buffer and a record header.
	union
	{
		uint8_t*          m_pBuffer;
		RecordHeaderData* m_header;
	};
	// Size of the buffer in bytes.
	size_t m_bufferSize_bytes;
	// Pointer into the buffer for emit operations.
	uint8_t* m_pEmitPtr;
};

#pragma pack(pop)
ONI_NAMESPACE_IMPLEMENTATION_END

#endif // ONIDATARECORDS_H
