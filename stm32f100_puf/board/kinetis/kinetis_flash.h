/*
 * kinetis_flash.h
 *
 *  Created on: 5 сент. 2017 г.
 *      Author: RLeonov
 */

#ifndef KINETIS_FLASH_H
#define KINETIS_FLASH_H

#include <stdint.h>

#define FLASH_SECTOR_SIZE                           2048
#define FLASH_SECTOR_SIZE_WORDS                     ((FLASH_SECTOR_SIZE) / sizeof(uint32_t))

typedef int (*RRWORD)(unsigned int);
#define RRWORD_SIZE                   12
const uint8_t __RRWORD[RRWORD_SIZE];

unsigned int flash_read(unsigned int addr, unsigned int* dst, unsigned int size_words);
unsigned int flash_write_sector(uint32_t* addr, uint32_t* buf, unsigned int size_words);

unsigned int flash_get_uid(uint8_t* data);

#endif /* KINETIS_FLASH_H */
