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

#define S1_US                       1000000
#define HPET_VALUE                  110000

#define TEST_PIN         P2

void main()
{
    //
    board_init();

//    pin_enable(P28, PIN_MODE_OUTPUT, PIN_PULL_NOPULL);

//    pin_set(P28);

    pin_enable(TEST_PIN, PIN_MODE_OUTPUT, PIN_PULL_NOPULL);
    pin_set(TEST_PIN);

#if (DFU_DEBUG)
    board_dbg_init();
    printf("Template, %s\n", __BUILD_TIME);
    printf("%s core, %d MHz\n", __MCU, power_get_core_clock() / 1000000);
#endif // DFU_DEBUG

//    nrf_radio_init();
//    nrf_radio_start();

    while(1)
    {
        delay_ms(1000);
        printf("TO: 1 s\n");
//        pin_toggle(P28);
        pin_toggle(TEST_PIN);

//        //svc_call(15, 0xF00000AC, 0x80000001, 0xFFFF);
//
////        NRF_TIMER0->TASKS_CAPTURE[0] = 1;
//        printf("timer: %d us\n", (NRF_TIMER0->CC[0]));
    }
}
