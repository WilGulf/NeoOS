extern no_interrupt_handler
extern isr80h_handler
extern interrupt_handler

global idt_load
idt_load:
    push ebp
    mov ebp, esp
    mov ebx, [ebp+8]
    lidt [ebx]
    pop ebp
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
    pushad
    call no_interrupt_handler
    popad
    iret

%macro interrupt 1
    global int%1
    int%1:
        pushad
        push esp
        push dword %1
        call interrupt_handler
        add esp, 8
        popad
        iret
%endmacro

%assign i 0
%rep 512
    interrupt i
%assign i i+1
%endrep

global isr80h_wrapper
isr80h_wrapper:
    pushad
    push esp
    push eax
    call isr80h_handler
    mov dword [tmp_res], eax
    add esp, 8
    popad
    mov eax, [tmp_res]
    iret

section .data

tmp_res: dd 0

%macro interrupt_array_entry 1
    dd int%1
%endmacro

global interrupt_pointer_table
interrupt_pointer_table:
%assign i 0
%rep 512
    interrupt_array_entry i
%assign i i+1
%endrep