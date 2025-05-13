#ifndef FF_FILE_HANDLER_INTERNAL_H_
#define FF_FILE_HANDLER_INTERNAL_H_

#include <stdio.h>

// ---
typedef unsigned int        DWORD;
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
((DWORD)(unsigned char)(ch0) | ((DWORD)(unsigned char)(ch1) << 8) |   \
((DWORD)(unsigned char)(ch2) << 16) | ((DWORD)(unsigned char)(ch3) << 24 ))

// extern "C"
// {
// #include "libavformat/avformat.h"
// #include "libavcodec/avcodec.h"
// }
// ---
#include <thread>

//REF: ffInternalReader
typedef struct ffInternalfh
{
    //Note: We need to use as
    //- stream_muxer
    //- stream_player


    int32_t stream_user_type;

} ffInternalfh;

#endif  // FF_FILE_HANDLER_INTERNAL_H_
