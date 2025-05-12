#include <iostream>
#include <sstream>      // std::ostringstream
#include <iomanip>      // std::setfill

#include "gv_ff_api.h"

void check_version()
{
    char tVersion[64] = {0};
    gv::ff_api_version(&tVersion[0]);
    char tDecoderVersion[64] = {0};
    gv::ff_decoder_version(&tDecoderVersion[0]);
    char tReaderVersion[64] = {0};
    gv::ff_reader_version(&tReaderVersion[0]);
}

//MARK - : Callback-Reader
//typedef int(*ff_reader_type_callback)(const int cbType, const void* pData, void* user_info);
int reader_callback(const int cbType, const void* pData, void* user_info)
{
    //
    spdlog::info("[ff]reader_callback>> (cbType={}", cbType);
    switch (cbType) {
        case k_ff_reader_callback_type_video_data:
        {
            ffReaderVideoInfo* videoInfo = (ffReaderVideoInfo*)pData;
            spdlog::info("[V]-{} x {}", videoInfo->width, videoInfo->height);
        }
            break;
        case k_ff_reader_callback_type_audio_data:
        {
            ffRtspReaderAudioInfo* audioInfo = (ffRtspReaderAudioInfo*)pData;
            spdlog::info("[A]-{} x {}", audioInfo->raw_data_size, audioInfo->sample_rate);
        }
            break;
        case k_ff_reader_callback_type_event_data:
        {
            ffRtspReaderEventInfo* eventInfo = (ffRtspReaderEventInfo*)pData;
            spdlog::info("[E]EOF-{}", eventInfo->read_eof);
        }
            break;
        case k_ff_reader_callback_type_event_login_result:
        {
            ffRtspReaderLoginResult* loginResult = (ffRtspReaderLoginResult*)pData;
            spdlog::info("[E]connect_result={}", loginResult->connect_result);
        }
            break;
        default:
            break;
    }
    spdlog::info("[ff]reader_callback<<");
}
//Utils-Func
std::string charToHex(unsigned char c) {
    short i = c;

    std::stringstream s;

    s << "%" << std::setw(2) << std::setfill('0') << std::hex << i;

    return s.str();
}
//Utils-Func
std::string urlEncode(const std::string &toEncode) {
    std::ostringstream out;

    for(std::string::size_type i=0; i < toEncode.length(); ++i) {
        short t = toEncode.at(i);

        if(
           t == 45 ||            // hyphen
           (t >= 48 && t <= 57) ||        // 0-9
           (t >= 65 && t <= 90) ||        // A-Z
           t == 95 ||            // underscore
           (t >= 97 && t <= 122) ||    // a-z
           t == 126            // tilde
           ) {
            out << toEncode.at(i);
        } else {
            out << charToHex(toEncode.at(i));
        }
    }

    return out.str();
}

void prepare_reader_info(ffReaderConnectionInfo* pInfo)
{
    //EX: rtsp://admin:Admin123%21@192.168.4.123:554/unicast/c31/s0/live
    std::string device_ip("192.168.4.123");
    strncpy( (char*) pInfo->domain_name , device_ip.c_str(), sizeof(pInfo->domain_name)-1);
    std::string device_user("admin");
    strncpy( (char*) pInfo->username, device_user.c_str(), sizeof(pInfo->username)-1);
    std::string device_pass("Admin123!");
    std::string encode_pass = urlEncode(device_pass);//URL-Encode
    strncpy( (char*) pInfo->password, encode_pass.c_str(), sizeof(pInfo->password)-1);
    std::string device_port("554");
    strncpy( (char*) pInfo->port, device_port.c_str(), sizeof(pInfo->port)-1);
    std::string device_path("unicast/c31/s0/live");
    strncpy( (char*) pInfo->path, device_path.c_str(), sizeof(pInfo->path)-1);
    pInfo->connection_type = k_ff_reader_transport_type_tcp;

    //Setup For Callback
    pInfo->reader_cb = reader_callback;
    //pInfo->reader_cb_user_arg = this;
}

void run_reader()
{
    ffReader* pReader = nullptr;
    bool bStartReading = false;
    ffReaderConnectionInfo info;
    memset(&info, 0, sizeof(info));
    prepare_reader_info(&info);

    pReader = gv::ff_reader_create(&info);


    bool bReading = true;
    while(bReading){
        if((pReader != nullptr)&&(bStartReading == false))
        {
            //command to start READing.
            ff_reader_action_info action_info;
            memset(&action_info, 0, sizeof(action_info));
            action_info.action_type = k_ff_reader_action_type_start;
            int action_result = gv::ff_reader_action(pReader, action_info);
            if(action_result == k_ff_reader_action_result_fail){
                spdlog::info("[ff-reader]action_type_start -- FAIL");
            } else if(action_result == k_ff_reader_action_result_ok){
                spdlog::info("[ff-reader]action_type_start -- OK");
            }
        }
        sleep(1000*100);
        //check reading result
    }
    //After read finished
    gv::ff_reader_release(&pReader);

}


int
main()
{
    gv::ff_api_init(nullptr);
    
    check_version();
    
    run_reader();

    gv::ff_api_shutdown();
    return 0;
}
