/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2019, RExOS team
    All rights reserved.

    author: RL (jam_roma@yahoo.com)
*/

#ifndef NRF_CONFIG_H_
#define NRF_CONFIG_H_

//---------------------- fast drivers definitions -----------------------------------
#define NRF_ADC_DRIVER                          0
#define NRF_WDT_DRIVER                          0
#define NRF_FLASH_DRIVER                        0
#define NRF_SPI_DRIVER                          0
#define NRF_UART_DRIVER                         1
#define NRF_RTC_DRIVER                          0
#define NRF_RADIO_DRIVER                        0

//------------------------------ CORE ------------------------------------------------


//------------------------------- UART -----------------------------------------------


//------------------------------ POWER -----------------------------------------------
/* High Frequency OSC. 0 mean internal RC */
#define HFCLK                                   16000000
/* Low Frequency OSC. 0 - not use LFCLK */
#define LFCLK                                   0
#define LFCLK_SOURCE                            CLOCK_LFCLKSRC_SRC_Synth

//------------------------------ TIMER -----------------------------------------------
#define HPET_TIMER                              TIMER_1
// compare counter from 0 to 3
#define HPET_TIMER_CC                           0
//only required if no NRF_RTC_DRIVER is set
#define SECOND_PULSE_TIMER                      TIMER_2
//------------------------------- ADC ------------------------------------------------


//------------------------------- DAC ------------------------------------------------


//------------------------------- USB ------------------------------------------------

//------------------------------- WDT ------------------------------------------------

//------------------------------ FLASH -----------------------------------------------

//------------------------------- I2C ------------------------------------------------



#endif /* NRF_CONFIG_H_ */
