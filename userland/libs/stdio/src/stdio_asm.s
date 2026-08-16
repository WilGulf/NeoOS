section .asm

global print:function
print:
    push ebp
    mov ebp, esp
    push dword[ebp+8]
    mov eax, 1
    int 0x80
    add esp, 4
    pop ebp
    ret

global getkey:function
getkey:
    push ebp
    mov ebp, esp
    mov eax, 2
    int 0x80
    pop ebp
    ret
    
global putchar
putchar:
    push ebp
    mov ebp, esp
    push dword [ebp+8]
    mov eax, 3
    int 0x80
    add esp, 4
    pop ebp
    ret

global clear:function
clear:
    push ebp
    mov ebp, esp
    mov eax, 9
    int 0x80
    pop ebp
    ret