#include <iostream>

//#include "pg_client.h"
#include "include/gv_ff_api.h"
//#include "include/ff_decoder.h"

void ff_api_init(void *cfg)
{
    spdlog::info("[ff]init>>");
    //fmt::print("Hello, world2\n");
    //ff_decoder_init();
    spdlog::info("[ff]init<<");
}

void ff_api_shutdown()
{
    spdlog::info("[ff]shutdown>><<");
}

void ff_api_version(char* pVersion)
{
    spdlog::info("[ff]version: {}", "0.1.0");
}

void ff_api_command()
{
    spdlog::info("[ff]command");
}
