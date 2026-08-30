#include "curses.h"
#include "format.h"

#include "stdlib.h"
#include "stdio.h"
#include "memory.h"
#include "stdbool.h"

#include <stdarg.h>

#define MAX_BUFFERS 4
#define ROWS 24
#define COLUMNS 80
#define MAX (ROWS * COLUMNS)

struct cell {
    char c;
    int color;
};

int front = 0;
struct cell *vbuffers[MAX_BUFFERS];
int vbuffer_positions[MAX_BUFFERS];
int vbuffer_current_colors[MAX_BUFFERS];

void vbuffer_free(int vbuffer) {
    free(vbuffers[vbuffer]);
    vbuffers[vbuffer] = NULL;
    vbuffer_positions[vbuffer] = 0;
}

void vbclear(int vbuffer) {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLUMNS; x++) {
            vbuffers[vbuffer][(y * COLUMNS) + x].c = ' ';
            vbuffers[vbuffer][(y * COLUMNS) + x].color = 0;
        }
    }
    
    vbuffer_positions[vbuffer] = 0;
}

void inits(void) {
    set_fb_cursor(false);

    for (int i = 0; i < MAX_BUFFERS; i++) {
        vbuffers[i] = NULL;
        vbuffer_positions[i] = 0;
    }
    
    clear();    
}

int vbinit(void) {
    int vbuffer = -1;
    for (int i = 0; i < MAX_BUFFERS; i++) {
        if (vbuffers[i]) {
            continue;
        } else {
            vbuffer = i;
            break;
        }
    }

    if (vbuffer < 0) {
        goto out;
    }

    vbuffers[vbuffer] = malloc(sizeof(struct cell) * ROWS * COLUMNS);
    if (!vbuffers[vbuffer]) {
        vbuffer = -1;
        goto out;
    }

    vbclear(vbuffer);

out:
    return vbuffer;
}

int vbclose(int vbuffer) {
    if (!vbuffers[vbuffer]) {
        return 0;
    }

    vbuffer_free(vbuffer);

    for (int i = 0; i < MAX_BUFFERS; i++) {
        if (vbuffers[i]) {
            front = i;
            break;
        }
    }

    if (front == vbuffer) {
        front = -1;
    }
    
    return 0;
}

int exits(void) {
    for (int i = 0; i < MAX_BUFFERS; i++) {
        if (vbuffers[i]) {
            vbuffer_free(i);
        }
    }

    set_fb_cursor(true);

    return 0;
}

int refresh() {
    if (front < 0) {
        return 0;
    }

    clear();

    int last_color = -1;

    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLUMNS; x++) {
            int pos = (y * COLUMNS) + x;
            int color = vbuffers[front][pos].color;

            if (color != last_color) {
                printf("\033[%dm", color);
                last_color = color;
            }
            
            printf("%c", vbuffers[front][pos].c);
        }
    }

    return 0;
}

int waitms(int ms) {
    if (ms <= 0) {
        return 0;
    }

    uint32_t start = get_ticks_ms();

    while ((uint32_t)(get_ticks_ms() - start) < (uint32_t)ms) {
        // wait
    }

    return 0;
}

int vbsetcolor(int vbuffer, int color) {
    if (color >= 30 && color <= 37) {
        vbuffer_current_colors[vbuffer] = color;
    }
    return 0;
}

int vbgetcolor(int vbuffer) {
    return vbuffer_current_colors[vbuffer];
}

int vbswitch(int vbuffer) {
    if (vbuffers[vbuffer]) {
        front = vbuffer;
    }
}

void vbscroll(int vbuffer) {
    if (!vbuffers[vbuffer]) {
        return;
    }

    for (int y = 1; y < ROWS; y++) {
        for (int x = 0; x < COLUMNS; x++) {
            int pos = (y * COLUMNS) + x;
            vbuffers[vbuffer][pos - COLUMNS] = vbuffers[vbuffer][pos];
        }
    }

    vbuffer_positions[vbuffer] = (ROWS - 1) * COLUMNS;
}

int printvb(int vbuffer, char *fmt, ...) {
    if (!vbuffers[vbuffer]) {
        return 0;
    }

    va_list args;
    va_start(args, fmt);
    char *str = format(fmt, args);
    char *p = str;
    va_end(args);

    while (*p) {
        if (*p == '\n') {
            int y = vbuffer_positions[vbuffer] / COLUMNS;
            vbuffer_positions[vbuffer] = (y * COLUMNS) + COLUMNS;
        } else {
            vbuffers[vbuffer][vbuffer_positions[vbuffer]].c = *p;
            vbuffers[vbuffer][vbuffer_positions[vbuffer]].color = vbuffer_current_colors[vbuffer];
            vbuffer_positions[vbuffer]++;
        }

        if (vbuffer_positions[vbuffer] >= MAX) {
            vbscroll(vbuffer);
        }

        p++;
    }

    return 0;
}