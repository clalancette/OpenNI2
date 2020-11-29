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
#ifndef DATARECORDS_H
#define DATARECORDS_H

#include "XnPlatform.h"
#include "XnStatus.h"
#include "XnPlayerTypes.h"

#pragma pack(push, 1)

#define HEADER_MAGIC_SIZE 4

enum RecordType
{
	RECORD_NODE_ADDED_1_0_0_4		= 0x02,
	RECORD_INT_PROPERTY				= 0x03,
	RECORD_REAL_PROPERTY			= 0x04,
	RECORD_STRING_PROPERTY			= 0x05,
	RECORD_GENERAL_PROPERTY			= 0x06,
	RECORD_NODE_REMOVED				= 0x07,
	RECORD_NODE_DATA_BEGIN			= 0x08,
	RECORD_NODE_STATE_READY			= 0x09,
	RECORD_NEW_DATA					= 0x0A,
	RECORD_END						= 0x0B,
	RECORD_NODE_ADDED_1_0_0_5		= 0x0C,
	RECORD_NODE_ADDED				= 0x0D,
	RECORD_SEEK_TABLE               = 0x0E,
};

#define INVALID_NODE_ID ((uint32_t)-1)
#define INVALID_TIMESTAMP ((uint64_t)-1)

struct RecordingHeader
{
	XnChar headerMagic[HEADER_MAGIC_SIZE];
	XnVersion version;
	uint64_t nGlobalMaxTimeStamp;
	uint32_t nMaxNodeID;
};

extern const RecordingHeader DEFAULT_RECORDING_HEADER;

class Record
{
public:
	Record(uint8_t* pData, size_t nMaxSize, XnBool bUseOld32Header);
	Record(const Record &other);
	RecordType GetType() const;
	uint32_t GetNodeID() const;
	uint32_t GetSize() const; //GetSize() returns just the fields' size, not including the payload
	uint32_t GetPayloadSize() const;
	uint64_t GetUndoRecordPos() const;

	void SetNodeID(uint32_t nNodeID);
	void SetPayloadSize(uint32_t nPayloadSize);
	void SetUndoRecordPos(uint64_t nUndoRecordPos);

	uint8_t* GetData(); //GetData() returns the entire encoded record
	const uint8_t* GetData() const; //GetData() returns the entire encoded record
	void SetData(uint8_t* pData, uint32_t nMaxSize);
	uint8_t* GetPayload(); //GetPayload() returns just the payload part of the record (after the fields)
	const uint8_t* GetPayload() const;
	const uint8_t* GetReadPos() const;
	XnBool IsHeaderValid() const;
	void ResetRead();

	XnStatus AsString(XnChar* strDest, uint32_t nSize, uint32_t& nCharsWritten);

protected:
	const uint8_t* GetWritePos() const;
	XnStatus StartWrite(uint32_t nRecordType);
	XnStatus Write(const void* pData, uint32_t nSize);
	XnStatus WriteString(const XnChar* str);
	XnStatus FinishWrite();

	XnStatus StartRead();

	//Copies the data to the specified output buffer
	XnStatus Read(void* pData, uint32_t nSize) const;

	//Gives a pointer to the string in the current position, and advances that position till after the string.
	XnStatus ReadString(const XnChar*& strDest) const;
	XnStatus FinishRead();

private:
	struct Header_old32
	{
		uint32_t m_nMagic;
		uint32_t m_nRecordType;
		uint32_t m_nNodeID;
		uint32_t m_nFieldsSize;
		uint32_t m_nPayloadSize;
		uint32_t m_nUndoRecordPos;
	};
	struct Header
	{
		uint32_t m_nMagic;
		uint32_t m_nRecordType;
		uint32_t m_nNodeID;
		uint32_t m_nFieldsSize;
		uint32_t m_nPayloadSize;
		uint64_t m_nUndoRecordPos;
	};

	union
	{
		Header* m_pHeader;
		uint8_t* m_pData;
	};

