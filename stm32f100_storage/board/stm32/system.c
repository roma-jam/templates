/*
 * system.c
 *
 *  Created on: 6 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#include "stm32.h"
#include "../config.h"

#if (SYS_DEBUG)
#include "../dbg.h"
#endif

void system_early_init()
{
    // Nothing to do
}


void on_hard_fault()
{
#if (SYS_DEBUG)
    printf("HARD FAULT\n");
#endif // CPU_DEBUG
    while(1);
}

void on_mem_manage()
{
#if (SYS_DEBUG)
    printf("MEM FAULT\n");
#endif // CPU_DEBUG
    while(1);
}

void on_bus_fault()
{
#if (SYS_DEBUG)
    printf("BUS FAULT\n");
#endif // CPU_DEBUG
    while(1);
}

void on_usage_fault()
{
#if (SYS_DEBUG)
    printf("USAGE FAULT\n");
#endif // CPU_DEBUG
    while(1);
}

void irq_handler()
{
#if (SYS_DEBUG)
    printf("IRQ stub\n");
#endif // CPU_DEBUG
    while(1);
}

void default_handler()
{
#if (SYS_DEBUG)
    printf("Unmasked IRQ\n");
#endif // CPU_DEBUG
    while(1);
}
