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
#ifndef PLAYERNODE_H
#define PLAYERNODE_H

#include "DataRecords.h"
#include "XnPlayerTypes.h"
#include "Formats/XnCodecIDs.h"
#include "Formats/XnStreamFormats.h"
#include "XnHash.h"
#include "XnEvent.h"
#include "XnStringsHash.h"

class XnCodec;

namespace oni_file {

class PlayerNode final
{
public:

	/** Prototype for state change callback function. **/
	typedef void (XN_CALLBACK_TYPE* EndOfFileReachedHandler)(void* pCookie);

	typedef struct
	{
		XnStatus (XN_CALLBACK_TYPE* Create)(void* pCookie, const char* strNodeName, XnCodecID nCodecId, XnCodec** ppCodec);
		void     (XN_CALLBACK_TYPE* Destroy)(void* pCookie, XnCodec* pCodec);
	} CodecFactory;

	PlayerNode(const XnChar* strName);
	~PlayerNode();

	//public functions
	XnStatus Init();
	XnStatus Destroy();

	//xn::ModulePlayer implementation
	XnStatus SetInputStream(void* pStreamCookie, XnPlayerInputStreamInterface* pStream);
	XnStatus ReadNext();
	XnStatus SetNodeNotifications(void* pNotificationsCookie, XnNodeNotifications* pNodeNotifications);
	XnStatus SetNodeCodecFactory(void* pFactoryCookie, PlayerNode::CodecFactory* pPlayerNodeCodecFactory);
	XnStatus SetRepeat(XnBool bRepeat);
	XnStatus SeekToTimeStamp(int64_t nTimeOffset, XnPlayerSeekOrigin origin);

	XnStatus SeekToFrame(const XnChar* strNodeName, int32_t nFrameOffset, XnPlayerSeekOrigin origin);
	XnStatus TellTimestamp(uint64_t& nTimestamp);
	XnStatus TellFrame(const XnChar* strNodeName, uint32_t& nFrameNumber);
	uint32_t GetNumFrames(const XnChar* strNodeName, uint32_t& nFrames);

