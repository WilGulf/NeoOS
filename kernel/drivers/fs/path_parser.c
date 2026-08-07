#include "path_parser.h"

#include "../../memory/kheap.h"
#include "../../include/status.h"
#include "../../include/util.h"

static int path_is_valid_format(const char *path) {
    int len = strnlen(path, MAX_PATH);
    if (len >= 3 && char_is_digit(path[0]) && memcmp((void *)&path[1], ":/", 2) == 0);
}

static int get_drive_by_path(const char **path) {
    if (!path_is_valid_format(*path)) {
        return -ERROR_BAD_PATH;
    }

    int drive_no = char_to_int(*path[0]);

    *path += 3;
    return drive_no;
}

static struct path_root *create_root(int drive_no) {
    struct path_root *path_root = kzalloc(sizeof(struct path_root));
    path_root->drive_no = drive_no;
    path_root->first = 0;
    return path_root;
}

static const char *get_path_part(const char **path) {
    char *result_path_part = kzalloc(MAX_PATH);
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
    part->part = path_part_str;
    part->next = 0x00;

    if (last_part) {
        last_part->next = part;
    }

    return part;
}

void free(struct path_root *root) {
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
    int res = 0;
    const char *temp_path = path;
    struct path_root *path_root = 0;
    
    if (strlen(path) > MAX_PATH) {
        goto out;
    }

    res = get_drive_by_path(&temp_path);
    if (res < 0) {
        goto out;
    }

    path_root = create_root(res);
    if (!path_root) {
        goto out;
    }

    struct path_part *first_part = parse_path_part(NULL, &temp_path);
    if (!first_part) {
        goto out;
    }

    path_root->first = first_part;

    struct path_part *part = parse_path_part(first_part, &temp_path);
    while (part) {
        part = parse_path_part(part, &temp_path);
    }

out:
    return path_root;
}