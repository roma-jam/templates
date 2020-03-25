/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: Alexey E. Kramarenko (alexeyk13@yandex.ru)
*/

#ifndef CONFIG_H
#define CONFIG_H

#define TIMEOUT_VALUE                               250

//NRF51 BOARD
//#define LED_PIN                                     P0_7

//#define LED_PIN                                     P0_6
#define LED_PIN                                     P0_8
//#define LED_PIN                                     P0_12
//#define LED_PIN                                     P1_9

#define BUTTON1_PIN                                 P0_9
#define BUTTON2_PIN                                 P0_8
#define BUTTON3_PIN                                 P0_10


//#define BUTTON_FLAGS   (PINBOARD_FLAG_DOWN_EVENT | PINBOARD_FLAG_UP_EVENT |   PINBOARD_FLAG_PULL | PINBOARD_FLAG_INVERTED)
#define BUTTON_FLAGS   (PINBOARD_FLAG_DOWN_EVENT | PINBOARD_FLAG_UP_EVENT | PINBOARD_FLAG_PULL | PINBOARD_FLAG_INVERTED)

#define B_PRESS_MS                                  150
#define B_PRESS_LONG_MS                             1000
#endif // CONFIG_H

// ================================ LCD ========================================
#define LCD_DOT_FULL                                0x07
#define LCD_DOT_EMPTY                               0x09
#define LCD_BITBYTE                                 1
#define LCD_SPI                                     0

#define LCD_UPDATE_MS                               200
#define LCD_WIDTH                                   84
#define LCD_HEIGHT                                  48

#define LCD_STR_HEIGHT                              8
#define LCD_STR_WIDTH                               16

/* PINS */
#define LCD_RESET_PIN                               P0_0
#define LCD_CE_PIN                                  P0_1
#define LCD_DC_PIN                                  P0_2
#define LCD_DIN_PIN                                 P0_3
#define LCD_SCLK_PIN                                P0_4
#define LCD_BCKLT_PIN                               P0_5

//plus space for BER cache in SRAM. Generally 1 block size - 2kb/4kb
#define VFS_PROCESS_SIZE                                    (1500 + 4096)
#define VFS_PROCESS_PRIORITY                                155

//total flash sectors -128KB user code.
#define FLASH_SECTORS_COUNT                                 200
//sectors per block for FAT16, pages per block for BER. BER superblock must fit
// sectors in BER block
#define FS_BLOCK_SECTORS                                    2
//same for 18x7, 18x2
#define FS_BLOCKS_COUNT                                     128


#define APP_DEBUG_LCD                               1
