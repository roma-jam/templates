/*
 * nrf_uart.c
 *
 *  Created on: 30 ???. 2019 ?.
 *      Author: jam_r
 */

#include "nrf.h"
#include "nrf_pin.h"
#include "nrf_power.h"
#include "nrf_uart.h"

static inline void board_dbg_set_baudrate(UART_PORT port, unsigned int baud)
{

}

void board_dbg_init()
{


    board_dbg_set_baudrate(UART, UART_BAUD);
}

void board_dbg(const char *const buf, unsigned int size)
{

}
