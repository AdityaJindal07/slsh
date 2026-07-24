#include <stdio.h>
#include "linux_console.h"
#include "shell_parser.h"
#include "cmd_engine.h"
#include "builtin_cmds.h"

int main(void) {
    // Register built-in demonstration commands: help, ping, echo
    if (register_builtin_commands() != CMD_ENGINE_OK) {
        printf("Failed to register built-in commands.\n");
        return 1;
    }

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
            printf("\n");
            if (parser.argc > 0) {
                // Execute command via function pointer lookup in command engine
                cmd_engine_status_t cmd_st = cmd_engine_execute_by_name(parser.argv[0], &parser);
                if (cmd_st != CMD_ENGINE_OK) {
                    printf("Error (%s): Unknown or invalid command '%s'\n",
                           cmd_engine_strerror(cmd_st), parser.argv[0]);
                }
            }
            // Reset for next line and show prompt again
            shell_parser_reset(&parser);
            linux_console_print_prompt();
        }
    }
    return 0;
}
