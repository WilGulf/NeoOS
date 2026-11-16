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

struct idt_descriptor {
    uint16_t offset1;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset2;
    uint32_t offset3;
    uint32_t reserved;
} __attribute__((packed));


struct idtr_descriptor {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct interrupt_frame {
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t reserved;
    uint64_t rbx;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rax;
    uint64_t ip;
    uint64_t cs;
    uint64_t flags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed));

void idt_init();

void enable_interrupts();
void disable_interrupts();

void isr80h_register_command(int command_id, ISR80H_COMMAND command);
int idt_register_interrupt_callback(int interrupt, INTERRUPT_CALLBACK_FUNCTION interrupt_callback);

#endif