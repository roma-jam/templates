/*
 * system.c
 *
 *  Created on: 14 рту. 2017 у.
 *      Author: RLeonov
 */

#include "lpc.h"
#include "../config.h"

#define WDT_UNLOCK_KEY1                         0xc520
#define WDT_UNLOCK_KEY2                         0xd928

#if (DFU_DEBUG)
#include "../dbg.h"
#endif

//HardFault Status Register
#define SCB_HFSR                                    (*(unsigned int*)0xE000ED2C)
#define HFSR_VECTTBL                                (1 << 1)

#define PSP_IN_LR                                   0xfffffffd
#define SVC_12                                      0xdf12
#define CALLER_ADDRESS                              6

void system_early_init()
{
    // Nothing to do
}


void on_hard_fault(unsigned int ret_value, unsigned int* stack_value)
{
#if (DFU_DEBUG)
    printf("HARD FAULT\n");
    if (SCB_HFSR & HFSR_VECTTBL)
        printf("Vector table read fault at %#.08x\n", stack_value[CALLER_ADDRESS]);
    //wrong sys call
    else if (*(uint16_t*)(stack_value[CALLER_ADDRESS] - 2) == SVC_12)
        printf("SYS call while disabled interrupts at %#.08x\n", stack_value[5] & 0xfffffffe);
    else
        printf("General hard fault at %#.08x\n", stack_value[CALLER_ADDRESS]);

    if (ret_value == PSP_IN_LR && (*(uint16_t*)(stack_value[CALLER_ADDRESS] - 2) == SVC_12))
        asm volatile ("cpsie i");
#endif // DEBUG
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

void irq_handler()
{
#if (DFU_DEBUG)
    printf("IRQ stub\n");
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
