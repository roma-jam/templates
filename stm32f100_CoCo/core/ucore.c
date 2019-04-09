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


/**
 * @brief   ROM image of the data segment start.
 * @pre     The symbol must be aligned to a 32 bits boundary.
 */
extern uint32_t _textdata;

/**
 * @brief   Data segment start.
 * @pre     The symbol must be aligned to a 32 bits boundary.
 */
extern uint32_t _data;

/**
 * @brief   Data segment end.
 * @pre     The symbol must be aligned to a 32 bits boundary.
 */
extern uint32_t _edata;

/**
 * @brief   BSS segment start.
 * @pre     The symbol must be aligned to a 32 bits boundary.
 */
extern uint32_t _bss_start;

/**
 * @brief   BSS segment end.
 * @pre     The symbol must be aligned to a 32 bits boundary.
 */
extern uint32_t _bss_end;


const char *const STAT_LINE="-------------------------------------------------------------------------\n";

//const char *const DAMAGED="     !!!DAMAGED!!!     ";

/*
 * Area fill code, it is a macro because here functions cannot be called
 * until stacks are initialized.
 */
#define fill32(start, end, filler) {                                        \
  uint32_t *p1 = start;                                                     \
  uint32_t *p2 = end;                                                       \
  while (p1 < p2)                                                           \
    *p1++ = filler;                                                         \
}

void ucore_startup()
{
#if (1)
    /* BSS segment initialization.*/
    fill32(&_bss_start, &_bss_end, 0);
#endif

#if (1)
    /* DATA segment initialization.*/
    {
      uint32_t *tp, *dp;

      tp = &_textdata;
      dp = &_data;
      while (dp < &_edata)
        *dp++ = *tp++;
    }
#endif

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
