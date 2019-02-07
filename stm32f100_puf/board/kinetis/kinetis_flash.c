/*
 * kinetis_flash.c
 *
 *  Created on: 5 сент. 2017 г.
 *      Author: RLeonov
 */

#include <string.h>
#include "kinetis.h"
#include "kinetis_flash.h"
#include "kinetis_resident.h"

#define LOGICAL_SECTOR_SIZE                         512

#define write_sector(ram_func, addr, buf, size)     ((FLASH_FN_TYPE)((unsigned int)ram_func + 1))(addr, buf, size)

// return offset in bytes
unsigned int flash_read(unsigned int addr, unsigned int* dst, unsigned int size_bytes)
{
    memcpy(dst, (void*)(addr), size_bytes);
    return size_bytes;
}


unsigned int flash_write_sector(uint32_t* addr, uint32_t* buf, unsigned int size_words)
{
   uint8_t flash_resident[RESIDENT_SIZE];
   memcpy(flash_resident, __RESIDENT, RESIDENT_SIZE);

   return write_sector(flash_resident, addr, buf, FLASH_SECTOR_SIZE_WORDS);
}


unsigned int flash_get_uid(uint8_t* data)
{
    return flash_read((unsigned int)&SIM->UIDH, (unsigned int*)data, 16);
}
