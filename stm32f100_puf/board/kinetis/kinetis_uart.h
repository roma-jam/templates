/*
 * kinetis_uart.h
 *
 *  Created on: 5 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#ifndef KINETIS_UART_H_
#define KINETIS_UART_H_

#include <stdint.h>

typedef enum {
    UART_0 = 0,
    UART_1,
    UART_2,
    UART_3,
    UART_4,
    UART_5,
    LPUART,
    UART_MAX
}UART_PORT;

#endif /* KINETIS_UART_H_ */