	static const uint32_t MAGIC;
	mutable uint32_t m_nReadOffset;
	uint32_t m_nMaxSize;
protected:
	enum {HEADER_SIZE_current = sizeof(Header),
	      HEADER_SIZE_old32   = sizeof(Header_old32)};
	XnBool m_bUseOld32Header;

public:
	uint32_t HEADER_SIZE;
};

class NodeAdded_1_0_0_4_Record : public Record
{
public:
	NodeAdded_1_0_0_4_Record(uint8_t* pData, uint32_t nMaxSize, XnBool bUseOld32Header);
	NodeAdded_1_0_0_4_Record(const Record& record);
	void SetNodeName(const XnChar* strNodeName);
	void SetNodeType(XnProductionNodeType type);
	void SetCompression(XnCodecID compression);

	const XnChar* GetNodeName() const;
	XnProductionNodeType GetNodeType() const;
	XnCodecID GetCompression() const;

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, uint32_t nSize, uint32_t& nCharsWritten);

protected:
	XnStatus EncodeImpl();
	XnStatus DecodeImpl();

private:
	const XnChar* m_strNodeName;
	XnProductionNodeType m_type;
	XnCodecID m_compression;
};

class NodeAdded_1_0_0_5_Record : public NodeAdded_1_0_0_4_Record
{
public:
	NodeAdded_1_0_0_5_Record(uint8_t* pData, uint32_t nMaxSize, XnBool bUseOld32Header);
	NodeAdded_1_0_0_5_Record(const Record& record);

	void SetNumberOfFrames(uint32_t nNumberOfFrames);
	void SetMinTimestamp(uint64_t nMinTimestamp);
	void SetMaxTimestamp(uint64_t nMaxTimestamp);

	uint32_t GetNumberOfFrames() const;
	uint64_t GetMinTimestamp() const;
	uint64_t GetMaxTimestamp() const;

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, uint32_t nSize, uint32_t& nCharsWritten);

protected:
	XnStatus EncodeImpl();
	XnStatus DecodeImpl();

private:
	uint32_t m_nNumberOfFrames;
	uint64_t m_nMinTimestamp;
	uint64_t m_nMaxTimestamp;
};

class NodeAddedRecord final : public NodeAdded_1_0_0_5_Record
{
public:
	NodeAddedRecord(uint8_t* pData, uint32_t nMaxSize, XnBool bUseOld32Header);
	NodeAddedRecord(const Record& record);

	void SetSeekTablePosition(uint64_t nPos);

	uint64_t GetSeekTablePosition();

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, uint32_t nSize, uint32_t& nCharsWritten);

private:
	uint64_t m_nSeekTablePosition;
};

class NodeRemovedRecord final : public Record
{
public:
	NodeRemovedRecord(uint8_t* pData, uint32_t nMaxSize, XnBool bUseOld32Header);
	NodeRemovedRecord(const Record& record);

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, uint32_t nSize, uint32_t& nCharsWritten);
};

class GeneralPropRecord : public Record
{
public:
	GeneralPropRecord(uint8_t* pData, uint32_t nMaxSize, XnBool bUseOld32Header, uint32_t nPropRecordType = RECORD_GENERAL_PROPERTY);
	GeneralPropRecord(const Record& record);

	void SetPropName(const XnChar* strPropName);
	void SetPropDataSize(uint32_t nPropDataSize);
	void SetPropData(const void* pPropData);

	const XnChar* GetPropName() const;
	uint32_t GetPropDataSize() const;
	const void* GetPropData() const;

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, uint32_t nSize, uint32_t& nCharsWritten);
private:
	uint32_t m_nPropRecordType;
	const XnChar* m_strPropName;
	uint32_t m_nPropDataSize;
	void* m_pPropData;
};

class IntPropRecord final : public GeneralPropRecord
{
public:
	IntPropRecord(uint8_t* pData, uint32_t nMaxSize, XnBool bUseOld32Header);
	IntPropRecord(const Record& record);
	void SetValue(uint64_t nValue);
	uint64_t GetValue() const;
	XnStatus AsString(XnChar* strDest, uint32_t nSize, uint32_t& nCharsWritten);

private:
	uint64_t m_nValue;
};

