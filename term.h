#ifndef GALAGA_TERM_H
#define GALAGA_TERM_H

enum color
{
    BLACK   = 40,
    RED     = 41,
    GREEN   = 42,
    YELLOW  = 43,
    BLUE    = 44,
    MAGENTA = 45,
    CYAN    = 46,
    WHITE   = 47,
};

void disable_raw_mode();
void enable_raw_mode();
int  screen_size(int* x, int* y);
// 0-based column and row
void set_cursor(int x, int y);
void draw_color(enum color c);
void reset_color();
void clear_screen();

#endif
