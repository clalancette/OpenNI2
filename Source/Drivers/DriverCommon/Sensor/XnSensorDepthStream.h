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
#ifndef XNSENSORDEPTHSTREAM_H
#define XNSENSORDEPTHSTREAM_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <DDK/XnDepthStream.h>
#include "XnDeviceSensorProtocol.h"
#include "XnSensorStreamHelper.h"
#include <DepthUtils.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#if (XN_PLATFORM == XN_PLATFORM_LINUX_ARM || XN_PLATFORM == XN_PLATFORM_ANDROID_ARM)
	#define XN_DEPTH_STREAM_DEFAULT_INPUT_FORMAT				XN_IO_DEPTH_FORMAT_UNCOMPRESSED_12_BIT
	#define XN_DEPTH_STREAM_DEFAULT_RESOLUTION					XN_RESOLUTION_QQVGA
#else
	#define XN_DEPTH_STREAM_DEFAULT_INPUT_FORMAT				XN_IO_DEPTH_FORMAT_UNCOMPRESSED_11_BIT
	#define XN_DEPTH_STREAM_DEFAULT_RESOLUTION					XN_RESOLUTION_QVGA
#endif

#define XN_DEPTH_STREAM_DEFAULT_FPS							30
#define XN_DEPTH_STREAM_DEFAULT_OUTPUT_FORMAT				ONI_PIXEL_FORMAT_DEPTH_1_MM
#define XN_DEPTH_STREAM_DEFAULT_REGISTRATION				false
#define XN_DEPTH_STREAM_DEFAULT_REGISTRATION_TYPE			XN_PROCESSING_DONT_CARE
#define XN_DEPTH_STREAM_DEFAULT_HOLE_FILLER					true
#define XN_DEPTH_STREAM_DEFAULT_WHITE_BALANCE				true
#define XN_DEPTH_STREAM_DEFAULT_GAIN_OLD					50
#define XN_DEPTH_STREAM_DEFAULT_GMC_MODE					true
#define XN_DEPTH_STREAM_DEFAULT_CLOSE_RANGE					false
#define XN_DEPTH_STREAM_DEFAULT_SHIFT_MAP_APPENDED			true

#define XN_DEPTH_STREAM_DEFAULT_GMC_DEBUG					false
#define XN_DEPTH_STREAM_DEFAULT_WAVELENGTH_CORRECTION		false
#define XN_DEPTH_STREAM_DEFAULT_WAVELENGTH_CORRECTION_DEBUG	false

//---------------------------------------------------------------------------
// XnSensorDepthStream class
//---------------------------------------------------------------------------
class XnSensorDepthStream : public XnDepthStream, public IXnSensorStream
{
public:
	XnSensorDepthStream(const char* strName, XnSensorObjects* pObjects);
	~XnSensorDepthStream() { Free(); }

	//---------------------------------------------------------------------------
	// Overridden Methods
	//---------------------------------------------------------------------------
	XnStatus Init();
	XnStatus Free();
	XnStatus BatchConfig(const XnActualPropertiesHash& props) { return m_Helper.BatchConfig(props); }

	inline XnSensorStreamHelper* GetHelper() { return &m_Helper; }

	friend class XnDepthProcessor;
	friend class XnOniDepthStream;

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

