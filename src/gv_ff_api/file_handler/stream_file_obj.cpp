
//

#include "stream_file_obj.hpp"

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

bool stream_file_obj::handler_cb()
{
    return false;
}

//b5-Constructor
stream_file_obj::stream_file_obj(const file_obj_info& foi)
{
    b_sfo_log_ = true;
    filename = foi.fname;
    file.open(filename, foi.mode);
    if(!file.is_open()){
        if(b_sfo_log_){spdlog::info("[ff-stream_file_obj]open fail: {}", filename);}
    } else {
        if(b_sfo_log_){spdlog::info("[ff-stream_file_obj]open ok: {}", filename);}
    }
}

//b5-Destructor
stream_file_obj::~stream_file_obj()
{
    if (file.is_open()) {
        file.close();
        if(b_sfo_log_){spdlog::info("[ff-stream_file_obj]<Destructor>closed: {}", filename);}
    }
}
void stream_file_obj::shutdown()
{
    if(b_sfo_log_){spdlog::info("[ff-stream_file_obj]shutdown>><<", filename);}
}


}//end of namespace gv
