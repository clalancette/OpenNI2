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
#include <list>

#include "OniDataRecords.h"

ONI_NAMESPACE_IMPLEMENTATION_BEGIN

// NOTE: XnLib does not define UINT*_C like macros for some reason...
#define XN_UINT64_C(x) ((x) + (XN_MAX_UINT64 - XN_MAX_UINT64))
#define XN_UINT32_C(x) ((x) + (XN_MAX_UINT32 - XN_MAX_UINT32))

namespace {

// Converts NodeType into a string.
const char* AsString(uint32_t nodeType)
{
	switch (nodeType) {
	case NODE_TYPE_DEVICE: return "Device";
	case NODE_TYPE_DEPTH:  return "Depth";
	case NODE_TYPE_IMAGE:  return "Image";
	case NODE_TYPE_IR:     return "IR";
	default:
		;
	}
	return "Invalid";
}

} // namespace

RecordAssembler::RecordAssembler()
	: m_pBuffer(NULL),
	  m_bufferSize_bytes(0),
	  m_pEmitPtr(NULL)
{

}

RecordAssembler::~RecordAssembler()
{
	if (NULL != m_pBuffer)
	{
		XN_DELETE_ARR(m_pBuffer);
	}
}

void RecordAssembler::initialize()
{
	size_t maxHeaderSize_bytes =
		/* size of header POD   = */ sizeof(RecordHeaderData) +
		/* size of node name    = */ (ONI_MAX_STR + 1) +
		/* size of time stamp   = */ sizeof(uint64_t) +
		/* size of frame number = */ sizeof(uint32_t);

	m_bufferSize_bytes = (size_t)(maxHeaderSize_bytes +
		/* max video mode width (pixels)  = */ 1600 *
		/* max video mode height (pixels) = */ 1200 *
		/* max video mode bits per pixel  = */ 3    *
		/* worst case compression rate    = */ 1.2);

	m_pEmitPtr = m_pBuffer = XN_NEW_ARR(uint8_t, m_bufferSize_bytes);
	if (NULL == m_pBuffer)
	{
		m_bufferSize_bytes = 0;
	}
}

OniStatus RecordAssembler::serialize(XN_FILE_HANDLE file)
{
	// NOTE(oleksii): strange, but fieldsSize includes the size of header as
	// well...
	uint32_t serializedSize_bytes = m_header->fieldsSize + m_header->payloadSize;
	XnStatus status = xnOSWriteFile(file, m_pBuffer, serializedSize_bytes);
	return XN_STATUS_OK == status ? ONI_STATUS_OK : ONI_STATUS_ERROR;
}

// A handy macro that helps to reduce code duplicate and level up readability.
#define MUST_BE_INITIALIZED(...) \
	do { \
		if (0 == m_bufferSize_bytes) { return __VA_ARGS__; } \
	} while (0)

void RecordAssembler::emitCommonHeader(uint32_t recordType, uint32_t nodeId, uint64_t undoRecordPos)
{
	MUST_BE_INITIALIZED();
	xnOSMemSet(m_header, 0, sizeof(*m_header));
	// Reads as: "NIR\0"
	m_header->magic        = 0x0052494E;
	m_header->recordType   = recordType;
	m_header->nodeId       = nodeId;
	m_header->fieldsSize   = sizeof(*m_header);
	m_header->payloadSize  = XN_UINT32_C(0);
	m_header->undoRecordPos = undoRecordPos;
	// Reset emit pointer.
	m_pEmitPtr = m_pBuffer + sizeof(*m_header);
}

OniStatus RecordAssembler::emitString(const char* pStr, size_t& totalFieldsSize_bytes)
{
	MUST_BE_INITIALIZED(ONI_STATUS_ERROR);
	if (NULL == pStr)
	{
		return ONI_STATUS_BAD_PARAMETER;
	}

	// Initialize field's data.
	struct
	{
		uint32_t dataSize = 0;
		char   data[ONI_MAX_STR];
	} field;
	xnOSStrCopy(field.data, pStr, sizeof(field.data));
	field.dataSize = XN_MIN(xnOSStrLen(pStr) + 1, sizeof(field.data));
	field.data[sizeof(field.data) - 1] = '\0';

	// Emit field's data.
	size_t fieldSize_bytes = field.dataSize + sizeof(field.dataSize);
	OniStatus status = emitData(&field, fieldSize_bytes);
	if (ONI_STATUS_OK == status)
	{
		totalFieldsSize_bytes += fieldSize_bytes;
	}

	return status;
}

