/* Linker script for STM32F051R8 */

_Stack_Size = 0x400;  /* 1 kB */

ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 8K
}

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH

    .text :
    {
        . = ALIGN(4);
        _stext = .;
        *(.text*)
        . = ALIGN(4);
        _etext = .;
    } >FLASH

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH

    .init_array :
    {
        . = ALIGN(4);
        KEEP(*SORT(.init_array.*))
        KEEP(*(.init_array*))
        . = ALIGN(4);
    } >FLASH

    .fini_array :
    {
        . = ALIGN(4);
        KEEP(*SORT(.fini_array.*))
        KEEP(*(.fini_array*))
        . = ALIGN(4);
    } >FLASH

    /* initialized data */
    _sidata = LOADADDR(.data);
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT>FLASH

    /* uninitialized data */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } >RAM

    /* check that there is enough remaining space for the stack */
    _estack = ORIGIN(RAM) + LENGTH(RAM) - 1;
    _sstack = _estack - _Stack_Size;
    .user_stack :
    {
        . = ALIGN(4);
        . = . + _Stack_Size;
        . = ALIGN(4);
    } >RAM

    /* discard sections */
    /DISCARD/ :
    {
    }

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
