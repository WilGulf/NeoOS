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

#ifndef PS2_KEYBOARD_H
#define PS2_KEYBOARD_H

#define PS2_PORT 0x64
#define PS2_COMMAND_ENABLE_FIRST_PORT 0xAE

#define PS2_KEYBOARD_KEY_RELEASED 0x80
#define ISR_KEYBOARD_INTERRUPT 0x21
#define KEYBOARD_INPUT_PORT 0x60

struct keyboard *ps2_init();

#endif