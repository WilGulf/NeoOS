#include "stdlib.h"
#include "stdio.h"
#include "curses.h"
#include "promise.h"
#include "string.h"
#include "stdint.h"

#define MAX_ROWS 32
#define MAX_COLS 64

void declarations(void) {
    promise(PROMISE_FB | PROMISE_INPUT | PROMISE_FS);
}

typedef struct {
    char c;
    uint8_t color;
} Cell;

Cell logo[MAX_ROWS][MAX_COLS];
int longest_line = 0;
int lines = 0;

#define LOGO_LINES 8

void load_logo(void) {
    const char *logo_data[LOGO_LINES] = {
        "M    M              dMMb  dMMM",
        "MA   M              M  M  M",
        "MAl  M              M  M  M",
        "M A  M  dMMb  dMMb  M  M  M",
        "M ll M  M  M  M  M  M  M  TMMb",
        "M  A M  MMMP  M  M  M  M     M",
        "M  lLM  M     M  M  M  M     M",
        "M.  VM  ?MMP  ?MMP  ?MMP  ?MMP"
    };

    lines = LOGO_LINES;
    longest_line = 0;

    for (int i = 0; i < lines; i++) {
        int j = 0;

        while (logo_data[i][j] && j < MAX_COLS - 1) {
            logo[i][j].c = logo_data[i][j];
            logo[i][j].color = COLOR_MAGENTA;
            j++;
        }

        logo[i][j].c = '\0';
        logo[i][j].color = 0;

        if (j > longest_line) {
            longest_line = j;
        }
    }
}

int main(int argc, char **argv) {
    inits();
    int id = vbinit();

    load_logo();

    int line_to_update = 0;
    while (1) {
        vbclear(id);
        char c = getkey();
        if (c == 'q' || c == 'Q') {
            exits();
            exit();
        }

        for (int i = 0; i < lines;) {      
            for (int times = 0; times < 3; times++) {
                printvb(id, " ");
            }

            if (i < lines) {
                int j = 0;

                while (logo[i][j].c) {
                    if (i != line_to_update) {
                        vbsetcolor(id, COLOR_MAGENTA);
                    } else {
                        vbsetcolor(id, COLOR_BLACK);
                    }
                    
                    vbsetbg(id, COLOR_BLACK);

                    printvb(id, "%c", logo[i][j].c);

                    j++;
                }
            }

            printvb(id, "\n");

            i++;
        }

        waitms(50);

        refresh();
        line_to_update++;
        if (line_to_update > lines) {
            line_to_update = 0;
        }
    }
    
    exits();
    exit();
}