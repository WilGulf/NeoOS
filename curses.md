[< BACK](README.md)

# Curses

Curses is the implementation of a ncurses style library available in NeoOS programs.

## How it works

### Initializing

The initiasation of curses is not neccessary but it is recommended for it to work as expected.
Initiasation esentially cleans everything up and prepares the terminal interface to be written to.


**inits()**: Initializes the screen.

**exits()**: Cleans up after the program and returns to normal writing mode like printf and putchar.

## Virtual buffers

Every program gets up to 4 different virtual buffers. The virtual buffers are like a terminal the program can write to.
This allows the program to not need to rewrite to the screen to switch to a new view.
The virtual buffers are modified using the different vb commands:

### Creating and deleting

Creating a virtual buffer is dont using the **vbinit()** function. That returns the id of the buffer as a int that will be used for modifying it. When a buffer is no longer needed it is recommended to close it to make it's slot available for a new buffer, that is done with the **vbclose(int vb)** function.

### Printing

**printvb(int vb, const char *fmt, ...)**: The default C print that instead prints to the specified buffer.

**putvb(int vb, char c)**: Putchar for the specified buffer.

**vbmove(int vb, int x, int y)**: Moves where the next print will happen in the frame buffer.

### Coloring

Curses supports printing with 8 different colors which are set before the printing.
When printing to a buffer curses knows which colors the buffer is currently using and prints with it.
Changing the color is done with the color commands:

**vbsetcolor(int vb, int color)**: Sets the buffers color to specified one.

**vbsetbg(int vb, int color)**: Sets the background color.

**vbgetcolor(int vb, bool bg)**: Returns the current color used in the vbuffer. If bg is true it returns the background color instead of the foreground.

### Other

**vbscroll()**: Scrolls all the lines in the buffer by one line.

**vbclear()**: Clears the buffer and moves the position to the top-left.

**vbswitch()**: Switches which buffer is in the front and then displayed when refresh is called.

**refresh()**: Refreshes the screen.

**waitms(int ms)**: Waits the specified amount of milliseconds.