#ifndef DEPTHSTREAM_HPP
#define DEPTHSTREAM_HPP

#include <algorithm> // for transform()
#include <cmath> // for M_PI
#include <cstdio> // for memcpy
#include <map>
#include <utility>

#include "libfreenect.hpp"
#include "Driver/OniDriverAPI.h"
#include "PS1080.h"
#include "VideoStream.hpp"
#include "S2D.h"
#include "D2S.h"

namespace FreenectDriver
{
class DepthStream final : public VideoStream
{
public:
	static constexpr OniSensorType SENSOR_TYPE = ONI_SENSOR_DEPTH;
	// from NUI library and converted to radians
	static constexpr float DIAGONAL_FOV = 70 * (M_PI / 180);
	static constexpr float HORIZONTAL_FOV = 58.5 * (M_PI / 180);
	static constexpr float VERTICAL_FOV = 45.6 * (M_PI / 180);
	// from DepthKinectStream.cpp
	static constexpr int MAX_VALUE = 10000;
	static constexpr unsigned long long GAIN_VAL = 42;
	static constexpr unsigned long long CONST_SHIFT_VAL = 200;
	static constexpr unsigned long long MAX_SHIFT_VAL = 2047;
	static constexpr unsigned long long PARAM_COEFF_VAL = 4;
	static constexpr unsigned long long SHIFT_SCALE_VAL = 10;
	static constexpr unsigned long long ZERO_PLANE_DISTANCE_VAL = 120;
	static constexpr double ZERO_PLANE_PIXEL_SIZE_VAL = 0.10520000010728836;
	static constexpr double EMITTER_DCMOS_DISTANCE_VAL = 7.5;

	DepthStream(Freenect::FreenectDevice* pDevice);

	static OniSensorInfo getSensorInfo()
	{
		FreenectDepthModeMap supported_modes = getSupportedVideoModes();
		OniVideoMode* modes = new OniVideoMode[supported_modes.size()];
		std::transform(supported_modes.begin(), supported_modes.end(), modes, ExtractKey());
		OniSensorInfo sensors = { SENSOR_TYPE, static_cast<int>(supported_modes.size()), modes };
		return sensors;
	}

	OniImageRegistrationMode getImageRegistrationMode() const { return image_registration_mode; }
	OniStatus setImageRegistrationMode(OniImageRegistrationMode mode)
	{
		if (!isImageRegistrationModeSupported(mode))
		{
			return ONI_STATUS_NOT_SUPPORTED;
		}
		image_registration_mode = mode;
		return setVideoMode(video_mode);
	}

	// from StreamBase
	bool isImageRegistrationModeSupported(OniImageRegistrationMode mode) { return (mode == ONI_IMAGE_REGISTRATION_OFF || mode == ONI_IMAGE_REGISTRATION_DEPTH_TO_COLOR); }

	bool isPropertySupported(int propertyId) override
	{
		switch (propertyId)
		{
			default:
				return VideoStream::isPropertySupported(propertyId);
			case ONI_STREAM_PROPERTY_HORIZONTAL_FOV:
			case ONI_STREAM_PROPERTY_VERTICAL_FOV:
			case ONI_STREAM_PROPERTY_MAX_VALUE:
			case XN_STREAM_PROPERTY_GAIN:
			case XN_STREAM_PROPERTY_CONST_SHIFT:
			case XN_STREAM_PROPERTY_MAX_SHIFT:
			case XN_STREAM_PROPERTY_PARAM_COEFF:
			case XN_STREAM_PROPERTY_SHIFT_SCALE:
			case XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE:
			case XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE:
			case XN_STREAM_PROPERTY_EMITTER_DCMOS_DISTANCE:
			case XN_STREAM_PROPERTY_S2D_TABLE:
			case XN_STREAM_PROPERTY_D2S_TABLE:
				return true;
		}
	}

