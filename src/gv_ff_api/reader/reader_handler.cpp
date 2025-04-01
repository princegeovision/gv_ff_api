
//

#include "reader_handler.hpp"

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

bool ffReaderHandler::handler_cb()
{
    return false;
}

ffReaderHandler::ffReaderHandler()
{

}

ffReaderHandler::~ffReaderHandler()
{

}
void ffReaderHandler::setupTimeStart()
{
    auto now = std::chrono::system_clock::now();

    // Convert it to system time (time_t) for localtime
    //std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    time_start = std::chrono::system_clock::to_time_t(now);
    // Convert to tm structure for local time
    std::tm now_tm = *std::localtime(&time_start);

    // Get the current time in microseconds (as a duration)
    auto now_microsec = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());

    // Output the time in the desired format (for example, YYYY-MM-DD HH:MM:SS.micros)
    //std::cout << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S") << "."
    //          << now_microsec.count() % 1000000 << std::endl;
    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
    auto ts_ms = now_microsec.count() % 1000000;
    std::string formated_ts = oss.str();
    spdlog::info("[handler]setupTimeStart: {}.{}", formated_ts.c_str(), ts_ms);
}
int64_t ffReaderHandler::getTimeDiff()
{
    //boost::posix_time::time_duration time_count = boost::posix_time::microsec_clock::local_time() - time_start;
    //int64_t total_time = time_count.total_milliseconds();
    auto now = std::chrono::system_clock::now();
    // Convert it to system time (time_t) for localtime
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    int64_t diff = now_time_t - time_start;
    return diff;
}
void ffReaderHandler::logResult(int result)
{
    if(result==AVERROR_UNKNOWN){
        spdlog::info("AVERROR_UNKNOWN\n");
    } else if(result==AVERROR_BUG2){
        spdlog::info("AVERROR_BUG2\n");
    } else if(result==AVERROR_BUG){
        spdlog::info("AVERROR_BUG\n");
    } else if(result==AVERROR_BSF_NOT_FOUND){
        spdlog::info("AVERROR_BSF_NOT_FOUND\n");
    } else if(result==AVERROR_BUFFER_TOO_SMALL){
        spdlog::info("AVERROR_BUFFER_TOO_SMALL\n");
    } else if(result==AVERROR_DECODER_NOT_FOUND){
        spdlog::info("AVERROR_DECODER_NOT_FOUND\n");
    } else if(result==AVERROR_DEMUXER_NOT_FOUND){
        spdlog::info("AVERROR_DEMUXER_NOT_FOUND\n");
    } else if(result==AVERROR_ENCODER_NOT_FOUND){
        spdlog::info("AVERROR_ENCODER_NOT_FOUND\n");
    } else if(result==AVERROR_EOF){
        spdlog::info("AVERROR_EOF\n");
    } else if(result==AVERROR_EXIT){
        spdlog::info("AVERROR_EXIT\n");
    } else if(result==AVERROR_EXTERNAL){
        spdlog::info("AVERROR_EXTERNAL\n");
    } else if(result==AVERROR_FILTER_NOT_FOUND){
        spdlog::info("AVERROR_FILTER_NOT_FOUND\n");
    } else if(result==AVERROR_MUXER_NOT_FOUND){
        spdlog::info("AVERROR_MUXER_NOT_FOUND\n");
    } else if(result==AVERROR_OPTION_NOT_FOUND){
        spdlog::info("AVERROR_OPTION_NOT_FOUND\n");
    } else if(result==AVERROR_PATCHWELCOME){
        spdlog::info("AVERROR_PATCHWELCOME\n");
    } else if(result==AVERROR_PROTOCOL_NOT_FOUND){
        spdlog::info("AVERROR_PROTOCOL_NOT_FOUND\n");
    } else if(result==AVERROR_STREAM_NOT_FOUND){
        spdlog::info("AVERROR_STREAM_NOT_FOUND\n");
    //
    //} else if(result==AVERROR_BUG){
    }
    
}

}//end of namespace gv