OniStatus RecordAssembler::emitData(const void* pData, size_t dataSize_bytes)
{
	MUST_BE_INITIALIZED(ONI_STATUS_ERROR);
	xnOSMemCopy(m_pEmitPtr, pData, dataSize_bytes);
	m_pEmitPtr += dataSize_bytes;
	return ONI_STATUS_OK;
}

template<typename T>
OniStatus RecordAssembler::emit(const T& field, size_t& totalFieldsSize_bytes)
{
	MUST_BE_INITIALIZED(ONI_STATUS_ERROR);
	size_t fieldSize_bytes = sizeof(field);
	OniStatus status = emitData(&field, fieldSize_bytes);
	if (ONI_STATUS_OK == status)
	{
		totalFieldsSize_bytes += fieldSize_bytes;
	}
	return status;
}

OniStatus RecordAssembler::emit_RECORD_NODE_ADDED_1_0_0_5(
	uint32_t nodeType,
	uint32_t nodeId,
	uint32_t codecId,
	uint32_t numberOfFrames,
	uint64_t minTimeStamp,
	uint64_t maxTimeStamp)
{
	MUST_BE_INITIALIZED(ONI_STATUS_ERROR);

	// NOTE: fields size will be modified by EmitString, and Emit calls.
	emitCommonHeader(RECORD_NODE_ADDED_1_0_0_5, nodeId, /*undoRecordPos*/ 0);

	// Fields of v. 1.0.0.5 record (size & offset are in bytes [decimal notation]):
	// Size     Offset      Fields                    Notes
	// ====     ======      ======                    =====
	//           0          +-------------------+
	//   4              +-->| Size of Node Name |\                                /
	//           4      |   +-------------------+ |-- emitted by EmitString
	//                  |   | Data of Node Name |/
	//       Y = 4 + X -+   +-------------------+
	//   4                  | Node Type         |
	//           Y + 4      +-------------------+
	//   4                  | Codec ID          |
	//           Y + 8      +-------------------+
	//   4                  | Number of Frames  |
	//           Y + 12     +-------------------+
	//   8                  | Min Time Stamp    |
	//           Y + 20     +-------------------+
	//   8                  | Max Time Stamp    |
	//           Y + 28     +-------------------+
	size_t fieldsSize = m_header->fieldsSize;
	emitString(AsString(nodeType),  fieldsSize);
	emit(nodeType,                  fieldsSize);
	emit(codecId,                   fieldsSize);
	emit(numberOfFrames,            fieldsSize);
	emit(minTimeStamp,              fieldsSize);
	emit(maxTimeStamp,              fieldsSize);

	m_header->fieldsSize = (uint32_t)fieldsSize;

	return ONI_STATUS_OK;
}

OniStatus RecordAssembler::emit_RECORD_NODE_ADDED(
	uint32_t nodeType,
	uint32_t nodeId,
	uint32_t codecId,
	uint32_t numberOfFrames,
	uint64_t minTimeStamp,
	uint64_t maxTimeStamp,
	uint64_t seekTablePosition)
{
	MUST_BE_INITIALIZED(ONI_STATUS_ERROR);

	OniStatus status = emit_RECORD_NODE_ADDED_1_0_0_5(
		nodeType,
		nodeId,
		codecId,
		numberOfFrames,
		minTimeStamp,
		maxTimeStamp);
	m_header->recordType = RECORD_NODE_ADDED;

	// NOTE(oleksii): I'm not sure if it's really 1.0.0.6 version...
	//
	// NOTE: offset is given related to v. 1.0.0.5 fields.
	//
	// Fields of v. 1.0.0.6 record (size & offset are in bytes [decimal notation]):
	// Size     Offset      Fields                    Notes
	// ====     ======      ======                    =====
	//                      +---------------------+
	//                      | V. 1.0.0.5 Fields   |
	//           0          +---------------------+
	//   8                  | Seek table position |
	//           8          +---------------------+
	size_t fieldsSize = m_header->fieldsSize;
	emit(seekTablePosition, fieldsSize);
	m_header->fieldsSize = (uint32_t)fieldsSize;

	return status;
}

