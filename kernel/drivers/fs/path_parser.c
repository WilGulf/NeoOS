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

#include "path_parser.h"

#include "../../memory/kheap.h"
#include "../../include/status.h"
#include "../../include/util.h"
#include "../../include/config.h"
#include "../../include/stdint.h"

#include "../../drivers/io/io.h"

static int path_is_valid_format(const char *path) {
    int len = strnlen(path, MAX_PATH);
    return len >= 3 && (char_is_digit(path[0]) || path[0] == 'D') && memcmp((void *)&path[1], ":/", 2) == 0;
}

static char get_drive_by_path(const char **path) {
    if (!path_is_valid_format(*path)) {
        return -ERROR_BAD_PATH;
    }

    char drive_id = *path[0];

    *path += 3;
    return drive_id;
}

static struct path_root *create_root(int drive_no) {
    struct path_root *path_root = kzalloc(sizeof(struct path_root));
    if (!path_root) {
        return NULL;
    }

    path_root->drive_id = drive_no;
    path_root->first = 0;
    return path_root;
}

static const char *get_path_part(const char **path) {
    char *result_path_part = kzalloc(MAX_PATH);
    if (!result_path_part) {
        return NULL;
    }
    int i = 0;
    while (**path != '/' && **path != 0x00) {
        result_path_part[i] = **path;
        *path += 1;
        i++;
    }

    if (**path == '/') {
        *path += 1;
    }

    if (i == 0) {
        kfree(result_path_part);
        result_path_part = 0;
    }

    return result_path_part;
}

struct path_part *parse_path_part(struct path_part *last_part, const char **path) {
    const char *path_part_str = get_path_part(path);
    if (!path_part_str) {
        return 0;
    }

    struct path_part *part = kzalloc(sizeof(struct path_part));
    if (!part) {
        kfree((void *)path_part_str);
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
        kfree((void *) part->part);
        kfree(part);
        part = next_part;
    }

    kfree(root);
}

struct path_root *parse_path(const char *path, const char *current_directory_path) {
    struct path_part *first_part = NULL;
    struct path_part *part = NULL;
    
    int res = 0;
    const char *temp_path = path;
    struct path_root *path_root = NULL;
    
    if (strlen(path) > MAX_PATH) {
        goto out;
    }

    res = get_drive_by_path(&temp_path);
    if (res < 0) {
        res = -ERROR_IO;
        goto out;
    }

    path_root = create_root(res);
    if (!path_root) {
        res = -ERROR_IO;
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

    part = parse_path_part(first_part, &temp_path);
    while (part) {
        part = parse_path_part(part, &temp_path);
    }

out:
    if (res < 0) {
        if (path_root) {
            kfree(path_root);
            path_root = NULL;
        }
        if (first_part) {
            kfree(first_part);
            first_part = NULL;
        }
    }
    return path_root;
}

int path_is_absolute(const char *path) {
    return path_is_valid_format(path);
}

int get_full_path(const char *cwd, const char *in, char *out, size_t out_size) {
    size_t buffer_sz = MAX_PATH * 2;
    char buffer[buffer_sz];

    if (!cwd || !cwd[0]) {
        return -ERROR_IO;
    }

    if (!out_size) {
        return -ERROR_IO;
    }

    if (!in || in[0] == '\0') {
        in = ".";
    }

    if (path_is_valid_format(in)) {
        strncpy(buffer, in, buffer_sz);
        buffer[buffer_sz - 1] = '\0';
    } else {
        strncpy(buffer, cwd, buffer_sz);
        buffer[buffer_sz - 1] = '\0';
        size_t len = strlen(buffer);
        if (buffer[len - 1] != '/' && in[0] != '/') {
            buffer[len] = '/';
            strncpy(buffer + len + 1, in, buffer_sz - len - 1);
        } else if (buffer[len - 1] == '/' && in[0] == '/') {
            buffer[len--] = '\0';
            strncpy(buffer + len, in, buffer_sz - len - 1);
        } else {
            strncpy(buffer + len, in, buffer_sz - len - 1);
        }

        buffer[buffer_sz - 1] = '\0';
    }

    char *buffer_ptr = buffer;
    char drive = *buffer_ptr;
    buffer_ptr += 3;

    char part[PATH_SEGMENT_MAX];
    size_t part_len = 0;

    char parts[PATH_MAX_DEPTH][PATH_SEGMENT_MAX];
    size_t depth = 0;
    while (1) {
        char c = *buffer_ptr;

        if (c != '/' && c != '\0') {
            if (part_len < PATH_SEGMENT_MAX - 1) {
                part[part_len++] = c;
            } else {
                break;
            }
        } else {
            part[part_len] = '\0';

            // Process part
            if (!strcmp(part, ".")) {
                
            } else if (!strcmp(part, "..")) {
                if (depth > 0) {
                    depth--;
                }
            } else if (!part[0]) {
                
            } else {
                if (depth == PATH_MAX_DEPTH) {
                    break;
                }
                strncpy(parts[depth++], part, PATH_SEGMENT_MAX);
            }

            part_len = 0;
        }

        if (c == '\0') {
            break;
        }

        buffer_ptr++;
    }

    char output[sizeof(parts)];
    output[0] = drive;
    output[1] = ':';
    output[2] = '/';
    output[3] = '\0';

    for (int i = 0; i < depth; i++) {
        size_t len = strlen(output);
        if (i != 0) {
            output[len] = '/';
            strcpy(output + len + 1, parts[i]);
        } else {
            strcpy(output + len, parts[i]);
        }
    }
    output[sizeof(output) - 1] = '\0';

    strncpy(out, output, out_size);
    out[out_size - 1] = '\0';

    return 0;
}