bits 64

section .text

global exit:function
exit:
    mov rax, 0
    int 0x80
    ud2

global promise:function
promise:
    push qword rdi
    mov rax, 1
    int 0x80
    add rsp, 8
    ret

global drop_privilege:function
drop_privilege:
    push qword rdi
    mov rax, 2
    int 0x80
    add rsp, 8
    ret

global get_kernel:function
get_kernel:
    push qword rdi
    mov rax, 3
    int 0x80
    add rsp, 8
    ret

global print:function
print:
    push qword rdi
    mov rax, 4
    int 0x80
    add rsp, 8
    ret

global putchar:function
putchar:
    push qword rdi
    mov rax, 5
    int 0x80
    add rsp, 8
    ret

global clear:function
clear:
    mov rax, 6
    int 0x80
    ret


global getkey:function
getkey:
    mov rax, 7
    int 0x80
    ret
    
global getkey_event:function
getkey_event:
    push qword rdi
    mov rax, 8
    int 0x80
    add rsp, 8
    ret


global kmalloc:function
kmalloc:
    push qword rdi
    mov rax, 9
    int 0x80
    add rsp, 8
    ret

global kfree:function
kfree:
    push qword rdi
    mov rax, 10
    int 0x80
    add rsp, 8
    ret

global process_get_arguments:function
process_get_arguments:
    push qword rdi
    mov rax, 11
    int 0x80
    add rsp, 8
    ret

global system_run:function
system_run:
    push qword rdi
    mov rax, 12
    int 0x80
    add rsp, 8
    ret

global system_run_as:function
system_run_as:
    push qword rsi
    push qword rdi
    mov rax, 13
    int 0x80
    add rsp, 16
    ret

global fork_run:function
fork_run:
    push qword rdi
    mov rax, 14
    int 0x80
    add rsp, 8
    ret

global fork_run_as:function
fork_run_as:
    push qword rsi
    push qword rdi
    mov rax, 15
    int 0x80
    add rsp, 16
    ret

global get_processes:function
get_processes:
    push qword rsi
    push qword rdi
    mov rax, 16
    int 0x80
    add rsp, 16
    ret

global kill:function
kill:
    push qword rdi
    mov rax, 17
    int 0x80
    add rsp, 8
    ret

global fopen:function
fopen:
    push qword rsi
    push qword rdi
    mov rax, 18
    int 0x80
    add rsp, 16
    ret

global fclose:function
fclose:
    push qword rdi
    mov rax, 19
    int 0x80
    add rsp, 8
    ret

global fread:function
fread:
    push qword rcx
    push qword rdx
    push qword rsi
    push qword rdi
    mov rax, 20
    int 0x80
    add rsp, 32
    ret

global fstat:function
fstat:
    push qword rsi
    push qword rdi
    mov rax, 21
    int 0x80
    add rsp, 16
    ret

global fseek:function
fseek:
    push qword rdx
    push qword rsi
    push qword rdi
    mov rax, 22
    int 0x80
    add rsp, 24
    ret

global get_ticks_ms:function
get_ticks_ms:
    mov rax, 23
    int 0x80
    ret

global remove:function
remove:
    push qword rdi
    mov rax, 26
    int 0x80
    add rsp, 8
    ret

global fwrite:function
fwrite:
    push qword rcx
    push qword rdx
    push qword rsi
    push qword rdi
    mov rax, 27
    int 0x80
    add rsp, 32
    ret

global sys_readdir:function
sys_readdir:
    push qword rsi
    push qword rdi
    mov rax, 28
    int 0x80
    add rsp, 16
    ret

global chwd:function
chwd:
    push qword rdi
    mov rax, 29
    int 0x80
    add rsp, 8
    ret

global get_cwd:function
get_cwd:
    push qword rsi
    push qword rdi
    mov rax, 30
    int 0x80
    add rsp, 16
    ret

global create:function
create:
    push qword rdi
    mov rax, 31
    int 0x80
    add rsp, 8
    ret

global mkdir:function
mkdir:
    push qword rdi
    mov rax, 32
    int 0x80
    add rsp, 8
    ret

global mmap:function
mmap:
    push qword rdx
    push qword rsi
    push qword rdi
    mov rax, 33
    add rsp, 24
    ret

global munmap:function
munmap:
    push qword rsi
    push qword rdi
    mov rax, 34
    add rsp, 16
    ret