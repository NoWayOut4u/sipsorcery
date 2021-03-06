#include "ImageConvert.h"

namespace SIPSorceryMedia {

	ImageConvert::ImageConvert()
	{ }

	ImageConvert::~ImageConvert()
	{
		sws_freeContext(_swsContextRGBToYUV);
		sws_freeContext(_swsContextYUVToRGB);
		sws_freeContext(_swsContext);
	}

	int ImageConvert::ConvertRGBtoYUV(unsigned char* bmp, int width, int height, /* out */ array<Byte> ^% buffer)
	{
		AVPixelFormat rgbSourceFormat = AV_PIX_FMT_BGR24;
		AVPixelFormat yuvOutputFormat = AV_PIX_FMT_YUV420P;

		_swsContextRGBToYUV = sws_getCachedContext(_swsContextRGBToYUV, width, height, rgbSourceFormat, width, height, yuvOutputFormat, SWS_BILINEAR, NULL, NULL, NULL);

		if (!_swsContextRGBToYUV) {
			fprintf(stderr, "Could not initialize the conversion context in ImageConvert::ConvertRGBtoYUV.\n");
			return -1;
		}

		AVFrame* dstFrame = av_frame_alloc();
		int num_bytes = avpicture_get_size(yuvOutputFormat, width, height);
		int bufferSize = num_bytes*sizeof(uint8_t);
		uint8_t* dstFrameBuffer = (uint8_t *)av_malloc(bufferSize);
		avpicture_fill((AVPicture*)dstFrame, dstFrameBuffer, yuvOutputFormat, width, height);

		uint8_t * srcData[1] = { bmp };		// RGB has one plane
		int srcLinesize[1] = { 3 * width };	// RGB stride

		int res = sws_scale(_swsContextRGBToYUV, srcData, srcLinesize, 0, height, dstFrame->data, dstFrame->linesize);

		if (res == 0) {
			fprintf(stderr, "The conversion failed in ImageConvert::ConvertRGBtoYUV.\n");
			return -1;
		}
	
		buffer = gcnew array<Byte>(bufferSize);
		Marshal::Copy((IntPtr)dstFrameBuffer, buffer, 0, bufferSize);

		av_freep(&dstFrameBuffer);
		av_frame_free(&dstFrame);
		
		return 0;
	}
	int ImageConvert::ConvertYUVToRGB(unsigned char* yuv, int width, int height, /* out */ array<Byte> ^% buffer)
	{
		AVPixelFormat yuvSourceFormat = AV_PIX_FMT_YUV420P;
		AVPixelFormat rgbOutputFormat = AV_PIX_FMT_BGR24;

		//if (_swsContextYUVToRGB == NULL) {
		_swsContextYUVToRGB = sws_getCachedContext(_swsContextYUVToRGB, width, height, yuvSourceFormat, width, height, rgbOutputFormat, SWS_BILINEAR, NULL, NULL, NULL);
		//}

		if (!_swsContextYUVToRGB) {
			fprintf(stderr, "Could not initialize the conversion context in ImageConvert::ConvertRGBtoYUV.\n");
			return -1;
		}

		AVFrame* srcFrame = av_frame_alloc();
		int srcNumBytes = avpicture_get_size(yuvSourceFormat, width, height);
		int srcBufferSize = srcNumBytes * sizeof(uint8_t);
		uint8_t* srcFrameBuffer = (uint8_t *)av_malloc(srcBufferSize);
		avpicture_fill((AVPicture*)srcFrame, yuv, yuvSourceFormat, width, height);

		AVFrame* dstFrame = av_frame_alloc();
		int num_bytes = avpicture_get_size(rgbOutputFormat, width, height);
		int bufferSize = num_bytes*sizeof(uint8_t);
		uint8_t* dstFrameBuffer = (uint8_t *)av_malloc(bufferSize);
		avpicture_fill((AVPicture*)dstFrame, dstFrameBuffer, rgbOutputFormat, width, height);

		int res = sws_scale(_swsContextYUVToRGB, srcFrame->data, srcFrame->linesize, 0, height, dstFrame->data, dstFrame->linesize);

		if (res == 0) {
			fprintf(stderr, "The conversion failed in ImageConvert::ConvertRGBtoYUV.\n");
			return -1;
		}

		buffer = gcnew array<Byte>(bufferSize);
		Marshal::Copy((IntPtr)dstFrameBuffer, buffer, 0, bufferSize);

		av_freep(&dstFrameBuffer);
		av_frame_free(&dstFrame);
		av_freep(&srcFrameBuffer);
		av_frame_free(&srcFrame);

		return 0;
	}

	int ImageConvert::ConvertToI420(const uint8_t * img, VideoSubTypesEnum inputFormat, int width, int height, /* out */ array<Byte> ^% buffer)
	{
		auto inputPixelFormat = VideoSubTypesHelper::GetPixelFormatForVideoSubType(inputFormat);

		_swsContext = sws_getCachedContext(_swsContext, width, height, inputPixelFormat, width, height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);

		if (!_swsContext) {
			fprintf(stderr, "Could not initialize the conversion context in ImageConvert::ConvertToI420.\n");
			return -1;
		}

		AVFrame* srcFrame = av_frame_alloc();
		int srcNumBytes = avpicture_get_size(inputPixelFormat, width, height);
		avpicture_fill((AVPicture*)srcFrame, img, inputPixelFormat, width, height);

		AVFrame* dstFrame = av_frame_alloc();
		int num_bytes = avpicture_get_size(AV_PIX_FMT_YUV420P, width, height);
		int bufferSize = num_bytes*sizeof(uint8_t);
		uint8_t* dstFrameBuffer = (uint8_t *)av_malloc(bufferSize);
		avpicture_fill((AVPicture*)dstFrame, dstFrameBuffer, AV_PIX_FMT_YUV420P, width, height);

		int res = sws_scale(_swsContext, &img, srcFrame->linesize, 0, height, dstFrame->data, dstFrame->linesize);

		if (res == 0) {
			printf("The conversion failed in ImageConvert::ConvertToI420.\n");
			return -1;
		}

		buffer = gcnew array<Byte>(bufferSize);
		Marshal::Copy((IntPtr)dstFrameBuffer, buffer, 0, bufferSize);

		av_freep(&dstFrameBuffer);
		av_frame_free(&srcFrame);
		av_frame_free(&dstFrame);

		return 0;
	}
}
