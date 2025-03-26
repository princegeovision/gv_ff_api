#include <iostream>

#include "ff_reader.h"

#include "ff_reader_internal.h"

namespace gv
{
    bool reader_logger_ = false;//25 items
    void ff_reader_version(char* pVersion)    
    {
        spdlog::info("[ff-reader]version: {}", "1.0.0");
        unsigned codecVer = avcodec_version();//"4.4.3#3"
        int ver_major,ver_minor,ver_micro;
        ver_major = (codecVer>>16)&0xff;    //58
        ver_minor = (codecVer>>8)&0xff;     //134
        ver_micro = (codecVer)&0xff;        //100
        spdlog::info("[ff-reader]version: {}.{}.{}-{}", \
            ver_major, ver_minor, ver_micro, \
            avcodec_license());
    }

    void ff_reader_prepare()
    {
        //av_log_set_level(AV_LOG_DEBUG);
        int result = avformat_network_init();
        if (result < 0)
        {
            return;
        }
        //ffmpeg 4.4, no need to call this.
        //av_register_all();
    }
    void ff_reader_release(ffReader** reader)
    {
        if (*reader)
        {
            avformat_network_deinit();
            
            ffInternalReader* interal = (*reader)->internal;
            
            avcodec_close(interal->codec_ctx);
            avcodec_close(interal->codec_ctx_audio);
            
            avformat_flush(interal->format_ctx);
            avformat_close_input(&interal->format_ctx);
            
            delete interal;
            free(*reader);
            
            *reader = nullptr;
            if(reader_logger_){FF_RELEASE_MSG("[RTSP] release success!!\n");}
        }
    }
}
