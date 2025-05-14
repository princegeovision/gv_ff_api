#ifndef FF_STREAM_HANDLER_H_
#define FF_STREAM_HANDLER_H_

//CXX11
#include <chrono>

#include <map>

#include "ff_file_handler_internal.h"
#include "ff_file_handler_type.h"

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
        int32_t insertToMap(ffFileHandler* pfh);
        void logResult(int result);

    private:
        std::map<int32_t, ffFileHandler*> fh_map;
        int32_t         m_base_index;
        std::time_t     time_start;
    };
    //Note:
    // - has many "stream_file_manager" as model part
    // - 
};//end of namespace gv
#endif  // FF_STREAM_HANDLER_H_
