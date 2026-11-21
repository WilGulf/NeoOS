bits 64

extern kmain

CODE_SEG equ 0x08
DATA_SEG equ 0x10
LONG_MODE_CODE_SEG equ 0x18
LONG_MODE_DATA_SEG equ 0x20

section .text
global loader
loader:
    cli
    mov al, 'A'
    out 0xE9, al
    jmp long_mode_entry

long_mode_entry:
    mov rax, pml4_table
    mov cr3, rax

    lgdt [gdt_descriptor]

    mov ax, LONG_MODE_DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax



    mov rsp, 0x00200000
    mov rbp, rsp

    push qword 0x18
    push qword long_mode_new_gdt_complete
    retfq

long_mode_new_gdt_complete:
    mov al, 0x11
    out 0x20, al

    mov al, 0x20
    out 0x21, al

    mov al, 0x04
    out 0x21, al

    mov al, 0x01
    out 0x21, al



    mov al, 0x11
    out 0xA0, al

    mov al, 0x28
    out 0xA1, al

    mov al, 0x02
    out 0xA1, al

    mov al, 0x01
    out 0xA1, al



    mov al, 0xFB
    out 0x21, al

    mov al, 0xFF
    out 0xA1, al

    mov al, 0x20
    out 0x20, al
    out 0xA0, al



    extern kernel_virtual_start
    extern kernel_virtual_end
    extern kernel_physical_start
    extern kernel_physical_end

    push kernel_virtual_start
    push kernel_virtual_end
    push kernel_physical_start
    push kernel_physical_end

    xor ebp, ebp
    call kmain

.halt:
    cli
    hlt
    jmp .halt

global gdt
align 8
gdt:
    dq 0x0000000000000000

    ;32
    dw 0xFFFF
    dw 0
    db 0
    db 0x9A
    db 11001111b
    db 0

    dw 0xFFFF
    dw 0
    db 0
    db 0x92
    db 11001111b
    db 0

    ;64
    dw 0x0000
    dw 0x0000
    db 0x00
    db 0x9A
    db 0x20
    db 0x00

    dw 0x0000
    dw 0x0000
    db 0x00
    db 0x92
    db 0x00
    db 0x00

    dw 0x0000
    dw 0x0000
    db 0x00
    db 0xFA
    db 0x20
    db 0x00

    dw 0x0000
    dw 0x0000
    db 0x00
    db 0xF2
    db 0x00
    db 0x00

    dw 0x0000
    dw 0x0000
    db 0x00
    db 0x00
    db 0x00
    db 0x00

    dw 0x0000
    dw 0x0000
    db 0x00
    db 0x00
    db 0x00
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt - 1
    dq gdt

align 0x1000
pml4_table:
    dq pdpt_table + 0x03
    times 511 dq 0

align 0x1000
pdpt_table:
    dq pd_table + 0x03
    times 511 dq 0

align 0x1000
pd_table:
    %assign addr 0x00000000
    %rep 512
        dq addr + 0x83
        %assign addr addr + 0x200000
    %endrep
