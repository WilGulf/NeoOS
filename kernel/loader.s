global loader
extern kmain

MAGIC_NUMBER equ 0x1BADB002
FLAGS equ 1 << 0 | 1 << 1 |0 ; page align | mem info | use gfx
CHECKSUM equ -(MAGIC_NUMBER + FLAGS)

KERNEL_STACK_SIZE equ 4096

; Make GRUB know that this is actually a OS
section .multiboot
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM

    dd 0, 0, 0, 0, 0

section .bss
align 16
stack_bottom:
    resb 16384 * 8
stack_top:

section .boot
loader:
    jmp start

section .text
start:
    mov esp, stack_top
    push ebx
    push eax
    xor ebp, ebp

    extern kernel_virtual_start
    extern kernel_virtual_end
    extern kernel_physical_start
    extern kernel_physical_end

    push kernel_virtual_start
    push kernel_virtual_end
    push kernel_physical_start
    push kernel_physical_end

    call kmain
halt:
    hlt
    jmp halt