OniStatus RecordAssembler::emit_RECORD_NODE_STATE_READY(uint32_t nodeId)
{
	MUST_BE_INITIALIZED(ONI_STATUS_ERROR);
	emitCommonHeader(RECORD_NODE_STATE_READY, nodeId, /*undoRecordPos*/ 0);
	return ONI_STATUS_OK;
}

OniStatus RecordAssembler::emit_RECORD_NODE_REMOVED(uint32_t nodeId, uint64_t nodeAddedPos)
{
	MUST_BE_INITIALIZED(ONI_STATUS_ERROR);
	emitCommonHeader(RECORD_NODE_REMOVED, nodeId, /*undoRecordPos*/ nodeAddedPos);
	return ONI_STATUS_OK;
}

OniStatus RecordAssembler::emit_RECORD_SEEK_TABLE(uint32_t nodeId, uint32_t numFrames, const std::list<DataIndexEntry>& dataIndexEntryList)
{
	MUST_BE_INITIALIZED(ONI_STATUS_ERROR);

	// no fields today :-)
	emitCommonHeader(RECORD_SEEK_TABLE, nodeId, /*undoRecordPos*/ 0);

	size_t entrySize    = sizeof(DataIndexEntry);
	size_t nPayloadSize = (numFrames + 1) * entrySize;

	// Verify that there's enough room for the payload in the buffer.
	size_t roomLeft = m_bufferSize_bytes - size_t(m_pEmitPtr - m_pBuffer);
	if (roomLeft < nPayloadSize)
	{
		return ONI_STATUS_ERROR;
	}

	// Emit payload data.
	int nWritten  = 0;

	// start with an empty entry for frame 0 (frames start with 1)
	DataIndexEntry emptyEntry;
	xnOSMemSet(&emptyEntry, 0, entrySize);
	emitData(&emptyEntry, entrySize); ++nWritten;

	// now the table itself
	for (std::list<DataIndexEntry>::const_iterator it = dataIndexEntryList.begin(); it != dataIndexEntryList.end(); ++it)
	{
		emitData(&(*it), entrySize); ++nWritten;
	}

	XN_ASSERT((numFrames + 1) == uint32_t(nWritten));

	m_header->payloadSize = (uint32_t)nPayloadSize;

	return ONI_STATUS_OK;
}

OniStatus RecordAssembler::emit_RECORD_END()
{
	MUST_BE_INITIALIZED(ONI_STATUS_ERROR);
	emitCommonHeader(RECORD_END, /* nodeId (ignored) = */ XN_UINT32_C(0), /*undoRecordPos=*/0);
	return ONI_STATUS_OK;
}

OniStatus RecordAssembler::emit_RECORD_NODE_DATA_BEGIN(
	uint32_t nodeId,
	uint32_t framesCount,
	uint64_t maxTimeStamp)
{
	MUST_BE_INITIALIZED(ONI_STATUS_ERROR);

	// NOTE: fields size will be modified by EmitString, and Emit calls.
	emitCommonHeader(RECORD_NODE_DATA_BEGIN, nodeId, /*undoRecordPos*/ 0);

	// Fields of DATA_BEGIN record (size & offset are in bytes [decimal notation]):
	// Size     Offset      Fields                    Notes
	// ====     ======      ======                    =====
	//           0          +------------------+
	//   4                  | Frames Count     |
	//           4          +------------------+
	//   8                  | Max Time Stamp   |
	//           12         +------------------+
	size_t fieldsSize = m_header->fieldsSize;
	emit(framesCount,  fieldsSize);
	emit(maxTimeStamp, fieldsSize);
	m_header->fieldsSize = (uint32_t)fieldsSize;

	return ONI_STATUS_OK;
}

