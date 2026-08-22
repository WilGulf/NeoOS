section .text

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

global fopen:function
fopen:
    push ebp
    mov ebp, esp
    mov eax, 16
    push dword[ebp+12]
    push dword[ebp+8]
    int 0x80
    add esp, 8
    pop ebp
    ret

global fclose:function
fclose:
    push ebp
    mov ebp, esp
    mov eax, 17
    push dword[ebp+8]
    int 0x80
    add esp, 4
    pop ebp
    ret

global fread:function
fread:
    push ebp
    mov ebp, esp
    mov eax, 18
    push dword[ebp+20]
    push dword[ebp+16]
    push dword[ebp+12]
    push dword[ebp+8]
    int 0x80
    add esp, 16
    pop ebp
    ret

global fstat:function
fstat:
    push ebp
    mov ebp, esp
    mov eax, 19
    push dword[ebp+12]
    push dword[ebp+8]
    int 0x80
    add esp, 8
    pop ebp
    ret

global get_processes:function
get_processes:
    push ebp
    mov ebp, esp
    mov eax, 21
    push dword[ebp+12]
    push dword[ebp+8]
    int 0x80
    add esp, 8
    pop ebp
    ret

global kill:function
kill:
    push ebp
    mov ebp, esp
    mov eax, 22
    push dword[ebp+8]
    int 0x80
    add esp, 4
    pop ebp
    ret
