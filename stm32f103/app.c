/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2016, Alexey Kramarenko
    All rights reserved.
*/

#include "rexos/userspace/stdio.h"
#include "rexos/userspace/stdlib.h"
#include "rexos/userspace/process.h"
#include "rexos/userspace/sys.h"
#include "rexos/userspace/stm32/stm32_driver.h"
#include "rexos/userspace/ipc.h"
#include "rexos/userspace/uart.h"
#include "rexos/userspace/process.h"
#include "rexos/userspace/pin.h"
#include "rexos/userspace/gpio.h"
#include "app_private.h"
#include "checksum.h"
#include "config.h"

void app();

const REX __APP = {
    //name
    "App main",
    //size
    2048,
    //priority
    200,
    //flags
    PROCESS_FLAGS_ACTIVE | REX_FLAG_PERSISTENT_NAME,
    //function
    app
};


static inline void app_setup_dbg()
{
    BAUD baudrate;
    pin_enable(DBG_CONSOLE_TX_PIN, STM32_GPIO_MODE_OUTPUT_AF_PUSH_PULL_50MHZ, false);
//    pin_enable(DBG_CONSOLE_TX_PIN, STM32_GPIO_MODE_AF, AF4);
    uart_open(DBG_CONSOLE, UART_MODE_STREAM | UART_TX_STREAM);
    baudrate.baud = DBG_CONSOLE_BAUD;
    baudrate.data_bits = 8;
    baudrate.parity = 'N';
    baudrate.stop_bits= 1;
    uart_set_baudrate(DBG_CONSOLE, &baudrate);
    uart_setup_printk(DBG_CONSOLE);
    uart_setup_stdout(DBG_CONSOLE);
    open_stdout();
}

static inline void app_init(APP* app)
{
#if (APP_DEBUG)
    app_setup_dbg();
    printf("STM32 F103 Temlplate, CPU %d MHz\n", power_get_core_clock()/1000000);
    printf("%s\n", __BUILD_TIME);
#endif
}

void app()
{
    APP app;
    IPC ipc;

    app_init(&app);
//    checksum_init(&app);

    /* enable LED PIN */
//    led_init(&app);

    SYSTIME time;

    get_uptime(&time);

    while(1)
    {
        sleep_ms(333);
        printk("WUP, %d\n", systime_elapsed_ms(&time));
    }

    for (;;)
    {
        ipc_read(&ipc);
        switch (HAL_GROUP(ipc.cmd))
        {
            case HAL_LED:
                led_request(&app, &ipc);
                break;
            default:
#if (APP_DEBUG_ERRORS)
                printf("APP: unhandled IPC group %X, ITEM: %X\n", HAL_GROUP(ipc.cmd), HAL_ITEM(ipc.cmd));
#endif // APP_DEBUG_ERRORS
                error(ERROR_NOT_SUPPORTED);
                break;
        }
        ipc_write(&ipc);
    }
}
