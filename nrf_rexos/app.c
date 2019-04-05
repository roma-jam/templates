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
#include "../../rexos/midware/pinboard.h"
#include "../../rexos/userspace/nrf/nrf_driver.h"
#include "app_private.h"
#include "config.h"

void app();

const REX __APP = {
    //name
    "App main",
    //size
    1024,
    //priority
    200,
    //flags
    PROCESS_FLAGS_ACTIVE | REX_FLAG_PERSISTENT_NAME,
    //function
    app
};


static inline void stat()
{
    SYSTIME uptime;
    int i;
    unsigned int diff;

    get_uptime(&uptime);
    for (i = 0; i < TEST_ROUNDS; ++i)
        svc_test();
    diff = systime_elapsed_us(&uptime);
    printf("average kernel call time: %d.%dus\n", diff / TEST_ROUNDS, (diff / (TEST_ROUNDS / 10)) % 10);

    get_uptime(&uptime);
    for (i = 0; i < TEST_ROUNDS; ++i)
        process_switch_test();
    diff = systime_elapsed_us(&uptime);
    printf("average switch time: %d.%dus\n", diff / TEST_ROUNDS, (diff / (TEST_ROUNDS / 10)) % 10);

    printf("core clock: %d\n", power_get_core_clock());
    process_info();
}

static inline void app_setup_dbg()
{
    BAUD baudrate;
    pin_enable(DBG_CONSOLE_TX_PIN, PIN_MODE_OUTPUT, PIN_PULL_NOPULL);
    uart_open(DBG_CONSOLE, UART_MODE_STREAM | UART_TX_STREAM);
    baudrate.baud = DBG_CONSOLE_BAUD;
    baudrate.parity = 'N';
    uart_set_baudrate(DBG_CONSOLE, &baudrate);
    uart_setup_printk(DBG_CONSOLE);
//    uart_setup_stdout(DBG_CONSOLE);
//    open_stdout();
}

static inline void app_init(APP* app)
{
//    gpio_enable_pin(B14, GPIO_MODE_OUT);
//    gpio_reset_pin(B14);


    app_setup_dbg();
//    app->timer = timer_create(0, HAL_APP);
//    timer_start_ms(app->timer, 1000);

//    stat();
//    printf("App init\n");
}

static inline void app_timeout(APP* app)
{
    printf("app timer timeout test\n");
    timer_start_ms(app->timer, 1000);
}

void app()
{
    APP app;
//    IPC ipc;

    app_init(&app);

    SYSTIME time;

    get_uptime(&time);

    for (;;)
    {
        sleep_ms(1000);
//        exodriver_delay_us(1000000);
//        process_info();
        printk("WUP, %d\n", systime_elapsed_ms(&time));
    }

//    for(;;)
//    {
//        ipc_read(&ipc);
//        switch (HAL_GROUP(ipc.cmd))
//        {
//        case HAL_USBD:
//            comm_request(&app, &ipc);
//            break;
//        case HAL_IP:
//        case HAL_UDP:
//        case HAL_TCP:
//            net_request(&app, &ipc);
//            break;
//        case HAL_APP:
//            app_timeout(&app);
//            break;
//        default:
//            error(ERROR_NOT_SUPPORTED);
//            break;
//        }
//        ipc_write(&ipc);
//    }
}
