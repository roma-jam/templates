/*
 * board.c
 *
 *  Created on: 5 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#include "kinetis_power.h"
#include "kinetis.h"

void board_init()
{
//    power_init();
//    flash_init();
}

void board_reset()
{
    NVIC_SystemReset();
}
