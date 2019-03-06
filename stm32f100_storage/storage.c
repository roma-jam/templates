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

void storage_init(STORAGE* storage)
{
#if (STORAGE_DEBUG_REQUEST)
    printf("STORAGE: init\n");
#endif // STORAGE_DEBUG_REQUEST
    /* create entry by offset */

}

void storage_open(STORAGE* storage)
{
#if (STORAGE_DEBUG_REQUEST)
    printf("STORAGE: open\n");
#endif // STORAGE_DEBUG_REQUEST
    /* try to find entry */

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
