/* STM32F042F4P6 Linker Script */
/* 16KB Flash, 6KB SRAM */

MEMORY
{
    rom (rx)  : ORIGIN = 0x08000000, LENGTH = 16K
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 6K
}

ENTRY(reset_handler)

SECTIONS
{
    .text : {
        *(.vectors)
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > rom

    .data : {
        _data = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > ram AT > rom
    _data_loadaddr = LOADADDR(.data);

    .bss : {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > ram

    end = .;
}

PROVIDE(_stack = ORIGIN(ram) + LENGTH(ram));
