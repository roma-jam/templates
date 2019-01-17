/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2016, Alexey Kramarenko
    All rights reserved.
*/

#ifndef CONFIG_H
#define CONFIG_H

// =============================== LEDS ========================================
#define LED_GREEN_PIN                               C13

#define LED_BLINK_TIMEOUT_MS                        500

// =============================== USB =========================================
#define USB_PORT_NUM                                USB_0
#define USBD_PROCESS_SIZE                           1200
#define USBD_PROCESS_PRIORITY                       150

// ============================== CCID  ========================================
#define USB_CCID_INTERFACE                          0
#define USB_HID_INTERFACE                           0

// ================================= DEBUG =====================================

#define DBG_CONSOLE                                 UART_1
#define DBG_CONSOLE_TX_PIN                          A9
#define DBG_CONSOLE_BAUD                            115200

#define TEST_ROUNDS                                 10000

#define APP_DEBUG                                   1
#define APP_DEBUG_ERRORS                            1

#define APP_DEBUG_LCD                               0
#define APP_DEBUG_BUTTON                            1
#define APP_DEBUG_BATTERY                           0

#endif // CONFIG_H
