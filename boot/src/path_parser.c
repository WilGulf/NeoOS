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

#include <efi.h>
#include <efilib.h>
#include "string.h"

#include "path_parser.h"
#include "fs.h"

static struct path_root *create_root() {
    struct path_root *path_root = AllocatePool(sizeof(struct path_root));
    if (!path_root) {
        return NULL;
    }

    path_root->first = NULL;
    return path_root;
}

static const char *get_path_part(const char **path) {
    while (**path == '/') {
        (*path)++;
    }
    
    char *result_path_part = AllocatePool(MAX_PATH + 1);
    if (!result_path_part) {
        return NULL;
    }

    int i = 0;
    while (**path != '/' && **path != 0x00 && i < MAX_PATH) {
        result_path_part[i++] = **path;
        (*path)++;
    }

    result_path_part[i] = '\0';

    if (i == 0) {
        FreePool(result_path_part);
        return NULL;
    }

    return result_path_part;
}

struct path_part *parse_path_part(struct path_part *last_part, const char **path) {
    const char *path_part_str = get_path_part(path);
    if (!path_part_str) {
        return 0;
    }

    struct path_part *part = AllocatePool(sizeof(struct path_part));
    if (!part) {
        FreePool((void *)path_part_str);
        return 0;
    }

    part->part = path_part_str;
    part->next = 0x00;

    if (last_part) {
        last_part->next = part;
    }

    return part;
}

void path_parser_free(struct path_root *root) {
    struct path_part *part = root->first;
    while (part) {
        struct path_part *next_part = part->next;
        FreePool((void *) part->part);
        FreePool(part);
        part = next_part;
    }

    FreePool(root);
}

struct path_root *parse_path(const char *path) {
    struct path_part *first_part = NULL;
    struct path_part *part = NULL;
    
    int res = 0;
    const char *temp_path = path;
    struct path_root *path_root = NULL;
    
    if (strlen(path) > MAX_PATH) {
        goto out;
    }

    path_root = create_root();
    if (!path_root) {
        res = -1;
        goto out;
    }

    first_part = parse_path_part(NULL, &temp_path);
    path_root->first = first_part;

    if (first_part) {
        part = parse_path_part(first_part, &temp_path);
        while (part) {
            part = parse_path_part(part, &temp_path);
        }
    }

out:
    if (res < 0) {
        if (path_root) {
            FreePool(path_root);
            path_root = NULL;
        }
        if (first_part) {
            FreePool(first_part);
            first_part = NULL;
        }
    }
    return path_root;
}