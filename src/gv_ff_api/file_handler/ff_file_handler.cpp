#include <iostream>
#include <iomanip> //std::put_time
#include <sstream> //std::stringstream

#include "ff_file_handler.h"

#include "ff_file_handler_internal.h"

#include "stream_handler.hpp"

namespace gv
{
    bool fh_logger_ = true;//25 items
    // bool g_ffmpeg_network_prepare = false;
    // const unsigned int k_timeout_secs = 30;
    
    ffStreamHandler g_sh_handler = ffStreamHandler();



    //void ff_reader_version(char* pVersion)
    void ff_file_handler_version(char* pVersion)
    {
        spdlog::info("[ff-fh]version: {}", "0.2.0");
        unsigned codecVer = avcodec_version();//"4.4.3#3"
        int ver_major,ver_minor,ver_micro;
        ver_major = (codecVer>>16)&0xff;    //58
        ver_minor = (codecVer>>8)&0xff;     //134
        ver_micro = (codecVer)&0xff;        //100
        spdlog::info("[ff-fh]version: {}.{}.{}-{}", \
            ver_major, ver_minor, ver_micro, \
            avcodec_license());
    }

    //Private-FUNC-01
    //REF: setup_reader_object
    ffFileHandler* setup_file_object(ffFileHandlerInfo* inputInfo)
    {
        //01-
        spdlog::info("[ff-fh]setup_file_object>>");
        //ffFileHandler
        //display inputInfo
        spdlog::info("[ff-fh]inputInfo: Type:{}", inputInfo->fh_create_type);
        spdlog::info("[ff-fh]inputInfo: file_path:{}", inputInfo->file_path);
        spdlog::info("[ff-fh]inputInfo: file_name:{}", inputInfo->file_name);
        //Check File name is not empty
        std::string l_file_path = inputInfo->file_path;
        std::string l_file_name = inputInfo->file_name;
        if(l_file_name.length() > 0){
            //Has Name
            ffFileHandler* handler = (ffFileHandler*)malloc(sizeof(ffFileHandler));
            if (!handler)
            {
                return nullptr;
            }
            memset(handler, 0, sizeof(ffFileHandler));

            handler->internal = (ffInternalfh*)malloc(sizeof(ffInternalfh));
            if (!handler->internal)
            {
                free(handler);
                return nullptr;
            }
            gv::stream_file_mgr* stream_mgr = new gv::stream_file_mgr();
            bool ok = stream_mgr->init(std::string(l_file_name));
            if (!ok)
            {
                delete stream_mgr;
                free(handler->internal);
                free(handler);

                return nullptr;
            }
            //Before we return to handler, we need to create stream_id for it
            //file_stream_id
            handler->internal->p_fmgr = stream_mgr;
            int32_t fs_id = g_sh_handler.insertToMap(handler);
            handler->file_stream_id = fs_id;
            
            return handler;
        }
        //產生 ffInternalfh by file_path
        spdlog::info("[ff-fh]setup_file_object<<");
        return nullptr;
    }

    //PUBLIC-API-01
    ffFileHandler* ff_file_handler_create(ffFileHandlerInfo* inputInfo)
    {
        //REF: ffReader* ff_reader_create(ffReaderConnectionInfo* inputInfo)
        if(!inputInfo){
            spdlog::info("[ff-fh]ff_file_handler_create: NULL input");
        }

        return setup_file_object(inputInfo);
    }
    //PUBLIC-API-02
    void ff_file_handler_release(ffFileHandler** fHandler)
    {
        //REF: void ff_reader_release(ffReader** reader)
        if (*fHandler)
        {
            ffInternalfh* interal = (*fHandler)->internal;

            delete interal;
            free(*fHandler);
            *fHandler = nullptr;
            //FF_RELEASE_MSG
            if(fh_logger_){spdlog::info("[ff-fh] release success!!\n");}
        }
    }

    //PUBLIC-API-03
     int ff_file_handler_action(ffFileHandler* reader, ff_file_handler_action_info action_info)
     {
         return -1;
     }
}
