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

    while(1)
    {
        delay_ms(1000);
        pin_toggle(P28);
    }
}
