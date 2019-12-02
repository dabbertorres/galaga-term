#include "term.h"

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>

extern void die(const char* s);

static struct termios prev_term_state;

void disable_raw_mode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &prev_term_state) == -1) die("restore tcsetattr");
}

void enable_raw_mode()
{
    if (tcgetattr(STDIN_FILENO, &prev_term_state) == -1) die("tcgetattr");
    atexit(disable_raw_mode);

    // raw mode!
    struct termios raw = prev_term_state;
    // http://man7.org/linux/man-pages/man3/termios.3.html#DESCRIPTION -> "Raw mode"
    raw.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | INPCK | IXON);
    raw.c_oflag &= ~OPOST;
    raw.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    raw.c_cflag &= ~(CSIZE | PARENB);
    raw.c_cflag |= CS8;
    raw.c_cc[VMIN]  = 0; // make read() return ASAP
    raw.c_cc[VTIME] = 1; // make read() timeout after 100 milliseconds

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int screen_size(int* x, int* y)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) return -1;

    *x = ws.ws_col;
    *y = ws.ws_row;
    return 0;
}

// 0-based column and row
void set_cursor(int x, int y)
{
    printf("\033[%d;%df", y + 1, x + 1);
}

void draw_color(enum color c)
{
    printf("\033[39;%dm ", c);
}

void reset_color()
{
    printf("\033[0m");
}

void clear_screen()
{
    printf("\033[2J");
}
