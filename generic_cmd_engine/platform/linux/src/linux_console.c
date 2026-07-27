#include "linux_console.h"
#include "shell_io.h"

int linux_console_getchar(void) {
    return shell_getchar();
}

void linux_console_print_prompt(void) {
    shell_puts(LINUX_CONSOLE_PROMPT);
}
