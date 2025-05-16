
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
    file_status = 0;
    b_sfo_log_ = true;
    filename = foi.fname;
    file_mode = foi.mode;
    file.open(filename, foi.mode);
    if(!file.is_open()){
        if(b_sfo_log_){spdlog::info("[ff-stream_file_obj]open fail: {}", filename);}
        file_status = 2;
    } else {
        if(b_sfo_log_){spdlog::info("[ff-stream_file_obj]open ok: {}", filename);}
        file_status = 1;
    }
}

//b5-Destructor
stream_file_obj::~stream_file_obj()
{
    shutdown();
}
bool stream_file_obj::write_string(std::string inputStr)
{
    if(b_sfo_log_){spdlog::info("[ff-stream_file_obj]write_string>>", inputStr);}
    if(!file) {
        throw std::ios_base::failure("file stream is not in a good state");
    }
    //Note:
    // - For Binary
    //file.write(inputStr.c_str(), inputStr.size());
    file << inputStr;
    //m1-ok
    //file.close();//this will make content write into file.
    //m2-fail
    file.flush();//no-use for content appear in file.
    //shutdown();
    if(b_sfo_log_){spdlog::info("[ff-stream_file_obj]write_string<<");}
    return true;
}
void stream_file_obj::shutdown()
{
    if(b_sfo_log_){spdlog::info("[ff-stream_file_obj]shutdown>>", filename);}
    if (file.is_open()) {
        file.close();
        file_status = 3;
        if(b_sfo_log_){spdlog::info("[ff-stream_file_obj]shutdown-close");}
    }
    if(b_sfo_log_){spdlog::info("[ff-stream_file_obj]shutdown<<");}
}


}//end of namespace gv
