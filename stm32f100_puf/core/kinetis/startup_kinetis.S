#include "board/kinetis/kinetis.h"
#include "boot_kinetis_config.h"

/*********************** kinetis flash config *********************/
    .syntax unified
    .cpu cortex-m4
    .thumb
    .section  .flashconfig, "ax"

    /* user TRIM */
    .word FLASH_IRC_TRIM
    /* flash config */
    .word FLASH_BACKDOOR1
    .word FLASH_BACKDOOR2
    .word FLASH_FPROT
    .byte FLASH_FSEC
    .byte FLASH_FOPT
    //reserved
    .byte 0xFF
    .byte 0xFF
