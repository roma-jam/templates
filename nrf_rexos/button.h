/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: RL (jam_roma@yahoo.com)
*/

#ifndef BUTTON_H_
#define BUTTON_H_

#include "app.h"
#include "../../rexos/userspace/ipc.h"
#include "../../rexos/userspace/types.h"
#include "config.h"

typedef struct {
    HANDLE pinboard;
    HANDLE timer_double, timer_long, encoder_timer;
    bool pressed;
} BUTTON;

void button_init(APP* app);
void button_request(APP* app, IPC* ipc);

#endif /* BUTTON_H_ */
