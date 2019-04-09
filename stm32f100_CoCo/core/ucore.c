/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: RJ (jam_roma@yahoo.com)
*/

#include "ucore.h"
#include "ucore_lib.h"
#include "lib/pool.h"

#include "userspace/types.h"
#include "userspace/lib.h"
#include "userspace/stdlib.h"

#include "../dbg.h"

const char *const STAT_LINE="-------------------------------------------------------------------------\n";

//const char *const DAMAGED="     !!!DAMAGED!!!     ";

void ucore_startup()
{
    //setup __GLOBAL
    __GLOBAL->lib = (const void**)&__LIB;
    // setup pool
    pool_init(&__GLOBAL->pool, (void*)(SRAM_BASE + sizeof(GLOBAL)));
}

static unsigned int stack_used(unsigned int top, unsigned int end)
{
//    unsigned int cur;
//    for (cur = top; cur < end && *((unsigned int*)cur) == MAGIC_UNINITIALIZED; cur += 4) {}
//    return end - cur;
    return 0;
}


void ucore_stat()
{
    POOL_STAT stat;
    ((const LIB_STD*)__GLOBAL->lib[LIB_ID_STD])->pool_stat(&__GLOBAL->pool, &stat, get_sp());

    printf("\n    name           stack    size   used       free\n");
    printf(STAT_LINE);
    printf("%-20.20s ", "uCORE");
    printf("%4b  ", stack_used((unsigned int)pool_free_ptr(&__GLOBAL->pool), (unsigned int)sizeof(GLOBAL)));

//    if (err != ERROR_OK)
//        printk(DAMAGED);
//    else
    {
        printf(" %3b(%02d)   ", stat.used, stat.used_slots);
        printf("%3b/%3b(%02d) ", stat.free, stat.largest_free, stat.free_slots);
    }
    printf("\n");
    printf(STAT_LINE);
}
