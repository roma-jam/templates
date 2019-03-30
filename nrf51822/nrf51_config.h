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
/* High Frequency OSC. 0 mean internal RC */
#define HFCLK                       16000000
/* Low Frequency OSC. 0 - not use LFCLK */
#define LFCLK                       0
#define LFCLK_SOURCE                CLOCK_LFCLKSRC_SRC_Synth

//------------------------------ UART ------------------------------------------
//Use UART as default stdio
#define UART                        UART_0
#define UART_TX_PIN                 P9
#define UART_RX_PIN                 P11

#define UART_BAUD                   UART_BAUDRATE_BAUDRATE_Baud115200
#define UART_DATA_BITS              8
#define UART_PARITY                 'N'
#define UART_STOP_BITS              1

#endif /* NRF51_CONFIG_H_ */
