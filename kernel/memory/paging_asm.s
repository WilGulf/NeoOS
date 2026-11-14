bits 64

global paging_invalidate_tlb_entry
paging_invalidate_tlb_entry:
    invlpg [rdi]
    ret

global paging_load_directory
paging_load_directory:
    mov rax, rdi
    mov cr3, rax
    ret