/*
 * board.c
 *
 *  Created on: 5 ���� 2017 �.
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
