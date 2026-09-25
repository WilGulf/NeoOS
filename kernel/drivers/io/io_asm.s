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
    push ebp
    mov ebp, esp

    xor eax, eax
    mov edx, [ebp+8]
    in al, dx

    pop ebp
    ret

inw:
    push ebp
    mov ebp, esp

    xor eax, eax
    mov edx, [ebp+8]
    in ax, dx

    pop ebp
    ret