section .text

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

global system_run:function
system_run:
    push ebp
    mov ebp, esp
    mov eax, 7
    push dword[ebp+8]
    int 0x80
    add esp, 4
    pop ebp
    ret

global system_run_as:function
system_run_as:
    push ebp
    mov ebp, esp
    mov eax, 11
    push dword[ebp+12]
    push dword[ebp+8]
    int 0x80
    add esp, 8
    pop ebp
    ret

global fork_run:function
fork_run:
    push ebp
    mov ebp, esp
    mov eax, 12
    push dword[ebp+8]
    int 0x80
    add esp, 4
    pop ebp
    ret

global fork_run_as:function
fork_run_as:
    push ebp
    mov ebp, esp
    mov eax, 13
    push dword[ebp+12]
    push dword[ebp+8]
    int 0x80
    add esp, 8
    pop ebp
    ret
    
global process_get_arguments:function
process_get_arguments:
    push ebp
    mov ebp, esp
    mov eax, 8
    push dword[ebp+8]
    int 0x80
    add esp, 4
    pop ebp
    ret

global exit:function
exit:
    push ebp
    mov ebp, esp
    mov eax, 0
    int 0x80
    pop ebp
    ret

global promise:function
promise:
    push ebp
    mov ebp, esp
    mov eax, 10
    push dword[ebp+8]
    int 0x80
    add esp, 4
    pop ebp
    ret

global drop_privilege:function
drop_privilege:
    push ebp
    mov ebp, esp
    mov eax, 14
    push dword[ebp+8]
    int 0x80
    add esp, 4
    pop ebp
    ret

global get_kernel:function
get_kernel:
    push ebp
    mov ebp, esp
    mov eax, 15
    push dword[ebp+8]
    int 0x80
    add esp, 4
    pop ebp
    ret