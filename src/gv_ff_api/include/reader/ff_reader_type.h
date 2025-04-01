#ifndef FF_READER_TYPE_H_
#define FF_READER_TYPE_H_

#include <inttypes.h>

//struct gvInternalRtspReader;
struct ffInternalReader;
//Files: ff_decoder_type.h
enum ff_reader_audio_flag
{
    k_ff_reader_audio_not_exist = 0,
    k_ff_reader_audio_exist = 1,
};
//Better name L4 Type
//Transport = OSI Level4
enum ff_reader_transport_type
{
    k_ff_reader_transport_type_tcp      = 1,
    k_ff_reader_transport_type_udp      = 2,
    k_ff_reader_transport_type_unknown  = 3
};

enum ff_reader_callback_type
{
    k_ff_reader_callback_type_event_login_result = 1,
    k_ff_reader_callback_type_event_data = 2,
    k_ff_reader_callback_type_video_data = 3,
    k_ff_reader_callback_type_audio_data = 4
};

enum ff_reader_frame_status
{
    k_ff_reader_reader_yet_ready = 0,
    k_ff_reader_success_readed = 1,
    k_ff_reader_pause_read = 2,
    k_ff_reader_read_eof = 3
};
//RTSP
enum ff_reader_stream_reslut{

    READ_PACKET_ZERO_W_H = -3,  /*!< Read packet without video width and height */
    READ_PACKET_NOT_YET =-2,    /*!< Init value before read next packet. */
    READ_PACKET_EOF     =-1,    /*!< end of packet. */
    READ_PACKET_MISS    =0,     /*!< lost packet. */
    READ_PACKET_SUCCESS =1,     /*!< read next packet success. */
};

//Use in gvRtspReaderLoginResult
enum ff_reader_connect_result {
    k_ff_reader_cr_success = 0,
    k_ff_reader_cr_fail_url_unauthorized = -1,
    k_ff_reader_cr_fail_url = -2,
    k_ff_reader_cr_by_pass = -3
};

typedef enum ff_reader_rtsp_error_code {
    k_ff_reader_rtsp_err_ok = 0,
    k_ff_reader_rtsp_err_fail = -1,
    k_ff_reader_rtsp_err_internal = -2
} RTSPERRCode;

//This should be class for RAII Reason.
typedef struct ffReaderVideoInfo
{
    int32_t     codec_type;         ///< gvAvFileReaderVideoType
    uint8_t*    raw_data;           /*!< video packet data. encoded data.*/
    int32_t     raw_data_size;      /*!< video packet data size.*/
    uint8_t*    extra_data;
    int32_t     extra_data_size;
    int32_t     width;              /*!< picture width*/
    int32_t     height;             /*!< picture height*/
    bool        key_frame;          /*!< true mean key frame.*/
    double      timestamp;
    double      total_duration;
    
    //
    int     video_time_year;        /*!< year*/
    int     video_time_month;       /*!< month*/
    int     video_time_day;         /*!< day*/
    int     video_time_hour;        /*!< hour*/
    int     video_time_minute;      /*!< minute*/
    int     video_time_second;      /*!< second*/
    int     serial_number;
} ffReaderVideoInfo;

//This should be class for RAII Reason.
typedef struct ffRtspReaderAudioInfo
{
    int32_t     codec_type;         /*!< ffmpeg ref: AVCodecID.\n geolib ref: audio_compress_codec .*/
    uint8_t*    raw_data;           /*!< audio packet data. encoded data.*/
    int32_t     raw_data_size;      /*!< audio packet data size.*/
    int32_t     channels;           /*!< audio channel.*/
    int32_t     bit_per_sample;     /*!< audio bit per sample. \n ex:8K,16K*/
    int32_t     format;             /*!< audio format use enum audio_format_enum. \n ex:AF_8K_8b*/
    int32_t     sample_rate;        /*!< audio Sample rate.\n ex:8000*/
    bool        key_frame;
    double      timestamp;
    double      total_duration;
} ffRtspReaderAudioInfo;

typedef struct ffRtspReaderEventInfo
{
    int read_eof;  // true mean read end, release this connection.
} ffRtspReaderEventInfo;

//When Callback Type
// - k_ff_reader_callback_type_event_login_result
typedef struct ffRtspReaderLoginResult
{
    int connect_result;     // ref: gvRtspConnectResult
    int video_streaming_count;
    int audio_streaming_count;
} ffRtspReaderLoginResult;

//typedef void(*gv_rtsp_reader_video_callback)(const gvRtspReaderVideoInfo* video,void* user_info);
//typedef void(*gv_rtsp_reader_audio_callback)(const gvRtspReaderAudioInfo* audio,void* user_info);
//typedef void(*gv_rtsp_reader_event_callback)(const gvRtspReaderEventInfo* event,void* user_info);
//typedef void(*gv_rtsp_reader_loginresult_callback)(const gvRtspReaderLoginResult* result,void* user_info);
typedef int(*ff_reader_type_callback)(const int cbType, const void* pData, void* user_info);

typedef struct ffReaderConnectionInfo
{
    int     connection_type; /*!<  ref: ff_reader_transport_type */
    char    domain_name[512];     /*!<  ref: xxx.xxx.xxx.xxx ipv4*/
    char    username[512];        /*!<  user name*/
    char    password[512];        /*!<  password*/
    char    port[16];            /*!<  ip port*/
    char    path[1024];            /*!<  ref: xxx.sdp (file path)*/

    //Callback Related
    ff_reader_type_callback reader_cb;
    void*                   reader_cb_user_arg;
} ffReaderConnectionInfo;



//Reader
typedef struct ffReader
{
    struct ffInternalReader* internal;
    ff_reader_type_callback type_callback;
    bool check_audio_exsit_flag;
    void* user_arg;
} ffReader;

#endif  // FF_READER_TYPE_H_