	OniStatus getProperty(int propertyId, void* data, int* pDataSize)
	{
		switch (propertyId)
		{
			default:
				return VideoStream::getProperty(propertyId, data, pDataSize);

			case ONI_STREAM_PROPERTY_HORIZONTAL_FOV:				// float (radians)
				if (*pDataSize != sizeof(float))
				{
					LogError("Unexpected size for ONI_STREAM_PROPERTY_HORIZONTAL_FOV");
					return ONI_STATUS_ERROR;
				}
				*(static_cast<float*>(data)) = HORIZONTAL_FOV;
				return ONI_STATUS_OK;
			case ONI_STREAM_PROPERTY_VERTICAL_FOV:					// float (radians)
				if (*pDataSize != sizeof(float))
				{
					LogError("Unexpected size for ONI_STREAM_PROPERTY_VERTICAL_FOV");
					return ONI_STATUS_ERROR;
				}
				*(static_cast<float*>(data)) = VERTICAL_FOV;
				return ONI_STATUS_OK;
			case ONI_STREAM_PROPERTY_MAX_VALUE:						 // int
				if (*pDataSize != sizeof(int))
				{
					LogError("Unexpected size for ONI_STREAM_PROPERTY_MAX_VALUE");
					return ONI_STATUS_ERROR;
				}
				*(static_cast<int*>(data)) = MAX_VALUE;
				return ONI_STATUS_OK;

			case XN_STREAM_PROPERTY_PIXEL_REGISTRATION:		 // XnPixelRegistration (get only)
			case XN_STREAM_PROPERTY_WHITE_BALANCE_ENABLED:	// unsigned long long
			case XN_STREAM_PROPERTY_HOLE_FILTER:						// unsigned long long
			case XN_STREAM_PROPERTY_REGISTRATION_TYPE:			// XnProcessingType
			case XN_STREAM_PROPERTY_AGC_BIN:								// XnDepthAGCBin*
			case XN_STREAM_PROPERTY_PIXEL_SIZE_FACTOR:			// unsigned long long
			case XN_STREAM_PROPERTY_DCMOS_RCMOS_DISTANCE:	 // double
			case XN_STREAM_PROPERTY_CLOSE_RANGE:						// unsigned long long
				return ONI_STATUS_NOT_SUPPORTED;

			case XN_STREAM_PROPERTY_GAIN:									 // unsigned long long
				if (*pDataSize != sizeof(unsigned long long))
				{
					LogError("Unexpected size for XN_STREAM_PROPERTY_GAIN");
					return ONI_STATUS_ERROR;
				}
				*(static_cast<unsigned long long*>(data)) = GAIN_VAL;
				return ONI_STATUS_OK;
			case XN_STREAM_PROPERTY_CONST_SHIFT:						// unsigned long long
				if (*pDataSize != sizeof(unsigned long long))
				{
					LogError("Unexpected size for XN_STREAM_PROPERTY_CONST_SHIFT");
					return ONI_STATUS_ERROR;
				}
				*(static_cast<unsigned long long*>(data)) = CONST_SHIFT_VAL;
				return ONI_STATUS_OK;
			case XN_STREAM_PROPERTY_MAX_SHIFT:							// unsigned long long
				if (*pDataSize != sizeof(unsigned long long))
				{
					LogError("Unexpected size for XN_STREAM_PROPERTY_MAX_SHIFT");
					return ONI_STATUS_ERROR;
				}
				*(static_cast<unsigned long long*>(data)) = MAX_SHIFT_VAL;
				return ONI_STATUS_OK;
			case XN_STREAM_PROPERTY_PARAM_COEFF:						// unsigned long long
				if (*pDataSize != sizeof(unsigned long long))
				{
					LogError("Unexpected size for XN_STREAM_PROPERTY_PARAM_COEFF");
					return ONI_STATUS_ERROR;
				}
				*(static_cast<unsigned long long*>(data)) = PARAM_COEFF_VAL;
				return ONI_STATUS_OK;
			case XN_STREAM_PROPERTY_SHIFT_SCALE:						// unsigned long long
				if (*pDataSize != sizeof(unsigned long long))
				{
					LogError("Unexpected size for XN_STREAM_PROPERTY_SHIFT_SCALE");
					return ONI_STATUS_ERROR;
				}
				*(static_cast<unsigned long long*>(data)) = SHIFT_SCALE_VAL;
				return ONI_STATUS_OK;
			case XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE:		// unsigned long long
				if (*pDataSize != sizeof(unsigned long long))
				{
					LogError("Unexpected size for XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE");
					return ONI_STATUS_ERROR;
				}
				*(static_cast<unsigned long long*>(data)) = ZERO_PLANE_DISTANCE_VAL;
				return ONI_STATUS_OK;
			case XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE:	// double
				if (*pDataSize != sizeof(double))
				{
					LogError("Unexpected size for XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE");
					return ONI_STATUS_ERROR;
				}
				*(static_cast<double*>(data)) = ZERO_PLANE_PIXEL_SIZE_VAL;
				return ONI_STATUS_OK;
			case XN_STREAM_PROPERTY_EMITTER_DCMOS_DISTANCE: // double
				if (*pDataSize != sizeof(double))
				{
					LogError("Unexpected size for XN_STREAM_PROPERTY_EMITTER_DCMOS_DISTANCE");
					return ONI_STATUS_ERROR;
				}
				*(static_cast<double*>(data)) = EMITTER_DCMOS_DISTANCE_VAL;
				return ONI_STATUS_OK;
			case XN_STREAM_PROPERTY_S2D_TABLE:							// OniDepthPixel[]
				*pDataSize = sizeof(S2D);
				memcpy(data, S2D, *pDataSize);
				return ONI_STATUS_OK;
			case XN_STREAM_PROPERTY_D2S_TABLE:							// unsigned short[]
				*pDataSize = sizeof(D2S);
				memcpy(data, D2S, *pDataSize);
				return ONI_STATUS_OK;
		}
	}


