/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2016, Alexey Kramarenko
    All rights reserved.
*/

#include "../board.h"
#include "lpc_iap.h"

#include "../dbg.h"

#ifdef LPC18xx
#define IAP_LOCATION (*(volatile unsigned int *)(0x10400100))
#else //LPC11xx
#define IAP_LOCATION 0x1FFF1FF1
#endif //LPC18xx

typedef void (*IAP)(unsigned int command[], unsigned int result[]);

bool lpc_iap(LPC_IAP_TYPE* params, unsigned int cmd)
{
    IAP iap = (IAP)IAP_LOCATION;
    params->req[0] = cmd;
    iap(params->req, params->resp);
    return (params->resp[0] == IAP_RESULT_SUCCESS);
}
