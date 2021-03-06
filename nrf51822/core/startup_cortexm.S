//if you've got error in this line, than this file is compiled wrong

#include "config.h"
#include "core.h"

/* Define constants used in low-level initialization.  */

  .equ   ICER0,                 0xe000e180
  .equ   ICPR0,                 0xe000e280

  .equ   LR_TO_PROCESS_PSP,     0xfffffffd
  .equ   LR_TO_PROCESS_MSP,     0xfffffff9

/* exported global constant and functions */
    .global delay_ms
    .global delay_us

    .extern main

/* interrupt vectors */
    .section  .isr_vectors, "ax"

int_vectors:
    .word  (SRAM_BASE + SRAM_SIZE)
    .word  Reset_Handler
    .word  NMI_Handler
    .word  HardFault_Handler
#if defined(CORTEX_M3) || defined(CORTEX_M4)
    .word  MemManage_Handler
    .word  BusFault_Handler
    .word  UsageFault_Handler
#else
    .word  0
    .word  0
    .word  0
#endif
    .word  0
    .word  0
    .word  0
    .word  0
    .word  SVC_Handler
#if defined(CORTEX_M3) || defined(CORTEX_M4)
    .word  DebugMon_Handler
#else
    .word  0
#endif
    .word  0
    .word  PendSV_Handler
    .word  SysTick_Handler

    .rept IRQ_VECTORS_COUNT
        .word  irq_handler
    .endr

/*********************** reset vector handler *********************/
    .section  .reset, "ax"
    .syntax unified
#if defined(CORTEX_M3)
    .cpu cortex-m3
#elif defined(CORTEX_M4)
    .cpu cortex-m4
#else
    .cpu cortex-m0
#endif
    .thumb

    .thumb_func
Reset_Handler:
    cpsid i

    @disable all interrupts
    ldr r0, =0xffffffff
    ldr r1, =ICER0
    str r0, [r1]
#if defined(CORTEX_M3) || defined(CORTEX_M4)
    str r0, [r1, #0x4]
    str r0, [r1, #0x8]
    str r0, [r1, #0xc]
    str r0, [r1, #0x10]
    str r0, [r1, #0x14]
    str r0, [r1, #0x18]
    str r0, [r1, #0x1c]
#endif

    @clear all pending interrupts
    ldr r1, =ICPR0
    str r0, [r1]
#if defined(CORTEX_M3) || defined(CORTEX_M4)
    str r0, [r1, #0x4]
    str r0, [r1, #0x8]
    str r0, [r1, #0xc]
    str r0, [r1, #0x10]
    str r0, [r1, #0x14]
    str r0, [r1, #0x18]
    str r0, [r1, #0x1c]
#endif

    bl    system_early_init

    @switch to MSP
    ldr r0, =0x0;
    mrs r0, control
    ldr r0, =SP_PTR
    mov sp, r0

    @enable interrupts
    cpsie i
    bl     main
    @never reach
    b      .

    /* code segment */
    .section  .text, "ax"
    .syntax unified
    .thumb

    .thumb_func
delay_ms:
    ldr r1, =DELAY_COEF_MS
    muls    r0, r1

loop_ms:
    subs    r0, #1
    bge loop_ms

    bx      lr

    .thumb_func
delay_us:
    ldr r1, =DELAY_COEF_US
    muls    r0, r1

loop_us:
    subs    r0, #1
    bge loop_us

    bx      lr

/*********************** exception vectors handlers *********************/
.macro exception_enter
    mov   r0, lr
    movs  r1, #0
    subs  r1, r1, #3
    cmp   r0, r1
    bne   1f
    mrs   r1, psp
    b     2f
1:
    mrs   r1, msp
2:
.endm

    .thumb_func
HardFault_Handler:
    @exception_enter
    b   on_hard_fault

    .thumb_func
SVC_Handler:
    //mrs   r0, psp                       @ move psp value to r0
    mrs     r0,msp                      @ move msp value to r0
    ldmia r0, {r0-r3}                   @ store multiply register from r0 to r3
    bl    svc                           @ call c handler

    //return to thread mode
#if defined(CORTEX_M0)
    ldr   r0, =LR_TO_PROCESS_MSP        @ recover stack pointer to msp
    bx    r0                            @ return
#else
    ldr   pc, SW
SW: .word LR_TO_PROCESS_PSP
#endif

    .thumb_func
irq_handler:
    mrs   r0, ipsr
    subs  r0, r0, #16
    b     irq

#if defined(CORTEX_M3) || defined(CORTEX_M4)
    .thumb_func
MemManage_Handler:
    b   on_mem_manage

    .thumb_func
BusFault_Handler:
    b   on_bus_fault

    .thumb_func
UsageFault_Handler:
    b   on_usage_fault
#endif

/*********************** weak unhandled vectors *********************/
.macro weak_vector vector
    .weak   \vector
    .thumb_set \vector, default_handler
.endm

weak_vector   NMI_Handler
#if defined(CORTEX_M3) || defined(CORTEX_M4)
weak_vector   DebugMon_Handler
#endif
weak_vector   PendSV_Handler
weak_vector   SysTick_Handler
