/*
 * flash.c
 *
 *  Created on: 4 марта 2019 г.
 *      Author: RLeonov
 */

#include "flash.h"

/**************************** VARIABLES *****************************/
void flash_init()
{

}

void flash_read(const void* src, void* dst, unsigned int size)
{

}

void flash_write(const void* src, void* dst, unsigned int size)
{

}

const MEMORY_FUNCTIONS_STRUCT __MEMORY_FLASH  = {
        (MEMORY_INIT)flash_init,
        (MEMORY_READ)flash_read,
        (MEMORY_WRITE)flash_write
};
