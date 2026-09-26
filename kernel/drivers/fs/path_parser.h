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

#include "../../include/stdint.h"

#ifndef PATH_PARSER_H
#define PATH_PARSER_H

#define PATH_MAX_DEPTH 16
#define PATH_SEGMENT_MAX 32

struct path_root {
    char drive_id;
    struct path_part *first;
};

struct path_part {
    const char *part;
    struct path_part *next;
};

struct path_root *parse_path(const char *path, const char *current_directory_path);
void path_parser_free(struct path_root *root);

int path_is_absolute(const char *path);
int get_full_path(const char *cwd, const char *in, char *out, size_t out_size);

#endif