/*
 * led.h
 *
 *  Created on: 14 рту. 2018 у.
 *      Author: RLeonov
 */

#ifndef LED_H_
#define LED_H_

#include "app.h"
#include "rexos/userspace/stdlib.h"
#include "rexos/userspace/types.h"
#include "rexos/userspace/ipc.h"

typedef struct {
    HANDLE timer;
    bool is_on;
} LED;

void led_init(APP* app);
void led_on(APP* app);
void led_off(APP* app);
void led_toggle(APP* app);

void led_request(APP* app,  IPC* ipc);


#endif /* LED_H_ */
