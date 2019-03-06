/*
 * flash.h
 *
 *  Created on: 4 марта 2019 г.
 *      Author: RLeonov
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "storage.h"
#include "flash_update.h"

#if (SYS_DEBUG)
#define FLASH_DEBUG                     1
#endif // SYS_DEBUG

typedef struct {
    uint8_t ram_foo[FLASH_UPD_SIZE];
} MEMORY_CTX;

extern const MEMORY_FUNCTIONS_STRUCT __MEMORY_FLASH;


#endif /* FLASH_H_ */
