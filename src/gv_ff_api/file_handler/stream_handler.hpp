#ifndef FF_STREAM_HANDLER_H_
#define FF_STREAM_HANDLER_H_

//CXX11
#include <chrono>

#include "ff_file_handler_internal.h"


namespace gv
{

    //This class would be global object to handle other object
    class ffStreamHandler
    {
    public:
        static bool handler_cb();
    public:
        ffStreamHandler();
        virtual ~ffStreamHandler();

        //void setupTimeStart();
        //int64_t getTimeDiff();

        void logResult(int result);

    private:
        std::time_t     time_start;
    };
    //Note:
    // - has many "stream_file_manager" as model part
    // - 
};//end of namespace gv
#endif  // FF_STREAM_HANDLER_H_
