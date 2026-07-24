#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "cmd_engine.h"
#include "builtin_cmds.h"

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

    /* ID based tests */
    assert(cmd_engine_execute(0x40U, &context) == CMD_ENGINE_ERR_NOT_INITIALIZED);
    assert(cmd_engine_init(NULL, 1U) == CMD_ENGINE_ERR_INVALID_ARGUMENT);
    assert(cmd_engine_init(commands, 2U) == CMD_ENGINE_OK);
    assert(cmd_engine_execute(0x99U, &context) == CMD_ENGINE_ERR_UNKNOWN_COMMAND);
    assert(cmd_engine_execute(0x41U, &context) == CMD_ENGINE_ERR_INVALID_ARGUMENT);
    assert(cmd_engine_execute(0x40U, &context) == CMD_ENGINE_OK);
    assert(calls == 1U);
    assert(received_context == &context);

    /* Dynamic registration & string lookup tests (Checkpoint 3) */
    assert(command_register("test_cmd", record_call) == CMD_ENGINE_OK);
    assert(cmd_register("test_cmd", record_call) == CMD_ENGINE_ERR_DUPLICATE_COMMAND);

    /* Case-insensitive execution */
    calls = 0;
    assert(cmd_engine_execute_by_name("TEST_CMD", &context) == CMD_ENGINE_OK);
    assert(calls == 1);

    /* Built-in commands registration */
    assert(register_builtin_commands() == CMD_ENGINE_OK);
    assert(cmd_engine_execute_by_name("PING", NULL) == CMD_ENGINE_OK);
    assert(cmd_engine_execute_by_name("HELP", NULL) == CMD_ENGINE_OK);
    assert(cmd_engine_execute_by_name("ECHO", "hello world") == CMD_ENGINE_OK);

    /* List registered commands */
    const char *list = cmd_engine_list_commands();
    assert(list != NULL);
    assert(strstr(list, "ping") != NULL);
    assert(strstr(list, "help") != NULL);
    assert(strstr(list, "echo") != NULL);

    /* Error string test */
    assert(strcmp(cmd_engine_strerror(CMD_ENGINE_ERR_UNKNOWN_COMMAND), "Unknown command") == 0);

    printf("All Checkpoint 3 tests passed successfully!\n");
    return 0;
}
