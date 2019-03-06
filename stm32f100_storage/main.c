/*
 * main.c
 *
 *  Created on: 5 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#include <string.h>
#include "board.h"
#include "system.h"
#include "flash.h"

#if (SYS_DEBUG)
#include "dbg.h"
#endif // SYS_DEBUG

#include "flash_update.h"

/* divercify key */
void main()
{
    board_init();

#if (SYS_DEBUG)
    board_dbg_init();
    printf("Template STORAGE, v%d.%d, %s\n", VERSION >> 8, VERSION & 0xff, __BUILD_TIME);
    printf("%s core, %d MHz\n", __MCU, power_get_core_clock() / 1000000);
#endif // SYS_DEBUG

    STORAGE storage;
    MEMORY_CTX mem_ctx;

    /* memory init, read, write functions */
    storage.memory = &__MEMORY_FLASH;
    /* memory context */
    storage.mem_ctx = &mem_ctx;

    storage_open(&storage, (unsigned int)&__ETEXT, FLASH_BASE + FLASH_SIZE);

    // for creation
    //storage_create(&storage, FLASH_BASE + (64 * 1024), FLASH_BASE + FLASH_SIZE, FLASH_PAGE_SIZE, 128);

    /* FLASH WRITE FUNCTION TEST */
#if 0
    uint8_t temp1[32] =
    {
        0x5B, 0x28, 0xDD, 0xFF, 0xE5, 0x57, 0x1B, 0x16,
        0x86, 0x19, 0x9A, 0xE3, 0x95, 0xA9, 0x5E, 0xBB,
        0xF0, 0xAE, 0x9F, 0x30, 0xE2, 0x61, 0x6E, 0x6A,
        0x73, 0x11, 0x37, 0x1D, 0x1C, 0xDB, 0x4B, 0x88
    };

    uint8_t temp2[32] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00,
    };

    uint8_t verify[32] = {0};

    printf("flash program %#X ...", storage.offset);
    storage.memory->init(storage.mem_ctx);
    storage.memory->write(storage.mem_ctx, storage.offset, temp1, 32);
    printf("complete.\n", storage.offset);

    storage.memory->read(storage.mem_ctx, verify, storage.offset, 32);

    printf("read: ");
    for(int i = 0; i < 32; i++)
        printf("%02X ", verify[i]);
    printf("\n");

    printf((0 == memcmp(verify, temp1, 32))? "write OK.\n" : "write FAILURE.\n");
#endif // FLASH WRITE FUNCTION TEST

    while(1);
}
