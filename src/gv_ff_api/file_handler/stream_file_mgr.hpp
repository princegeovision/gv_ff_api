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

//#include "ff_file_handler_internal.h"
#include "stream_file_obj.hpp"


namespace gv
{
    //This class 
    class stream_file_mgr
    {
    public:
        static bool handler_cb();
    public:
        stream_file_mgr();
        virtual ~stream_file_mgr();

        bool init(std::string file_path);
        void logResult(int result);

    private:
        stream_file_obj* m_video_sfo_;
        bool            b_sfm_log_;
        std::time_t     time_start;
    };

};//end of namespace gv
#endif  // FF_STREAM_FILE_MGR_H_
