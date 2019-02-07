/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2017, Alexey Kramarenko
    All rights reserved.
*/

#ifndef KINETIS_H
#define KINETIS_H

#if defined(MK22FN512VDC12) || defined(MK22FN512VLL12) || defined(MK22FN512VLH12) || defined(MK22FN512VMP12) || defined(MK22FN512VFX12)
#define MK22F51212
#endif

#if defined(MK22FN256VDC12) || defined(MK22FN256VLL12) || defined(MK22FN256VMP12) || defined(MK22FN256VLH12)
#define MK22F25612
//256k
#define FLASH_SIZE                      0x40000
//48k
#define SRAM_SIZE_FULL                  0xC000
#define SRAM_BASE                       0x1fffc000
#endif

#if defined(MK22FN128CAK10) || defined(MK22FN128VDC10) || defined(MK22FN128VLH10) || defined(MK22FN128VLL10) || defined(MK22FN128VMP10)
#define MK2212810
//128k
#define FLASH_SIZE                      0x20000
//24k
#define SRAM_SIZE_FULL                  0x6000
#define SRAM_BASE                       0x1fffe000
#endif

#if defined(MK22FX512AVLQ12) || defined(MK22FX512AVMD12) || defined(MK22FX512AVLL12) || defined(MK22FX512AVLH12) || defined(MK22FX512AVLK12) || defined(MK22FX512AVMC12)
#define MK22F512A12
#endif

#if defined(MK22FN1M0AVLQ12) || defined(MK22FN1M0AVMD12) || defined(MK22FN1M0AVLL12) || defined(MK22FN1M0AVLH12) || defined(MK22FN1M0AVLK12) || defined(MK22FN1M0AVMC12)
#define MK22F1M0A12
#endif

#if defined(MK22F51212) || defined(MK22F512A12)
//512k
#define FLASH_SIZE                      0x80000
//128k
#define SRAM_SIZE_FULL                  0x20000
#define SRAM_BASE                       0x1fff0000
#endif

#if defined(MK22F1M0A12)
//1M
#define FLASH_SIZE                      0x100000
//128k
#define SRAM_SIZE_FULL                  0x20000
#define SRAM_BASE                       0x1fff0000
#endif

#if defined(MK22F51212) || defined(MK22F25612) || defined(MK22F12810)
#define MK22F12
#define UARTS_COUNT                     3
#define I2C_COUNT                       2
#endif

#if defined(MK22F12810)
#define MK22F10
#define UARTS_COUNT                     3
#define I2C_COUNT                       2
#endif

#if defined(MK22F512A12) || defined(MK22F1M0A12)
#define MK22FA12
#define UARTS_COUNT                     6
#define I2C_COUNT                       3
#endif

#if defined(MK22F12) || defined(MK22F10) || defined(MK22FA12)
#define MK22
#endif

#if defined(MK22)
#include "../boot_kinetis_config.h"

#define EXODRIVERS
#define STARTUP_HARDWARE_INIT
#define IRQ_VECTORS_COUNT               239
#ifndef CORTEX_M4
#define CORTEX_M4
#endif //CORTEX_M4
#define KINETIS
#define FLASH_BASE                      0x00000000

#if (KINETIS_USB)
#define SRAM_USB                        (SRAM_SIZE_FULL - 512)
#define SRAM_SIZE                       SRAM_USB
#else
#define SRAM_SIZE                       SRAM_SIZE_FULL
#endif //KINETIS_USB

#define SRAM_STACK                      (SRAM_BASE + SRAM_SIZE)

#endif //MK22

#if !defined(LDS) && !defined(__ASSEMBLER__)

#if defined(MK22)

#if defined(MK22F51212)
#include "MK22F51212.h"
#elif defined(MK22F25612)
#include "MK22F25612.h"
#elif defined(MK22F12810)
#include "MK22F12810.h"
#elif defined(MK22FA12)
#include "MK22FA12.h"
#endif

#endif //defined(MK22)

#endif // !defined(LDS) && !defined(__ASSEMBLER__)

#endif //KINETIS_H
