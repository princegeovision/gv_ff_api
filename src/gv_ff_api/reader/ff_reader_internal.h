#ifndef FF_READER_INTERNAL_H_
#define FF_READER_INTERNAL_H_

#include <stdio.h>

// platform define
#define FF_PLATFORM_APPLE 1
#define FF_PLATFORM_APPLE_IOS 2
#define FF_PLATFORM_WIN32 3
#define FF_PLATFORM_LINUX 4
#define FF_PLATFORM_ANDROID 5

// find platform
#if defined(__APPLE_CC__)
#   if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 40000 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 40000
#       define FF_PLATFORM FF_PLATFORM_APPLE_IOS
#   else
#       define FF_PLATFORM FF_PLATFORM_APPLE
#   endif
#elif defined(__WIN32__) || defined(_WIN32)
#   define FF_PLATFORM FF_PLATFORM_WIN32
#   error "did not support windows platform yet! abort! abort!"
#elif defined(__ANDROID__)
#	define FF_PLATFORM FF_PLATFORM_ANDROID
#else
#   define FF_PLATFORM FF_PLATFORM_LINUX
#   error "did not support linux platform yet! abort! abort!"
#endif
//define GEO_RELEASE_MSG
#if FF_PLATFORM != FF_PLATFORM_ANDROID
#   define FF_RELEASE_MSG(fmt, ...) printf(fmt, ## __VA_ARGS__)
#else
#   include <android/log.h>
#   define FF_RELEASE_MSG(fmt, ...) __android_log_print(ANDROID_LOG_VERBOSE, "ff_reader_", fmt, ## __VA_ARGS__)
#endif

// ---
typedef unsigned int        DWORD;
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
((DWORD)(unsigned char)(ch0) | ((DWORD)(unsigned char)(ch1) << 8) |   \
((DWORD)(unsigned char)(ch2) << 16) | ((DWORD)(unsigned char)(ch3) << 24 ))

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}
// ---
#include <thread>

typedef struct ffInternalReader
{
    int32_t         video_stream,audio_stream;
    AVFormatContext* format_ctx;
    AVCodecContext* codec_ctx;
    AVCodecContext* codec_ctx_audio;
    
    //boost::thread*              read_thread;
    std::thread*                read_thread;
    
    bool                        request_stop_read_;
    //boost::mutex                request_stop_read_mutex_;
    //boost::condition_variable   request_stop_read_condition_;
    std::mutex                  request_stop_read_mutex_;
    std::condition_variable     request_stop_read_condition_;
    //CXX11
    std::atomic<bool>           request_pause_read_;
    
    bool                        stop_read_audio_;
    
    int                         serial_num_;
    bool                        first_iframe;
} ffInternalReader;

//Internal-Functions DEF
//char* make_url_link(ffReaderConnectionInfo &urlparams, char* outputBuffer);

#endif  // FF_READER_INTERNAL_H_