	XnStatus ApplyRegistration(OniDepthPixel* pDetphmap);
	OniStatus GetSensorCalibrationInfo(void* data, int* dataSize);
	XnStatus PopulateSensorCalibrationInfo();

protected:
	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	XnStatus SetOutputFormat(OniPixelFormat nOutputFormat);
	XnStatus SetMirror(bool bIsMirrored);
	XnStatus SetResolution(XnResolutions nResolution);
	XnStatus SetFPS(uint32_t nFPS);
	virtual XnStatus SetInputFormat(XnIODepthFormats nInputFormat);
	virtual XnStatus SetRegistration(bool bRegistration);
	virtual XnStatus SetHoleFilter(bool bHoleFilter);
	virtual XnStatus SetWhiteBalance(bool bWhiteBalance);
	virtual XnStatus SetGain(uint32_t nGain);
	virtual XnStatus SetRegistrationType(XnProcessingType type);
	virtual XnStatus SetAGCBin(const XnDepthAGCBin* pBin);
	virtual XnStatus GetAGCBin(XnDepthAGCBin* pBin);
	XnStatus SetCropping(const OniCropping* pCropping);
	XnStatus SetActualRead(bool bRead);
	virtual XnStatus SetGMCMode(bool bGMCMode);
	virtual XnStatus SetCloseRange(bool bCloseRange);
	virtual XnStatus SetCroppingMode(XnCroppingMode mode);
	XnStatus GetImageCoordinatesOfDepthPixel(uint32_t x, uint32_t y, OniDepthPixel z, uint32_t imageXRes, uint32_t imageYRes, uint32_t& imageX, uint32_t& imageY);
	virtual XnStatus SetGMCDebug(bool bGMCDebug);
	virtual XnStatus SetWavelengthCorrection(bool bWavelengthCorrection);
	virtual XnStatus SetWavelengthCorrectionDebug(bool bWavelengthCorrectionDebug);

private:
	uint32_t CalculateExpectedSize();
	XnStatus DecideFirmwareRegistration(bool bRegistration, XnProcessingType registrationType, XnResolutions nRes);
	XnStatus DecidePixelSizeFactor();
	XnStatus SetCroppingImpl(const OniCropping* pCropping, XnCroppingMode mode);
	XnStatus CloseRangeControl(bool bEnabled);

	static XnStatus XN_CALLBACK_TYPE SetInputFormatCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetRegistrationCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetHoleFilterCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetWhiteBalanceCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetGainCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetRegistrationTypeCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetAGCBinCallback(XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetAGCBinCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetActualReadCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE DecideFirmwareRegistrationCallback(const XnProperty* pSender, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE DecidePixelSizeFactorCallback(const XnProperty* pSender, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE ReadAGCBinsFromFile(XnGeneralProperty* pSender, const char* csINIFile, const char* csSection);
	static XnStatus XN_CALLBACK_TYPE SetGMCModeCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetCloseRangeCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetCroppingModeCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE GetPixelRegistrationCallback(const XnGeneralProperty* pSender, const OniGeneralBuffer& gbValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetGMCDebugCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetWavelengthCorrectionCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetWavelengthCorrectionDebugCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);

	//---------------------------------------------------------------------------
	// Members
	//---------------------------------------------------------------------------
	XnSensorStreamHelper m_Helper;
	XnActualIntProperty m_InputFormat;
	XnActualIntProperty m_DepthRegistration;
	XnActualIntProperty m_HoleFilter;
	XnActualIntProperty m_WhiteBalance;
	XnActualIntProperty m_Gain;
	XnActualIntProperty m_RegistrationType;
	XnActualIntProperty m_CroppingMode;
	XnGeneralProperty m_AGCBin;

	XnActualIntProperty m_FirmwareMirror;
	XnActualIntProperty m_FirmwareRegistration;

	XnActualIntProperty m_FirmwareCropSizeX;
	XnActualIntProperty m_FirmwareCropSizeY;
	XnActualIntProperty m_FirmwareCropOffsetX;
	XnActualIntProperty m_FirmwareCropOffsetY;
	XnActualIntProperty m_FirmwareCropMode;

	XnActualIntProperty m_ActualRead;
	XnActualIntProperty m_GMCMode;
	XnActualIntProperty m_CloseRange;
	XnGeneralProperty m_PixelRegistration;

	XnActualRealProperty m_HorizontalFOV;
	XnActualRealProperty m_VerticalFOV;

	XnActualIntProperty m_GMCDebug;
	XnActualIntProperty m_WavelengthCorrection;
	XnActualIntProperty m_WavelengthCorrectionDebug;

	DepthUtilsHandle m_depthUtilsHandle;
	DepthUtilsSensorCalibrationInfo m_calibrationInfo;
	XnCallbackHandle m_hReferenceSizeChangedCallback;
};

#endif // XNSENSORDEPTHSTREAM_H
