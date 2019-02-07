/*
 * kinetis_uart.c
 *
 *  Created on: 5 θώνÿ 2017 γ.
 *      Author: RLeonov
 */


#include <stdbool.h>
#include "kinetis_uart.h"
#include "kinetis.h"
#include "kinetis_pin.h"
#include "kinetis_power.h"

static const UART_MemMapPtr UART_REG[UARTS_COUNT] =    {UART0, UART1, UART2};


static inline void board_dbg_set_baudrate(UART_PORT port, unsigned int baud)
{
    unsigned int divider, fraction;
    divider = (((port <= UART_0) ? power_get_core_clock() : power_get_clock(POWER_BUS_CLOCK)) << 1) / baud;
    fraction = divider & 0x1f;
    divider = (divider - fraction) / 32;

    UART_REG[port]->C1 &= ~(UART_C1_M_MASK | UART_C1_PE_MASK | UART_C1_PT_MASK);
    UART_REG[port]->BDH = (divider >> 8) & 0xf;
    UART_REG[port]->BDL = divider & 0xff;
    UART_REG[port]->C4 = fraction;
}

void board_dbg_init()
{
    pin_enable(UART_TX_PIN, UART_TX_PIN_MODE, true);
    //enable clocking
    SIM->SCGC4 |= (1 << (UART + SIM_SCGC4_UART0_SHIFT));
    UART_REG[UART]->C2 |= UART_C2_TE_MASK;
    board_dbg_set_baudrate(UART, UART_BAUD);
}

void board_dbg(const char *const buf, unsigned int size)
{
    int i;
    for(i = 0; i < size; ++i)
    {
        __NOP();
        while ((UART_REG[UART]->S1 & UART_S1_TDRE_MASK) == 0) {}
        UART_REG[UART]->D = buf[i];
    }
}
