/*
 * flash.c
 *
 *  Created on: 4 марта 2019 г.
 *      Author: RLeonov
 */

#include <string.h>
#include "flash.h"

#if (FLASH_DEBUG)
#include "dbg.h"
#endif // FLASH_DEBUG

/**************************** VARIABLES *****************************/
void flash_init(MEMORY_CTX* ctx)
{
    /* copy code to ram region */
    memcpy(ctx->ram_foo, __FLASH_UPD, FLASH_UPD_SIZE);
}

int flash_read(MEMORY_CTX* ctx, void* dst, unsigned int src_addr, unsigned int size)
{
    /* verify accessable */
    memcpy(dst, (void*)src_addr, size);
    return size;
}

int flash_write(MEMORY_CTX* ctx, const unsigned int dst_addr, void* src, unsigned int size)
{
    return flash_upd_sram(ctx->ram_foo, dst_addr, (unsigned int)src, size, false);
}

const MEMORY_FUNCTIONS_STRUCT __MEMORY_FLASH  = {
        (MEMORY_INIT)flash_init,
        (MEMORY_READ)flash_read,
        (MEMORY_WRITE)flash_write
};
