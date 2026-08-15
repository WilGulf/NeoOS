section .asm

global kmalloc:function
kmalloc:
    push ebp
    mov ebp, esp
    mov eax, 4
    push dword[ebp+8]
    int 0x80
    add esp, 4
    pop ebp
    ret

global kfree:function
kfree:
    push ebp
    mov ebp, esp
    mov eax, 5
    push dword[ebp+8]
    int 0x80
    add esp, 4
    pop ebp
    ret

global exec:function
exec:
    push ebp
    mov ebp, esp
    mov eax, 6
    push dword[ebp+8]
    int 0x80
    add esp, 4
    pop ebp
    ret
    
