/*
 * board.c
 *
 *  Created on: 30 ???. 2019 ?.
 *      Author: jam_r
 */


#include "nrf.h"
#include "nrf/nrf_power.h"

void board_init()
{
//    power_init(); // comment for decrease size
}

void board_reset()
{
    NVIC_SystemReset();
}
