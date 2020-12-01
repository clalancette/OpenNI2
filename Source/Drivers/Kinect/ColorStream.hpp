#ifndef COLORSTREAM_HPP
#define COLORSTREAM_HPP

#include <algorithm> // for transform()
#include <cmath> // for M_PI
#include <map>
#include <utility>

#include "libfreenect.hpp"
#include "Driver/OniDriverAPI.h"
#include "VideoStream.hpp"


namespace FreenectDriver
{
class ColorStream final : public VideoStream
{
public:
	static constexpr OniSensorType SENSOR_TYPE = ONI_SENSOR_COLOR;
	// from NUI library & converted to radians
	static constexpr float DIAGONAL_FOV = 73.9 * (M_PI / 180);
	static constexpr float HORIZONTAL_FOV = 62 * (M_PI / 180);
	static constexpr float VERTICAL_FOV = 48.6 * (M_PI / 180);

	ColorStream(Freenect::FreenectDevice* pDevice);

	static OniSensorInfo getSensorInfo()
	{
		FreenectVideoModeMap supported_modes = getSupportedVideoModes();
		OniVideoMode* modes = new OniVideoMode[supported_modes.size()];
		std::transform(supported_modes.begin(), supported_modes.end(), modes, ExtractKey());
		OniSensorInfo sensors = { SENSOR_TYPE, static_cast<int>(supported_modes.size()), modes };
		return sensors;
	}

	// from StreamBase
	bool isPropertySupported(int propertyId) override
	{
		switch(propertyId)
		{
		case ONI_STREAM_PROPERTY_HORIZONTAL_FOV:
		case ONI_STREAM_PROPERTY_VERTICAL_FOV:
		case ONI_STREAM_PROPERTY_AUTO_WHITE_BALANCE:
		case ONI_STREAM_PROPERTY_AUTO_EXPOSURE:
			return true;
		default:
			return VideoStream::isPropertySupported(propertyId);
		}
	}

	OniStatus getProperty(int propertyId, void* data, int* pDataSize) override
	{
		switch (propertyId)
		{
		case ONI_STREAM_PROPERTY_HORIZONTAL_FOV:     // float (radians)
		{
			if (*pDataSize != sizeof(float))
			{
				LogError("Unexpected size for ONI_STREAM_PROPERTY_HORIZONTAL_FOV");
				return ONI_STATUS_ERROR;
			}
			*(static_cast<float*>(data)) = HORIZONTAL_FOV;
			return ONI_STATUS_OK;
		}
		case ONI_STREAM_PROPERTY_VERTICAL_FOV:       // float (radians)
		{
			if (*pDataSize != sizeof(float))
			{
				LogError("Unexpected size for ONI_STREAM_PROPERTY_VERTICAL_FOV");
				return ONI_STATUS_ERROR;
			}
			*(static_cast<float*>(data)) = VERTICAL_FOV;
			return ONI_STATUS_OK;
		}

		// camera
		case ONI_STREAM_PROPERTY_AUTO_WHITE_BALANCE: // bool
		{
			if (*pDataSize != sizeof(bool))
			{
				LogError("Unexpected size for ONI_STREAM_PROPERTY_AUTO_WHITE_BALANCE");
				return ONI_STATUS_ERROR;
			}
			*(static_cast<bool*>(data)) = auto_white_balance;
			return ONI_STATUS_OK;
		}
		case ONI_STREAM_PROPERTY_AUTO_EXPOSURE:      // bool
		{
			if (*pDataSize != sizeof(bool))
			{
				LogError("Unexpected size for ONI_STREAM_PROPERTY_AUTO_EXPOSURE");
				return ONI_STATUS_ERROR;
			}
			*(static_cast<bool*>(data)) = auto_exposure;
			return ONI_STATUS_OK;
		}
		default:
			return VideoStream::getProperty(propertyId, data, pDataSize);
		}
	}

	OniStatus setProperty(int propertyId, const void* data, int dataSize) override
	{
		switch (propertyId)
		{
		// camera
		case ONI_STREAM_PROPERTY_AUTO_WHITE_BALANCE: // bool
		{
			if (dataSize != sizeof(bool))
			{
				LogError("Unexpected size for ONI_STREAM_PROPERTY_AUTO_WHITE_BALANCE");
				return ONI_STATUS_ERROR;
			}
			auto_white_balance = *(static_cast<const bool*>(data));
			int ret = device->setFlag(FREENECT_AUTO_WHITE_BALANCE, auto_white_balance);
			return (ret == 0) ? ONI_STATUS_OK : ONI_STATUS_ERROR;
		}
		case ONI_STREAM_PROPERTY_AUTO_EXPOSURE:      // bool
		{
			if (dataSize != sizeof(bool))
			{
				LogError("Unexpected size for ONI_STREAM_PROPERTY_AUTO_EXPOSURE");
				return ONI_STATUS_ERROR;
			}
			auto_exposure = *(static_cast<const bool*>(data));
			int ret = device->setFlag(FREENECT_AUTO_WHITE_BALANCE, auto_exposure);
			return (ret == 0) ? ONI_STATUS_OK : ONI_STATUS_ERROR;
		}
		case ONI_STREAM_PROPERTY_MIRRORING:          // bool
		{
			if (dataSize != sizeof(bool))
			{
				LogError("Unexpected size for ONI_STREAM_PROPERTY_MIRRORING");
				return ONI_STATUS_ERROR;
			}
			mirroring = *(static_cast<const bool*>(data));
			int ret = device->setFlag(FREENECT_MIRROR_VIDEO, mirroring);
			return (ret == 0) ? ONI_STATUS_OK : ONI_STATUS_ERROR;
		}
		default:
			return VideoStream::setProperty(propertyId, data, dataSize);
		}
	}

private:
	typedef std::map< OniVideoMode, std::pair<freenect_video_format, freenect_resolution> > FreenectVideoModeMap;

	static FreenectVideoModeMap getSupportedVideoModes();
	OniStatus setVideoMode(OniVideoMode requested_mode) override;
	void populateFrame(void* data, OniFrame* frame) const override;

	bool auto_white_balance;
	bool auto_exposure;
  };
}

#endif
