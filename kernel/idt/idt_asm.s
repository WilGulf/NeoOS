bits 64

extern no_interrupt_handler
extern isr80h_handler
extern interrupt_handler

tmp_rsp_storage: dq 0x00
%macro pushad_macro 0
    mov qword [tmp_rsp_storage], rsp
    push rax
    push rcx
    push rdx
    push rbx
    push qword[tmp_rsp_storage]
    push rbp
    push rsi
    push rdi
%endmacro

%macro popad_macro 0
    pop rdi
    pop rsi
    pop rbp
    pop qword[tmp_rsp_storage]
    pop rbx
    pop rdx
    pop rcx
    pop rax
    mov rsp, [tmp_rsp_storage]
%endmacro

global idt_load
idt_load:
    mov rbx, rdi
    lidt [rbx]
    ret

global enable_interrupts
enable_interrupts:
    sti
    ret

global disable_interrupts
disable_interrupts:
    cli
    ret

global no_interrupt
no_interrupt:
    pushad_macro
    call no_interrupt_handler
    popad_macro
    iretq

%macro interrupt 1
    global int%1
    int%1:
        pushad_macro
        mov rdi, %1
        mov rsi, rsp
        call interrupt_handler
        popad_macro
        iretq
%endmacro

%assign i 0
%rep 512
    interrupt i
%assign i i+1
%endrep

global isr80h_wrapper
isr80h_wrapper:
    pushad_macro
    mov rsi, rsp
    mov rdi, rax
    call isr80h_handler
    mov qword [tmp_res], rax
    popad_macro
    mov rax, [tmp_res]
    iretq

section .data

tmp_res: dq 0

%macro interrupt_array_entry 1
    dq int%1
%endmacro

global interrupt_pointer_table
interrupt_pointer_table:
%assign i 0
%rep 512
    interrupt_array_entry i
%assign i i+1
%endrep