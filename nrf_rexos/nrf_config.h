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
#define NRF_TIMER_DRIVER                        1
#define NRF_RADIO_DRIVER                        0

//------------------------------ CORE ------------------------------------------------


//------------------------------- UART -----------------------------------------------


//------------------------------ POWER -----------------------------------------------
/* High Frequency OSC. 0 mean internal RC */
#define HFCLK                                   16000000
/* Low Frequency OSC. 0 - not use LFCLK */
#define LFCLK                                   32768
#define LFCLK_SOURCE                            CLOCK_LFCLKSRC_SRC_Synth

//------------------------------ TIMER -----------------------------------------------
//Don't change this if you are not sure. Unused if RTC is configured
#if (NRF_RTC_DRIVER)
#define SECOND_RTC                              RTC_0
#define SECOND_CHANNEL                          RTC_CC0
#else
#define SECOND_CHANNEL                          TIMER_CC1
#endif // NRF_RTC_DRIVER
// Use another channel for HPET
#define HPET_TIMER                              TIMER_0
#define HPET_CHANNEL                            TIMER_CC0
//------------------------------- ADC ------------------------------------------------


//------------------------------- DAC ------------------------------------------------


//------------------------------- USB ------------------------------------------------

//------------------------------- WDT ------------------------------------------------

//------------------------------ FLASH -----------------------------------------------

//------------------------------- I2C ------------------------------------------------



#endif /* NRF_CONFIG_H_ */
