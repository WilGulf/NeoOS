;
;
; Copyright (C) 2026 Daniel McCarthy <daniel@dragonzap.com>
; This file is drerived from the PeachOS Kernel (github.com/nibblebits/PeachOS).
;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; version 2 as published by the Free Software Foundation.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
; See the GNU General Public License version 2 for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, see <https://www.gnu.org/licenses/>.
; 
; See NOTICE for the full list of GPL-derived files in this project.
;

bits 64

global task_return
task_return:
    push qword[rdi+88]
    push qword[rdi+80]
    mov rax, [rdi+72]
    or rax, 0x200
    push rax

    push qword 0x2B
    push qword[rdi+56]

    call restore_general_purpose_registers
    
    iretq

global restore_general_purpose_registers
restore_general_purpose_registers:
    mov rsi, [rdi+8]
    mov rbp, [rdi+16]
    mov rbx, [rdi+24]
    mov rdx, [rdi+32]
    mov rcx, [rdi+40]
    mov rax, [rdi+48]

    mov rdi, [rdi]
    ret

global user_registers
user_registers:
    mov ax, 0x2B
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ret