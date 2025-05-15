#include <iostream>
#include <iomanip> //std::put_time
#include <sstream> //std::stringstream

#include "ff_reader.h"

#include "ff_reader_internal.h"
#include "reader_handler.hpp"

namespace gv
{
    bool reader_logger_ = true;//25 items
    bool g_ffmpeg_network_prepare = false;
    const unsigned int k_timeout_secs = 30;
    
    ffReaderHandler g_handler = ffReaderHandler();

    //REF: gvRtspReadFrameStatus
    //return 0 = reader internal status is not ready to read.
    //return 1 = successfully read frame, from network.
    //return 2 = Pause status, we should not Read and callback as well.
    //return 3 = readed frame is EOF.
    int rtsp_get_next_frame(ffReader* reader, AVFrame* frame);

    void ff_reader_version(char* pVersion)
    {
        spdlog::info("[ff-reader]version: {}", "1.3.0");
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
        internal->request_stop_read_flag = false;
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
    bool estimate_ts_ = false;
    // CXX11-TS Function
    //Better TS from CXX11
    //We can call anything with no param to estimate duration in ms.
    //Get process time duration in ms (or could be seconds)
    template <typename Func>
    long long RTSPS_Reader_TimeFunc(Func f)
    {
        auto begin = std::chrono::steady_clock::now();
        f();
        auto end = std::chrono::steady_clock::now();
        //return duration_cast<seconds>(end - begin).count();
        return duration_cast<std::chrono::milliseconds>(end - begin).count();
    }
    //Thread-func
    void start_read_rtsp_stream(ffReader* reader)
    {
        if (!reader || !reader->internal)
        {
            return;
        }
        ffInternalReader* internal = reader->internal;
        //m1 Simplest way to lock a mutex for the duration of a scoped block.
        //std::lock_guard<std::mutex> request_stop_lock(internal->request_stop_read_mutex_);
        //m2  More flexible, feature-rich locking mechanism.
        std::unique_lock<std::mutex> request_stop_lock(internal->request_stop_read_mutex_);

        // start a worker to start reading frame
        AVFrame* frame = av_frame_alloc();

        if(reader_logger_){spdlog::info("[RTSP] read start!!\n");}
        //bool got_frame = false;
        int got_frame_value = 0;
        bool should_continue = true;//default = true, at least get 1 frame.

        do {
            //v2
            if(estimate_ts_){
                auto ms = RTSPS_Reader_TimeFunc([&](){got_frame_value = rtsp_get_next_frame(reader, frame);});
                if(reader_logger_){spdlog::info("[RTSP] Reader_TimeFunc=%lld\n", ms);}
            } else {
                //REF: gvRtspReadFrameStatus
                //return 0 = false, return 1 = true, restun 2 = skip
                got_frame_value = rtsp_get_next_frame(reader, frame);
                if(reader_logger_){spdlog::info("[RTSP] No Estimate TS(got_frame_value={})\n", got_frame_value);}
            }
            //k_ff_reader_reader_yet_ready = 0,
            //k_ff_reader_success_readed = 1,
            //k_ff_reader_pause_read = 2,
            //k_ff_reader_read_eof = 3
            if(internal->request_stop_read_flag == true){

                auto tp_now = std::chrono::system_clock::now();
                internal->request_stop_read_condition_.wait(request_stop_lock);
                if(reader_logger_){spdlog::info("[RTSP] success_readed(got_frame_value={})\n", got_frame_value);}

            }
//            if(got_frame_value  == k_ff_reader_reader_yet_ready) {
//                
//                auto tp_now = std::chrono::system_clock::now();
//                internal->request_stop_read_condition_.wait(request_stop_lock);
//                if(reader_logger_){spdlog::info("[RTSP] yet_ready(got_frame_value={})\n", got_frame_value);}
//
//            } else if(got_frame_value == k_ff_reader_success_readed){
//
//                auto tp_now = std::chrono::system_clock::now();
//                internal->request_stop_read_condition_.wait(request_stop_lock);
//                if(reader_logger_){spdlog::info("[RTSP] success_readed(got_frame_value={})\n", got_frame_value);}
//
//            } else if(got_frame_value == k_ff_reader_pause_read){
//                //Skip
//                auto tp = std::chrono::system_clock::now() + std::chrono::milliseconds(100);
//                //internal->request_stop_read_condition_.wait(request_stop_lock)
//
//            } else if(got_frame_value == k_ff_reader_read_eof){
//                auto tp = std::chrono::system_clock::now() + std::chrono::milliseconds(2000);
//            }

        } while (!internal->request_stop_read_flag && (should_continue == true));

        av_frame_free(&frame);
    }
    // internal used
    //-CONVERT-FUNC
    int32_t rtsp_convert_codec_type(const AVCodecID ffmpeg_codec_id)
    {
        unsigned int codec_type = 0;
        switch (ffmpeg_codec_id)
        {
            case AV_CODEC_ID_MPEG4:
            case AV_CODEC_ID_MSMPEG4V1:
            case AV_CODEC_ID_MSMPEG4V2:
            case AV_CODEC_ID_MSMPEG4V3:
                codec_type = MAKEFOURCC('G', 'M', '4', '0');
                break;
            case AV_CODEC_ID_MJPEG:
            case AV_CODEC_ID_LJPEG:
            case AV_CODEC_ID_JPEGLS:
            case AV_CODEC_ID_JPEG2000:
                codec_type = MAKEFOURCC('G', 'J', 'P', 'G');
                break;
            case AV_CODEC_ID_H264:
                codec_type = MAKEFOURCC('X', '2', '6', '4');
                break;
            case AV_CODEC_ID_HEVC:
                codec_type = MAKEFOURCC('G', 'H', 'E', 'C') ;
                break;
            default:
                codec_type = 0;
        }

        return codec_type;
    }
    //-CONVERT-FUNC
    unsigned int rtsp_audio_format_converter(int channels, int bits_per_coded_sample, int sample_rate)
    {
        int value_samplerate = sample_rate/ 1000;
        int value_bitspersample = bits_per_coded_sample ;

        std::string str1,str2,str3,last_str;
        std::stringstream string_samplerate(str1);
        std::stringstream string_bitspersample(str2);
        std::stringstream string_channels(str3);
        string_samplerate<<value_samplerate;
        string_bitspersample<<value_bitspersample;
        string_channels<<channels;

        //ex: AF_44K_8b
        if(channels == 1)
        {
            last_str="AF_"+string_samplerate.str()+"K_"+string_bitspersample.str()+"b";
            last_str.c_str();
        }
        else
        {
            last_str="AF_"+string_samplerate.str()+"K_"+string_bitspersample.str()+"b_"+string_channels.str()+"H";
            last_str.c_str();
        }

        //RtspLog((char*)"Audio Format", (char*)last_str.c_str(),write_error_flag);

        //Note: channels=3->return=0.
        if      (last_str.compare("AF_8K_8b")==0)      return AF_8K_8b;
        else if (last_str.compare("AF_32K_8b")==0)     return AF_32K_8b;
        else if (last_str.compare("AF_44K_8b")==0)     return AF_44K_8b;
        else if (last_str.compare("AF_48K_8b")==0)     return AF_48K_8b;
        else if (last_str.compare("AF_8K_3b")==0)      return AF_8K_3b;
        else if (last_str.compare("AF_32K_16b")==0)    return AF_32K_16b;
        else if (last_str.compare("AF_44K_16b")==0)    return AF_44K_16b;
        else if (last_str.compare("AF_48K_16b")==0)    return AF_48K_16b;
        else if (last_str.compare("AF_8K_4b")==0)      return AF_8K_4b;
        else if (last_str.compare("AF_8K_5b")==0)      return AF_8K_5b;
        else if (last_str.compare("AF_8K_6b")==0)      return AF_8K_6b;
        else if (last_str.compare("AF_8K_7b")==0)      return AF_8K_7b;
        else if (last_str.compare("AF_8K_2b")==0)      return AF_8K_2b;
        else if (last_str.compare("AF_8K_16b")==0)     return AF_8K_16b;
        else if (last_str.compare("AF_16K_8b")==0)     return AF_16K_8b;
        else if (last_str.compare("AF_16K_16b")==0)    return AF_16K_16b;
        else if (last_str.compare("AF_44K_8b_2H")==0)  return AF_44K_8b_2H;
        else if (last_str.compare("AF_44K_16b_2H")==0) return AF_44K_16b_2H;
        else if (last_str.compare("AF_16K_8b_2H")==0)  return AF_16K_8b_2H;
        else if (last_str.compare("AF_16K_16b_2H")==0) return AF_16K_16b_2H;
        else if (last_str.compare("AF_8K_8b_2H")==0)   return AF_8K_8b_2H;
        else if (last_str.compare("AF_8K_16b_2H")==0)  return AF_8K_16b_2H;
        else if (last_str.compare("AF_8K_3b_2H")==0)   return AF_8K_3b_2H;
        else if (last_str.compare("AF_32K_16b_2H")==0) return AF_32K_16b_2H;
        else if (last_str.compare("AF_24K_16b")==0)    return AF_24K_16b;
        else                                           return 0;
    }
    //-CONVERT-FUNC
    int32_t rtsp_convert_audio_codec_type(const AVCodecID ffmpeg_codec_id)
    {
        int32_t codec_type = 0;
        switch (ffmpeg_codec_id)
        {
            case AV_CODEC_ID_ADPCM_MS:
                codec_type =EN_AC_MS_ADPCM;
                break;
            case AV_CODEC_ID_MP1:
                codec_type = EN_AC_MP1;
                break;
            case AV_CODEC_ID_MP2:
                codec_type = EN_AC_MP2;
                break;
            case AV_CODEC_ID_MP3:
                codec_type = EN_AC_MP3;
                break;
            case AV_CODEC_ID_MP4ALS:
                codec_type = EN_AC_MP4_AAC;
                break;
            case AV_CODEC_ID_AAC:
                codec_type = EN_AC_MP4_AAC;
                break;
            case AV_CODEC_ID_PCM_MULAW:
                codec_type = EN_AC_G711;
                break;
            case AV_CODEC_ID_PCM_ALAW:
                codec_type = EN_AC_PCM;
                break;
                break;
            case AV_CODEC_ID_ADPCM_IMA_QT:
            case AV_CODEC_ID_ADPCM_IMA_WAV:
            case AV_CODEC_ID_ADPCM_IMA_DK3:
            case AV_CODEC_ID_ADPCM_IMA_DK4:
            case AV_CODEC_ID_ADPCM_IMA_WS:
            case AV_CODEC_ID_ADPCM_IMA_SMJPEG:
            case AV_CODEC_ID_ADPCM_4XM:
            case AV_CODEC_ID_ADPCM_XA:
            case AV_CODEC_ID_ADPCM_ADX:
            case AV_CODEC_ID_ADPCM_EA:
            case AV_CODEC_ID_ADPCM_G726:
            case AV_CODEC_ID_ADPCM_CT:
            case AV_CODEC_ID_ADPCM_SWF:
            case AV_CODEC_ID_ADPCM_YAMAHA:
            case AV_CODEC_ID_ADPCM_SBPRO_4:
            case AV_CODEC_ID_ADPCM_SBPRO_3:
            case AV_CODEC_ID_ADPCM_SBPRO_2:
            case AV_CODEC_ID_ADPCM_THP:
            case AV_CODEC_ID_ADPCM_IMA_AMV:
            case AV_CODEC_ID_ADPCM_EA_R1:
            case AV_CODEC_ID_ADPCM_EA_R3:
            case AV_CODEC_ID_ADPCM_EA_R2:
            case AV_CODEC_ID_ADPCM_IMA_EA_SEAD:
            case AV_CODEC_ID_ADPCM_IMA_EA_EACS:
            case AV_CODEC_ID_ADPCM_EA_XAS:
            case AV_CODEC_ID_ADPCM_EA_MAXIS_XA:
            case AV_CODEC_ID_ADPCM_IMA_ISS:
            case AV_CODEC_ID_ADPCM_G722:
            case AV_CODEC_ID_ADPCM_IMA_APC:
                codec_type = EN_AC_INTEL_ADPCM;
                break;

            default:
                codec_type = 0;

        }

        return codec_type;
    }
    //
    //Previously we check "reader->internal->request_pause_read_" flag before we read.
    //SOP01 : check flag(true) , then read next frame
    //SOP02 : check flag(false) , do nothing.
    //return 0 = false
    //return 1 = true
    //restun 2 = skip
    int rtsp_get_next_frame(ffReader* reader, AVFrame* frame)
    {
        int ret_value = 0;
        if (!reader || \
            !reader->internal || \
            !reader->internal->format_ctx || \
            !reader->internal->format_ctx->streams[reader->internal->video_stream] || \
            !reader->internal->codec_ctx)
        {
            return ret_value;
        }
        AVFormatContext* format_ctx = reader->internal->format_ctx;
        int32_t video_stream = reader->internal->video_stream;
        int32_t audio_stream = reader->internal->audio_stream;

        int64_t duration = format_ctx->duration;
        int64_t frames_total_count = format_ctx->streams[video_stream]->nb_frames;
        float avg_fps = (float)format_ctx->streams[video_stream]->avg_frame_rate.num / (float)format_ctx->streams[video_stream]->avg_frame_rate.den;
        if (frames_total_count == 0)
        {
            frames_total_count = (int64_t)((duration / (double)AV_TIME_BASE) * avg_fps + 0.5);
        }

        double video_stream_time_factor = av_q2d(format_ctx->streams[video_stream]->time_base);
        double audio_stream_time_factor = reader->check_audio_exsit_flag? av_q2d(format_ctx->streams[audio_stream]->time_base): 0;

        double duration_secs = (double)duration / (double)AV_TIME_BASE;
        //This value help us save do we have readed frame from do-while-loop.
        int got_frame = 0;
        //This flag use to determine "Do we need to keep_reading?"
        //Flag is default = true.
        //Flag is false. when frame packet data is read from ffmpeg.
        //To finish this run, do-while-loop will break when flag == True.
        bool read_next = true;

        //gv_rtsp_reader_video_callback videocallback = reader->video_callback;
        //gv_rtsp_reader_audio_callback audiocallback = reader->audio_callback;
        //gv_rtsp_reader_event_callback eventcallback = reader->event_callback;
        AVCodecContext* codec_ctx = reader->internal->codec_ctx;
        AVCodecContext* codec_ctx_audio = reader->internal->codec_ctx_audio;

        do {

            //g_handler.setupTimeStart();
            if(reader->internal->request_pause_read_ == true){

                ret_value = k_ff_reader_pause_read;//k_rtsp_pause_read;
                if(reader_logger_){spdlog::info("[RTSP] Pause continue\n");}
                break; // Fixed Bug for Busy loop when RTSP Reading is pause.
            } else {
                //Statements
                AVPacket* packet = av_packet_alloc();
                int result = av_read_frame(format_ctx, packet);
                if(0 > result)
                {
                    //meaning return error with packet blank.
                    if (result == AVERROR(EAGAIN))
                    {
                        continue;
                    } else {
                        //Feature-Mark : (1017y22)
                        //ffmpeg "av_read_frame" EOF in this loop will create event rapidly
                        //Which could crash the whole app.
                        ffRtspReaderEventInfo event_info;
                        memset(&event_info, 0, sizeof(event_info));
                        event_info.read_eof = READ_PACKET_EOF;
                        //eventcallback(&event_info, reader->user_arg);

                        reader->type_callback(k_ff_reader_callback_type_event_data, (void*)&event_info, reader->user_arg);
                        if(reader_logger_){spdlog::info("[RTSP] event callback(EOF)\n");}
                        ret_value = k_ff_reader_read_eof;//k_rtsp_read_eof;
                        break;
                    }
                }
                //-Video-Part
                if(video_stream == packet->stream_index)
                {
                    reader->internal->serial_num_ ++ ;

                    got_frame = 1;
                    ret_value = 1;
                    read_next = false;

                    if(reader->internal->request_pause_read_ == true){
                        //Skip VideoCallback (During Reading, RTSP Reader pause status is changed.
                        if(reader_logger_){spdlog::info("[RTSP] request_pause_read_== true\n");}

                        ret_value = 2;
                    } else {
                        if(reader_logger_){spdlog::info("[RTSP] request_pause_read_== false\n");}
                        double frame_time = packet->dts * video_stream_time_factor;

                        ffReaderVideoInfo info;
                        memset(&info, 0, sizeof(info));

                        time_t now = time(0);
                        tm *ltm = localtime(&now);
                        info.video_time_year                = ltm->tm_year ;
                        info.video_time_day                 = ltm->tm_mday;
                        info.video_time_month               = ltm->tm_mon;
                        info.video_time_hour                = ltm->tm_hour;
                        info.video_time_minute              = ltm->tm_min;
                        info.video_time_second              = ltm->tm_sec;
                        if(reader_logger_){spdlog::info("[RTSP] videocallback[ts:]<{} - {} - {} >({} : {} : {}) \n", \
                                                          info.video_time_year+1900, \
                                                          info.video_time_month+1, \
                                                          info.video_time_day, \
                                                          info.video_time_hour, \
                                                          info.video_time_minute, \
                                                          info.video_time_second );}
                        info.codec_type = rtsp_convert_codec_type(codec_ctx->codec_id);
                        info.width = codec_ctx->width;
                        info.height = codec_ctx->height;
                        info.key_frame = (AV_PKT_FLAG_KEY == (packet->flags & AV_PKT_FLAG_KEY));
                        info.timestamp = frame_time;
                        info.total_duration = duration_secs;
                        info.raw_data = (uint8_t*)malloc(packet->size);
                        info.serial_number = reader->internal->serial_num_;

                        memcpy(info.raw_data, packet->data, packet->size);

                        if (codec_ctx->extradata && codec_ctx->extradata_size > 0)
                        {
                            info.extra_data = (uint8_t*)malloc(codec_ctx->extradata_size);
                            memcpy(info.extra_data, codec_ctx->extradata, codec_ctx->extradata_size);
                            info.extra_data_size = codec_ctx->extradata_size;
                        }

                        info.raw_data_size = packet->size;
                        //videocallback(&info, reader->user_arg);
                        reader->type_callback(k_ff_reader_callback_type_video_data, (void*)&info, reader->user_arg);
                        if(reader_logger_){spdlog::info("[RTSP] videocallback[key:{}]<No:{}> ({} x {}) : {}\n", info.key_frame,info.serial_number, info.width, info.height, info.raw_data_size);}
                        free(info.raw_data);
                        free(info.extra_data);
                    }

                }
                //-Audio-Part
                if(audio_stream == packet->stream_index)
                {
                    got_frame = 1;
                    ret_value = 1;
                    read_next = false;

                    if(!reader->internal->stop_read_audio_)
                    {
                        if(reader->internal->request_pause_read_ == true){
                            if(reader_logger_){spdlog::info("[RTSP] audiocallback(Skip)\n");}
                            ret_value = 2;
                        } else {
                            double frame_time = packet->dts * audio_stream_time_factor;

                            ffRtspReaderAudioInfo audio_info;
                            memset(&audio_info, 0, sizeof(audio_info));

                            audio_info.codec_type = rtsp_convert_audio_codec_type(codec_ctx_audio->codec_id);
                            audio_info.timestamp = frame_time;
                            audio_info.total_duration = duration_secs;

                            audio_info.raw_data = (uint8_t*)malloc(packet->size);
                            memcpy(audio_info.raw_data, packet->data, packet->size);

                            audio_info.raw_data_size = packet->size;
                            audio_info.sample_rate   = codec_ctx_audio->sample_rate;
                            audio_info.channels      = codec_ctx_audio->channels;
                            audio_info.bit_per_sample= codec_ctx_audio->bits_per_coded_sample;
                            audio_info.key_frame = (AV_PKT_FLAG_KEY == (packet->flags & AV_PKT_FLAG_KEY));
                            audio_info.format = rtsp_audio_format_converter(codec_ctx_audio->channels, codec_ctx_audio->bits_per_coded_sample, codec_ctx_audio->sample_rate);

                            //audiocallback(&audio_info, reader->user_arg);
                            reader->type_callback(k_ff_reader_callback_type_audio_data, (void*)&audio_info, reader->user_arg);
                            if(reader_logger_){spdlog::info("[RTSP] audiocallback<C:{}> <S:{}> <B:{}> <size={}>\n", audio_info.channels,audio_info.bit_per_sample, audio_info.sample_rate, audio_info.raw_data_size);}
                            free(audio_info.raw_data);
                        }
                    }
                }

                av_packet_free(&packet);
            }
        } while (read_next && !reader->internal->request_stop_read_flag);

        return ret_value;
    }

    //Internal-
    bool reader_start(ffReader* reader)
    {
        if (!reader || !reader->internal || reader->internal->read_thread)
        {
            return false;
        }
        {
            std::lock_guard<std::mutex> lock(reader->internal->request_stop_read_mutex_);
            reader->internal->read_thread = new std::thread(std::bind(start_read_rtsp_stream, reader));
        }
        return true;
    }
    bool reader_stop(ffReader* reader)
    {
        if (!reader || !reader->internal || !reader->internal->read_thread)
        {
            return false;
        }
        {
            //Note:
            // - This part is not the same as org-design
            // - We find std::cv and mutex work differently
            // - In order to get lock and notify and delete reading thread
            // - we need to set flag to make thread inside wait for us.
            reader->internal->request_stop_read_flag = true;
            reader->internal->request_pause_read_ = false;
            std::lock_guard<std::mutex> lock(reader->internal->request_stop_read_mutex_);


        }
        reader->internal->request_stop_read_condition_.notify_all();
        reader->internal->read_thread->join();
        delete reader->internal->read_thread;
        reader->internal->read_thread = nullptr;
        {
            std::lock_guard<std::mutex> lock(reader->internal->request_stop_read_mutex_);
            reader->internal->request_stop_read_flag = false;
        }
        return true;
    }
    //PUBLIC-API-01
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
    //PUBLIC-API-02
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
    //PUBLIC-API-03
    int ff_reader_action(ffReader* reader, ff_reader_action_info action_info)
    {
        int action_result = k_ff_reader_action_result_fail;
        int action = action_info.action_type;

        switch (action) {
            case k_ff_reader_action_type_start:
            {
                if(reader_logger_){FF_RELEASE_MSG("[ACTION]-Start\n");}
                bool start_result = reader_start(reader);
                if(start_result == true){
                    action_result = k_ff_reader_action_result_ok;
                } else {
                    action_result = k_ff_reader_action_result_fail;
                }
            }
                break;
            case k_ff_reader_action_type_stop:
            {
                if(reader_logger_){FF_RELEASE_MSG("[ACTION]-Stop\n");}
                bool stop_result = reader_stop(reader);
                if(stop_result == true){
                    action_result = k_ff_reader_action_result_ok;
                } else {
                    action_result = k_ff_reader_action_result_fail;
                }
            }
                break;
            default:
                break;
        }
        return action_result;
    }
}
