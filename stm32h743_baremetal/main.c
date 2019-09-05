/*
 * main.c
 *
 *  Created on: 4 θώνÿ 2017 γ.
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

    delay_ms(999);


    board_init();

    gpio_enable(B0, GPIO_MODE_OUT);
    gpio_enable(B7, GPIO_MODE_OUT);
    gpio_enable(B14, GPIO_MODE_OUT);

#if (DFU_DEBUG)
    board_dbg_init();
    printf("STM32H743 bare matal, CPU %d MHz\n", power_get_core_clock() / 1000000);
#endif // DFU_DEBUG


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
        delay_ms(500);
        if(pin_get(B7))
        {
            pin_reset(B7);
            pin_set(B14);
        }
        else
        {
            pin_set(B7);
            pin_reset(B14);
        }

        /* main cycle */
    }
}
