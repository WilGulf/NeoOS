ORG 0x7C00
BITS 16

%ifndef KERNEL_SECTORS
%define KERNEL_SECTORS 256          ; the Makefile overrides this
%endif

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    in  al, 0x92                    ; enable A20
    or  al, 2
    out 0x92, al

    lgdt [gdt_descriptor]
    mov eax, cr0
    or  eax, 1
    mov cr0, eax
    jmp CODE_SEG:load32

BITS 32
load32:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    mov eax, 1                      ; LBA 1 = sector after the boot sector
    mov ecx, KERNEL_SECTORS
    mov edi, 0x100000
    call ata_lba_read
    jmp CODE_SEG:0x100000           ; -> loader

; eax = start LBA, ecx = sector count, edi = destination
ata_lba_read:
.next:
    push eax
    push ecx
    mov  ebx, eax

    mov  dx, 0x1F6
    shr  eax, 24
    or   al, 0xE0                   ; master, LBA mode
    out  dx, al

    mov  dx, 0x1F2
    mov  al, 1
    out  dx, al

    mov  eax, ebx
    mov  dx, 0x1F3
    out  dx, al
    shr  eax, 8
    mov  dx, 0x1F4
    out  dx, al
    shr  eax, 8
    mov  dx, 0x1F5
    out  dx, al

    mov  dx, 0x1F7
    mov  al, 0x20                   ; READ SECTORS
    out  dx, al
.wait:
    in   al, dx
    and  al, 0x88                   ; BSY clear, DRQ set
    cmp  al, 0x08
    jne  .wait

    mov  ecx, 256
    mov  dx, 0x1F0
    rep  insw

    pop  ecx
    pop  eax
    inc  eax
    loop .next
    ret

gdt_start:
    dq 0
gdt_code:
    dw 0xFFFF, 0
    db 0, 0x9A, 0xCF, 0
gdt_data:
    dw 0xFFFF, 0
    db 0, 0x92, 0xCF, 0
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 510 - ($ - $$) db 0
dw 0xAA55