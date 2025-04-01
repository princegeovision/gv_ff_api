#include <iostream>
#include <iomanip> //std::put_time

#include "ff_reader.h"

#include "ff_reader_internal.h"
#include "reader_handler.hpp"

namespace gv
{
    bool reader_logger_ = false;//25 items
    bool g_ffmpeg_network_prepare = false;
    const unsigned int k_timeout_secs = 30;
    
    ffReaderHandler g_handler = ffReaderHandler();

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
    //Time-Function

    //Callback for ffmpeg
    //return 1 to interrupt.
    //return 0 to continue.
    //REF: https://stackoverflow.com/questions/68414179/avformatcontext-interrupt-callback-proper-usage
    //Design to interrupt after 12s
    static int rtsp_connect_interrupt_callback(void *ctx)
    {
        //ts-replace
        int64_t total_time = g_handler.getTimeDiff();
        //boost::posix_time::time_duration time_count = boost::posix_time::microsec_clock::local_time() - time_start;
        //int64_t total_time = time_count.total_milliseconds();
        //int64_t total_time = 0;
        if(total_time > k_timeout_secs * 1000) //12s
        {
            if(reader_logger_){spdlog::info("[RTSP]rtsp_connect_interrupt_callback = 0 [interrupt][](ts=%lld)\n", total_time);}
            return 1;//interrupt
        }
        //wait for no time, so continue
        //if(debug_logger_){GEO_RELEASE_MSG("[RTSP]rtsp_connect_interrupt_callback = 1 [continue](ts=%lld)\n", total_time);}
        return 0;//continue
    }


