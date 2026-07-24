#include <stdio.h>
#include "builtin_cmds.h"
#include "shell_parser.h"

/* --------------------------------------------------------------- */
/* Built‑in command callbacks                                        */
/* --------------------------------------------------------------- */

/* help: prints a comma‑separated list of all registered commands */
static void help_cmd(void *context)
{
    (void)context; /* unused */
    const char *list = cmd_engine_list_commands();
    if (list && *list) {
        printf("Available commands: %s\n", list);
    } else {
        printf("No commands registered.\n");
    }
}

/* ping: prints a simple pong response */
static void ping_cmd(void *context)
{
    (void)context; /* unused */
    printf("pong\n");
}

/* echo: prints arguments passed as context */
static void echo_cmd(void *context)
{
    if (!context) {
        printf("\n");
        return;
    }

    const shell_parser_t *parser = (const shell_parser_t *)context;
    if (parser->argc > 0 && parser->argc <= SHELL_PARSER_MAX_ARGS && parser->argv[0] != NULL) {
        for (int i = 1; i < parser->argc; ++i) {
            printf("%s%s", parser->argv[i], (i == parser->argc - 1) ? "" : " ");
        }
        printf("\n");
    } else {
        printf("%s\n", (const char *)context);
    }
}

/* --------------------------------------------------------------- */
/* Registration helper – call this from main before entering the
   command‑parsing loop. */
/* --------------------------------------------------------------- */
cmd_engine_status_t register_builtin_commands(void)
{
    cmd_engine_status_t st;
    st = cmd_register("help", help_cmd);
    if (st != CMD_ENGINE_OK) return st;
    st = cmd_register("ping", ping_cmd);
    if (st != CMD_ENGINE_OK) return st;
    st = cmd_register("echo", echo_cmd);
    return st;
}
