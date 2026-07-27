#include <stdio.h>
#include "shell_io.h"
#include "shell_parser.h"
#include "cmd_engine.h"
#include "builtin_cmds.h"
#include "subsystems.h"

int main(void) {
    /* 1. Register built-in commands (help, ping, echo) */
    if (register_builtin_commands() != CMD_ENGINE_OK) {
        shell_printf("Error registering built-in commands.\n");
        return 1;
    }

    /* 2. Register subsystem commands for OBDH, EPS, and TTC */
    obdh_register_commands();
    eps_register_commands();
    ttc_register_commands();

    /* 3. Initialize shell parser */
    shell_parser_t parser;
    shell_parser_init(&parser);

    shell_puts("\n=====================================================\n");
    shell_puts("  SAT-OS Interactive Shell (HAL / Linux Console)     \n");
    shell_puts("=====================================================\n");
    shell_printf("Available Commands: %s\n\n", cmd_engine_list_commands());

    /* Print initial prompt */
    shell_puts("slsh> ");

    while (1) {
        /* Read next character via Hardware Abstraction Layer (shell_getchar) */
        int ch = shell_getchar();
        if (ch == EOF) break; /* End of input / Ctrl+D */

        shell_parser_status_t status = shell_parser_process_char(&parser, (char)ch);
        if (status == SHELL_PARSER_LINE_READY) {
            shell_puts("\n");
            if (parser.argc > 0) {
                /* Dispatch command via function pointer lookup in command engine */
                cmd_engine_status_t cmd_st = cmd_engine_execute_by_name(parser.argv[0], &parser);
                if (cmd_st != CMD_ENGINE_OK) {
                    shell_printf("Error (%s): Unknown or invalid command '%s'\n",
                                 cmd_engine_strerror(cmd_st), parser.argv[0]);
                }
            }
            /* Reset parser for next line and show prompt */
            shell_parser_reset(&parser);
            shell_puts("slsh> ");
        }
    }
    return 0;
}
