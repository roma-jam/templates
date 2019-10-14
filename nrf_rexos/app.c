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
#include "../../rexos/userspace/nrf/nrf_driver.h"
#include "../../rexos/userspace/nrf/nrf_ble.h"
#include "../../rexos/kernel/dbg.h"
#include "app_private.h"
#include "button.h"
#include "config.h"
#include "fs.h"
#include "lcd.h"
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
    app_setup_dbg();

    app->timer = timer_create(0, HAL_APP);
    timer_start_ms(app->timer, TIMEOUT_VALUE);

#if (1)
    stat();
#endif //

#if (0)
    printf("App init\n");
    printf("Flash size: %d KB\n", flash_get_size_bytes() >> 10);
    printf("Flash page size: %d B\n", flash_get_page_size_bytes());
    printf("HWID: %X\n", NRF_FICR->CONFIGID);
#endif

#if (NRF_DECODE_RESET)
    uint32_t rr = get_exo(HAL_REQ(HAL_POWER, NRF_POWER_GET_RESET_REASON), 0, 0, 0);
    printf("Reset Reason (%#X)\n", rr);
    if(rr & POWER_RESETREAS_DIF_Msk)
        printf(" DIF\n");
    if(rr & POWER_RESETREAS_LPCOMP_Msk)
        printf(" LPCOMP\n");
    if(rr & POWER_RESETREAS_OFF_Msk)
        printf(" WAKEUP GPIO\n");
    if(rr & POWER_RESETREAS_LOCKUP_Msk)
        printf(" LOCKUP\n");
    if(rr & POWER_RESETREAS_SREQ_Msk)
        printf(" AIRCR.SYSRESETREQ\n");
    if(rr & POWER_RESETREAS_DOG_Msk)
        printf(" WDOG\n");
    if(rr & POWER_RESETREAS_RESETPIN_Msk)
        printf(" RESET PIN\n");
#endif // NRF_DECODE_RESET

    sleep_ms(100);
}

const uint8_t __ADV[] = {
        0x40, // TYPE
        0x16, // LEN
        0x31, // MAC
        0x32,
        0x33,
        0x34,
        0x35,
        0x36,
        0x0E, // LEN
        BLE_ADV_TYPE_NAME,
        'R',
        'o',
        'm',
        'a',
        'J',
        'a',
        'm',
        ' ',
        'n',
        'R',
        'F',
        '5',
        '2',
        0x00
};

const uint8_t __TEST_PKT[] = {
        0xAA, // TYPE
        0x55, // LEN
        0x31, // MAC
        0x32,
        0x33,
        0x34,
        0x35,
        0x36,
        0x37, // LEN
        0x38, // TYPE: DEVICE NAME
};

static inline void app_timeout(APP* app)
{
    /* go sleep */
//    power_set_mode(POWER_MODE_STOP);

    timer_start_ms(app->timer, TIMEOUT_VALUE);

    /* send advertise packet */
    // RADIO
#if (1)
    int res = 0;
    /* create IO */
    IO* io = io_create(sizeof(RADIO_STACK) + 256); // 254 is max pkt

    // RX
#if (0)
    /* begin rx */
    for(uint8_t n = 0; n < 1; n++)
    {
        res = radio_rx_sync(HAL_RF, KERNEL_HANDLE, 0, io, 3000);
        if(res > 0)
        {
#if (1)
            ble_debug_adv_common(io);
#else
            printf("DATA: ");
            for(int i = 0; i < io->data_size; i++)
                printf("%02X ", ((uint8_t*)(io_data(io)))[i]);
            printf("\n");
#endif //
        }
    }
#else

    io_data_append(io, __ADV, sizeof(__ADV));
//    io_data_append(io, __TEST_PKT, sizeof(__TEST_PKT));

    if(radio_tx_sync(HAL_RF, KERNEL_HANDLE, 0, io, 7000))
    {
        printf("tx (%d) ok\n", io->data_size);
    }
    else
        printf("tx failure, %d\n", get_last_error());
#endif //

    io_destroy(io);
//    radio_send_adv(0, NULL, 0);
#endif // RADIO

    // WATCHDOG
#if (0)
    wdt_kick();
#endif // WDT

    // TEMPERATURE SENSOR
#if (0)
    printf("Temp: %d\n", get_exo(HAL_REQ(HAL_TEMP, IPC_READ), 0, 0, 0));
#endif //

    // ADC TEST
#if (0)
    adc_open();

//    printf("adc: %d, error %d\n", adc_get(NRF_ADC_INPUT_P0_06, 0), get_last_error());
//    printf("adc: %d mV\n", ADC2mV(adc_get(NRF_ADC_INPUT_P0_06, 0), 3600, 10));
    lcd_printf(app, 1, 0, "battery: %d mV\n", ADC2mV(adc_get(NRF_ADC_INPUT_P0_06, 0) << 1, 3600, 10));

    adc_close();
#endif //


    // LED
#if (1)
    if(app->led_on)
        gpio_reset_pin(LED_PIN);
    else
        gpio_set_pin(LED_PIN);

    app->led_on = !app->led_on;
#endif //

}

