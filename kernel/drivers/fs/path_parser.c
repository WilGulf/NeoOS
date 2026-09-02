#include "path_parser.h"

#include "../../memory/kheap.h"
#include "../../include/status.h"
#include "../../include/util.h"
#include "../../include/config.h"

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
    if (!first_part) {
        res = -ERROR_IO;
        goto out;
    }

    path_root->first = first_part;

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