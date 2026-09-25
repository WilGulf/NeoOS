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

global task_return
task_return:
    mov ebp, esp
    mov ebx, [ebp+4]
    push dword [ebx+44]
    push dword [ebx+40]

    mov eax, [ebx+36]
    or eax, 0x200
    push eax

    push dword [ebx+32]
    push dword [ebx+28]

    mov ax, [ebx+44]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword [ebp+4]
    call restore_general_purpose_registers
    add esp, 4

    iretd

global restore_general_purpose_registers
restore_general_purpose_registers:
    push ebp
    mov ebp, esp
    mov ebx, [ebp+8]
    mov edi, [ebx]
    mov esi, [ebx+4]
    mov ebp, [ebx+8]
    mov edx, [ebx+16]
    mov ecx, [ebx+20]
    mov eax, [ebx+24]
    mov ebx, [ebx+12]
    add esp, 4
    ret

global user_registers
user_registers:
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ret