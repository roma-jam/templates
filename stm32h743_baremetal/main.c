/*
 * main.c
 *
 *  Created on: 4 ���� 2017 �.
 *      Author: RomaJam
 */

#include "board.h"
#include "system.h"
#include "config.h"

#if (DFU_DEBUG)
#include "dbg.h"
#endif


void main(void)
{
//    uint8_t c = 15;
    SYSTEM system;
    system.reboot = false;
    board_init();

#if (DFU_DEBUG)
    board_dbg_init();
    printf("STM32H743 bare matal, CPU %d MHz\n", power_get_core_clock() / 1000000);
#endif // DFU_DEBUG

    delay_ms(999);

    // Enable LED
//    gpio_enable(C13, GPIO_MODE_OUT);
//    pin_reset(C13);

//    printf("%#x\n", c);

    while(!system.reboot)
    {
//        printf("c addr, %#X\n", &c);
//        printf("%#x\n", c++);
//        printf("%#x\n", c);
//        printf("LED blink\n");
//        delay_ms(500);
//        if(pin_get(C13))
//            pin_reset(C13);
//        else
//            pin_set(C13);
        /* main cycle */
    }
}
