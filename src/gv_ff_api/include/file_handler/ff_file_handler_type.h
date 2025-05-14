#ifndef FF_FILE_HANDLER_TYPE_H_
#define FF_FILE_HANDLER_TYPE_H_

#include <inttypes.h>

struct ffInternalfh;
//struct ffInternalReader;

typedef enum ff_file_handler_create_type {
    k_ff_file_handler_unknown_type = 0,
    k_ff_file_handler_create_new_file = 1,
    k_ff_file_handler_open_exist_file = 2,
    //k_ff_file_handler_err_fail_create = 2
} FHCreateType;


typedef enum ff_file_handler_error_code {
    k_ff_file_handler_err_ok = 0,
    k_ff_file_handler_err_fail_open = -1,
    k_ff_file_handler_err_fail_create = -2
} FHERRCode;

//Support Create and Open as we handle file.
typedef enum ff_file_handler_open_status_code {
    k_ff_file_handler_create_s_ok = 0,
    k_ff_file_handler_create_s_fail_exist = -1,
    k_ff_file_handler_create_s_fail_no_p = -2
} FHStatusCode;

enum ff_file_handler_action_type
{
    k_ff_file_handler_action_type_unknown             = 0,
    k_ff_file_handler_action_type_write               = 100,
    k_ff_file_handler_action_type_read                = 101,
};

// << Callback DEF >>
typedef int(*ff_file_handler_type_callback)(const int cbType, const void* pData, void* user_info);

typedef struct ffFileHandlerInfo
{
    int32_t fh_create_type;     /*!<  ref: ff_file_handler_create_type */
    
    char    file_name[512];     /*!<  ref: xxx.xxx.xxx.xxx ipv4*/
    char    file_path[1024];    /*!<  ref: xxx.sdp (file path)*/
    //int64_t file_stream_id;     //pre-define, if library can respect this?
    
    //Callback Related
    ff_file_handler_type_callback   fh_cb;
    void*                           fh_cb_user_arg;
} ffFileHandlerInfo;

// typedef struct ff_reader_action_info
// {
//     enum ff_reader_action_type  action_type;
//     bool                        b_flag;
//     int32_t                     n_value;
//     const char*                 s_detail;
// } ff_reader_action_info;
typedef struct ff_file_handler_action_info
{
    enum ff_file_handler_action_type    action_type;
    bool                                b_flag;
    int32_t                             n_value;
    const char*                         s_detail;
} ff_file_handler_action_info;

//Reader
typedef struct ffFileHandler
{
    struct ffInternalfh* internal;
    int32_t file_stream_id;
    ff_file_handler_type_callback type_callback;
    bool check_audio_exsit_flag;
    void* user_arg;
} ffFileHandler;

//File Open Info
typedef struct ffFileOpenInfo
{
    char    file_path[1024];

} ffFileOpenInfo;

#endif  // FF_READER_TYPE_H_
