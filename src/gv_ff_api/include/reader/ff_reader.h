#ifndef FF_READER_H_
#define FF_READER_H_
//CXX include Guard
#pragma once

//3rd-party
#include <spdlog/spdlog.h>
#include <fmt/core.h>

#ifdef __cplusplus
extern "C"
{
//FF-PART
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
//#include "libavresample/avresample.h"
#include "libswresample/swresample.h"
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
//FF-PART-End
#endif // #ifdef __cplusplus

//-main-section
#include <stdint.h>

#if GEO_PLATFORM == GEO_PLATFORM_APPLE
extern "C"
{
//#include "libavcodec/vda.h"
}
#include <CoreVideo/CVPixelBuffer.h>
#endif  // #if GEO_PLATFORM == GEO_PLATFORM_APPLE

#include "ff_decoder_type.h"
//-- 

namespace gv
{

void ff_reader_version(char* pVersion);
void ff_reader_prepare();

//bool ff_decoder_init();
//void ff_decoder_shutdown();


}

//-main-section-end
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif  // FF_READER_H_
