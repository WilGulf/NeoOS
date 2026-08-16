extern c_start

section .asm

global _start
_start:
    call c_start
    ret