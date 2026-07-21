#include "cmd_engine.h"

static const cmd_entry_t *command_table;
static size_t command_count;

cmd_engine_status_t cmd_engine_init(const cmd_entry_t *table, size_t count)
{
    if ((table == NULL) || (count == 0U)) {
        command_table = NULL;
        command_count = 0U;
        return CMD_ENGINE_ERR_INVALID_ARGUMENT;
    }

    command_table = table;
    command_count = count;
    return CMD_ENGINE_OK;
}

cmd_engine_status_t cmd_engine_execute(uint32_t cmd_id, void *context)
{
    size_t index;

    if ((command_table == NULL) || (command_count == 0U)) {
        return CMD_ENGINE_ERR_NOT_INITIALIZED;
    }

    for (index = 0U; index < command_count; ++index) {
        if (command_table[index].id == cmd_id) {
            if (command_table[index].handler == NULL) {
                return CMD_ENGINE_ERR_INVALID_ARGUMENT;
            }
            command_table[index].handler(context);
            return CMD_ENGINE_OK;
        }
    }

    return CMD_ENGINE_ERR_UNKNOWN_COMMAND;
}
