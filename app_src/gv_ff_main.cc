#include <iostream>

#include "gv_ff_api.h"

int
main()
{
    gv::ff_api_init(nullptr);
    char tVersion[64] = {0};
    gv::ff_api_version(&tVersion[0]);
    gv::ff_api_shutdown();
    return 0;
}
