#include <iostream>

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

int
main()
{
    gv::ff_api_init(nullptr);
    
    check_version();

    gv::ff_api_shutdown();
    return 0;
}
