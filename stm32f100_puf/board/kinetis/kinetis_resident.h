/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2017, Alexey Kramarenko
    All rights reserved.
*/

#ifndef KINETIS_RESIDENT_H
#define KINETIS_RESIDENT_H

#include <stdint.h>

#define RESIDENT_SIZE                                         264

extern const uint8_t __RESIDENT[RESIDENT_SIZE];

typedef int (*FLASH_FN_TYPE)(uint32_t*, uint32_t*, unsigned int);

#endif // KINETIS_RESIDENT_H
