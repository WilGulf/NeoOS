ENTRY(_start)
OUTPUT_FORMAT(binary)


SECTIONS {
    . = 0x400000;

    .multiboot ALIGN (0x1000):
    {
        *(.multiboot)
    }

    .note.gnu.build-id ALIGN (0x1000):
    {
        *(.note.gnu.build-id)
    }

    .boot ALIGN (0x1000):
    {
        *(.boot)
    }

    .text ALIGN (0x1000) : AT(ADDR(.text))
    {
        *(.text)
    }
    .rodata ALIGN (0x1000) : AT(ADDR(.rodata))
    {
        *(.rodata*)
    }
    .data ALIGN (0x1000) : AT(ADDR(.data))
    {
        *(.data)
    }
    .bss ALIGN (0x1000) : AT(ADDR(.bss))
    {
        *(COMMON)
        *(.bss)
    }

    kernel_virtual_start = 0x00100000;
    kernel_virtual_end = .;
    kernel_physical_start = 0x00100000;
    kernel_physical_end = .;

}