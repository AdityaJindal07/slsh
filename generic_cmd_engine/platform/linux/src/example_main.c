#include <stdio.h>

#include "cmd_engine.h"

enum {
    CMD_ID_PING = 0x40U
};

static void handle_ping(void *context)
{
    const char *source = context;
    printf("ping received from %s\n", source);
}

int main(void)
{
    static const cmd_entry_t commands[] = {
        { CMD_ID_PING, handle_ping }
    };

    if (cmd_engine_init(commands, sizeof(commands) / sizeof(commands[0])) != CMD_ENGINE_OK) {
        return 1;
    }

    return cmd_engine_execute(CMD_ID_PING, "Linux adapter") == CMD_ENGINE_OK ? 0 : 1;
}