class RealPropRecord final : public GeneralPropRecord
{
public:
	RealPropRecord(uint8_t* pData, uint32_t nMaxSize, XnBool bUseOld32Header);
	RealPropRecord(const Record& record);
	void SetValue(double dValue);
	double GetValue() const;
	XnStatus AsString(XnChar* strDest, uint32_t nSize, uint32_t& nCharsWritten);

private:
	double m_dValue;
};

class StringPropRecord final : public GeneralPropRecord
{
public:
	StringPropRecord(uint8_t* pData, uint32_t nMaxSize, XnBool bUseOld32Header);
	StringPropRecord(const Record& record);
	void SetValue(const XnChar* strValue);
	const XnChar* GetValue() const;
	XnStatus AsString(XnChar* strDest, uint32_t nSize, uint32_t& nCharsWritten);
};

struct SeekInfo
{
	uint32_t m_nFrames;
	uint64_t m_nMaxTimeStamp;
};

class NodeDataBeginRecord final : public Record
{
public:
	NodeDataBeginRecord(uint8_t* pData, uint32_t nMaxSize, XnBool bUseOld32Header);
	NodeDataBeginRecord(const Record& record);

	uint32_t GetNumFrames() const;
	uint64_t GetMaxTimeStamp() const;

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, uint32_t nSize, uint32_t& nCharsWritten);

private:
	SeekInfo m_seekInfo;
};

class NodeStateReadyRecord final : public Record
{
public:
	NodeStateReadyRecord(uint8_t* pData, uint32_t nMaxSize, XnBool bUseOld32Header);
	NodeStateReadyRecord(const Record& record);
	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, uint32_t nSize, uint32_t& nCharsWritten);
};

/*This class represents only the data record HEADER, not the data itself.
  This is because we want to be able to read/write the data directly without copying it.*/
class NewDataRecordHeader final : public Record
{
public:
	enum {MAX_SIZE = Record::HEADER_SIZE_current + //Record header
	             (XN_MAX_NAME_LENGTH + 1) + //Max node name + terminating null
	             sizeof(uint64_t) + //Data timestamp
				 sizeof(uint32_t)}; //Frame number

	NewDataRecordHeader(uint8_t* pData, uint32_t nMaxSize, XnBool bUseOld32Header);
	NewDataRecordHeader(const Record& record);
	void SetTimeStamp(uint64_t nTimeStamp);
	void SetFrameNumber(uint32_t nFrameNumber);

	uint64_t GetTimeStamp() const;
	uint32_t GetFrameNumber() const;

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, uint32_t nSize, uint32_t& nCharsWritten);

private:
	uint64_t m_nTimeStamp;
	uint32_t m_nFrameNumber;
};

typedef struct
{
	uint64_t nTimestamp;
	uint32_t nConfigurationID;
	uint32_t nSeekPos;
} DataIndexEntry_old32;

typedef struct _DataIndexEntry
{
	uint64_t nTimestamp;
	uint32_t nConfigurationID;
	uint64_t nSeekPos;

	static void FillFromOld32Entry(struct _DataIndexEntry *newEntry, DataIndexEntry_old32 *old32Entry)
	{
		newEntry->nTimestamp 		= old32Entry->nTimestamp;
		newEntry->nConfigurationID 	= old32Entry->nConfigurationID;
		newEntry->nSeekPos 			= old32Entry->nSeekPos;
	}
} DataIndexEntry;

class DataIndexRecordHeader final : public Record
{
public:
	DataIndexRecordHeader(uint8_t* pData, uint32_t nMaxSize, XnBool bUseOld32Header);
	DataIndexRecordHeader(const Record& record);

	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, uint32_t nSize, uint32_t& nCharsWritten);
};

class EndRecord : public Record
{
public:
	EndRecord(uint8_t* pData, uint32_t nMaxSize, XnBool bUseOld32Header);
	EndRecord(const Record& record);
	XnStatus Encode();
	XnStatus Decode();
	XnStatus AsString(XnChar* strDest, uint32_t nSize, uint32_t& nCharsWritten);
};
#pragma pack(pop)

#endif // DATARECORDS_H
