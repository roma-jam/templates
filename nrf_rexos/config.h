/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: Alexey E. Kramarenko (alexeyk13@yandex.ru)
*/

#ifndef CONFIG_H
#define CONFIG_H

#define TIMEOUT_VALUE                               1000

#define LED_PIN                                     P28

#define BUTTON_PIN                                  P9


//#define BUTTON_FLAGS   (PINBOARD_FLAG_DOWN_EVENT | PINBOARD_FLAG_UP_EVENT |   PINBOARD_FLAG_PULL | PINBOARD_FLAG_INVERTED)
#define BUTTON_FLAGS   (PINBOARD_FLAG_DOWN_EVENT | PINBOARD_FLAG_UP_EVENT)

#define B_PRESS_MS                                  150
#define B_PRESS_LONG_MS                             1000
#endif // CONFIG_H


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
