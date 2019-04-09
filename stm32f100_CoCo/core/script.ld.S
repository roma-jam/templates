
/*
    universal LD-script for ARM processors
 */
//if you've got error on this line, than this file is not preprocessed

#include "core.h"

MEMORY
{
    FLASH (rx) : ORIGIN = FLASH_BASE, LENGTH = FLASH_SIZE
    RAM (rw)   : ORIGIN = SRAM_BASE, LENGTH = SRAM_SIZE
    BSS (rw)   : ORIGIN = SRAM_BSS_BASE, LENGTH = SRAM_BSS_SIZE
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

    _textdata = __ETEXT;

    .data :
    {
        . = ALIGN(4);
        PROVIDE(_data = .);
        *(.data)
        . = ALIGN(4);
        *(.data.*)
        . = ALIGN(4);
        *(.ramtext)
        . = ALIGN(4);
        PROVIDE(_edata = .);
    } > BSS AT > FLASH

    .bss :
    {
        . = ALIGN(4);
        PROVIDE(_bss_start = .);
        *(.bss .bss.* .gnu.linkonce.t.*)
        . = ALIGN(4);
        *(.glue_7t) *(.glue_7)
        . = ALIGN(4);
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        . = ALIGN(4);
        PROVIDE(_bss_end = .);
    } > BSS


    _end = . ;
    PROVIDE(__END = .);
}
