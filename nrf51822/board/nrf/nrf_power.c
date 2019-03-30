/*
 * nrf_power.c
 *
 *  Created on: 30 ???. 2019 ?.
 *      Author: jam_r
 */

#include "nrf.h"
#include "nrf_power.h"


/** Define the source of the LF Clock. Used in @ref lfclk_init. */
#define SRC_LFCLK                   CLOCK_LFCLKSRC_SRC_Synth
/** HFCLK frequency in Hertz, constant of 16 MHz. */
#define HFCLK_FREQUENCY             (16000000UL)
/** LFCLK frequency in Hertz, constant of 32.768 kHz. */
#define LFCLK_FREQUENCY             (32768UL)

void power_init()
{
    /* Check if 16 MHz crystal oscillator is already running. */
    if (!(NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_SRC_Xtal))
    {
        NRF_CLOCK->INTENSET = CLOCK_INTENSET_HFCLKSTARTED_Msk;
        // Enable wake-up on event
        SCB->SCR |= SCB_SCR_SEVONPEND_Msk;

        NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
        NRF_CLOCK->TASKS_HFCLKSTART = 1;
        /* Wait for the external oscillator to start up. */
        while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
        /* Clear the event and the pending interrupt */
        NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
        NVIC_ClearPendingIRQ(POWER_CLOCK_IRQn);
        NRF_CLOCK->INTENSET = 0;
    }

    if(!(NRF_CLOCK->LFCLKSTAT & CLOCK_LFCLKSTAT_STATE_Running))
    {
      NRF_CLOCK->LFCLKSRC = (SRC_LFCLK << CLOCK_LFCLKSRC_SRC_Pos);

      NRF_CLOCK->INTENSET = CLOCK_INTENSET_LFCLKSTARTED_Msk;
      // Enable wake-up on event
      SCB->SCR |= SCB_SCR_SEVONPEND_Msk;

      NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
      NRF_CLOCK->TASKS_LFCLKSTART = 1;
      /* Wait for the external oscillator to start up. */
      while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
      /* Clear the event and the pending interrupt */
      NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
      NVIC_ClearPendingIRQ(POWER_CLOCK_IRQn);
      NRF_CLOCK->INTENSET = 0;
    }
}

unsigned int power_get_clock(POWER_CLOCK_TYPE clock_type)
{

}

unsigned int power_get_core_clock()
{

}

