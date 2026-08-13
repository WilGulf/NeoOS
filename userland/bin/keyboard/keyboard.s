section .asm

global _start
_start:
    call getkey
    push message
    mov eax, 1
    int 0x80
    add esp, 4

    jmp _start

getkey:
    mov eax, 2
    int 0x80
    cmp eax, 0x00
    je getkey
    ret

section .data
message: db 'Hello from a binary program!', 0x0A, 0