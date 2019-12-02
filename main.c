#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include "term.h"

extern void die(const char* s);

extern void open_log(const char* file);
extern void write_log(const char* fmt, ...);

void draw_ship(int x, int y);

struct bullet
{
    int x;
    int y;
    int active;
};

void draw_bullet(struct bullet* bullet);

int main(int argc, char** argv)
{
    enable_raw_mode();

    open_log("debug.log");

    int width, height;
    if (screen_size(&width, &height) != 0) die("screen_size()");

    // 3 bullet limit
    int           next_bullet = 0;
    struct bullet bullets[3]  = {0};
    const int     bullets_len = sizeof(bullets) / sizeof(bullets[0]);
    // TODO bullet timing

    const int bullet_update_rate  = 2;
    int       bullet_update_count = 0;

    int cursor_x = width / 2, cursor_y = height / 2;

    clear_screen();
    draw_ship(cursor_x, cursor_y);

    int  bytes_read;
    char in_buf[4];
    while ((bytes_read = read(STDIN_FILENO, &in_buf[0], 1)) >= 0)
    {
        bullet_update_count++;

        if (bytes_read != 0)
        {
            if (in_buf[0] == '\033')
            {
                if (read(STDIN_FILENO, &in_buf[1], 1) != 1) return 0; // just ESC - exit
                if (read(STDIN_FILENO, &in_buf[2], 1) != 1) die("read");

                if (in_buf[1] == '[')
                {
                    switch (in_buf[2])
                    {
                    case 'A':
                        cursor_y--;
                        break;

                    case 'B':
                        cursor_y++;
                        break;

                    case 'C':
                        cursor_x++;
                        break;

                    case 'D':
                        cursor_x--;
                        break;

                    default:
                        write_log("not handled escape code: %c", in_buf[2]);
                        break;
                    }
                }
                else
                {
                    write_log("not handled escape code: %c", in_buf[1]);
                }
            }
            else if (in_buf[0] == ' ')
            {
                // fire!
                if (next_bullet < bullets_len)
                {
                    bullets[next_bullet].active = 1;
                    bullets[next_bullet].x      = cursor_x + 2;
                    bullets[next_bullet].y      = cursor_y - 3; // TODO make sizes available

                    while (next_bullet < bullets_len && bullets[next_bullet].active)
                        next_bullet++;
                }
            }
        }

        if (bullet_update_count >= bullet_update_rate)
        {
            bullet_update_count = 0;
            for (int i = 0; i < bullets_len; i++)
            {
                bullets[i].y--;
                if (bullets[i].y <= 0)
                {
                    bullets[i].active = 0;
                    next_bullet       = i;
                }
            }
        }

        clear_screen();
        draw_ship(cursor_x, cursor_y);

        for (int i = 0; i < bullets_len; i++)
        {
            if (bullets[i].active) draw_bullet(&bullets[i]);
        }

        fflush(stdout);
    }

    set_cursor(0, 0);
    reset_color();

    return 0;
}

void draw_ship(int x, int y)
{
#define WIDTH 7
#define HEIGHT 7

    static const enum color sprite[WIDTH][HEIGHT] = {
        // clang-format off
        {BLACK, BLACK, BLACK, WHITE, BLACK, BLACK, BLACK},
        {BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK},
        {BLACK, BLACK, WHITE, BLUE,  WHITE, BLACK, BLACK},
        {BLACK, RED,   WHITE, BLUE,  WHITE, RED,   BLACK},
        {BLACK, WHITE, WHITE, BLUE,  WHITE, WHITE, BLACK},
        {BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK},
        {WHITE, BLACK, RED,   RED,   RED,   BLACK, WHITE},
        // clang-format on
    };

    reset_color();
    for (int r = 0; r < HEIGHT; r++)
    {
        for (int c = 0; c < WIDTH; c++)
        {
            set_cursor(c + x, r + y);
            draw_color(sprite[r][c]);
        }
    }
    reset_color();

#undef WIDTH
#undef HEIGHT
}

void draw_bullet(struct bullet* bullet)
{
#define WIDTH 3
#define HEIGHT 3

    static const enum color sprite[WIDTH][HEIGHT] = {
        {WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE},
    };

    reset_color();
    for (int r = 0; r < HEIGHT; r++)
    {
        for (int c = 0; c < WIDTH; c++)
        {
            set_cursor(c + bullet->x, r + bullet->y);
            draw_color(sprite[r][c]);
        }
    }
    reset_color();

#undef WIDTH
#undef HEIGHT
}
