
//

#include "stream_handler.hpp"

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

bool ffStreamHandler::handler_cb()
{
    return false;
}

ffStreamHandler::ffStreamHandler()
{

}

ffStreamHandler::~ffStreamHandler()
{

}
// void ffStreamHandler::setupTimeStart()
// {
//     auto now = std::chrono::system_clock::now();

//     time_start = std::chrono::system_clock::to_time_t(now);

//     std::tm now_tm = *std::localtime(&time_start);

//     auto now_microsec = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());

//     std::ostringstream oss;
//     oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
//     auto ts_ms = now_microsec.count() % 1000000;
//     std::string formated_ts = oss.str();
//     spdlog::info("[handler]setupTimeStart: {}.{}", formated_ts.c_str(), ts_ms);
// }
// int64_t ffStreamHandler::getTimeDiff()
// {
//     //boost::posix_time::time_duration time_count = boost::posix_time::microsec_clock::local_time() - time_start;
//     //int64_t total_time = time_count.total_milliseconds();
//     auto now = std::chrono::system_clock::now();
//     // Convert it to system time (time_t) for localtime
//     std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
//     int64_t diff = now_time_t - time_start;
//     return diff;
// }
void ffStreamHandler::logResult(int result)
{
    
}

}//end of namespace gv
