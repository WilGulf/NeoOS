#ifndef PATH_PARSER_H
#define PATH_PARSER_H

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

#endif