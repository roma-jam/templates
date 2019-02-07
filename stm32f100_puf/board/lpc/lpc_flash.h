/*
 * lpc_flash.h
 *
 *  Created on: 29 но€б. 2017 г.
 *      Author: RLeonov
 */

#ifndef LPC_FLASH_H_
#define LPC_FLASH_H_

#include <stdint.h>

#define FLASH_SECTOR_SIZE                           2048 // THIS IS EMULATE VALUE
#define FLASH_SECTOR_SIZE_WORDS                     ((FLASH_SECTOR_SIZE) / sizeof(uint32_t))

#define FLASH_PAGE_SIZE                             0x200
#define FLASH_SMALL_SECTOR_SIZE                     (8 * 1024)
#define FLASH_BIG_SECTOR_SIZE                       (64 * 1024)

void flash_init();
unsigned int flash_read(unsigned int addr, unsigned int* dst, unsigned int size_words);
unsigned int flash_write_sector(uint32_t* addr, uint32_t* buf, unsigned int size_words);

unsigned int flash_get_uid(uint8_t* data);

#endif /* LPC_FLASH_H_ */
