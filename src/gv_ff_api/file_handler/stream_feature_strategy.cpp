
//

#include "stream_feature_strategy.hpp"

//3rd-party
#include <spdlog/spdlog.h>
#include <fmt/core.h>

//CXX11
#include <chrono>
#include <time.h>   //ctime_s , ctime
#include <stdio.h>


#include <iostream>
#include <sstream>      // std::ostringstream
#include <iomanip>      //std::put_time

namespace gv
{

//MARK: BaseClass:FeatureStrategy
FeatureStrategy::FeatureStrategy() noexcept
{
    b_fs_log_ = true;
}

//int FeatureStrategy::write(char* inputData)
//{
//    //Base Class No implementation
//    return -1;
//}
//int FeatureStrategy::read(char* outputData)
//{
//    //Base Class No implementation
//    return -1;
//}

//MARK: Feature:FeatureWrite

//FeatureWrite::FeatureWrite() noexcept
//{
//
//}

int FeatureWrite::write(char* inputData)
{
    //if(b_fs_log_){spdlog::info("[ff-stream_file_mgr]open video ok: {}", video_path);}
    return -1;
}

//MARK: Feature:FeatureReader

int FeatureReader::read(char* outputData)
{
    return -1;
}




}//end of namespace gv
