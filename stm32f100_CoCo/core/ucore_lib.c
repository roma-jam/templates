/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: RJ (jam_roma@yahoo.com)
*/

#include "lib/lib_std.h"

void lib_stub () { return; }

const void *const __LIB[] = {
    (const void *const)&__LIB_STD,
};
