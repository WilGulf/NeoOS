global load_idt
load_idt:
    mov eax, [esp + 4]
    lidt [eax]
    ret

%macro no_error_code_interrupt_handler 1
    global interrupt_handler_%1
    interrupt_handler_%1:
        cli
        push dword 0
        push dword %1
        jmp common_interrupt_handler
%endmacro

%macro error_code_interrupt_handler 1
    global interrupt_handler_%1
    interrupt_handler_%1:
        cli
        push dword %1
        jmp common_interrupt_handler
%endmacro

%macro irq_interrupt 2
    global irq%1
    irq%1:
        cli
        push dword 0
        push dword %2
        jmp irq_interrupt_handler
%endmacro

no_error_code_interrupt_handler 0
no_error_code_interrupt_handler 1
no_error_code_interrupt_handler 2
no_error_code_interrupt_handler 3
no_error_code_interrupt_handler 4
no_error_code_interrupt_handler 5
no_error_code_interrupt_handler 6
no_error_code_interrupt_handler 7

error_code_interrupt_handler 8
error_code_interrupt_handler 9
error_code_interrupt_handler 10
error_code_interrupt_handler 11
error_code_interrupt_handler 12
error_code_interrupt_handler 13
error_code_interrupt_handler 14

no_error_code_interrupt_handler 15
no_error_code_interrupt_handler 16
no_error_code_interrupt_handler 17
no_error_code_interrupt_handler 18
no_error_code_interrupt_handler 19
no_error_code_interrupt_handler 20
no_error_code_interrupt_handler 21
no_error_code_interrupt_handler 22
no_error_code_interrupt_handler 23
no_error_code_interrupt_handler 24
no_error_code_interrupt_handler 25
no_error_code_interrupt_handler 26
no_error_code_interrupt_handler 27
no_error_code_interrupt_handler 28
no_error_code_interrupt_handler 29
no_error_code_interrupt_handler 30
no_error_code_interrupt_handler 31

no_error_code_interrupt_handler 128
no_error_code_interrupt_handler 177

irq_interrupt 0, 32
irq_interrupt   1,    33
irq_interrupt   2,    34
irq_interrupt   3,    35
irq_interrupt   4,    36
irq_interrupt   5,    37
irq_interrupt   6,    38
irq_interrupt   7,    39
irq_interrupt   8,    40
irq_interrupt   9,    41
irq_interrupt  10,    42
irq_interrupt  11,    43
irq_interrupt  12,    44
irq_interrupt  13,    45
irq_interrupt  14,    46
irq_interrupt  15,    47

extern interrupt_handler
common_interrupt_handler:
    pusha
    mov eax, ds
    push eax
    mov eax, cr2
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call interrupt_handler

    add esp, 8
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa
    add esp, 8
    iret

extern irq_handler
irq_interrupt_handler:
    pusha
    mov eax,ds
    push eax
    mov eax, cr2
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call irq_handler

    add esp, 8
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa
    add esp, 8
    iret