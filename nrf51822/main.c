/*
 * main.c
 *
 *  Created on: 30 mar 2019 y.
 *      Author: jam_r
 */

#include <string.h>
#include "board.h"

#if (DFU_DEBUG)
#include "dbg.h"
#endif // DFU_DEBUG

extern void svc_call(unsigned int num, unsigned int param1, unsigned int param2, unsigned int param3);

/** Prescalar to the HF Clock for the TIMER0 peripheral based on f = HFCLK/(2^prescaler) */
#define TIMER_PRESCALER         4
/** Number of bits in the timer. TIMER 1 and 2 can only be 8 or 16 bit */
#define TIMER_BITSIZE           TIMER_BITMODE_BITMODE_32Bit


#define RTC_PRESCALER               ((LFCLK/32768) - 1)

void main()
{
    //
    board_init();

    pin_enable(P28, PIN_MODE_OUTPUT, PIN_PULL_NOPULL);

    pin_set(P28);

#if (DFU_DEBUG)
    board_dbg_init();
    printf("Template, %s\n", __BUILD_TIME);
    printf("%s core, %d MHz\n", __MCU, power_get_core_clock() / 1000000);
#endif // DFU_DEBUG

//    nrf_radio_init();
//    nrf_radio_start();

    /* init timer */
//    NRF_TIMER0->TASKS_STOP     = 1;                         // Stop timer.
//    NRF_TIMER0->MODE           = TIMER_MODE_MODE_Timer;     // Set the timer in Timer Mode.
//    //NRF_TIMER0->MODE           = TIMER_MODE_MODE_Counter;     // Set the timer in Timer Mode.
//    NRF_TIMER0->PRESCALER      = TIMER_PRESCALER;           // Prescaler 0 produces 16 MHz.
//    NRF_TIMER0->BITMODE        = TIMER_BITSIZE;             // 32 bit mode.
//    NRF_TIMER0->TASKS_CLEAR    = 1;                         // clear the task first to be usable for later.
//
//
//    NRF_TIMER0->CC[0] = 1000000;
//    NRF_TIMER0->CC[1] = 330000;
//
//
//    NRF_TIMER0->INTENSET |= TIMER_INTENSET_COMPARE0_Msk | TIMER_INTENSET_COMPARE1_Msk;
//
//    NVIC_EnableIRQ(TIMER0_IRQn);
//
//    NRF_TIMER0->TASKS_START   = 1;                          // Start timer.

    // --------------------- RTC ------------------------
    NRF_RTC0->TASKS_STOP = 1;
    NRF_RTC0->PRESCALER = RTC_PRESCALER;      // Set prescaler to a TICK of RTC_FREQUENCY.
//    NVIC_SetPriority(RTC1_IRQn,PRIORITY_RTC1_IRQn);
    NVIC_EnableIRQ(RTC0_IRQn);                // Enable Interrupt for RTC1 in the core.

    NRF_RTC0->CC[0]    = 32767;

    NRF_RTC0->EVENTS_COMPARE[0] = 0;

    NRF_RTC0->EVTENSET |= RTC_INTENSET_COMPARE0_Msk;

    NRF_RTC0->INTENSET |= RTC_INTENSET_COMPARE0_Msk;

    NRF_RTC0->TASKS_START = 1;


    while(1);

//    {
//        delay_ms(1000);
//        //pin_toggle(P28);
//
//        //svc_call(15, 0xF00000AC, 0x80000001, 0xFFFF);
//
////        NRF_TIMER0->TASKS_CAPTURE[0] = 1;
//        printf("timer: %d us\n", (NRF_TIMER0->CC[0]));
//    }
}

void RTC0_IRQHandler()
{
    printf("RTC SECOND, rtc cnt %d\n", NRF_RTC1->COUNTER);
    pin_toggle(P28);
    NRF_RTC0->EVENTS_COMPARE[0] = 0;
    NRF_RTC0->CC[0] += 32767;
    printf("next shot %d\n", NRF_RTC0->CC[0]);
}

void TIMER0_IRQHandler()
{
    if ((NRF_TIMER0->EVENTS_COMPARE[0] != 0) && ((NRF_TIMER0->INTENSET & TIMER_INTENSET_COMPARE0_Msk) != 0))
    {
        printf("TIMER SECOND, tc %d\n", NRF_TIMER0->CC[0]);
        pin_toggle(P28);
        NRF_TIMER0->EVENTS_COMPARE[0] = 0;           // Clear compare register 0 event
        NRF_TIMER0->TASKS_CLEAR = 1;
//        NRF_TIMER0->CC[0] = 0;
    }

    if ((NRF_TIMER0->EVENTS_COMPARE[1] != 0) && ((NRF_TIMER0->INTENSET & TIMER_INTENSET_COMPARE1_Msk) != 0))
    {
        printf("TIMER HPET, tc %d\n", NRF_TIMER0->CC[1]);
//        pin_toggle(P28);
        NRF_TIMER0->EVENTS_COMPARE[1] = 0;           // Clear compare register 0 event
//        NRF_TIMER0->TASKS_CLEAR = 1;
//        NRF_TIMER0->CC[0] = 0;
    }
}
