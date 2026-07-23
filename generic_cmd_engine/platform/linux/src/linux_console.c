#include "linux_console.h"
#include <stdio.h>

int linux_console_getchar(void) {
    /* Simple wrapper around getchar(). In a real implementation you might set the terminal
       to raw mode to avoid line buffering, but for this checkpoint getchar() is sufficient. */
    return getchar();
}

void linux_console_print_prompt(void) {
    fputs(LINUX_CONSOLE_PROMPT, stdout);
    fflush(stdout);
}
