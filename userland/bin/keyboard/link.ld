ENTRY(_start)
OUTPUT_FORMAT(binary)

SECTIONS {
    . = 0x400000;

    .text : ALIGN (0x1000)
    {
        *(.text)
    }

    .asm : ALIGN (0x1000)
    {
        *(.asm)
    }

    .rodata : ALIGN (0x1000)
    {
        *(.rodata*)
    }

    .data : ALIGN (0x1000)
    {
        *(.data)
    }

    .bss : ALIGN (0x1000)
    {
        *(COMMON)
        *(.bss)
    }
}