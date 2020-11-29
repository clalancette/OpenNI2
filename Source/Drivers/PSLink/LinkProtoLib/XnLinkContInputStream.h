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
#ifndef XNLINKCONTINPUTSTREAM_H
#define XNLINKCONTINPUTSTREAM_H

#include "XnLinkMsgParser.h"
#include "XnLinkInputStream.h"
#include <XnStatus.h>
#include <XnEvent.h>
#include <XnDump.h>

#include "XnLinkLogParser.h"

typedef uint32_t XnStreamFormat;

namespace xn
{

class LinkContInputStream : public LinkInputStream
{
public:
	LinkContInputStream();
	virtual ~LinkContInputStream();
    virtual XnStatus Init(LinkControlEndpoint* pLinkControlEndpoint,
                          XnStreamType streamType,
                          uint16_t nStreamID, 
                          IConnection* pConnection);
    //LinkInputStream methods
    virtual bool IsInitialized() const;
	virtual void Shutdown();
	virtual XnStatus HandlePacket(const LinkPacketHeader& header, const uint8_t* pData, bool& bPacketLoss);
	virtual const void* GetData() const;
	virtual uint32_t GetDataSize() const;
	virtual uint64_t GetTimestamp() const { return 0; }
	virtual const void* GetNextData() const;
	virtual uint32_t GetNextDataSize() const;
	virtual uint64_t GetNextTimestamp() const { return 0; }
	virtual bool IsNewDataAvailable() const;
	virtual XnStatus UpdateData();

    virtual bool IsStreaming() const;

	virtual XnStreamFragLevel GetStreamFragLevel() const { return XN_LINK_STREAM_FRAG_LEVEL_CONTINUOUS; }

	typedef void (XN_CALLBACK_TYPE* NewDataAvailableHandler)(void* pCookie);
	virtual XnStatus RegisterToNewDataAvailable(NewDataAvailableHandler pHandler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromNewDataAvailable(XnCallbackHandle hCallback);

    //Other methods
    virtual void SetDumpName(const char* strDumpName);
    virtual void SetDumpOn(bool bDumpOn);

protected:
	virtual XnStatus StartImpl();
	virtual XnStatus StopImpl();

private:
	LinkLogParser m_logParser;

	static const uint32_t CONT_STREAM_PREDEFINED_BUFFER_SIZE;
	mutable XN_CRITICAL_SECTION_HANDLE m_hCriticalSection; //Protects buffers info
	bool m_bNewDataAvailable;
    bool m_bInitialized;
	bool m_bStreaming;
	
	uint32_t m_nUserBufferMaxSize;
	uint32_t m_nUserBufferCurrentSize;
	uint8_t* m_pUserBuffer;

	uint32_t m_nWorkingBufferCurrentSize;
	uint8_t* m_pWorkingBuffer;

	char m_strDumpName[XN_FILE_MAX_PATH];
	XnDumpFile* m_pDumpFile;

	xnl::EventNoArgs m_newDataAvailableEvent;
};

}

#endif // XNLINKCONTINPUTSTREAM_H
