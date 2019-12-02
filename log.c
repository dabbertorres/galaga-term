#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern void die(const char* s);

static FILE* log_file = NULL;

void close_log()
{
    fclose(log_file);
}

void open_log(const char* file)
{
    log_file = fopen(file, "a");
    if (log_file == NULL) die("opening log file");
    atexit(close_log);
}

void write_log(const char* fmt, ...)
{
    va_list argp;

    time_t now = time(NULL);
    char   buf[9];
    strftime(buf, sizeof(buf), "%T", localtime(&now));
    fprintf(log_file, "%s - ", buf);

    va_start(argp, fmt);

    vfprintf(log_file, fmt, argp);

    va_end(argp);

    fprintf(log_file, "\n");
}
