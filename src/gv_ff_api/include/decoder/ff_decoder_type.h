#ifndef FF_DECODER_TYPE_H_
#define FF_DECODER_TYPE_H_

#include <inttypes.h>

//Files: ff_decoder_type.h
enum ff_decoder_type
{
    k_ff_decoder_type_h264 = 1,
    k_ff_decoder_type_h265,
};

enum ff_decoder_error_code
{
    //>0 = buffer_size
    //0 = size is zero
    k_ff_decode_error_unknown = -1,
    k_ff_decode_error_check_decoder = -2,
    k_ff_decode_error_send_packet = -3,
    k_ff_decode_error_no_picture = -4,
    k_ff_decode_error_transfer_data = -5,
    k_ff_decode_error_b4_transfer_alloc = -6,
};
enum ff_decoder_output_type
{
    k_ff_decoder_output_type_yuv420p,          //(video default)
    k_ff_decoder_output_type_mediacodec_yuv,   //(Android HW)
    k_ff_decoder_output_type_videotool_nv12,   //(Apple HW)
};
//ff_decoder_setup
//ff_decoder_input

#endif  // FF_DECODER_TYPE_H_
