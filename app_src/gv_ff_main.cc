#include <iostream>

#include "gv_ff_api.h"

int
main()
{
    ff_api_init(nullptr);
    char tVersion[64] = {0};
    ff_api_version(&tVersion[0]);
    ff_api_shutdown();
    return 0;
}
