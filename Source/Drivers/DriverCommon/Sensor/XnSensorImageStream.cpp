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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnDeviceSensorInit.h"
#include "XnSensorImageStream.h"
#include "XnSensor.h"
#include "XnBayerImageProcessor.h"
#include "XnUncompressedBayerProcessor.h"
#include "XnPSCompressedImageProcessor.h"
#include "XnJpegImageProcessor.h"
#include "XnJpegToRGBImageProcessor.h"
#include "XnPassThroughImageProcessor.h"
#include "XnUncompressedYUV422toRGBImageProcessor.h"
#include "XnUncompressedYUYVtoRGBImageProcessor.h"
#include "YUV.h"
#include "Bayer.h"
#include <XnProfiling.h>
#include <math.h>
#include <XnFormatsStatus.h>

//---------------------------------------------------------------------------
// XnSensorImageStream class
//---------------------------------------------------------------------------
XnSensorImageStream::XnSensorImageStream(const char* StreamName, XnSensorObjects* pObjects) :
	XnImageStream(StreamName, false),
	m_Helper(pObjects),
	m_InputFormat(XN_STREAM_PROPERTY_INPUT_FORMAT, "InputFormat", XN_IMAGE_STREAM_DEFAULT_INPUT_FORMAT),
	m_AntiFlicker(XN_STREAM_PROPERTY_FLICKER, "Flicker", XN_IMAGE_STREAM_DEFAULT_FLICKER),
	m_ImageQuality(XN_STREAM_PROPERTY_QUALITY, "Quality", XN_IMAGE_STREAM_DEFAULT_QUALITY),
	m_CroppingMode(XN_STREAM_PROPERTY_CROPPING_MODE, "CroppingMode", XN_CROPPING_MODE_NORMAL),
	m_FirmwareMirror(0, "FirmwareMirror", false, StreamName),
	m_FirmwareCropSizeX(0, "FirmwareCropSizeX", 0, StreamName),
	m_FirmwareCropSizeY(0, "FirmwareCropSizeY", 0, StreamName),
	m_FirmwareCropOffsetX(0, "FirmwareCropOffsetX", 0, StreamName),
	m_FirmwareCropOffsetY(0, "FirmwareCropOffsetY", 0, StreamName),
	m_FirmwareCropMode(0, "FirmwareCropMode", XN_FIRMWARE_CROPPING_MODE_DISABLED, StreamName),

	m_AutoExposure(ONI_STREAM_PROPERTY_AUTO_EXPOSURE, "AutoExposure", XN_IMAGE_STREAM_DEFAULT_AUTO_EXPOSURE),
	m_AutoWhiteBalance(ONI_STREAM_PROPERTY_AUTO_WHITE_BALANCE, "AutoWhiteBalance", XN_IMAGE_STREAM_DEFAULT_AWB),

	m_Exposure(ONI_STREAM_PROPERTY_EXPOSURE, "Exposure", XN_IMAGE_STREAM_DEFAULT_EXPOSURE_BAR),
	m_Gain(ONI_STREAM_PROPERTY_GAIN, "Gain", XN_IMAGE_STREAM_DEFAULT_GAIN),
	m_FastZoomCrop(XN_STREAM_PROPERTY_FAST_ZOOM_CROP, "FastZoomCrop", false),

	m_ActualRead(XN_STREAM_PROPERTY_ACTUAL_READ_DATA, "ActualReadData", false),
	m_HorizontalFOV(ONI_STREAM_PROPERTY_HORIZONTAL_FOV, "HorizontalFov"),
	m_VerticalFOV(ONI_STREAM_PROPERTY_VERTICAL_FOV, "VerticalFov")
{
}

