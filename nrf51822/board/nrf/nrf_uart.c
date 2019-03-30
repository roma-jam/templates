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

void board_dbg_init()
{
    /* Configure TX and RX pins */
    pin_enable(UART_TX_PIN, PIN_MODE_OUTPUT, PIN_PULL_NOPULL);
//    nrf_gpio_cfg_input(RX_PIN_NUMBER, NRF_GPIO_PIN_NOPULL);

    /* set TX_PIN */
    NRF_UART0->PSELTXD = UART_TX_PIN;
//    NRF_UART0->PSELRXD = UART_RX_PIN;

    /* Configure other UART parameters, BAUD rate is defined in uart_nrf.h  */
    NRF_UART0->BAUDRATE = (UART_BAUD << UART_BAUDRATE_BAUDRATE_Pos);
    NRF_UART0->ENABLE = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
    NRF_UART0->EVENTS_RXDRDY = 0;

    /* Enable UART RX interrupt only */
    //NRF_UART0->INTENSET = (UART_INTENSET_RXDRDY_Set << UART_INTENSET_RXDRDY_Pos);

    /* Start reception and transmission */
    NRF_UART0->TASKS_STARTTX = 1;
}

void board_dbg(const char *const buf, unsigned int size)
{
    int i;
    for(i = 0; i < size; ++i)
    {
        NRF_UART0->TXD = buf[i];
        /* wait previous TX complete */
        while(NRF_UART0->EVENTS_TXDRDY != 1);
        /* clear flag */
        NRF_UART0->EVENTS_TXDRDY = 0;
        /* send new data */

    }
}
