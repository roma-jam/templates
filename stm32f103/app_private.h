/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2016, Alexey Kramarenko
    All rights reserved.
*/

#ifndef APP_PRIVATE_H
#define APP_PRIVATE_H

#include "app.h"
#include "rexos/userspace/types.h"
#include "rexos/userspace/ipc.h"
#include "led.h"

typedef enum {
    HAL_LED = HAL_APP,
} HAL_APP_GROUPS;

typedef enum {
    APP_TIMER_LED = 0x00,
} APP_TIMER;

typedef struct _APP {
    LED led;
} APP;

#endif // APP_PRIVATE_H
