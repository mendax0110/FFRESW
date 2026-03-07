/*
 * MCXN947 Linker Script (Flash configuration)
 * Memory map for NXP MCXN947 Cortex-M33 core 0
 *
 * Adjust addresses and sizes to match the specific MCXN947 variant.
 */

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 1024K
    SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 512K
}

ENTRY(Reset_Handler)

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    .data : AT(_etext)
    {
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM

    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM

    .heap :
    {
        __HeapBase = .;
        __end__ = .;
        end = __end__;
        KEEP(*(.heap*))
        __HeapLimit = .;
    } > SRAM

    .stack (ORIGIN(SRAM) + LENGTH(SRAM) - 0x2000) :
    {
        __StackLimit = .;
        KEEP(*(.stack*))
        . = . + 0x2000;
        __StackTop = .;
    } > SRAM

    PROVIDE(__stack = __StackTop);
}