void print_buf(const char* s, uint8_t* buf, unsigned int size)
{
    printk("%s\n", s);
    unsigned int i = 0;
    for (i = 0; i < size; ++i)
    {
        printk("%02X ", buf[i]);
        if ((i % 0x10) == 0xf)
            printk("\n");
    }
    if (size % 0x10)
        printk("\n");
}


void app()
{
    APP app;
    IPC ipc;

    app_init(&app);

    pin_enable(LED_PIN, PIN_MODE_OUTPUT, PIN_PULL_NOPULL);
    gpio_set_pin(LED_PIN);
    app.led_on = true;

    // BUTTON
#if (0)
    button_init(&app);
#endif // BUTTON

    // STORAGE TEST
#if (0)
    fs_init(&app);
    fs_deinit(&app);
#endif //

    // LCD
#if (0)
    lcd_init(&app);
#endif // LCD

#if (1)
//    app.ble = ble_open();
    app.radio = radio_open("BLE", RADIO_MODE_BLE_1Mbit);
    /* set channel */
    radio_set_channel(BLE_ADV_CHANNEL_39);
    /* listen adv */
#if (0)
    /* create IO */
    IO* io = io_create(sizeof(RADIO_STACK) + 256); // 254 is max pkt
    /* begin rx */
    unsigned int res = 0;
    for(uint8_t n = 0; n < 1; n++)
    {
        res = radio_rx_sync(HAL_RF, KERNEL_HANDLE, 0, io, 500);
        printf("rx: %d\n", res);
        if(res > 0)
        {
            ble_debug_adv_common(io);
        }
    }

    io_destroy(io);
#endif //
#endif // BLE

    // RADIO
#if (0)
    app.radio = radio_open("RADIO stack", RADIO_MODE_RF_250Kbit);
#endif // RADIO

    // FLASH TEST
#if (0)
//    uint8_t buf_test[1024] = { 0 };
    unsigned int page_size = flash_get_page_size_bytes();
    process_info();
    sleep_ms(200);
    storage_open(HAL_FLASH, KERNEL_HANDLE, process_get_current());


//    for(int i = 0; i < 1024; i++)
//        buf_test[i] = i;

    IO* io = io_create(page_size);

    flash_page_read(NRF_FLASH_USER_CODE_SIZE, io, page_size);

    printf("error: %d\n", get_last_error());

    print_buf("page", io_data(io), io->data_size);

//    memcpy(io_data(io), buf_test, 1024);

//    flash_page_write(NRF_FLASH_USER_CODE_SIZE, io);

//    flash_page_read(NRF_FLASH_USER_CODE_SIZE, io, page_size);

//    print_buf("page 2", io_data(io), io->data_size);

    io_destroy(io);

    storage_close(HAL_FLASH, KERNEL_HANDLE, process_get_current());
    sleep_ms(200);
    process_info();
#endif // FLASH

    // GPIO WAKEUP TEST
#if (0)

    NRF_GPIOTE->CONFIG[0] = (BUTTON_PIN << GPIOTE_CONFIG_PSEL_Pos) & GPIOTE_CONFIG_PSEL_Msk;
    NRF_GPIOTE->CONFIG[0] = GPIOTE_CONFIG_MODE_Event;
    NRF_GPIOTE->CONFIG[0] = GPIOTE_CONFIG_POLARITY_LoToHi;
    NRF_GPIO->PIN_CNF[BUTTON_PIN] |= PIN_SENSE_LOW;

#endif //

    // WDT TEST
#if (0)
    NRF_WDT->POWER = 1;
    NRF_WDT->TASKS_START = 0;
    NRF_WDT->CRV = (32768*10);
    NRF_WDT->CONFIG =
        WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos |
        WDT_CONFIG_SLEEP_Pause << WDT_CONFIG_SLEEP_Pos;
    NRF_WDT->RREN |= WDT_RREN_RR0_Msk;
    NRF_WDT->TASKS_START = 1;

    printf("%X\n", NRF_WDT->REQSTATUS);
    printf("%X\n", NRF_WDT->RUNSTATUS);

#endif //

    // RNG TEST
#if (0)
    SYSTIME time;
    IO* io = io_create(32);
    unsigned int ms = 0;
    get_uptime(&time);
    rng_get(io, 32);
    ms = systime_elapsed_ms(&time);

    printf("RNG: ");
    for(int i = 0; i < 32; i++)
        printf("%02X ", ((uint8_t*)(io_data(io)))[i]);
    printf(", (%d ms)\n", ms);
    io_destroy(io);
#endif // RNG_TEST

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
//        case HAL_PINBOARD:
//            button_request(&app, &ipc);
//            break;
        default:
            error(ERROR_NOT_SUPPORTED);
            break;
        }
        ipc_write(&ipc);
    }
}
