/*
 * storage.h
 *
 *  Created on: 4 марта 2019 г.
 *      Author: RLeonov
 */

#ifndef STORAGE_H_
#define STORAGE_H_

#include <stdint.h>
#include "config.h"

#if (SYS_DEBUG)
#define STORAGE_DEBUG                           1
#endif // SYS_DEBUG


#if (STORAGE_DEBUG)
#define STORAGE_DEBUG_REQUEST                   1
#endif // STORAGE_DEBUG


/* PRIMITIVES */
typedef void (*MEMORY_INIT)();
typedef void (*MEMORY_READ)(const void*, void*, unsigned int);
typedef void (*MEMORY_WRITE)(const void*, void*, unsigned int);

typedef struct {
    MEMORY_INIT     init;
    MEMORY_READ     read;
    MEMORY_WRITE    write;
} MEMORY_FUNCTIONS_STRUCT;

/* PROTOTYPES */
typedef struct {
    const MEMORY_FUNCTIONS_STRUCT* memory;
    unsigned int offset;
    unsigned int cluster_size;

} STORAGE;

typedef struct {
    unsigned int total_clusters;
    unsigned int free_clusters;
} STORAGE_INFO;

void storage_init(STORAGE* storage);
void storage_open(STORAGE* storage);
void storage_close(STORAGE* storage);
void storage_get_info(STORAGE* storage, STORAGE_INFO* info);

void storage_file_open(STORAGE* storage, uint32_t file_id);
void storage_file_close(STORAGE* storage, uint32_t file_id);

void storage_file_write(STORAGE* storage, uint32_t file_id, uint8_t* in, unsigned int size);
void storage_file_read(STORAGE* storage, uint32_t file_id, uint8_t* out, unsigned int size);

void storage_begin(STORAGE* storage);
void storage_commit(STORAGE* storage);
void storage_revert(STORAGE* storage);



#endif /* STORAGE_H_ */
