#ifndef FF_READER_HANDLER_H_
#define FF_READER_HANDLER_H_

//CXX11
#include <chrono>

#include "ff_reader_internal.h"


namespace gv
{
    // typedef struct ffReaderHandler
    // {
    //     //CXX11
    //     std::time_t     time_start;
        

    // } ffReaderHandler;

    class ffReaderHandler
    {
    public:
        static bool handler_cb();
    public:
        ffReaderHandler();
        virtual ~ffReaderHandler();

        void setupTimeStart();
        int64_t getTimeDiff();

        void logResult(int result);

    private:
        std::time_t     time_start;
    };

};//end of namespace gv
#endif  // FF_READER_HANDLER_H_
