/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: RJ (jam_roma@yahoo.com)
*/

#ifndef _UCORE_H_
#define _UCORE_H_

#include "core/core.h"
#include "userspace/types.h"
#include "userspace/cc_macro.h"

typedef struct {
    void (*svc_irq)(unsigned int, unsigned int, unsigned int, unsigned int);
    const void** lib;
    POOL pool;
} GLOBAL;

#define __GLOBAL     ((GLOBAL*)(SRAM_BASE))

//#define MAGIC_UNINITIALIZED                            0xcdcdcdcd
//#define MAGIC_UNINITIALIZED_BYTE                       0xcd

void ucore_startup();

void ucore_stat();

/**
    \brief arch-dependent stack pointer query
    \details Same for every ARM, so defined here
    \retval stack pointer
*/
__STATIC_INLINE void* get_sp()
{
  void* result;
  __ASM volatile ("mov %0, sp" : "=r" (result));
  return result;
}

#endif /* _UCORE_H_ */
