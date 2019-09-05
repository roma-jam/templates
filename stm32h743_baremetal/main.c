/*
 * main.c
 *
 *  Created on: 4 θώνÿ 2017 γ.
 *      Author: RomaJam
 */

#include "board.h"
#include "system.h"
#include "config.h"
#include "core.h"

#if (DFU_DEBUG)
#include "dbg.h"
#endif


bool is_address_valid(volatile const uint32_t *address)
{
    uint32_t value = 0;
    bool is_valid = true;
    /* Cortex-M3, Cortex-M4, Cortex-M4F, Cortex-M7 are supported */
    static const uint32_t BFARVALID_MASK = (0x80 << SCB_CFSR_BUSFAULTSR_Pos);

    /* Clear BusFault (BFARVALID) flag by writing 1 to it */
    SCB->CFSR |= BFARVALID_MASK;

    /* Ignore BusFault by enabling BFHFNMIGN and disabling interrupts */
    uint32_t mask = __get_FAULTMASK();

    __disable_fault_irq();

    SCB->CCR |= SCB_CCR_BFHFNMIGN_Msk;

    /* probe address */
    value = *address;

    /* Check BFARVALID flag */
    if ((SCB->CFSR & BFARVALID_MASK) != 0)
    {
        /* Bus Fault occured reading the address */
        is_valid = false;
    }

    /* Reenable BusFault by clearing  BFHFNMIGN */
    SCB->CCR &= ~SCB_CCR_BFHFNMIGN_Msk;
    __set_FAULTMASK(mask);

    return is_valid;
}

void main(void)
{
//    uint8_t c = 15;
    SYSTEM system;
    system.reboot = false;

    SCB_EnableICache();
    SCB_EnableDCache();

    delay_ms(999);


    board_init();

    gpio_enable(B0, GPIO_MODE_OUT);
    gpio_enable(B7, GPIO_MODE_OUT);
    gpio_enable(B14, GPIO_MODE_OUT);

#if (DFU_DEBUG)
    board_dbg_init();
    printf("STM32H743 bare matal, CPU %d MHz\n", power_get_core_clock() / 1000000);
#endif // DFU_DEBUG

    uint32_t rev = (DBGMCU->IDCODE & DBGMCU_IDCODE_REV_ID_Msk) >> DBGMCU_IDCODE_REV_ID_Pos;
    uint32_t dev = (DBGMCU->IDCODE & DBGMCU_IDCODE_DEV_ID_Msk) >> DBGMCU_IDCODE_DEV_ID_Pos;
    printf("rev: %X, dev: %X\n", rev, dev);


    /* check RAM access */
    uint32_t addr = SRAM_BASE;
    uint32_t sram_size = 0;
    do
    {
        sram_size += sizeof(uint32_t);
        addr += sizeof(uint32_t);
    }
    while (is_address_valid((const uint32_t*)addr));

    printf("DTCM SRAM: %d KB\n", sram_size >> 10);

    addr = SRAM_AXI_BASE;
    sram_size = 0;
    do
    {
        sram_size += sizeof(uint32_t);
        addr += sizeof(uint32_t);
    }
    while (is_address_valid((const uint32_t*)addr));

    printf("AXI SRAM: %d KB\n", sram_size >> 10);

    addr = SRAM1_BASE;
    sram_size = 0;
    do
    {
        sram_size += sizeof(uint32_t);
        addr += sizeof(uint32_t);
    }
    while (is_address_valid((const uint32_t*)addr));

    printf("SRAM1: %d KB\n", sram_size >> 10);

    addr = SRAM2_BASE;
    sram_size = 0;
    do
    {
        sram_size += sizeof(uint32_t);
        addr += sizeof(uint32_t);
    }
    while (is_address_valid((const uint32_t*)addr));

    printf("SRAM2: %d KB\n", sram_size >> 10);

    addr = SRAM3_BASE;
    sram_size = 0;
    do
    {
        sram_size += sizeof(uint32_t);
        addr += sizeof(uint32_t);
    }
    while (is_address_valid((const uint32_t*)addr));

    printf("SRAM3: %d KB\n", sram_size >> 10);

    addr = SRAM4_BASE;
    sram_size = 0;
    do
    {
        sram_size += sizeof(uint32_t);
        addr += sizeof(uint32_t);
    }
    while (is_address_valid((const uint32_t*)addr));

    printf("SRAM4: %d KB\n", sram_size >> 10);

    addr = SRAM_BACKUP_BASE;
    sram_size = 0;
    do
    {
        sram_size += sizeof(uint32_t);
        addr += sizeof(uint32_t);
    }
    while (is_address_valid((const uint32_t*)addr));

    printf("SRAM BACKUP: %d KB\n", sram_size >> 10);

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
