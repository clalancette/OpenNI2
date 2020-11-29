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
#ifndef XNSENSORIRSTREAM_H
#define XNSENSORIRSTREAM_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <DDK/XnIRStream.h>
#include "XnSensorStreamHelper.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_IR_STREAM_DEFAULT_FPS				30
#define XN_IR_STREAM_DEFAULT_RESOLUTION			XN_RESOLUTION_QVGA
#define XN_IR_STREAM_DEFAULT_OUTPUT_FORMAT		ONI_PIXEL_FORMAT_GRAY16
#define XN_IR_STREAM_DEFAULT_MIRROR				FALSE

//---------------------------------------------------------------------------
// XnSensorIRStream class
//---------------------------------------------------------------------------
class XnSensorIRStream final : public XnIRStream, public IXnSensorStream
{
public:
	XnSensorIRStream(const XnChar* StreamName, XnSensorObjects* pObjects);
	~XnSensorIRStream() { Free(); }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Init() override;
	XnStatus Free() override;
	XnStatus BatchConfig(const XnActualPropertiesHash& props) { return m_Helper.BatchConfig(props); }

	inline XnSensorStreamHelper* GetHelper() { return &m_Helper; }

	friend class XnIRProcessor;

private:
	inline XnSensorFirmwareParams* GetFirmwareParams() const { return m_Helper.GetFirmware()->GetParams(); }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Open() { return m_Helper.Open(); }
	XnStatus Close() override { return m_Helper.Close(); }
	XnStatus CalcRequiredSize(uint32_t* pnRequiredSize) const override;
	XnStatus CropImpl(OniFrame* pFrame, const OniCropping* pCropping) override;
	XnStatus ConfigureStreamImpl() override;
	XnStatus OpenStreamImpl() override;
	XnStatus CloseStreamImpl() override;
	XnStatus CreateDataProcessor(XnDataProcessor** ppProcessor) override;
	XnStatus MapPropertiesToFirmware() override;
	void GetFirmwareStreamConfig(XnResolutions* pnRes, uint32_t* pnFPS) override { *pnRes = GetResolution(); *pnFPS = GetFPS(); }

	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	XnStatus SetOutputFormat(OniPixelFormat nOutputFormat);
	XnStatus SetResolution(XnResolutions nResolution) override;
	XnStatus SetFPS(uint32_t nFPS) override;
	XnStatus SetCropping(const OniCropping* pCropping) override;
	XnStatus SetCroppingMode(XnCroppingMode mode);
	XnStatus SetActualRead(XnBool bRead);

	XnStatus OnIsMirroredChanged();
	XnStatus SetCroppingImpl(const OniCropping* pCropping, XnCroppingMode mode);

	XnStatus FixFirmwareBug();

	static XnStatus XN_CALLBACK_TYPE IsMirroredChangedCallback(const XnProperty* pSender, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetActualReadCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetCroppingModeCallback(XnActualIntProperty* pSender, XnUInt64 nValue, void* pCookie);

	XnActualIntProperty m_InputFormat;
	XnActualIntProperty m_CroppingMode;

	XnSensorStreamHelper m_Helper;
	XnActualIntProperty m_FirmwareCropSizeX;
	XnActualIntProperty m_FirmwareCropSizeY;
	XnActualIntProperty m_FirmwareCropOffsetX;
	XnActualIntProperty m_FirmwareCropOffsetY;
	XnActualIntProperty m_FirmwareCropMode;

	XnActualIntProperty m_ActualRead;
};


#endif // XNSENSORIRSTREAM_H
