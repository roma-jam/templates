/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2017, Alexey Kramarenko
    All rights reserved.
*/

#include "stdlib.h"
#include "ucore.h"

void* malloc(size_t size)
{
    return ((const LIB_STD*)__GLOBAL->lib[LIB_ID_STD])->pool_malloc(&__GLOBAL->pool, size, get_sp());
}

void* realloc(void* ptr, size_t size)
{
    return ((const LIB_STD*)__GLOBAL->lib[LIB_ID_STD])->pool_realloc(&__GLOBAL->pool, ptr, size, get_sp());
}

void free(void* ptr)
{
    ((const LIB_STD*)__GLOBAL->lib[LIB_ID_STD])->pool_free(&__GLOBAL->pool, ptr);
}

unsigned long atou(const char *const buf, int size)
{
    return ((const LIB_STD*)__GLOBAL->lib[LIB_ID_STD])->atou(buf, size);
}

int utoa(char* buf, unsigned long value, int radix, bool uppercase)
{
    return ((const LIB_STD*)__GLOBAL->lib[LIB_ID_STD])->utoa(buf, value, radix, uppercase);
}

unsigned int srand()
{
    // TODO:
    return 0;
}

unsigned int rand(unsigned int* seed)
{
    // TODO:
    return 0;
}

const STD_MEM __STD_MEM = {
    malloc,
    realloc,
    free
};
