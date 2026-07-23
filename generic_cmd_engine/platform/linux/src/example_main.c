#include <stdio.h>
#include "linux_console.h"
#include "shell_parser.h"
#include "cmd_engine.h"

int main(void) {
    // Initialise parser
    shell_parser_t parser;
    shell_parser_init(&parser);

    // Print prompt
    linux_console_print_prompt();

    while (1) {
        int ch = linux_console_getchar();
        if (ch == EOF) break; // End of input
        shell_parser_status_t status = shell_parser_process_char(&parser, (char)ch);
        if (status == SHELL_PARSER_LINE_READY) {
            // Line is ready – display parsed arguments
            printf("\n[Parser] argc=%d\n", parser.argc);
            for (int i = 0; i < parser.argc; ++i) {
                printf("argv[%d] = '%s'\n", i, parser.argv[i]);
            }
            // Reset for next line and show prompt again
            shell_parser_reset(&parser);
            linux_console_print_prompt();
        }
        // No command execution at this checkpoint – just parsing
    }
    return 0;
}
