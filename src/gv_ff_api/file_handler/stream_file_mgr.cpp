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

bool stream_file_mgr::handler_cb()
{
    return false;
}

stream_file_mgr::stream_file_mgr()
{
    b_sfm_log_ = true;
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
    input_info.mode = std::ios::out | std::ios::app ; //Creating a File When Opening (for Writing)

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
    if(support_audio)
    {
        std::string audio_path = file_path + ".gap";
        file_obj_info audio_info;
        audio_info.fname = audio_path;
        //Creating a File When Opening (for Writing)
        audio_info.mode = std::ios::out | std::ios::app ;
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

}//end of namespace gv
