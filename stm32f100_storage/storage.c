/*
 * storage.c
 *
 *  Created on: 4 марта 2019 г.
 *      Author: RLeonov
 */

#include "storage.h"

#if (STORAGE_DEBUG)
#include "dbg.h"
#endif // STORAGE_DEBUG

bool storage_create(STORAGE* storage,
                    unsigned int offset,
                    unsigned int max_size,
                    unsigned int page_size,
                    unsigned int cluster_size)
{
    STORAGE_ENTRY_PAGE* entry = (STORAGE_ENTRY_PAGE*)storage->stack;
    unsigned int total_clusters = 0;
    int res = 0;

    /* count cluster count */
    total_clusters = (max_size - offset) / cluster_size;

    /* create entry by offset */
    entry->magic = STORAGE_ENTRY_MAGIC;
    entry->max_size = max_size;
    entry->page_size = page_size;
    entry->cluster_size = cluster_size;
    entry->total_clusters = total_clusters;

    /* count CRC32 */
    // TODO

    storage->memory->init(storage->mem_ctx);
    /* write entry to flash */
    res = storage->memory->write(storage->mem_ctx, offset, storage->stack, sizeof(STORAGE_ENTRY_PAGE));

    if(res < 0)
    {
#if (STORAGE_DEBUG_ERRORS)
        printf("STORAGE: write entry failure\n");
#endif // STORAGE_DEBUG_ERRORS
        return false;
    }

#if (STORAGE_DEBUG_REQUEST)
    printf("STORAGE: create\n");
    printf("offset: %#X\n", offset);
    printf("max_size: %#X\n", entry->max_size);
    printf("page_size: %#X\n", entry->page_size);
    printf("cluster_size: %#X\n", entry->cluster_size);
    printf("total_clusters: %#X\n", entry->total_clusters);
#endif // STORAGE_DEBUG_REQUEST
    return true;
}

bool storage_open(STORAGE* storage, unsigned int offset, unsigned int max_size)
{
    unsigned int addr = offset;
    STORAGE_ENTRY_PAGE* entry = NULL;
#if (STORAGE_DEBUG_REQUEST)
    printf("STORAGE: open offset %#X, size %#X\n", offset, max_size);
#endif // STORAGE_DEBUG_REQUEST

    /* try to find entry page */
    for(; addr < max_size - sizeof(uint32_t); addr += sizeof(uint32_t))
    {
        if(STORAGE_ENTRY_MAGIC == (*(uint32_t*)addr))
        {
            /* init memory context */
            storage->memory->init(storage->mem_ctx);
            /* magic already correct */
            entry = (STORAGE_ENTRY_PAGE*)addr;

            /* verify CRC32 */
            // TODO:

            /* fill structure */
            storage->offset = addr;
            storage->max_size = entry->max_size;
            storage->page_size = entry->page_size;
            storage->cluster_size = entry->cluster_size;
            storage->total_clusters = entry->total_clusters;
#if (STORAGE_DEBUG_INFO)
            printf("STORAGE: magic found\n");
            printf("offset: %#X\n", storage->offset);
            printf("max_size: %#X\n", storage->max_size);
            printf("page_size: %#X\n", storage->page_size);
            printf("cluster_size: %#X\n", storage->cluster_size);
            printf("total_clusters: %#X\n", storage->total_clusters);
#endif // STORAGE_DEBUG_INFO
            return true;
        }
    }
#if (STORAGE_DEBUG_ERRORS)
    printf("STORAGE: magic not found\n");
#endif // STORAGE_DEBUG_ERRORS
    return false;

}

void storage_close(STORAGE* storage)
{
#if (STORAGE_DEBUG_REQUEST)
    printf("STORAGE: close\n");
#endif // STORAGE_DEBUG_REQUEST
}

void storage_get_info(STORAGE* storage, STORAGE_INFO* info)
{
#if (STORAGE_DEBUG_REQUEST)
    printf("STORAGE: get info\n");
#endif // STORAGE_DEBUG_REQUEST
}

void storage_file_open(STORAGE* storage, uint32_t file_id)
{
#if (STORAGE_DEBUG_REQUEST)
    printf("STORAGE: file open\n");
#endif // STORAGE_DEBUG_REQUEST
}


void storage_file_close(STORAGE* storage, uint32_t file_id)
{
#if (STORAGE_DEBUG_REQUEST)
    printf("STORAGE: file close\n");
#endif // STORAGE_DEBUG_REQUEST
}

void storage_file_write(STORAGE* storage, uint32_t file_id, uint8_t* in, unsigned int size)
{
#if (STORAGE_DEBUG_REQUEST)
    printf("STORAGE: file write\n");
#endif // STORAGE_DEBUG_REQUEST
}

void storage_file_read(STORAGE* storage, uint32_t file_id, uint8_t* out, unsigned int size)
{
#if (STORAGE_DEBUG_REQUEST)
    printf("STORAGE: file read\n");
#endif // STORAGE_DEBUG_REQUEST
}

void storage_begin(STORAGE* storage)
{
#if (STORAGE_DEBUG_REQUEST)
    printf("STORAGE: begin\n");
#endif // STORAGE_DEBUG_REQUEST
}

void storage_commit(STORAGE* storage)
{
#if (STORAGE_DEBUG_REQUEST)
    printf("STORAGE: commit\n");
#endif // STORAGE_DEBUG_REQUEST
}

void storage_revert(STORAGE* storage)
{
#if (STORAGE_DEBUG_REQUEST)
    printf("STORAGE: revert\n");
#endif // STORAGE_DEBUG_REQUEST
}

