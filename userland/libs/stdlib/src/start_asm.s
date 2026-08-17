extern c_start

section .text

global _start
_start:
    call c_start
    ret

global wait_for_promise
wait_for_promise:
    push ebp
    mov ebp, esp
    mov eax, 11
    int 0x80
    pop ebp
    ret