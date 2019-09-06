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

typedef enum {
    APP_TIMER_BUTTON_DOUBLE_PRESS = 0x00,
    APP_TIMER_BUTTON_LONG_PRESS,
} APP_TIMER;

typedef struct _APP {
    uint8_t led_num;
    bool led_on;
    HANDLE timer;
} APP;

#endif // APP_PRIVATE_H