XnStatus XnSensorImageStream::Init()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// init base
	nRetVal = XnImageStream::Init();
	XN_IS_STATUS_OK(nRetVal);

	m_InputFormat.UpdateSetCallback(SetInputFormatCallback, this);
	m_AntiFlicker.UpdateSetCallback(SetAntiFlickerCallback, this);
	m_ImageQuality.UpdateSetCallback(SetImageQualityCallback, this);
	m_CroppingMode.UpdateSetCallback(SetCroppingModeCallback, this);
	m_AutoExposure.UpdateSetCallback(SetAutoExposureCallback, this);
	m_Exposure.UpdateSetCallback(SetExposureCallback, this);
	m_Gain.UpdateSetCallback(SetGainCallback, this);
	m_FastZoomCrop.UpdateSetCallback(SetFastZoomCropCallback, this);
	m_AutoWhiteBalance.UpdateSetCallback(SetAutoWhiteBalanceCallback, this);
	m_ActualRead.UpdateSetCallback(SetActualReadCallback, this);

	// add properties
	XN_VALIDATE_ADD_PROPERTIES(this, &m_InputFormat, &m_AntiFlicker, &m_ImageQuality,
		&m_CroppingMode, &m_ActualRead, &m_HorizontalFOV, &m_VerticalFOV, &m_AutoExposure, &m_AutoWhiteBalance, &m_Exposure, &m_Gain, &m_FastZoomCrop);

	// set base properties default values
	nRetVal = ResolutionProperty().UnsafeUpdateValue(XN_IMAGE_STREAM_DEFAULT_RESOLUTION);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = FPSProperty().UnsafeUpdateValue(XN_IMAGE_STREAM_DEFAULT_FPS);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = OutputFormatProperty().UnsafeUpdateValue(XN_IMAGE_STREAM_DEFAULT_OUTPUT_FORMAT);
	XN_IS_STATUS_OK(nRetVal);

	double fZPPS = m_Helper.GetFixedParams()->GetZeroPlanePixelSize();
	int nZPD = m_Helper.GetFixedParams()->GetZeroPlaneDistance();

	nRetVal = m_HorizontalFOV.UnsafeUpdateValue(2*atan(fZPPS*XN_SXGA_X_RES/2/nZPD));
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_VerticalFOV.UnsafeUpdateValue(2*atan(fZPPS*XN_VGA_Y_RES*2/2/nZPD));
	XN_IS_STATUS_OK(nRetVal);

	// init helper
	nRetVal = m_Helper.Init(this, this);
	XN_IS_STATUS_OK(nRetVal);

	// data processor
	nRetVal = m_Helper.RegisterDataProcessorProperty(m_InputFormat);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.RegisterDataProcessorProperty(ResolutionProperty());
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.RegisterDataProcessorProperty(ResolutionProperty());
	XN_IS_STATUS_OK(nRetVal);

	// register supported modes
	nRetVal = AddSupportedModes(m_Helper.GetPrivateData()->FWInfo.imageModes.data(), m_Helper.GetPrivateData()->FWInfo.imageModes.size());
	XN_IS_STATUS_OK(nRetVal);

	// check if our current (default) configuration is valid
	uint16_t nValidInputFormat = XN_IMAGE_STREAM_DEFAULT_INPUT_FORMAT;
	bool bModeFound = false;

	const std::vector<XnCmosPreset>& aSupportedModes = GetSupportedModes();

	for (uint32_t i = 0; i < aSupportedModes.size(); ++i)
	{
		if (aSupportedModes[i].nResolution == XN_IMAGE_STREAM_DEFAULT_RESOLUTION &&
			aSupportedModes[i].nFPS == XN_IMAGE_STREAM_DEFAULT_FPS)
		{
			// found
			if (!bModeFound)
			{
				bModeFound = true;
				nValidInputFormat = aSupportedModes[i].nFormat;
			}

			if (aSupportedModes[i].nFormat == XN_IMAGE_STREAM_DEFAULT_INPUT_FORMAT)
			{
				nValidInputFormat = XN_IMAGE_STREAM_DEFAULT_INPUT_FORMAT;
				break;
			}
		}
	}

	if (!bModeFound)
	{
		xnLogWarning(XN_MASK_DEVICE_SENSOR, "Default mode (res + FPS) is not supported by device. Changing defaults...");

		nRetVal = ResolutionProperty().UnsafeUpdateValue(aSupportedModes[0].nResolution);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = FPSProperty().UnsafeUpdateValue(aSupportedModes[0].nFPS);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = m_InputFormat.UnsafeUpdateValue(aSupportedModes[0].nFormat);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		// just update input format
		nRetVal = m_InputFormat.UnsafeUpdateValue(nValidInputFormat);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::Free()
{
	m_Helper.Free();
	XnImageStream::Free();
	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::MapPropertiesToFirmware()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_Helper.MapFirmwareProperty(m_InputFormat, GetFirmwareParams()->m_ImageFormat, false);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(ResolutionProperty(), GetFirmwareParams()->m_ImageResolution, false);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(FPSProperty(), GetFirmwareParams()->m_ImageFPS, false);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_AntiFlicker, GetFirmwareParams()->m_ImageFlickerDetection, true);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_ImageQuality, GetFirmwareParams()->m_ImageQuality, true);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareMirror, GetFirmwareParams()->m_ImageMirror, true);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareCropSizeX, GetFirmwareParams()->m_ImageCropSizeX, true);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareCropSizeY, GetFirmwareParams()->m_ImageCropSizeY, true);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareCropOffsetX, GetFirmwareParams()->m_ImageCropOffsetX, true);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareCropOffsetY, GetFirmwareParams()->m_ImageCropOffsetY, true);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FirmwareCropMode, GetFirmwareParams()->m_ImageCropMode, true);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_AutoExposure, GetFirmwareParams()->m_ImageAutoExposure, true);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_AutoWhiteBalance, GetFirmwareParams()->m_ImageAutoWhiteBalance, true);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_Exposure, GetFirmwareParams()->m_ImageExposureBar, true);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_Gain, GetFirmwareParams()->m_ImageGain, true);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.MapFirmwareProperty(m_FastZoomCrop, GetFirmwareParams()->m_FastZoomCrop, true);
	XN_IS_STATUS_OK(nRetVal);;

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::ValidateMode()
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validity checks
	XnIOImageFormats nInputFormat = (XnIOImageFormats)m_InputFormat.GetValue();
	OniPixelFormat nOutputFormat = GetOutputFormat();
	XnResolutions nResolution = GetResolution();
	uint32_t nFPS = GetFPS();

	// check that input format matches output format
	switch (nOutputFormat)
	{
	case ONI_PIXEL_FORMAT_RGB888:
		if (nInputFormat != XN_IO_IMAGE_FORMAT_YUV422 &&
			nInputFormat != XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422 &&
			nInputFormat != XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUYV &&
			nInputFormat != XN_IO_IMAGE_FORMAT_BAYER &&
			nInputFormat != XN_IO_IMAGE_FORMAT_UNCOMPRESSED_BAYER)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Input format %d cannot be converted to RGB24!", nInputFormat);
		}
		break;
	case ONI_PIXEL_FORMAT_YUV422:
		if (nInputFormat != XN_IO_IMAGE_FORMAT_YUV422 &&
			nInputFormat != XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Input format %d cannot be converted to YUV422!", nInputFormat);
		}
		break;
	case ONI_PIXEL_FORMAT_YUYV:
		if (nInputFormat != XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUYV)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Input format %d cannot be converted to YUYV!", nInputFormat);
		}
		break;
	case ONI_PIXEL_FORMAT_JPEG:
		if (nInputFormat != XN_IO_IMAGE_FORMAT_JPEG)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Input format %d cannot be converted to JPEG!", nInputFormat);
		}
		break;
	case ONI_PIXEL_FORMAT_GRAY8:
		if (nInputFormat != XN_IO_IMAGE_FORMAT_UNCOMPRESSED_BAYER &&
			nInputFormat != XN_IO_IMAGE_FORMAT_BAYER)
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Input format %d cannot be converted to Gray8!", nInputFormat);
		}
		break;
	default:
		// we shouldn't have reached here. Theres a check at SetOutputFormat.
		XN_ASSERT(false);
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Unsupported image output format: %d!", nOutputFormat);
	}

	// now check that mode exists
	XnCmosPreset preset = { (uint16_t)nInputFormat, (uint16_t)nResolution, (uint16_t)nFPS };
	nRetVal = ValidateSupportedMode(preset);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::ConfigureStreamImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;

	xnUSBShutdownReadThread(GetHelper()->GetPrivateData()->pSpecificImageUsb->pUsbConnection->UsbEp);

	nRetVal = SetActualRead(true);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = ValidateMode();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.ConfigureFirmware(m_InputFormat);
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.ConfigureFirmware(ResolutionProperty());
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.ConfigureFirmware(FPSProperty());
	XN_IS_STATUS_OK(nRetVal);;
	nRetVal = m_Helper.ConfigureFirmware(m_AntiFlicker);
	XN_IS_STATUS_OK(nRetVal);;

	// image quality is only relevant for JPEG
	if (m_InputFormat.GetValue() == XN_IO_IMAGE_FORMAT_JPEG)
	{
		nRetVal = m_Helper.ConfigureFirmware(m_ImageQuality);
		XN_IS_STATUS_OK(nRetVal);;
	}

	nRetVal = m_Helper.ConfigureFirmware(m_FirmwareMirror);
	XN_IS_STATUS_OK(nRetVal);;

	if (GetResolution() != XN_RESOLUTION_UXGA && GetResolution() != XN_RESOLUTION_SXGA)
	{
		nRetVal = m_Helper.GetCmosInfo()->SetCmosConfig(XN_CMOS_TYPE_IMAGE, GetResolution(), GetFPS());
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetActualRead(bool bRead)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if ((bool)m_ActualRead.GetValue() != bRead)
	{
		if (bRead)
		{
			xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Creating USB image read thread...");
			XnSpecificUsbDevice* pUSB = GetHelper()->GetPrivateData()->pSpecificImageUsb;
			nRetVal = xnUSBInitReadThread(pUSB->pUsbConnection->UsbEp, pUSB->nChunkReadBytes, pUSB->nNumberOfBuffers, pUSB->nTimeout, XnDeviceSensorProtocolUsbEpCb, pUSB);
			XN_IS_STATUS_OK(nRetVal);
		}
		else
		{
			xnLogVerbose(XN_MASK_DEVICE_SENSOR, "Shutting down USB image read thread...");
			xnUSBShutdownReadThread(GetHelper()->GetPrivateData()->pSpecificImageUsb->pUsbConnection->UsbEp);
		}

		nRetVal = m_ActualRead.UnsafeUpdateValue(bRead);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::OpenStreamImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = GetFirmwareParams()->m_Stream0Mode.SetValue(XN_VIDEO_STREAM_COLOR);
	XN_IS_STATUS_OK(nRetVal);

	// Cropping
	if (m_FirmwareCropMode.GetValue() != XN_FIRMWARE_CROPPING_MODE_DISABLED)
	{
		nRetVal = m_Helper.ConfigureFirmware(m_FirmwareCropSizeX);
		XN_IS_STATUS_OK(nRetVal);;
		nRetVal = m_Helper.ConfigureFirmware(m_FirmwareCropSizeY);
		XN_IS_STATUS_OK(nRetVal);;
		nRetVal = m_Helper.ConfigureFirmware(m_FirmwareCropOffsetX);
		XN_IS_STATUS_OK(nRetVal);;
		nRetVal = m_Helper.ConfigureFirmware(m_FirmwareCropOffsetY);
		XN_IS_STATUS_OK(nRetVal);;
	}
	nRetVal = m_Helper.ConfigureFirmware(m_FirmwareCropMode);
	XN_IS_STATUS_OK(nRetVal);

	if (m_Helper.GetPrivateData()->FWInfo.bImageAdjustmentsSupported)
	{
		nRetVal = m_Helper.ConfigureFirmware(m_AutoExposure);
		XN_IS_STATUS_OK(nRetVal);
		nRetVal = m_Helper.ConfigureFirmware(m_AutoWhiteBalance);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nRetVal = SetAutoExposureForOldFirmware(m_AutoExposure.GetValue() == true);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = SetAutoWhiteBalanceForOldFirmware(m_AutoWhiteBalance.GetValue() == true);
		XN_IS_STATUS_OK(nRetVal);
	}

	nRetVal = XnImageStream::Open();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::CloseStreamImpl()
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = GetFirmwareParams()->m_Stream0Mode.SetValue(XN_VIDEO_STREAM_OFF);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = SetActualRead(false);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnImageStream::Close();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetOutputFormat(OniPixelFormat nOutputFormat)
{
	XnStatus nRetVal = XN_STATUS_OK;

	switch (nOutputFormat)
	{
	case ONI_PIXEL_FORMAT_GRAY8:
	case ONI_PIXEL_FORMAT_YUV422:
	case ONI_PIXEL_FORMAT_YUYV:
	case ONI_PIXEL_FORMAT_RGB888:
	case ONI_PIXEL_FORMAT_JPEG:
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Unsupported image output format: %d", nOutputFormat);
	}

	nRetVal = m_Helper.BeforeSettingDataProcessorProperty();
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnImageStream::SetOutputFormat(nOutputFormat);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.AfterSettingDataProcessorProperty();
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetMirror(bool bIsMirrored)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// set firmware mirror
	bool bFirmwareMirror = (bIsMirrored == true && m_Helper.GetFirmwareVersion() >= XN_SENSOR_FW_VER_5_0);

	xnOSEnterCriticalSection(GetLock());

	nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareMirror, (uint16_t)bFirmwareMirror);
	if (nRetVal != XN_STATUS_OK)
	{
		xnOSLeaveCriticalSection(GetLock());
		return (nRetVal);
	}

	// update prop
	nRetVal = XnImageStream::SetMirror(bIsMirrored);
	xnOSLeaveCriticalSection(GetLock());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetFPS(uint32_t nFPS)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_Helper.BeforeSettingFirmwareParam(FPSProperty(), (uint16_t)nFPS);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnImageStream::SetFPS(nFPS);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.AfterSettingFirmwareParam(FPSProperty());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetResolution(XnResolutions nResolution)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_Helper.BeforeSettingFirmwareParam(ResolutionProperty(), (uint16_t)nResolution);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnImageStream::SetResolution(nResolution);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = m_Helper.AfterSettingFirmwareParam(ResolutionProperty());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetInputFormat(XnIOImageFormats nInputFormat)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// validity checks
	switch (nInputFormat)
	{
	case XN_IO_IMAGE_FORMAT_YUV422:
	case XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422:
	case XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUYV:
	case XN_IO_IMAGE_FORMAT_JPEG:
	case XN_IO_IMAGE_FORMAT_BAYER:
	case XN_IO_IMAGE_FORMAT_UNCOMPRESSED_BAYER:
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Unknown image input format: %d", nInputFormat);
	}

	nRetVal = m_Helper.SimpleSetFirmwareParam(m_InputFormat, (uint16_t)nInputFormat);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetAntiFlicker(uint32_t nFrequency)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_Helper.SimpleSetFirmwareParam(m_AntiFlicker, (uint16_t)nFrequency);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetImageQuality(uint32_t /*nQuality*/)
{
	// check relevance
	if (m_InputFormat.GetValue() != XN_IO_IMAGE_FORMAT_JPEG)
	{
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_UNSUPPORTED_PARAMETER, XN_MASK_DEVICE_SENSOR, "Image quality is only supported when input format is JPEG");
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetCroppingImpl(const OniCropping* pCropping, XnCroppingMode mode)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnFirmwareCroppingMode firmwareMode = m_Helper.GetFirmwareCroppingMode(mode, pCropping->enabled);

	nRetVal = ValidateCropping(pCropping);
	XN_IS_STATUS_OK(nRetVal);

	xnOSEnterCriticalSection(GetLock());

	if (m_Helper.GetFirmwareVersion() > XN_SENSOR_FW_VER_3_0)
	{
		nRetVal = m_Helper.StartFirmwareTransaction();
		if (nRetVal != XN_STATUS_OK)
		{
			xnOSLeaveCriticalSection(GetLock());
			return (nRetVal);
		}

		if (pCropping->enabled)
		{
			nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropSizeX, (uint16_t) pCropping->width);

			if (nRetVal == XN_STATUS_OK)
				nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropSizeY, (uint16_t) pCropping->height);

			if (nRetVal == XN_STATUS_OK)
				nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropOffsetX, (uint16_t) pCropping->originX);

			if (nRetVal == XN_STATUS_OK)
				nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropOffsetY, (uint16_t) pCropping->originY);
		}

		if (nRetVal == XN_STATUS_OK)
		{
			nRetVal = m_Helper.SimpleSetFirmwareParam(m_FirmwareCropMode, (uint16_t)firmwareMode);
		}

		if (nRetVal != XN_STATUS_OK)
		{
			m_Helper.RollbackFirmwareTransaction();
			m_Helper.UpdateFromFirmware(m_FirmwareCropMode);
			m_Helper.UpdateFromFirmware(m_FirmwareCropOffsetX);
			m_Helper.UpdateFromFirmware(m_FirmwareCropOffsetY);
			m_Helper.UpdateFromFirmware(m_FirmwareCropSizeX);
			m_Helper.UpdateFromFirmware(m_FirmwareCropSizeY);
			xnOSLeaveCriticalSection(GetLock());
			return (nRetVal);
		}

		nRetVal = m_Helper.CommitFirmwareTransactionAsBatch();
		if (nRetVal != XN_STATUS_OK)
		{
			m_Helper.UpdateFromFirmware(m_FirmwareCropMode);
			m_Helper.UpdateFromFirmware(m_FirmwareCropOffsetX);
			m_Helper.UpdateFromFirmware(m_FirmwareCropOffsetY);
			m_Helper.UpdateFromFirmware(m_FirmwareCropSizeX);
			m_Helper.UpdateFromFirmware(m_FirmwareCropSizeY);
			xnOSLeaveCriticalSection(GetLock());
			return (nRetVal);
		}
	}

	nRetVal = m_CroppingMode.UnsafeUpdateValue(mode);
	XN_REFERENCE_VARIABLE(nRetVal);
	XN_ASSERT(nRetVal == XN_STATUS_OK);

	nRetVal = XnImageStream::SetCropping(pCropping);
	xnOSLeaveCriticalSection(GetLock());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetCropping(const OniCropping* pCropping)
{
	return SetCroppingImpl(pCropping, (XnCroppingMode)m_CroppingMode.GetValue());
}

