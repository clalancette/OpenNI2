/*****************************************************************************
*									     *
*  OpenNI 2.x Alpha							     *
*  Copyright (C) 2012 PrimeSense Ltd.					     *
*									     *
*  This file is part of OpenNI. 					     *
*									     *
*  Licensed under the Apache License, Version 2.0 (the "License");	     *
*  you may not use this file except in compliance with the License.	     *
*  You may obtain a copy of the License at				     *
*									     *
*      http://www.apache.org/licenses/LICENSE-2.0			     *
*									     *
*  Unless required by applicable law or agreed to in writing, software	     *
*  distributed under the License is distributed on an "AS IS" BASIS,	     *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and	     *
*  limitations under the License.					     *
*									     *
*****************************************************************************/
#ifndef XNSENSORIMAGESTREAM_H
#define XNSENSORIMAGESTREAM_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <DDK/XnImageStream.h>
#include "XnSensorStreamHelper.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_IMAGE_STREAM_DEFAULT_FPS			30
#define XN_IMAGE_STREAM_DEFAULT_RESOLUTION		XN_RESOLUTION_QVGA
#if (XN_PLATFORM == XN_PLATFORM_WIN32)
	#define XN_IMAGE_STREAM_DEFAULT_INPUT_FORMAT	XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422
#else
	//non win32 works with BULK, so it's better to compress the image
	#define XN_IMAGE_STREAM_DEFAULT_INPUT_FORMAT	XN_IO_IMAGE_FORMAT_YUV422
#endif
#define XN_IMAGE_STREAM_DEFAULT_OUTPUT_FORMAT		ONI_PIXEL_FORMAT_RGB888
#define XN_IMAGE_STREAM_DEFAULT_FLICKER 		0
#define XN_IMAGE_STREAM_DEFAULT_QUALITY 		3
#define XN_IMAGE_STREAM_DEFAULT_BRIGHTNESS		128
#define XN_IMAGE_STREAM_DEFAULT_CONTRAST		32
#define XN_IMAGE_STREAM_DEFAULT_SATURATION		128
#define XN_IMAGE_STREAM_DEFAULT_SHARPNESS		32
#define XN_IMAGE_STREAM_DEFAULT_AWB			true
#define XN_IMAGE_STREAM_DEFAULT_COLOR_TEMP		5000
#define XN_IMAGE_STREAM_DEFAULT_BACKLIGHT_COMP		1
#define XN_IMAGE_STREAM_DEFAULT_GAIN			100
#define XN_IMAGE_STREAM_DEFAULT_ZOOM			100
#define XN_IMAGE_STREAM_DEFAULT_AUTO_EXPOSURE		true
#define XN_IMAGE_STREAM_DEFAULT_EXPOSURE_BAR		0
#define XN_IMAGE_STREAM_DEFAULT_PAN			0
#define XN_IMAGE_STREAM_DEFAULT_TILT			0
#define XN_IMAGE_STREAM_DEFAULT_LOW_LIGHT_COMP		true

//---------------------------------------------------------------------------
// XnSensorImageStream class
//---------------------------------------------------------------------------
class XnSensorImageStream : public XnImageStream, public IXnSensorStream
{
public:
	XnSensorImageStream(const char* StreamName, XnSensorObjects* pObjects);
	~XnSensorImageStream() { Free(); }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Init();
	XnStatus Free();
	XnStatus BatchConfig(const XnActualPropertiesHash& props) { return m_Helper.BatchConfig(props); }

	uint32_t CalculateExpectedSize();

	inline XnSensorStreamHelper* GetHelper() { return &m_Helper; }

	friend class XnImageProcessor;

protected:
	inline XnSensorFirmwareParams* GetFirmwareParams() const { return m_Helper.GetFirmware()->GetParams(); }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Open() { return m_Helper.Open(); }
	XnStatus Close() { return m_Helper.Close(); }
	XnStatus CropImpl(OniFrame* pFrame, const OniCropping* pCropping);
	XnStatus Mirror(OniFrame* pFrame) const;
	XnStatus ConfigureStreamImpl();
	XnStatus OpenStreamImpl();
	XnStatus CloseStreamImpl();
	XnStatus CreateDataProcessor(XnDataProcessor** ppProcessor);
	XnStatus MapPropertiesToFirmware();
	void GetFirmwareStreamConfig(XnResolutions* pnRes, uint32_t* pnFPS) { *pnRes = GetResolution(); *pnFPS = GetFPS(); }

	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	XnStatus SetOutputFormat(OniPixelFormat nOutputFormat);
	XnStatus SetMirror(bool bIsMirrored);
	XnStatus SetResolution(XnResolutions nResolution);
	XnStatus SetFPS(uint32_t nFPS);
	virtual XnStatus SetInputFormat(XnIOImageFormats nInputFormat);
	virtual XnStatus SetAntiFlicker(uint32_t nFrequency);
	virtual XnStatus SetImageQuality(uint32_t nQuality);
	virtual XnStatus SetCropping(const OniCropping* pCropping);
	virtual XnStatus SetCroppingMode(XnCroppingMode mode);
	XnStatus SetActualRead(bool bRead);
	virtual XnStatus SetAutoExposure(bool bAutoExposure);
	virtual XnStatus SetAutoWhiteBalance(bool bAutoWhiteBalance);
	virtual XnStatus SetExposure(uint64_t nValue);
	virtual XnStatus SetGain(uint64_t nValue);
	virtual XnStatus SetFastZoomCrop(bool bFastZoomCrop);
private:
	XnStatus ValidateMode();
	XnStatus SetCroppingImpl(const OniCropping* pCropping, XnCroppingMode mode);
	XnStatus SetAutoExposureForOldFirmware(bool bAutoExposure);
	XnStatus SetAutoWhiteBalanceForOldFirmware(bool bAutoWhiteBalance);

	static XnStatus XN_CALLBACK_TYPE SetInputFormatCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetAntiFlickerCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetImageQualityCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetActualReadCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetCroppingModeCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetAutoExposureCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetAutoWhiteBalanceCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetExposureCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetGainCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetFastZoomCropCallback(XnActualIntProperty* pSendoer, uint64_t nValue, void* pCookie);

	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnSensorStreamHelper m_Helper;
	XnActualIntProperty m_InputFormat;
	XnActualIntProperty m_AntiFlicker;
	XnActualIntProperty m_ImageQuality;
	XnActualIntProperty m_CroppingMode;

	XnActualIntProperty m_FirmwareMirror;

	XnActualIntProperty m_FirmwareCropSizeX;
	XnActualIntProperty m_FirmwareCropSizeY;
	XnActualIntProperty m_FirmwareCropOffsetX;
	XnActualIntProperty m_FirmwareCropOffsetY;
	XnActualIntProperty m_FirmwareCropMode;

	XnActualIntProperty m_AutoExposure;
	XnActualIntProperty m_AutoWhiteBalance;
	XnActualIntProperty m_Exposure;
	XnActualIntProperty m_Gain;
	XnActualIntProperty m_FastZoomCrop;

	XnActualIntProperty m_ActualRead;

	XnActualRealProperty m_HorizontalFOV;
	XnActualRealProperty m_VerticalFOV;
};

#endif // XNSENSORIMAGESTREAM_H
