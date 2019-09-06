/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: Alexey E. Kramarenko (alexeyk13@yandex.ru)
*/

#include "../../rexos/userspace/stdio.h"
#include "../../rexos/userspace/stdlib.h"
#include "../../rexos/userspace/process.h"
#include "../../rexos/userspace/sys.h"
#include "../../rexos/userspace/gpio.h"
#include "../../rexos/userspace/ipc.h"
#include "../../rexos/userspace/systime.h"
#include "../../rexos/userspace/wdt.h"
#include "../../rexos/userspace/uart.h"
#include "../../rexos/userspace/process.h"
#include "../../rexos/userspace/power.h"
#include "../../rexos/userspace/pin.h"
#include "../../rexos/userspace/flash.h"
#include "../../rexos/userspace/rng.h"
#include "../../rexos/userspace/adc.h"
#include "../../rexos/userspace/storage.h"
#include "../../rexos/midware/pinboard.h"
#include "../../rexos/kernel/dbg.h"
#include "../../rexos/userspace/stm32/stm32_driver.h"
#include "app_private.h"
#include "config.h"
#include <string.h>

void app();

const REX __APP = {
    //name
    "App main",
    //size
    (1 * 1024),
    //priority
    200,
    //flags
    PROCESS_FLAGS_ACTIVE | REX_FLAG_PERSISTENT_NAME,
    //function
    app
};

const uint8_t led[3] = { B0, B7, B14 };

//static inline void stat()
//{
//    SYSTIME uptime;
//    int i;
//    unsigned int diff;

//    get_uptime(&uptime);
//    for (i = 0; i < TEST_ROUNDS; ++i)
//        svc_test();
//    diff = systime_elapsed_us(&uptime);
//    printf("average kernel call time: %d.%dus\n", diff / TEST_ROUNDS, (diff / (TEST_ROUNDS / 10)) % 10);
//
//    get_uptime(&uptime);
//    for (i = 0; i < TEST_ROUNDS; ++i)
//        process_switch_test();
//    diff = systime_elapsed_us(&uptime);
//    printf("average switch time: %d.%dus\n", diff / TEST_ROUNDS, (diff / (TEST_ROUNDS / 10)) % 10);
//
//    printf("core clock: %d\n", power_get_core_clock());

//    sleep_ms(100);
//    process_info();

//}

static inline void app_setup_dbg()
{
    BAUD baudrate;
    pin_enable(DBG_CONSOLE_TX_PIN, STM32_GPIO_MODE_AF, AF7);
    uart_open(DBG_CONSOLE, UART_MODE_STREAM | UART_TX_STREAM);
    baudrate.baud = DBG_CONSOLE_BAUD;
    baudrate.parity = 'N';
    uart_set_baudrate(DBG_CONSOLE, &baudrate);
    uart_setup_printk(DBG_CONSOLE);
    uart_setup_stdout(DBG_CONSOLE);
    open_stdout();
}

static inline void app_init(APP* app)
{
    app_setup_dbg();

    app->timer = timer_create(0, HAL_APP);
    timer_start_ms(app->timer, TIMEOUT_VALUE);
}

static inline void app_timeout(APP* app)
{
    timer_start_ms(app->timer, TIMEOUT_VALUE);

    gpio_reset_pin(led[app->led_num]);
    app->led_num++;
    if(app->led_num >= 3)
        app->led_num = 0;
    gpio_set_pin(led[app->led_num]);
    app->led_on = !app->led_on;
}

void app()
{
    APP app;
    IPC ipc;

    app_init(&app);

    printf("MCU %dMHz\n", power_get_core_clock() / 1000000);

    gpio_enable_pin(led[0], GPIO_MODE_OUT);
    gpio_enable_pin(led[1], GPIO_MODE_OUT);
    gpio_enable_pin(led[2], GPIO_MODE_OUT);

    app.led_on = false;
    app.led_num = 0;

    sleep_ms(200);
    process_info();

    for(;;)
    {
        ipc_read(&ipc);
        switch (HAL_GROUP(ipc.cmd))
        {
        case HAL_APP:
            app_timeout(&app);
          break;
        default:
            error(ERROR_NOT_SUPPORTED);
            break;
        }
        ipc_write(&ipc);
    }
}
