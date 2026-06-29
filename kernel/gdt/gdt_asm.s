global load_gdt

load_gdt:
    mov eax, [esp + 4]
    lgdt [eax]

    mov eax, 0x10
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax

    jmp 0x08:flush_cs

flush_cs:
    ret
