/*
 * nrf51_config.h
 *
 *  Created on: 30 ???. 2019 ?.
 *      Author: jam_r
 */

#ifndef NRF51_CONFIG_H_
#define NRF51_CONFIG_H_


// ----------------------------- LED -------------------------------------------
#define LED_PIN                     P31
#define LED_PIN_MODE1               GPIO_MODE_OUT


//------------------------------ POWER -----------------------------------------

//------------------------------ UART ------------------------------------------
//Use UART as default stdio
#define UART                                    UART_0
#define UART_TX_PIN                             P1
//#define UART_AF_NUMBER                          AF1

#define UART_BAUD                               115200
#define UART_DATA_BITS                          8
#define UART_PARITY                             'N'
#define UART_STOP_BITS                          1

#endif /* NRF51_CONFIG_H_ */