    bool ff_reader_prepare()
    {
        if(g_ffmpeg_network_prepare == false){
            //av_log_set_level(AV_LOG_DEBUG);
            int result = avformat_network_init();
            if (result < 0)
            {
                return false;
            }
            //ffmpeg 4.4, no need to call this.
            av_register_all();
            g_ffmpeg_network_prepare = true;
            return true;
        }
        return true;
    }
    char* make_url_link(ffReaderConnectionInfo *urlparams, char* outputBuffer)
    {
        if(urlparams->username[0] == 0       ||\
           urlparams->password[0] == 0       ||\
           urlparams->domain_name[0] == 0    ||\
           urlparams->port[0] == 0 )//          ||\
           //urlparams->path[0] == 0 )
        {

            return (char*)"";
        }
        unsigned long name_size =   strlen(urlparams->username) +    \
                                    strlen(urlparams->password) +    \
                                    strlen(urlparams->domain_name) + \
                                    strlen(urlparams->port) +        \
                                    strlen(urlparams->path) + 30;//WHY 30 -> Buffer bigger ?
        char* urlname = outputBuffer ;
        //urlname=(char*)malloc(name_size);
        memset(urlname,0,name_size);
        strcpy(urlname,"rtsp://");
        strcat(urlname,urlparams->username);
        strcat(urlname,":");
        strcat(urlname,urlparams->password);
        strcat(urlname,"@");
        strcat(urlname,urlparams->domain_name);
        strcat(urlname,":");
        strcat(urlname,urlparams->port);
        strcat(urlname,"/");
        strcat(urlname,urlparams->path);

        spdlog::info("[ff-reader] URL = {}\n", urlname);
        return urlname;
    }
    //internal func
    void rtsp_about_avformat_flush(AVFormatContext *s)
    {
        bool bShouldSkipFlush = true;
        if(bShouldSkipFlush){
            //Neet Test on Platform (iOS and Android)
        } else {
            int ret_flush = avformat_flush(s);
            spdlog::info("[RTSP] rtsp_about_avformat_flush!!(ret_flush=%d)\n", ret_flush);
        }
    }
    //internal-func
    ffReader* setup_reader_object(ffReaderConnectionInfo* inputInfo)
    {
        //Step0: Check URL
        char url_path[4096];
        memset( url_path, 0 , sizeof( url_path));

        make_url_link(inputInfo, url_path);

        AVFormatContext* format_ctx = avformat_alloc_context();
        AVDictionary*    pdictionary = NULL;
        //Step1:
        //default 使用 RTSP + TCP
        if(inputInfo->connection_type == k_ff_reader_transport_type_udp)
        {
            av_dict_set(&pdictionary,"rtsp_transport","udp",0);
            if(reader_logger_){spdlog::info("[RTSP] rtps_transport = udp\n");}

        } else{
            //av_dict_set(&pdictionary, "timeout", "-1", 0); // in secs
            //av_dict_set(&pdictionary, "stimeout", "30000000", 0); // in 30 secs
            //av_dict_set(&pdictionary, "stimeout", "-1", 0); // in 1.5 secs
            //av_dict_set(&pdictionary, "listen_timeout", "-1", 0);//RTSP in Seconds
            //Note-1121y24, stimeout ->ok
            // both stimeout and listen_timeout -> open fail.
            //"set timeout (in micrroseconds) of socket TCP I/O operations"
            //av_dict_set(&pdictionary, "stimeout", "10000000", 0);//10->20s
            //av_dict_set(&pdictionary, "stimeout", "20000000", 0);//20->40s
            //av_dict_set(&pdictionary, "stimeout", "25000000", 0);//25->51s
            av_dict_set(&pdictionary, "stimeout", "30000000", 0);//30->60s
            //av_dict_set(&pdictionary, "stimeout", "50000000", 0);//50s-82s
            //av_dict_set(&pdictionary, "stimeout ", "100000", 0);//60 , key has " "
            //av_dict_set(&pdictionary, "stimeout", "200000", 0);//60 (still, don't know why)
            //av_dict_set(&pdictionary, "listen_timeout", "3", 0);
            av_dict_set(&pdictionary,"rtsp_transport","tcp",0);
            if(reader_logger_){spdlog::info("[RTSP] rtps_transport = tcp\n");}
        }
        //Step2:
        format_ctx->interrupt_callback.callback = &rtsp_connect_interrupt_callback;//setup Interrupt Callback
        g_handler.setupTimeStart();

        int result = 0;
        result = avformat_open_input(&format_ctx, url_path, NULL, &pdictionary);
        if( pdictionary ){
            av_dict_free(&pdictionary);
        }
        ffRtspReaderLoginResult login_res;
        memset(&login_res, 0, sizeof(login_res));
        login_res.connect_result= result;
        if(result != 0){
            //ffmpeg3.0 support it ->AVERROR_HTTP_UNAUTHORIZED
            char buf[1024] ={0};
            av_strerror(result, buf, 1024);
            //ex: "Connection refused" -> maybe wrong port.
            spdlog::info("[RTSP] avformat_open_input open fail!!(reason=%s)\n", buf);
            if(result==AVERROR_INVALIDDATA)
            {
                login_res.connect_result= k_ff_reader_cr_fail_url_unauthorized;//(-1)
                spdlog::info("[RTSP_reader] url is valid,but account, password or filename is incorrect\n");

            } else {
                g_handler.logResult(result);

                login_res.connect_result = k_ff_reader_cr_fail_url;//(-2)
                spdlog::info("[RTSP_reader] url is invalid(URL={})\n", url_path);
            }
            //k_ff_reader_callback_type_event_login_result = 1
            inputInfo->reader_cb(k_ff_reader_callback_type_event_login_result, (void*)&login_res, inputInfo->reader_cb_user_arg);
            //params->loginresult_callback(&login_res, params->user_arg);
            return nullptr;
        }
        // Cloud crash, during pause & resume on Android Platform.
        // [REF:run twice may run into issue](https://stackoverflow.com/questions/70424422/twice-call-avformat-find-stream-info-crashed)
        result = avformat_find_stream_info(format_ctx, NULL);
        if(result < 0)
        {
            rtsp_about_avformat_flush(format_ctx);//avformat_flush(format_ctx);
            avformat_close_input(&format_ctx);
            return nullptr;
        }
        // Find the video stream and audio stream
        int stream_index = -1;
        int stream_index_audio = -1;
        bool audio_exist_or_not = k_ff_reader_audio_not_exist;
        AVCodecContext* codec_ctx_audio = nullptr;
        for(unsigned int i = 0; i < format_ctx->nb_streams; ++i)
        {
            //replace ffmpeg 4.4
            //format_ctx->streams[i]->codecpar->codec_type
            //from "format_ctx->streams[i]->codec->codec_type"
            if(format_ctx->streams[i] && \
               format_ctx->streams[i]->codec && \
               format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                stream_index = i;
                login_res.video_streaming_count++;
            }
            else if(format_ctx->streams[i] && \
                    format_ctx->streams[i]->codec && \
                    format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
            {
                stream_index_audio = i;
                audio_exist_or_not = k_ff_reader_audio_exist;
                codec_ctx_audio = format_ctx->streams[stream_index_audio]->codec;
                login_res.audio_streaming_count++;
            }
        }
        spdlog::info("[RTSP_reader] video Stream index = {} (-1 mean not found stream)\n", stream_index);
        spdlog::info("[RTSP_reader] audio Stream index = {} (-1 mean not found stream)\n", stream_index_audio);
        if(stream_index == -1)
        {
            rtsp_about_avformat_flush(format_ctx);//avformat_flush(format_ctx);
            avformat_close_input(&format_ctx);
            return nullptr;
        }
        AVCodecContext* codec_ctx = format_ctx->streams[stream_index]->codec;

        AVCodec* codec = avcodec_find_decoder(codec_ctx->codec_id);
        if(!codec)
        {
            rtsp_about_avformat_flush(format_ctx);//avformat_flush(format_ctx);
            avformat_close_input(&format_ctx);
            return nullptr;
        }
        result = avcodec_open2(codec_ctx, codec, NULL);
        if(result < 0)
        {
            rtsp_about_avformat_flush(format_ctx);//avformat_flush(format_ctx);
            avformat_close_input(&format_ctx);
            return nullptr;
        }
        ffInternalReader* internal = new ffInternalReader();
        internal->format_ctx = format_ctx;
        //video
        internal->codec_ctx = codec_ctx;
        internal->video_stream = stream_index;
        //audio
        internal->codec_ctx_audio = codec_ctx_audio;
        internal->audio_stream = stream_index_audio;

        internal->read_thread = nullptr;
        internal->request_stop_read_ = false;
        internal->request_pause_read_ = false;
        internal->first_iframe = false;
        internal->serial_num_    = 0;
        internal->stop_read_audio_ = true;
        
        ffReader* reader = (ffReader*)malloc(sizeof(ffReader));
        memset(reader, 0, sizeof(ffReader));

        reader->internal = internal;//setup new internal object
        reader->type_callback = inputInfo->reader_cb;
        reader->user_arg = inputInfo->reader_cb_user_arg;
        reader->check_audio_exsit_flag = audio_exist_or_not;
        login_res.connect_result = k_ff_reader_cr_success;//(0)
        
        inputInfo->reader_cb(k_ff_reader_callback_type_event_login_result, (void*)&login_res, inputInfo->reader_cb_user_arg);
        return reader;
    }
    ffReader* ff_reader_create(ffReaderConnectionInfo* inputInfo)
    {
        if(!inputInfo){
            spdlog::info("[ff-reader]ff_reader_create: NULL input");
        }

        //Prepare FFmpeg Network
        bool bPrepare = ff_reader_prepare();
        if(bPrepare == false){
            spdlog::info("[ff-reader]ff_reader_create: failure by  avformat_network_init result < 0");
            return nullptr;
        }
        return setup_reader_object(inputInfo);
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