	const XnChar* GetSupportedFormat();
	XnBool IsEOF();
	XnStatus RegisterToEndOfFileReached(EndOfFileReachedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	void UnregisterFromEndOfFileReached(XnCallbackHandle hCallback);

	static XnStatus ValidateStream(void *pStreamCookie, XnPlayerInputStreamInterface* pInputStream);

private:
	struct RecordUndoInfo
	{
		RecordUndoInfo() { Reset(); }
		void Reset() { nRecordPos = 0; nUndoRecordPos = 0; }
		uint64_t nRecordPos;
		uint64_t nUndoRecordPos;
	};

	typedef xnl::XnStringsHashT<RecordUndoInfo> RecordUndoInfoMap;

	struct PlayerNodeInfo
	{
		PlayerNodeInfo();
		~PlayerNodeInfo();

		void Reset();

		XnBool bValid;
		XnChar strName[XN_MAX_NAME_LENGTH];
		uint64_t nLastDataPos;
		XnCodecID compression;
		uint32_t nFrames;
		uint32_t nCurFrame;
		uint64_t nMaxTimeStamp;
		XnBool bStateReady;
		XnBool bIsGenerator;
		XnCodec* pCodec;
		RecordUndoInfoMap recordUndoInfoMap;
		RecordUndoInfo newDataUndoInfo;
		DataIndexEntry* pDataIndex;
	};

	XnStatus ProcessRecord(XnBool bProcessPayload);
	XnStatus SeekToTimeStampAbsolute(uint64_t nDestTimeStamp);
	XnStatus SeekToTimeStampRelative(int64_t nOffset);
	XnStatus UndoRecord(PlayerNode::RecordUndoInfo& undoInfo, uint64_t nDestPos, XnBool& nUndone);
	XnStatus SeekToFrameAbsolute(uint32_t nNodeID, uint32_t nFrameNumber);
	XnStatus ProcessEachNodeLastData(uint32_t nIDToProcessLast);

	static int32_t CompareVersions(const XnVersion* pV0, const XnVersion* pV1);
	XnStatus OpenStream();
	XnStatus Read(void* pData, uint32_t nSize, uint32_t& nBytesRead);
	XnStatus ReadRecordHeader(Record& record);
	XnStatus ReadRecordFields(Record& record);
	//ReadRecord reads just the fields of the record, not the payload.
	XnStatus ReadRecord(Record& record);
	XnStatus SeekStream(XnOSSeekType seekType, int64_t nOffset);
	uint64_t TellStream();
	XnStatus CloseStream();

	XnBool IsTypeGenerator(XnProductionNodeType type);

	XnStatus HandleRecord(Record& record, XnBool bHandleRecord);
	XnStatus HandleNodeAddedImpl(uint32_t nNodeID, XnProductionNodeType type, const XnChar* strName, XnCodecID compression, uint32_t nNumberOfFrames, uint64_t nMinTimestamp, uint64_t nMaxTimestamp);
	XnStatus HandleNodeAddedRecord(NodeAddedRecord record);
	XnStatus HandleGeneralPropRecord(GeneralPropRecord record);
	XnStatus HandleIntPropRecord(IntPropRecord record);
	XnStatus HandleRealPropRecord(RealPropRecord record);
	XnStatus HandleStringPropRecord(StringPropRecord record);
	XnStatus HandleNodeRemovedRecord(NodeRemovedRecord record);
	XnStatus HandleNodeStateReadyRecord(NodeStateReadyRecord record);
	XnStatus HandleNodeDataBeginRecord(NodeDataBeginRecord record);
	XnStatus HandleNewDataRecord(NewDataRecordHeader record, XnBool bHandleRecord);
	XnStatus HandleDataIndexRecord(DataIndexRecordHeader record, XnBool bReadPayload);
	XnStatus HandleEndRecord(EndRecord record);
	XnStatus Rewind();
	XnStatus ProcessUntilFirstData();
	PlayerNodeInfo* GetPlayerNodeInfo(uint32_t nNodeID);
	XnStatus RemovePlayerNodeInfo(uint32_t nNodeID);
	uint32_t GetPlayerNodeIDByName(const XnChar* strNodeName);
	PlayerNodeInfo* GetPlayerNodeInfoByName(const XnChar* strNodeName);
	XnStatus SaveRecordUndoInfo(PlayerNodeInfo* pPlayerNodeInfo, const XnChar* strPropName, uint64_t nRecordPos, uint64_t nUndoRecordPos);
	XnStatus GetRecordUndoInfo(PlayerNodeInfo* pPlayerNodeInfo, const XnChar* strPropName, uint64_t& nRecordPos, uint64_t& nUndoRecordPos);
	XnStatus SkipRecordPayload(Record record);
	XnStatus SeekToRecordByType(uint32_t nNodeID, RecordType type);
	DataIndexEntry* FindTimestampInDataIndex(uint32_t nNodeID, uint64_t nTimestamp);
	DataIndexEntry** GetSeekLocationsFromDataIndex(uint32_t nNodeID, uint32_t nDestFrame);

	// BC functions
	XnStatus HandleNodeAdded_1_0_0_5_Record(NodeAdded_1_0_0_5_Record record);
	XnStatus HandleNodeAdded_1_0_0_4_Record(NodeAdded_1_0_0_4_Record record);

	static const uint64_t DATA_MAX_SIZE;
	static const uint64_t RECORD_MAX_SIZE;
	static const XnVersion OLDEST_SUPPORTED_FILE_FORMAT_VERSION;
	static const XnVersion FIRST_FILESIZE64BIT_FILE_FORMAT_VERSION;

	XnVersion m_fileVersion;
	XnChar m_strName[XN_MAX_NAME_LENGTH];
	XnBool m_bOpen;
	XnBool m_bIs32bitFileFormat;
	uint8_t* m_pRecordBuffer;
	uint8_t* m_pUncompressedData;
	void* m_pStreamCookie;
	XnPlayerInputStreamInterface* m_pInputStream;
	void* m_pNotificationsCookie;
	XnNodeNotifications* m_pNodeNotifications;
	void* m_pNodeCodecFactoryCookie;
	PlayerNode::CodecFactory* m_pNodeCodecFactory;
	XnBool m_bRepeat;
	XnBool m_bDataBegun;
	XnBool m_bEOF;

	uint64_t m_nTimeStamp;
	uint64_t m_nGlobalMaxTimeStamp;

	xnl::EventNoArgs m_eofReachedEvent;

	PlayerNodeInfo* m_pNodeInfoMap;
	uint32_t m_nMaxNodes;

	DataIndexEntry** m_aSeekTempArray;

	XnMapOutputMode m_lastOutputMode;
};

}

#endif // PLAYERNODE_H
