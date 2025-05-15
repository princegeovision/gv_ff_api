#ifndef FF_STREAM_FILE_OBJ_H_
#define FF_STREAM_FILE_OBJ_H_

//3rd-party
#include <spdlog/spdlog.h>
#include <fmt/core.h>

//CXX11
#include <chrono>

//for "std::ios::openmode"
#include <fstream>

#include <string>
//#include "ff_file_handler_internal.h"


namespace gv
{
    class file_obj_info
    {
    public:
        std::string fname;
        std::ios::openmode mode;
    };
    //This class
    class stream_file_obj
    {
    private:
        std::fstream file;
        std::string filename;
        bool b_sfo_log_;
    public:
        static bool handler_cb();
    public:
        //b5-Constructor
        stream_file_obj(const file_obj_info& foi);
        //b5-Destructor
        virtual ~stream_file_obj();
        //b5-Copy Constructor
        stream_file_obj(const stream_file_obj& other) : filename(other.filename)
        {
            file.open(filename, std::ios::in | std::ios::out);
            if(b_sfo_log_){spdlog::info("[ff-stream_file_obj]<Copy Constructor>Coped file: {}", filename);}
        }
        //b5-Copy Assignment Operator
        stream_file_obj& operator=(const stream_file_obj& other) 
        {
            if (this != &other) {
                if (file.is_open()) file.close();
                filename = other.filename;
                file.open(filename, std::ios::in | std::ios::out);
                if(b_sfo_log_){spdlog::info("[ff-stream_file_obj]<Copy Assignment>Coped file: {}", filename);}
            }
            return *this;
        }
        //b5-Move Constructor
        stream_file_obj(stream_file_obj&& other) noexcept
        {
            file = std::move(other.file);
            filename = std::move(other.filename);
            if(b_sfo_log_){spdlog::info("[ff-stream_file_obj]<Move Constructor>Moved file: {}", filename);}
        }
        //b5-Move Assignment Operator
        stream_file_obj& operator=(stream_file_obj&& other) noexcept
        {
            if (this != &other) {
                if (file.is_open()) file.close();
                file = std::move(other.file);
                filename = std::move(other.filename);
                if(b_sfo_log_){spdlog::info("[ff-stream_file_obj]<Move Assignment>Moved file: {}", filename);}
            }
            return *this;
        }
        bool opened()
        {
            if (file.is_open()){
                return true;
            } else {
                return false;
            }
        }
        void shutdown();
        //void logResult(int result);

    private:
        std::time_t     time_start;
    };
};//end of namespace gv
#endif  // FF_STREAM_FILE_OBJ_H_
