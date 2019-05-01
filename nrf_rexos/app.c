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
#include "../../rexos/midware/pinboard.h"
#include "../../rexos/userspace/nrf/nrf_driver.h"
#include "../../rexos/userspace/nrf/radio.h"
#include "app_private.h"
#include "button.h"
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

    sleep_ms(100);
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
    uart_setup_stdout(DBG_CONSOLE);
    open_stdout();
}

static inline void app_init(APP* app)
{
    gpio_enable_pin(P28, GPIO_MODE_OUT);
    gpio_reset_pin(P28);

    app_setup_dbg();

    app->timer = timer_create(0, HAL_APP);
    timer_start_ms(app->timer, 300);

//    stat();
    printf("App init\n");

    printf("RAMON %X\n", NRF_POWER->RAMON);
    printf("RAMSTATUS %X\n", NRF_POWER->RAMSTATUS);

#if (NRF_DECODE_RESET)
    uint32_t rr = get_exo(HAL_REQ(HAL_POWER, NRF_POWER_GET_RESET_REASON), 0, 0, 0);
    printf("Reset Reason (%d): ", rr);
    switch(rr)
    {
        case POWER_RESETREAS_DIF_Msk:
            printf("DIF\n");
            break;
        case POWER_RESETREAS_LPCOMP_Msk:
            printf("LPCOMP\n");
            break;
        case POWER_RESETREAS_OFF_Msk:
            printf("WAKEUP GPIO\n");
            break;
        case POWER_RESETREAS_LOCKUP_Msk:
            printf("LOCKUP\n");
            break;
        case POWER_RESETREAS_SREQ_Msk:
            printf("AIRCR.SYSRESETREQ\n");
            break;
        case POWER_RESETREAS_DOG_Msk:
            printf("WDOG\n");
            break;
        case POWER_RESETREAS_RESETPIN_Msk:
            printf("RESET PIN\n");
            break;
        default:
            printf("NOT DETECTED\n");
    }
#endif // NRF_DECODE_RESET
}

static inline void app_timeout(APP* app)
{
    /* go sleep */
//    power_set_mode(POWER_MODE_STOP);

    printf("timeout\n");

    timer_start_ms(app->timer, 300);

    /* send advertise packet */
//    radio_send_adv(0, NULL, 0);

    /* toggle led */
#if (1)
    if(app->led_on)
        gpio_reset_pin(LED_PIN);
    else
        gpio_set_pin(LED_PIN);

    app->led_on = !app->led_on;
#endif //

}

void app()
{
    APP app;
    IPC ipc;

    app_init(&app);

    pin_enable(LED_PIN, PIN_MODE_OUTPUT, PIN_PULL_NOPULL);
    gpio_reset_pin(LED_PIN);
    app.led_on = false;

//    button_init(&app);

//    app.ble = ble_open();
//    radio_listen_adv_channel(100, 0, 500);



    for(;;)
    {
        ipc_read(&ipc);
        switch (HAL_GROUP(ipc.cmd))
        {
        case HAL_APP:
            app_timeout(&app);
          break;
        case HAL_PINBOARD:
            button_request(&app, &ipc);
            break;
        default:
            error(ERROR_NOT_SUPPORTED);
            break;
        }
        ipc_write(&ipc);
    }
}
