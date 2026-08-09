section .asm

global _start
_start:
    push message
    mov eax, 1
    int 0x80
    add esp, 4

    jmp $

section .data
message: db 'Hello from a binary program!', 0x0A, 0