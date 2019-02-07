/*
 * system.c
 *
 *  Created on: 6 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#include "kinetis.h"
#include "../config.h"

#define WDT_UNLOCK_KEY1                         0xc520
#define WDT_UNLOCK_KEY2                         0xd928

#if (DFU_DEBUG)
#include "../dbg.h"

#define HALT()                                  for(;;)
#endif

void system_early_init()
{
    //disable watchdog
    WDOG->UNLOCK = WDT_UNLOCK_KEY1;
    WDOG->UNLOCK = WDT_UNLOCK_KEY2;
    WDOG->STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;

    /*
        e9005:Core: Store immediate overlapping exception return operation might vector to
        incorrect interrupt
     */
    SCnSCB->ACTLR |= SCnSCB_ACTLR_DISDEFWBUF_Msk;
}


void on_hard_fault()
{
#if (DFU_DEBUG)
    printf("HARD FAULT\n");
    HALT();
#endif // CPU_DEBUG
    NVIC_SystemReset();
}

void on_mem_manage()
{
#if (DFU_DEBUG)
    printf("MEM FAULT\n");
    HALT();
#endif // CPU_DEBUG
    NVIC_SystemReset();
}

void on_bus_fault()
{
#if (DFU_DEBUG)
    printf("BUS FAULT\n");
    HALT();
#endif // CPU_DEBUG
    NVIC_SystemReset();
}

void on_usage_fault()
{
#if (DFU_DEBUG)
    printf("USAGE FAULT\n");
    HALT();
#endif // CPU_DEBUG
    NVIC_SystemReset();
}

void irq_handler()
{
#if (DFU_DEBUG)
    printf("IRQ stub\n");
    HALT();
#endif // CPU_DEBUG
    NVIC_SystemReset();
}

void default_handler()
{
#if (DFU_DEBUG)
    printf("Unmasked IRQ\n");
    HALT();
#endif // CPU_DEBUG
    NVIC_SystemReset();
}
