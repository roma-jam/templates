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
#define NRF_RTC_DRIVER                          1
#define NRF_TIMER_DRIVER                        1
#define NRF_RADIO_DRIVER                        0

//------------------------------ CORE ------------------------------------------------


//------------------------------- UART -----------------------------------------------
//disable for some memory saving if not blocking IO is required
#define UART_IO_MODE_SUPPORT                                0
//default values for IO mode
#define UART_CHAR_TIMEOUT_US                                10000000
#define UART_INTERLEAVED_TIMEOUT_US                         10000
//size of every uart internal buf. Increasing this you will get less irq ans ipc calls, but faster processing
#define UART_BUF_SIZE                                       16
//generally UART is used as stdout/stdio, so fine-tuning is required only on hi load
#define UART_STREAM_SIZE                                    32

//------------------------------ POWER -----------------------------------------------
/* High Frequency OSC. 0 mean internal RC */
#define HFCLK                                   16000000
/* Low Frequency OSC. 0 - not use LFCLK */
#define LFCLK                                   32768
#define LFCLK_SOURCE                            CLOCK_LFCLKSRC_SRC_Synth

//------------------------------ TIMER -----------------------------------------------
#if (NRF_RTC_DRIVER)
#define KERNEL_SECOND_RTC                       0
#endif //

//Don't change this if you are not sure. Unused if RTC is configured
#if (KERNEL_SECOND_RTC)
#define SECOND_RTC                              RTC_0
#define SECOND_RTC_CHANNEL                      RTC_CC0
#else
#define SECOND_CHANNEL                          TIMER_CC1
#endif // KERNEL_SECOND_RTC
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
