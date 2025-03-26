#include <iostream>

#include "ff_reader.h"


namespace gv
{
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
    // bool ff_decoder_init()
    // {
    //     spdlog::info("[ff-decoder]init>>");
    //     std::unique_lock<std::mutex> lock(g_ffmpeg_mutex);
    //     if (!g_avcodec_registed)
    //     {
    //         //av_register_all();// (use in reader) //has been deprecated in ffmpeg 4.0
    //         avcodec_register_all();
    //         g_avcodec_registed = true;
    //     }

    //     spdlog::info("[ff-decoder]init<<");
    //     return false;
    // }
    // void ff_decoder_shutdown()
    // {
    //     spdlog::info("[ff-decoder]shutdown>><<");
    // }
}
