#ifndef CURSES_H
#define CURSES_H

#define COLOR_BLACK 30
#define COLOR_RED 31
#define COLOR_GREEN 32
#define COLOR_YELLOW 33
#define COLOR_BLUE 34
#define COLOR_MAGENTA 35
#define COLOR_CYAN 36
#define COLOR_WHITE 37
#define COLOR_RESET 0

int vbinit(void);
int vbclose(int vbuffer);

void inits(void);
int exits(void);

int refresh();
int vbswitch(int vbuffer);

void vbscroll(int vbuffer);
int printvb(int vbuffer, char *fmt, ...);
int putvb(int vbuffer, char c);
void vbclear(int vbuffer);

int vbsetcolor(int vbuffer, int color);
int vbsetbg(int vbuffer, int color) ;
int vbgetcolor(int vbuffer, bool bg);

int vbmove(int vbuffer, int x, int y);

int waitms(int ms);

#endif