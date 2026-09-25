/*
 *
 * Copyright (C) 2026 Daniel McCarthy <daniel@dragonzap.com>
  * This file is drerived from the PeachOS Kernel (github.com/nibblebits/PeachOS).
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 * 
 * See NOTICE for the full list of GPL-derived files in this project.
 */

#ifndef IDT_H
#define IDT_H

#include "../include/stdint.h"

#define TOTAL_INTERRUPTS 512
#define MAX_ISR80H_COMMANDS 1024

struct interrupt_frame;
typedef void *(*ISR80H_COMMAND)(struct interrupt_frame *frame);
typedef void (*INTERRUPT_CALLBACK_FUNCTION)();

struct idt_desc {
    uint16_t offset_lower;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_upper;
} __attribute__((packed));


struct idtr_desc {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct interrupt_frame {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
} __attribute__((packed));

void idt_init();

void enable_interrupts();
void disable_interrupts();

void isr80h_register_command(int command_id, ISR80H_COMMAND command);
int idt_register_interrupt_callback(int interrupt, INTERRUPT_CALLBACK_FUNCTION interrupt_callback);

#endif