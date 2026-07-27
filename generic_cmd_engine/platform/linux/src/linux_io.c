#include "shell_io.h"
#include <stdio.h>
#include <stdarg.h>

int shell_getchar(void)
{
    return getchar();
}

int shell_putchar(int ch)
{
    int ret = putchar(ch);
    fflush(stdout);
    return ret;
}

int shell_puts(const char *str)
{
    if (str == NULL) {
        return EOF;
    }
    int ret = fputs(str, stdout);
    fflush(stdout);
    return ret;
}

int shell_printf(const char *fmt, ...)
{
    if (fmt == NULL) {
        return -1;
    }
    va_list args;
    va_start(args, fmt);
    int printed = vprintf(fmt, args);
    va_end(args);
    fflush(stdout);
    return printed;
}
