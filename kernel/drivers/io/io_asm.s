global outb
global outw

global inb
global inw

outb:
    mov al, [esp + 8]
    mov dx, [esp + 4]
    out dx, al
    ret

outw:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12]
    mov edx, [ebp+8]
    out dx, ax

    pop ebp
    ret

inb:
    mov dx, [esp + 4]
    in al, dx
    ret

inw:
    push ebp
    mov ebp, esp

    xor eax, eax
    mov edx, [ebp+8]
    in ax, dx

    pop ebp
    ret