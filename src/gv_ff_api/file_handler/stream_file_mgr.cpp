//
//  stream_file_mgr.cpp
//  stream_file_mgr
//
//  Created by ArronPJ on 05/08/25.
//  Copyright (c) 2025 GTDigital. All rights reserved.
//


#include "stream_file_mgr.hpp"



//3rd-party
#include <spdlog/spdlog.h>
#include <fmt/core.h>

//CXX11
#include <chrono>
#include <time.h>   //ctime_s , ctime
#include <stdio.h>

//MARK: Singleton
#include <iostream>
#include <sstream>      // std::ostringstream
#include <iomanip>      //std::put_time

namespace gv
{

stream_file_status::stream_file_status()
{
    //Note:
    // - When Player release we need to reset stream Feature Status
    _waitFirstFrame = true;
    _waitFirstAudio = true;
}

//file_progress::file_progress()
//{
//    //Note:
//    has_progress_info = false;
//    progress = 0.0f;
//}

bool stream_file_mgr::handler_cb()
{
    return false;
}
static const char basis_64[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int B64encode_len(int len)
{
    return ((len + 2) / 3 * 4) + 1;
}
int B64encode(char *encoded, const char *string, int len)
{
    int i;
    char *p;

    p = encoded;
    for (i = 0; i < len - 2; i += 3) {
        *p++ = basis_64[(string[i] >> 2) & 0x3F];
        *p++ = basis_64[((string[i] & 0x3) << 4) |
                        ((int) (string[i + 1] & 0xF0) >> 4)];
        *p++ = basis_64[((string[i + 1] & 0xF) << 2) |
                        ((int) (string[i + 2] & 0xC0) >> 6)];
        *p++ = basis_64[string[i + 2] & 0x3F];
    }
    if (i < len) {
        *p++ = basis_64[(string[i] >> 2) & 0x3F];
        if (i == (len - 1)) {
            *p++ = basis_64[((string[i] & 0x3) << 4)];
            *p++ = '=';
        }
        else {
            *p++ = basis_64[((string[i] & 0x3) << 4) |
                            ((int) (string[i + 1] & 0xF0) >> 4)];
            *p++ = basis_64[((string[i + 1] & 0xF) << 2)];
        }
        *p++ = '=';
    }

    *p++ = '\0';
    return p - encoded;
}
int compare_float(const float a, const float b)
{
    if (a > b){
        return 1;
    } else if (a < b) {
        return -1;
    } else {
        return 0;
    }
}
//Return "" mean fail
//We use AVCodecContext as Extra_Data, this method will make upgrade ffmpeg version impossible
//or the header information will need to add ffmpeg version info inside.
std::string stream_file_mgr::get_encode_text(uint8_t* inputData, \
                                             int32_t inputSize, \
                                             int32_t codec)
{
    char* encoded_extradata = nullptr;
    int encoded_extradata_size = 0;
    if(inputData && inputSize > 0){
        int encoded_extradata_length = B64encode_len(inputSize);
        if (0 >= encoded_extradata_length)
        {
            return "";
        }
        encoded_extradata = (char*)malloc(encoded_extradata_length);
        if (!encoded_extradata)
        {
            return "";
        }
        encoded_extradata_size = B64encode(encoded_extradata, (char*)inputData, inputSize);
        if (0 >= encoded_extradata_size)
        {
            free(encoded_extradata);
            return "";
        }
        std::string extra_data = std::string(encoded_extradata);
        std::ostringstream jsonStream;
        //1128613959, GHEC = 1004 k_ff_reader_codec_type_hevc
        //808734023 , GM40 = 1001 k_ff_reader_codec_type_mp4
        //875967064 , X264 = 1003 k_ff_reader_codec_type_264
        //1196444231, GJPG = 1002 k_ff_reader_codec_type_jpg
        int32_t codec_value = codec;
        jsonStream << "{\n"
                   << "  \"extradata\":\"" << extra_data << "\",\n"
                   << "  \"codec\": " << codec_value << "\n"
                   << "}";
        std::string jsonString = jsonStream.str();
        return jsonString;
    }
    return "";
}
//'G' = 0x47, 'H' = 0x48, 'E' = 0x45, 'C' = 0x43
//codec: 1128613959;//0x43454847

stream_file_mgr::stream_file_mgr()
{
    b_sfm_log_ = true;
    //std::unique_ptr<gv::FeatureStrategy> strategy = std::make_unique<gv::FeatureStrategy>();
    //std::unique_ptr<gv::FeatureWrite> strategy = std::make_unique<gv::FeatureWrite>();
    strategy = std::make_unique<gv::FeatureWrite>();
}

stream_file_mgr::~stream_file_mgr()
{

}

bool stream_file_mgr::init(std::string file_path, bool support_audio)
{
    bool init_result = false;
    std::string video_path = file_path + ".gvp";
    file_obj_info input_info;
    input_info.fname = video_path;
    //input_info.mode = std::ios::in | std::ios::out;// Read mode and write mode
    input_info.mode = std::ios::out | std::ios::app; //Creating a File When Opening (for Writing)

    m_video_sfo_ = new stream_file_obj(input_info);

    bool inited_video = m_video_sfo_->opened();

    if (!inited_video)
    {
        if(b_sfm_log_){spdlog::info("[ff-stream_file_mgr]open video fail: {}", video_path);}
        init_result = false;
        m_file_path_ = file_path;
        return init_result;
    } else {
        if(b_sfm_log_){spdlog::info("[ff-stream_file_mgr]open video ok: {}", video_path);}
        init_result = true;
    }
    std::string video_header_path = file_path + "_h.gvp";
    file_obj_info header_info;
    header_info.fname = video_header_path;
    //std::ios::app : Appends to existing file
    //std::ios::trunc: Clears existing content
    header_info.mode = std::ios::out | std::ios::trunc; //Creating a File When Opening (for Writing)
    m_video_header_sfo_ = new stream_file_obj(header_info);
    bool inited_video_header = m_video_header_sfo_->opened();
    if (!inited_video_header)
    {
        if(b_sfm_log_){spdlog::info("[ff-stream_file_mgr]create video header fail: {}", video_path);}
    } else {
        if(b_sfm_log_){spdlog::info("[ff-stream_file_mgr]create video header ok: {}", video_path);}
    }

    if(support_audio)
    {
        std::string audio_path = file_path + ".gap";
        file_obj_info audio_info;
        audio_info.fname = audio_path;
        //Creating a File When Opening (for Writing)
        audio_info.mode = std::ios::out | std::ios::app;
        m_audio_sfo_ = new stream_file_obj(audio_info);
        bool inited_audio = m_audio_sfo_->opened();
        if (!inited_audio)
        {
            //Since Audio file is optional in current support,
            //This will not return as flag.
            if(b_sfm_log_){spdlog::info("[ff-stream_file_mgr]open audio fail: {}", video_path);}
            //init_result = false;
            //return init_result;
        } else {
            if(b_sfm_log_){spdlog::info("[ff-stream_file_mgr]open video ok: {}", video_path);}
            init_result = true;
        }
    }
    m_file_path_ = file_path;
    return init_result;
}
void stream_file_mgr::shutdown()
{
    //m_video_sfo_
    if(m_video_sfo_){
        m_video_sfo_->shutdown();
        delete m_video_sfo_;
        m_video_sfo_ = nullptr;
    }
    //m_audio_sfo_
    if(m_audio_sfo_){
        m_audio_sfo_->shutdown();
        delete m_audio_sfo_;
        m_audio_sfo_ = nullptr;
    }
}
bool stream_file_mgr::write_info(const ff_file_handler_action_info ainfo)
{
    //Note: action Info can be
    // - Video Frame
    // - Audio Frame
    if(ainfo.detail_type == k_ff_file_handler_detail_type_video)
    {
        ffReaderVideoInfo* pVideoInfo = (ffReaderVideoInfo*)ainfo.p_detail;
        //Phase01: Write Header if header is not exist yet.
        bool b_write_header = (m_status_._waitFirstFrame) ? true : false;
        if(b_write_header){
            //Write Header Data

            //Extra-Data, use base64encode
            if(b_sfm_log_){spdlog::info("[ff-stream_file_mgr]pVideoInfo->extra_data_size {}", pVideoInfo->extra_data_size);}
            //if(b_sfm_log_){spdlog::info("[ff-stream_file_mgr]pVideoInfo->codec_type {}", pVideoInfo->codec_type);}//un-clear use GV-Define MACRO
            if(b_sfm_log_){spdlog::info("[ff-stream_file_mgr]pVideoInfo->ff_codec_type {}", pVideoInfo->ff_codec_type);}
            std::string header_str = stream_file_mgr::get_encode_text(pVideoInfo->extra_data, \
                                                                      pVideoInfo->extra_data_size, \
                                                                      pVideoInfo->ff_codec_type);
            bool result_wh = m_video_header_sfo_->write_string(header_str);
            if(result_wh == true){
                //Once write_header is ok, update flag
                m_status_._waitFirstFrame = false;
            }
            if(b_sfm_log_){spdlog::info("[ff-stream_file_mgr]header_str: {}-({})", header_str, result_wh);}
        }
        //Phase02:
        //if(b_sfm_log_){spdlog::info("[ff-stream_file_mgr]Phase02");}
        file_progress fpi = getProgress(pVideoInfo);
        //Phase03:
        if(b_sfm_log_){spdlog::info("[ff-stream_file_mgr]Phase03");}
        //Write Frame into 
        //m_video_sfo_
        //REF-S: GvStreamPlayerWriteInput
    } else if(ainfo.detail_type == k_ff_file_handler_detail_type_audio)
    {
        if(b_sfm_log_){spdlog::info("[ff-stream_file_mgr]write_info (A)");}

    }
    return false;
}
//Private-fun
file_progress stream_file_mgr::getProgress(ffReaderVideoInfo* vi)
{
    if(b_sfm_log_){spdlog::info("[ff-stream_file_mgr]getProgress>>");}
    file_progress fpi;

    bool has_progress_info = (1 == compare_float(vi->total_duration, 0.0));
    double progress = vi->timestamp / vi->total_duration * 100.0f;
    fpi.has_progress_info = has_progress_info;
    fpi.progress = progress;
    if(b_sfm_log_){spdlog::info("[ff-stream_file_mgr]getProgress<<");}
    return fpi;
}

}//end of namespace gv