XnStatus XnSensorImageStream::SetCroppingMode(XnCroppingMode mode)
{
	switch (mode)
	{
	case XN_CROPPING_MODE_NORMAL:
	case XN_CROPPING_MODE_INCREASED_FPS:
	case XN_CROPPING_MODE_SOFTWARE_ONLY:
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_DEVICE_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "Bad cropping mode: %u", mode);
	}

	return SetCroppingImpl(GetCropping(), mode);
}

XnStatus XnSensorImageStream::SetAutoExposureForOldFirmware(bool bAutoExposure)
{
	XnStatus nRetVal = XN_STATUS_OK;
	uint16_t nCmosRegValue;

	nRetVal = XnHostProtocolSetCMOSRegisterI2C(m_Helper.GetPrivateData(), (XnCMOSType)0, 0xf0, 1);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnHostProtocolGetCMOSRegisterI2C(m_Helper.GetPrivateData(), (XnCMOSType)0, 0x6, nCmosRegValue);
	XN_IS_STATUS_OK(nRetVal);

	if (bAutoExposure)
	{
		nCmosRegValue |= 0x4000;
	}
	else
	{
		nCmosRegValue &= ~0x4000;
	}

	nRetVal = XnHostProtocolSetCMOSRegisterI2C(m_Helper.GetPrivateData(), (XnCMOSType)0, 0x6, nCmosRegValue);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus XnSensorImageStream::SetAutoExposure(bool bAutoExposure)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (!m_Helper.GetPrivateData()->FWInfo.bImageAdjustmentsSupported)
	{
		nRetVal = SetAutoExposureForOldFirmware(bAutoExposure);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = m_AutoExposure.UnsafeUpdateValue(bAutoExposure);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nRetVal = m_Helper.SimpleSetFirmwareParam(m_AutoExposure, (uint16_t)bAutoExposure);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetAutoWhiteBalanceForOldFirmware(bool bAutoWhiteBalance)
{
	XnStatus nRetVal = XN_STATUS_OK;
	uint16_t nCmosRegValue;

	nRetVal = XnHostProtocolSetCMOSRegisterI2C(m_Helper.GetPrivateData(), (XnCMOSType)0, 0xf0, 1);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = XnHostProtocolGetCMOSRegisterI2C(m_Helper.GetPrivateData(), (XnCMOSType)0, 0x6, nCmosRegValue);
	XN_IS_STATUS_OK(nRetVal);

	if (bAutoWhiteBalance)
	{
		nCmosRegValue |= 0x2;
	}
	else
	{
		nCmosRegValue &= ~0x2;
	}

	nRetVal = XnHostProtocolSetCMOSRegisterI2C(m_Helper.GetPrivateData(), (XnCMOSType)0, 0x6, nCmosRegValue);
	XN_IS_STATUS_OK(nRetVal);

	return XN_STATUS_OK;
}

XnStatus XnSensorImageStream::SetAutoWhiteBalance(bool bAutoWhiteBalance)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (!m_Helper.GetPrivateData()->FWInfo.bImageAdjustmentsSupported)
	{
		nRetVal = SetAutoWhiteBalanceForOldFirmware(bAutoWhiteBalance);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = m_AutoWhiteBalance.UnsafeUpdateValue(bAutoWhiteBalance);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		nRetVal = m_Helper.SimpleSetFirmwareParam(m_AutoWhiteBalance, (uint16_t)bAutoWhiteBalance);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::SetExposure(uint64_t nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (!m_Helper.GetPrivateData()->FWInfo.bImageAdjustmentsSupported)
	{
		return (XN_STATUS_UNSUPPORTED_VERSION);
	}

	nRetVal = m_Helper.SimpleSetFirmwareParam(m_Exposure, (uint16_t)nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}
XnStatus XnSensorImageStream::SetGain(uint64_t nValue)
{
	XnStatus nRetVal = XN_STATUS_OK;

	if (!m_Helper.GetPrivateData()->FWInfo.bImageAdjustmentsSupported)
	{
		return (XN_STATUS_UNSUPPORTED_VERSION);
	}

	nRetVal = m_Helper.SimpleSetFirmwareParam(m_Gain, (uint16_t)nValue);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}
XnStatus XnSensorImageStream::SetFastZoomCrop(bool bFastZoomCrop)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = m_Helper.SimpleSetFirmwareParam(m_FastZoomCrop, (uint16_t)bFastZoomCrop);

	return nRetVal;
}

XnStatus XnSensorImageStream::CropImpl(OniFrame* pFrame, const OniCropping* pCropping)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// if firmware cropping is disabled, crop
	if (m_FirmwareCropMode.GetValue() == XN_FIRMWARE_CROPPING_MODE_DISABLED)
	{
		nRetVal = XnImageStream::CropImpl(pFrame, pCropping);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus XnSensorImageStream::Mirror(OniFrame* pFrame) const
{
	XnStatus nRetVal = XN_STATUS_OK;

	// only perform mirror if it's our job. if mirror is performed by FW, we don't need to do anything.
	if (m_FirmwareMirror.GetValue() == false)
	{
		nRetVal = XnImageStream::Mirror(pFrame);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

uint32_t XnSensorImageStream::CalculateExpectedSize()
{
	uint32_t nExpectedImageBufferSize = GetXRes() * GetYRes();

	// when cropping is turned on, actual IR size is smaller
	const OniCropping* pCropping = GetCropping();
	if (pCropping->enabled)
	{
		nExpectedImageBufferSize = pCropping->width * pCropping->height;
	}

	switch (m_InputFormat.GetValue())
	{
	case XN_IO_IMAGE_FORMAT_YUV422:
	case XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422:
	case XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUYV:
		// in YUV each pixel is represented in 2 bytes (actually 2 pixels are represented by 4 bytes)
		nExpectedImageBufferSize *= 2;
		break;
	case XN_IO_IMAGE_FORMAT_BAYER:
		// each pixel is one byte.
		break;
	case XN_IO_IMAGE_FORMAT_JPEG:
		// image should be in RGB now - 3 bytes a pixel
		nExpectedImageBufferSize *= 3;
		break;
	default:
		XN_LOG_WARNING_RETURN(XN_STATUS_ERROR, XN_MASK_DEVICE_SENSOR, "Does not know to calculate expected size for input format %d", m_InputFormat.GetValue());
	}

	return nExpectedImageBufferSize;
}

XnStatus XnSensorImageStream::CreateDataProcessor(XnDataProcessor** ppProcessor)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnFrameBufferManager* pBufferManager;
	nRetVal = StartBufferManager(&pBufferManager);
	XN_IS_STATUS_OK(nRetVal);

	XnStreamProcessor* pNew;

	switch (m_InputFormat.GetValue())
	{
	case XN_IO_IMAGE_FORMAT_BAYER:
		XN_VALIDATE_NEW_AND_INIT(pNew, XnBayerImageProcessor, this, &m_Helper, pBufferManager);
		break;
	case XN_IO_IMAGE_FORMAT_YUV422:
		XN_VALIDATE_NEW_AND_INIT(pNew, XnPSCompressedImageProcessor, this, &m_Helper, pBufferManager);
		break;
	case XN_IO_IMAGE_FORMAT_JPEG:
		if (GetOutputFormat() == ONI_PIXEL_FORMAT_JPEG)
		{
			XN_VALIDATE_NEW_AND_INIT(pNew, XnJpegImageProcessor, this, &m_Helper, pBufferManager);
		}
		else if (GetOutputFormat() == ONI_PIXEL_FORMAT_RGB888)
		{
			XN_VALIDATE_NEW_AND_INIT(pNew, XnJpegToRGBImageProcessor, this, &m_Helper, pBufferManager);
		}
		else
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "invalid output format %d!", GetOutputFormat());
		}
		break;
	case XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUV422:
		if (GetOutputFormat() == ONI_PIXEL_FORMAT_YUV422)
		{
			XN_VALIDATE_NEW_AND_INIT(pNew, XnPassThroughImageProcessor, this, &m_Helper, pBufferManager);
		}
		else if (GetOutputFormat() == ONI_PIXEL_FORMAT_RGB888)
		{
			XN_VALIDATE_NEW_AND_INIT(pNew, XnUncompressedYUV422toRGBImageProcessor, this, &m_Helper, pBufferManager);
		}
		else
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "invalid output format %d!", pBufferManager);
		}
		break;
	case XN_IO_IMAGE_FORMAT_UNCOMPRESSED_YUYV:
		if (GetOutputFormat() == ONI_PIXEL_FORMAT_YUYV)
		{
			XN_VALIDATE_NEW_AND_INIT(pNew, XnPassThroughImageProcessor, this, &m_Helper, pBufferManager);
		}
		else if (GetOutputFormat() == ONI_PIXEL_FORMAT_RGB888)
		{
			XN_VALIDATE_NEW_AND_INIT(pNew, XnUncompressedYUYVtoRGBImageProcessor, this, &m_Helper, pBufferManager);
		}
		else
		{
			XN_LOG_WARNING_RETURN(XN_STATUS_BAD_PARAM, XN_MASK_DEVICE_SENSOR, "invalid output format %d!", pBufferManager);
		}
		break;
	case XN_IO_IMAGE_FORMAT_UNCOMPRESSED_BAYER:
		XN_VALIDATE_NEW_AND_INIT(pNew, XnUncompressedBayerProcessor, this, &m_Helper, pBufferManager);
		break;
	default:
		return XN_STATUS_IO_INVALID_STREAM_IMAGE_FORMAT;
	}

	*ppProcessor = pNew;

	return XN_STATUS_OK;
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetInputFormatCallback(XnActualIntProperty* /*pSender*/, uint64_t nValue, void* pCookie)
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetInputFormat((XnIOImageFormats)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetAntiFlickerCallback(XnActualIntProperty* /*pSender*/, uint64_t nValue, void* pCookie)
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetAntiFlicker((uint32_t)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetImageQualityCallback(XnActualIntProperty* /*pSender*/, uint64_t nValue, void* pCookie)
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetImageQuality((uint32_t)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetActualReadCallback(XnActualIntProperty* /*pSender*/, uint64_t nValue, void* pCookie)
{
	XnSensorImageStream* pThis = (XnSensorImageStream*)pCookie;
	return pThis->SetActualRead(nValue == true);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetCroppingModeCallback(XnActualIntProperty* /*pSender*/, uint64_t nValue, void* pCookie)
{
	XnSensorImageStream* pStream = (XnSensorImageStream*)pCookie;
	return pStream->SetCroppingMode((XnCroppingMode)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetAutoExposureCallback(XnActualIntProperty* /*pSender*/, uint64_t nValue, void* pCookie)
{
	XnSensorImageStream* pStream = (XnSensorImageStream*)pCookie;
	return pStream->SetAutoExposure((bool)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetAutoWhiteBalanceCallback(XnActualIntProperty* /*pSender*/, uint64_t nValue, void* pCookie)
{
	XnSensorImageStream* pStream = (XnSensorImageStream*)pCookie;
	return pStream->SetAutoWhiteBalance((bool)nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetExposureCallback(XnActualIntProperty*, uint64_t nValue, void* pCookie)
{
	XnSensorImageStream* pStream = (XnSensorImageStream*)pCookie;
	return pStream->SetExposure(nValue);
}

XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetGainCallback(XnActualIntProperty*, uint64_t nValue, void* pCookie)
{
	XnSensorImageStream* pStream = (XnSensorImageStream*)pCookie;
	return pStream->SetGain(nValue);
}
XnStatus XN_CALLBACK_TYPE XnSensorImageStream::SetFastZoomCropCallback(XnActualIntProperty* /*pSender*/, uint64_t nValue, void* pCookie)
{
	XnSensorImageStream* pStream = (XnSensorImageStream*)pCookie;
	return pStream->SetFastZoomCrop((bool)nValue);
}
