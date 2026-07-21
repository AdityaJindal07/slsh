#include <assert.h>
#include <stdint.h>

#include "cmd_engine.h"

static unsigned int calls;
static void *received_context;

static void record_call(void *context)
{
    ++calls;
    received_context = context;
}

int main(void)
{
    int context = 7;
    static const cmd_entry_t commands[] = {
        { 0x40U, record_call },
        { 0x41U, NULL }
    };

    assert(cmd_engine_execute(0x40U, &context) == CMD_ENGINE_ERR_NOT_INITIALIZED);
    assert(cmd_engine_init(NULL, 1U) == CMD_ENGINE_ERR_INVALID_ARGUMENT);
    assert(cmd_engine_init(commands, 2U) == CMD_ENGINE_OK);
    assert(cmd_engine_execute(0x99U, &context) == CMD_ENGINE_ERR_UNKNOWN_COMMAND);
    assert(cmd_engine_execute(0x41U, &context) == CMD_ENGINE_ERR_INVALID_ARGUMENT);
    assert(cmd_engine_execute(0x40U, &context) == CMD_ENGINE_OK);
    assert(calls == 1U);
    assert(received_context == &context);

    return 0;
}
