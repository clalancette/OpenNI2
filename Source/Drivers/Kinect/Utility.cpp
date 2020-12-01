#include "Driver/OniDriverAPI.h"

OniVideoMode makeOniVideoMode(OniPixelFormat pixel_format, int resolution_x, int resolution_y, int frames_per_second)
{
	OniVideoMode mode;
	mode.pixelFormat = pixel_format;
	mode.resolutionX = resolution_x;
	mode.resolutionY = resolution_y;
	mode.fps = frames_per_second;
	return mode;
}

bool operator<(const OniDeviceInfo& left, const OniDeviceInfo& right)
{
	return (strcmp(left.uri, right.uri) < 0);
}

bool operator==(const OniVideoMode& left, const OniVideoMode& right)
{
	return (left.pixelFormat == right.pixelFormat && left.resolutionX == right.resolutionX
					&& left.resolutionY == right.resolutionY && left.fps == right.fps);
}

bool operator<(const OniVideoMode& left, const OniVideoMode& right)
{
	return (left.resolutionX * left.resolutionY < right.resolutionX * right.resolutionY);
}
