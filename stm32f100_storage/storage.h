/*
 * storage.h
 *
 *  Created on: 4 марта 2019 г.
 *      Author: RLeonov
 */

#ifndef STORAGE_H_
#define STORAGE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "config.h"

#if (SYS_DEBUG)
#define STORAGE_DEBUG                           1
#endif // SYS_DEBUG


#if (STORAGE_DEBUG)
#define STORAGE_DEBUG_INFO                      1
#define STORAGE_DEBUG_REQUEST                   1
#define STORAGE_DEBUG_ERRORS                    1
#endif // STORAGE_DEBUG

#define STORAGE_ENTRY_MAGIC                     0xA324EACB

/* PRIMITIVES */
typedef void (*MEMORY_INIT)(void* ctx);
typedef int (*MEMORY_READ)(void* ctx, void*, unsigned int, unsigned int);
typedef int (*MEMORY_WRITE)(void* ctx, unsigned int, void*, unsigned int);

typedef struct {
    MEMORY_INIT     init;
    MEMORY_READ     read;
    MEMORY_WRITE    write;
} MEMORY_FUNCTIONS_STRUCT;

/* PROTOTYPES */
typedef struct {
    /* create stack due to absent primitives function and ao not exceed ram during working */
    uint8_t stack[2048];
    void* mem_ctx;
    const MEMORY_FUNCTIONS_STRUCT* memory;
    unsigned int offset;
    unsigned int max_size;
    unsigned int page_size;
    unsigned int cluster_size;
    unsigned int total_clusters;
} STORAGE;

#pragma pack(push, 1)
typedef struct {
    uint32_t magic;
    uint32_t max_size;
    uint32_t page_size;
    uint32_t cluster_size;
    uint32_t total_clusters;
    uint32_t crc32;
} STORAGE_ENTRY_PAGE;
#pragma pack(pop)

typedef struct {
    unsigned int total_clusters;
    unsigned int free_clusters;
} STORAGE_INFO;

bool storage_create(STORAGE* storage,
                    unsigned int offset,
                    unsigned int max_size,
                    unsigned int page_size,
                    unsigned int cluster_size);
bool storage_open(STORAGE* storage, unsigned int offset, unsigned int max_size);
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
