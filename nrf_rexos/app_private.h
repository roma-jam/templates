/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: Alexey E. Kramarenko (alexeyk13@yandex.ru)
*/

#ifndef APP_PRIVATE_H
#define APP_PRIVATE_H

#include <stdint.h>
#include "app.h"
#include "comm.h"
#include "net.h"
#include "button.h"
#include "fs.h"

typedef enum {
    APP_TIMER_BUTTON_DOUBLE_PRESS = 0x00,
    APP_TIMER_BUTTON_LONG_PRESS,
} APP_TIMER;

typedef struct _APP {
    bool led_on;
    HANDLE timer;
    HANDLE ble;
    HANDLE lcd;
    BUTTON button;
    FS_TYPE fs;
} APP;

#endif // APP_PRIVATE_H
