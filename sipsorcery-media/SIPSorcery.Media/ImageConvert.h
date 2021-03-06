#pragma once

#include <stdio.h>

#include "VideoSubTypes.h"

extern "C"
{
#include "libswscale\swscale.h"
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libavutil\avutil.h"
}

using namespace System;
using namespace System::Runtime::InteropServices;

namespace SIPSorceryMedia {

	public ref class ImageConvert
	{
	public:
		ImageConvert();
		~ImageConvert();
		int ConvertRGBtoYUV(unsigned char* bmp, int width, int height, /* out */ array<Byte> ^% buffer);
		int ConvertYUVToRGB(unsigned char* bmp, int width, int height, /* out */ array<Byte> ^% buffer);
		int ConvertToI420(const uint8_t* img, VideoSubTypesEnum inputFormat, int width, int height, /* out */ array<Byte> ^% buffer);

	private:
		SwsContext* _swsContextRGBToYUV = NULL;
		SwsContext* _swsContextYUVToRGB = NULL;
		SwsContext* _swsContext = nullptr;
	};
}