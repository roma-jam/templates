/*
 * lpc_flash.c
 *
 *  Created on: 29 но€б. 2017 г.
 *      Author: RLeonov
 */

#include <string.h>
#include "lpc.h"
#include "lpc_power.h"
#include "lpc_flash.h"
#include "lpc_iap.h"

#include "../dbg.h"

#define UID_SZ                                  16
#define FLASH_PAGE_SIZE                         0x200
#define FLASH_SMALL_SECTOR_SIZE                 (8 * 1024)
#define FLASH_BIG_SECTOR_SIZE                   (64 * 1024)
#define FLASH_SMALL_SECTORS_COUNT               8
#define FLASH_SMALL_SECTORS_SIZE                (FLASH_SMALL_SECTORS_COUNT * FLASH_SMALL_SECTOR_SIZE)

#define LOGICAL_SECTOR_SIZE                     512


typedef struct {
    unsigned int first_page, last_page;
    unsigned int sector;
    unsigned int bank;
    unsigned int size;
} FLASH_ADDR_BLOCK_TYPE;

static bool lpc_flash_decode_addr_block(unsigned int addr, FLASH_ADDR_BLOCK_TYPE* addr_block, unsigned int size)
{
    if ((addr % FLASH_PAGE_SIZE) || (size % FLASH_PAGE_SIZE))
        return false;

    addr_block->first_page = addr;
    //bank A
    if (addr_block->first_page + FLASH_PAGE_SIZE <= FLASH_SIZE)
        addr_block->bank = 0;
    //bank B
    else
        return false;

    //first 8 sectors are 8KB
    if (addr_block->first_page <= FLASH_SMALL_SECTORS_SIZE)
    {
        addr_block->sector = addr_block->first_page / FLASH_SMALL_SECTOR_SIZE;
        addr_block->size = FLASH_SMALL_SECTOR_SIZE - (addr_block->first_page % FLASH_SMALL_SECTOR_SIZE);
    }
    else
    {
        addr_block->sector = FLASH_SMALL_SECTORS_COUNT + (addr_block->first_page - FLASH_SMALL_SECTORS_SIZE) / FLASH_BIG_SECTOR_SIZE;
        addr_block->size = FLASH_BIG_SECTOR_SIZE - ((addr_block->first_page - FLASH_SMALL_SECTORS_SIZE) % FLASH_BIG_SECTOR_SIZE);
    }
    if (addr_block->size > size)
        addr_block->size = size;
    addr_block->first_page += FLASH_BASE_BANK_ADDR;
    addr_block->last_page = addr_block->first_page + addr_block->size - FLASH_PAGE_SIZE;
    return true;
}

static inline bool lpc_flash_is_block_empty(unsigned int* addr, unsigned int words)
{
    unsigned int i;
    for (i = 0; i < words; ++i)
        if (addr[i] != 0xffffffff)
            return false;
    return true;
}

static bool lpc_flash_prepare_sector(FLASH_ADDR_BLOCK_TYPE* addr_block, LPC_IAP_TYPE* iap)
{
    iap->req[1] = iap->req[2] = addr_block->sector;
    iap->req[3] = addr_block->bank;
    return lpc_iap(iap, IAP_CMD_PREPARE_SECTORS_FOR_WRITE);
}

static bool lpc_flash_erase_block(FLASH_ADDR_BLOCK_TYPE* addr_block, unsigned int clock_khz)
{
    LPC_IAP_TYPE iap;

    if (lpc_flash_is_block_empty((unsigned int*)addr_block->first_page, addr_block->size / sizeof(unsigned int)))
        return true;

    if (!lpc_flash_prepare_sector(addr_block, &iap))
        return false;

    iap.req[1] = addr_block->first_page;
    iap.req[2] = addr_block->last_page;
    iap.req[3] = clock_khz;

    return lpc_iap(&iap, IAP_CMD_ERASE_PAGE);
}

static bool lpc_flash_write_block(FLASH_ADDR_BLOCK_TYPE* addr_block, void* buf, unsigned int clock_khz)
{
    LPC_IAP_TYPE iap;
    unsigned int i, chunk_size;

    for (i = 0; i < addr_block->size; i += chunk_size)
    {
        chunk_size = addr_block->size - i;
        //only 4096, 1024 and 512 are valid IAP values
        if (chunk_size >= 4096)
            chunk_size = 4096;
        else if (chunk_size >= 1024)
            chunk_size = 1024;
        if (!lpc_flash_prepare_sector(addr_block, &iap))
            return false;
        iap.req[1] = addr_block->first_page + i;
        iap.req[2] = (unsigned int)((uint8_t*)buf + i);
        iap.req[3] = chunk_size;
        iap.req[4] = clock_khz;
        if (!lpc_iap(&iap, IAP_CMD_COPY_RAM_TO_FLASH))
            return false;
    }
    return true;
}

void flash_init()
{
    LPC_IAP_TYPE iap;
    lpc_iap(&iap, IAP_CMD_INIT);
}

// return offset in bytes
unsigned int flash_read(unsigned int addr, unsigned int* dst, unsigned int size_words)
{
    memcpy(dst, (void*)(addr), size_words);
    return size_words;
}


unsigned int flash_write_sector(uint32_t* addr, uint32_t* buf, unsigned int size_words)
{
    FLASH_ADDR_BLOCK_TYPE addr_block;
    unsigned int clock_hz = power_get_core_clock() / 1000;

    if (size_words != FLASH_SECTOR_SIZE_WORDS)
        return 0;

    if(!lpc_flash_decode_addr_block((unsigned int)addr, &addr_block , FLASH_SECTOR_SIZE))
        return 0;

    if(!lpc_flash_erase_block(&addr_block, clock_hz))
        return 0;

    if(!lpc_flash_write_block(&addr_block, buf, clock_hz))
        return 0;

    return size_words;
}


unsigned int flash_get_uid(uint8_t* data)
{
    LPC_IAP_TYPE iap;

    if(!lpc_iap(&iap, IAP_CMD_READ_PART_ID))
        return 0;

    memcpy(data, (uint8_t*)&iap.resp[1], UID_SZ);
    return UID_SZ;
}
