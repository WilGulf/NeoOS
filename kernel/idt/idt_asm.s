extern int21h_handler
extern no_interrupt_handler
extern isr80h_handler

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

global int21h
int21h:
    pushad
    call int21h_handler
    popad
    iret

global no_interrupt
no_interrupt:
    pushad
    call no_interrupt_handler
    popad
    iret

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
    iretd

section .data
tmp_res: dd 0