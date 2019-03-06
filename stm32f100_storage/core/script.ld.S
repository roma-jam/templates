
/*
    universal LD-script for ARM processors
 */
//if you've got error on this line, than this file is not preprocessed

#include "core.h"

MEMORY
{
    FLASH (rx) : ORIGIN = FLASH_BASE, LENGTH = FLASH_SIZE
    RAM (rw)   : ORIGIN = SRAM_BASE, LENGTH = SRAM_SIZE
}

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vectors));
        KEEP(*(.flashconfig));
        KEEP(*(.reset));
        *(.text .text.* .gnu.linkonce.t.*)
        *(.glue_7t) *(.glue_7)
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        . = ALIGN(4);
        _etext = .;
        PROVIDE(__ETEXT = .);
    } > FLASH

    _end = . ;
    PROVIDE(__END = .);
}
