//
//  stream_file_mgr.cpp
//  stream_file_mgr
//
//  Created by ArronPJ on 05/08/25.
//  Copyright (c) 2025 GTDigital. All rights reserved.
//
#ifndef FF_STREAM_FILE_MGR_H_
#define FF_STREAM_FILE_MGR_H_

//CXX11
#include <chrono>
#include <memory>  // For std::unique_ptr

//#include "ff_file_handler_internal.h"
#include "ff_file_handler_type.h"
#include "stream_file_obj.hpp"
#include "stream_feature_strategy.hpp"

//Related
#include "ff_reader_type.h"

namespace gv
{
    class stream_file_status
    {
    public:
        bool _waitFirstFrame;//default=true
        bool _waitFirstAudio;//default=true
    public:
        stream_file_status();
    };
    class file_progress
    {
    public:
        bool has_progress_info;
        double progress;
    public:
        // Default constructor
        file_progress()
            : has_progress_info(false), progress(0.0f) {
        }
        // Destructor
        ~file_progress() {
            // No dynamic memory to free, but defined for completeness
            spdlog::info("Destructor called");
        }
        // Copy constructor
        file_progress(const file_progress& other)
            : has_progress_info(other.has_progress_info), progress(other.progress) {
            spdlog::info("Copy constructor called");
        }
        // Copy assignment operator
        file_progress& operator=(const file_progress& other) {
            spdlog::info("Copy assignment operator called");
            if (this != &other) {
                has_progress_info = other.has_progress_info;
                progress = other.progress;
            }
            return *this;
        }
        // Move constructor
        file_progress(file_progress&& other) noexcept
            : has_progress_info(other.has_progress_info), progress(other.progress) {
            spdlog::info("Move constructor called");
            // Optionally reset source
            other.has_progress_info = false;
            other.progress = 0.0;
        }
        // Move assignment operator
        file_progress& operator=(file_progress&& other) noexcept {
            spdlog::info("Move assignment operator called");
            if (this != &other) {
                has_progress_info = other.has_progress_info;
                progress = other.progress;

                // Optionally reset source
                other.has_progress_info = false;
                other.progress = 0.0;
            }
            return *this;
        }
    };
    //This class
    class stream_file_mgr
    {
    public:
        static bool handler_cb();
        static std::string get_encode_text(uint8_t* inputData, \
                                           int32_t inputSize, \
                                           int32_t codec);
    public:
        stream_file_mgr();
        virtual ~stream_file_mgr();

        bool init(std::string file_path, bool support_audio);
        void shutdown();

        bool write_info(const ff_file_handler_action_info ainfo);
        void logResult(int result);
    private:
        file_progress getProgress(ffReaderVideoInfo* vi);
    private:
        std::string         m_file_path_;

        stream_file_obj*    m_video_sfo_;
        stream_file_obj*    m_video_header_sfo_;
        
        stream_file_obj*    m_audio_sfo_;
        //std::unique_ptr<FeatureWrite> strategy;
        std::unique_ptr<gv::FeatureWrite> strategy;
        stream_file_status  m_status_;
        bool                b_sfm_log_;
        std::time_t         time_start;
    };

};//end of namespace gv
#endif  // FF_STREAM_FILE_MGR_H_