OniStatus RecordAssembler::emit_RECORD_NEW_DATA(
	uint32_t    nodeId,
	uint64_t    undoRecordPos,
	uint64_t    timeStamp,
	uint32_t    frameId,
	const void* data,
	size_t     dataSize_bytes)
{
	MUST_BE_INITIALIZED(ONI_STATUS_ERROR);

	// NOTE: fields size will be modified by EmitString, and Emit calls.
	emitCommonHeader(RECORD_NEW_DATA, nodeId, undoRecordPos);

	// Fields of NEW_DATA record (size & offset are in bytes [decimal notation]):
	// Size     Offset      Fields                    Notes
	// ====     ======      ======                    =====
	//           0          +---------------------+
	//   8                  | Time Stamp          |
	//           8          +---------------------+
	//   4                  | Seek table position |
	//           12         +---------------------+
	size_t fieldsSize = m_header->fieldsSize;
	emit(timeStamp, fieldsSize);
	emit(frameId,   fieldsSize);
	m_header->fieldsSize = (uint32_t)fieldsSize;

	// Verify that there's enough room for the payload in the buffer.
	size_t roomLeft = m_bufferSize_bytes - size_t(m_pEmitPtr - m_pBuffer);
	if (roomLeft < dataSize_bytes)
	{
		return ONI_STATUS_ERROR;
	}

	// Emit payload data.
	emitData(data, dataSize_bytes);
	m_header->payloadSize = (uint32_t)dataSize_bytes;

	return ONI_STATUS_OK;
}

OniStatus RecordAssembler::emit_RECORD_GENERAL_PROPERTY(
	uint32_t    nodeId,
	uint64_t    undoRecordPos,
	const char* propertyName,
	const void* data,
	size_t     dataSize_bytes)
{
	MUST_BE_INITIALIZED(ONI_STATUS_ERROR);

	// NOTE: fields size will be modified by EmitString, and Emit calls.
	emitCommonHeader(RECORD_GENERAL_PROPERTY, nodeId, undoRecordPos);

	// XXX(oleksii): A very odd thing: though the file format states to be 64-bit
	// compatible, the size of general property field is 32-bit :-(
	//
	// Fields of GENERAL_PROPERTY record (size & offset are in bytes [decimal notation]):
	// Size     Offset      Fields                    Notes
	// ====     ======      ======                    =====
	//           0          +-----------------------+
	//   4              +-->| Size of Property Name |\                                  /
	//           4      |   +-----------------------+ |-- emitted by EmitString
	//                  |   | Data of Property Name |/
	//       Y = 4 + X -+   +-----------------------+
	//   4               +->| Size of Property Data |
	//           Y + 4   |  +-----------------------+
	//                   |  | Data                  |
	//       Z = Y + 4 + W  +-----------------------+
	size_t fieldsSize = m_header->fieldsSize;
	emitString(propertyName,       fieldsSize);
	emit(uint32_t(dataSize_bytes), fieldsSize);
	m_header->fieldsSize = (uint32_t)fieldsSize;

	OniStatus status = emitData(data, dataSize_bytes);
	if (ONI_STATUS_OK == status)
	{
		m_header->fieldsSize += (uint32_t)dataSize_bytes;
	}
	return status;
}

OniStatus RecordAssembler::emit_RECORD_INT_PROPERTY(
        uint32_t    nodeId,
        uint64_t    undoRecordPos,
        const char* propertyName,
        uint64_t    data)
{
	MUST_BE_INITIALIZED(ONI_STATUS_ERROR);

	OniStatus status = emit_RECORD_GENERAL_PROPERTY(
		nodeId,
		undoRecordPos,
		propertyName,
		&data,
		sizeof(data));
	if (ONI_STATUS_OK == status)
	{
		m_header->recordType = RECORD_INT_PROPERTY;
	}
	return status;
}

OniStatus RecordAssembler::emit_RECORD_REAL_PROPERTY(
	uint32_t    nodeId,
	uint64_t    undoRecordPos,
	const char* propertyName,
	double    data)
{
	MUST_BE_INITIALIZED(ONI_STATUS_ERROR);

	OniStatus status = emit_RECORD_GENERAL_PROPERTY(
		nodeId,
		undoRecordPos,
		propertyName,
		&data,
		sizeof(data));
	if (ONI_STATUS_OK == status)
	{
		m_header->recordType = RECORD_REAL_PROPERTY;
	}
	return status;
}

ONI_NAMESPACE_IMPLEMENTATION_END
