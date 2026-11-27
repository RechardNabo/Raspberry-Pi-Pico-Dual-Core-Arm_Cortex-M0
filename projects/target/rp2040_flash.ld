/* rp2040_flash.ld — Pico 2MB flash layout: boot2 at 0x10000000, vectors+code
 * at 0x10000100, SRAM 264K striped at 0x20000000. */
MEMORY {
    FLASH(rx)  : ORIGIN = 0x10000000, LENGTH = 2048K
    SRAM(rwx)  : ORIGIN = 0x20000000, LENGTH = 264K
}

ENTRY(Reset_Handler)

SECTIONS {
    .boot2 0x10000000 : {
        KEEP(*(.boot2))
    } > FLASH

    .vectors 0x10000100 : {
        KEEP(*(.vectors))
    } > FLASH

    .text : {
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        __etext = .;
    } > FLASH

    .data : {
        . = ALIGN(4);
        __data_start__ = .;
        *(.data*)
        . = ALIGN(4);
        __data_end__ = .;
    } > SRAM AT > FLASH

    .bss : {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        end = .;
        _end = .;
    } > SRAM

    . = ORIGIN(SRAM) + LENGTH(SRAM);
    __StackTop = .;
}
