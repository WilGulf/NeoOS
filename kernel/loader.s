bits 32

extern kmain

CODE_SEG equ 0x08
DATA_SEG equ 0x10
LONG_MODE_CODE_SEG equ 0x18
LONG_MODE_DATA_SEG equ 0x20

section .text
global loader
loader:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp

    lgdt [gdt_descriptor]

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov eax, pml4_table
    mov cr3, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x100
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    jmp LONG_MODE_CODE_SEG:long_mode_entry

bits 64
long_mode_entry:
    mov ax, LONG_MODE_DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rsp, 0x00200000
    mov rbp, rsp

    extern kernel_virtual_start
    extern kernel_virtual_end
    extern kernel_physical_start
    extern kernel_physical_end

    push kernel_virtual_start
    push kernel_virtual_end
    push kernel_physical_start
    push kernel_physical_end

    jmp kmain

halt:
    hlt
    jmp halt

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

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt - 1
    dd gdt

align 0x1000
pml4_table:
    dq pdpt_table + 0x03
    times 511 dq 0

align 0x1000
pdpt_table:
    dq pd_table + 0x03
    times 511 dq 0

%define ps_flag 0x03
%define page_increment 0x1000

align 0x1000
pd_table:
    dq pt_table + ps_flag
    times 511 dq 0

align 0x1000
pt_table:
    %assign addr 0x00000000
    %rep 512
        dq addr + ps_flag
        %assign addr addr + page_increment
    %endrep
