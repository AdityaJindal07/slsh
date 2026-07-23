#ifndef LINUX_CONSOLE_H
#define LINUX_CONSOLE_H

#include <stddef.h>

/* Prompt string displayed to the user */
#define LINUX_CONSOLE_PROMPT "slsh> "

/* Simple function to read a character from stdin (raw mode may be configured by caller). */
int linux_console_getchar(void);

/* Print the prompt to stdout */
void linux_console_print_prompt(void);

#endif /* LINUX_CONSOLE_H */
