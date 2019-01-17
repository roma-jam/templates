/*
 * led.c
 *
 *  Created on: 14 рту. 2018 у.
 *      Author: RLeonov
 */

#include "app_private.h"
#include "rexos/userspace/stm32/stm32_driver.h"
#include "rexos/userspace/gpio.h"
#include "rexos/userspace/stdio.h"
#include "config.h"

void led_init(APP* app)
{
    gpio_enable_pin(LED_GREEN_PIN, GPIO_MODE_OUT);

    /* disable led */
    led_off(app);

    /* create timer */
    app->led.timer = timer_create(0, HAL_LED);
    if(INVALID_HANDLE == app->led.timer)
    {
#if (APP_DEBUG_ERRORS)
        printf("LED: timer create failure\n");
#endif // APP_DEBUG_ERRORS
    }
    else
        timer_start_ms(app->led.timer, LED_BLINK_TIMEOUT_MS);
}

void led_on(APP* app)
{
    gpio_set_pin(LED_GREEN_PIN);
    app->led.is_on = true;
}

void led_off(APP* app)
{
    gpio_reset_pin(LED_GREEN_PIN);
    app->led.is_on = false;
}

void led_toggle(APP* app)
{
    if(app->led.is_on)
        led_off(app);
    else
        led_on(app);
}

void led_request(APP* app,  IPC* ipc)
{
    switch (HAL_ITEM(ipc->cmd))
    {
        case IPC_TIMEOUT:
            printf("LED: blink\n");
            led_toggle(app);
            timer_start_ms(app->led.timer, LED_BLINK_TIMEOUT_MS);
            break;

        default:
            /* unhandled ipc */
            break;
    }
}
