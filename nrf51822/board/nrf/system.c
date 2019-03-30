/*
 * system.c
 *
 *  Created on: 6 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#include "nrf.h"
#include "config.h"
#include "board.h"

#if (DFU_DEBUG)
#include "../dbg.h"
#endif

void system_early_init()
{
    // Nothing to do
}


void on_hard_fault()
{
#if (DFU_DEBUG)
    printf("HARD FAULT\n");
#endif // CPU_DEBUG
    while(1);
}

void on_mem_manage()
{
#if (DFU_DEBUG)
    printf("MEM FAULT\n");
#endif // CPU_DEBUG
    while(1);
}

void on_bus_fault()
{
#if (DFU_DEBUG)
    printf("BUS FAULT\n");
#endif // CPU_DEBUG
    while(1);
}

void on_usage_fault()
{
#if (DFU_DEBUG)
    printf("USAGE FAULT\n");
#endif // CPU_DEBUG
    while(1);
}

void irq(int vector)
{
    if(RADIO_IRQn == vector)
    {
        RADIO_IRQHandler();
        return;
    }

#if (DFU_DEBUG)
    printf("IRQ stub, %d\n", vector);
#endif // CPU_DEBUG
    while(1);
}

void default_handler()
{
#if (DFU_DEBUG)
    printf("Unmasked IRQ\n");
#endif // CPU_DEBUG
    while(1);
}