	void notifyAllProperties() override
	{
		double nDouble;
		int size = sizeof(nDouble);

		getProperty(XN_STREAM_PROPERTY_EMITTER_DCMOS_DISTANCE, &nDouble, &size);
		raisePropertyChanged(XN_STREAM_PROPERTY_EMITTER_DCMOS_DISTANCE, &nDouble, size);

		unsigned long long nUll;
		size = sizeof(nUll);
		getProperty(XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE, &nUll, &size);
		raisePropertyChanged(XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE, &nUll, size);

		getProperty(XN_STREAM_PROPERTY_GAIN, &nUll, &size);
		raisePropertyChanged(XN_STREAM_PROPERTY_GAIN, &nUll, size);

		getProperty(XN_STREAM_PROPERTY_CONST_SHIFT, &nUll, &size);
		raisePropertyChanged(XN_STREAM_PROPERTY_CONST_SHIFT, &nUll, size);

		getProperty(XN_STREAM_PROPERTY_MAX_SHIFT, &nUll, &size);
		raisePropertyChanged(XN_STREAM_PROPERTY_MAX_SHIFT, &nUll, size);

		getProperty(XN_STREAM_PROPERTY_SHIFT_SCALE, &nUll, &size);
		raisePropertyChanged(XN_STREAM_PROPERTY_SHIFT_SCALE, &nUll, size);

		getProperty(XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE, &nUll, &size);
		raisePropertyChanged(XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE, &nUll, size);

		getProperty(XN_STREAM_PROPERTY_PARAM_COEFF, &nUll, &size);
		raisePropertyChanged(XN_STREAM_PROPERTY_PARAM_COEFF, &nUll, size);

		int nInt;
		size = sizeof(nInt);

		getProperty(ONI_STREAM_PROPERTY_MAX_VALUE, &nInt, &size);
		raisePropertyChanged(ONI_STREAM_PROPERTY_MAX_VALUE, &nInt, size);

		unsigned short nBuff[10001];
		size = sizeof(S2D);
		getProperty(XN_STREAM_PROPERTY_S2D_TABLE, nBuff, &size);
		raisePropertyChanged(XN_STREAM_PROPERTY_S2D_TABLE, nBuff, size);

		size = sizeof(D2S);
		getProperty(XN_STREAM_PROPERTY_D2S_TABLE, nBuff, &size);
		raisePropertyChanged(XN_STREAM_PROPERTY_D2S_TABLE, nBuff, size);

		VideoStream::notifyAllProperties();
	}

private:
	typedef std::map< OniVideoMode, std::pair<freenect_depth_format, freenect_resolution> > FreenectDepthModeMap;
	OniImageRegistrationMode image_registration_mode;

	static FreenectDepthModeMap getSupportedVideoModes();
	OniStatus setVideoMode(OniVideoMode requested_mode) override;
	void populateFrame(void* data, OniFrame* frame) const override;
};

}

#endif
