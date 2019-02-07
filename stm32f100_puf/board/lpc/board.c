/*
 * board.c
 *
 *  Created on: 14 рту. 2017 у.
 *      Author: RLeonov
 */


#include "lpc/lpc_power.h"
#include "lpc/lpc_flash.h"
#include "lpc.h"

void board_init()
{
    //power_init(); // comment for decrease size
    flash_init();
}

void board_reset()
{
    LPC_RGU->RESET_CTRL0 = RGU_RESET_ACTIVE_STATUS0_CORE_RST_Msk;
    __NOP();
//    NVIC_SystemReset();
}
