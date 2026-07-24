#include "cmd_engine.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

/* -------------------------------------------------------------------------- */
/* Core static command table (case‑insensitive, linear search) */
#define CMD_MAX_ENTRIES 32

typedef struct {
    char name[32];               /* lower‑case command name */
    cmd_handler_t handler;       /* function pointer */
} cmd_slot_t;

static cmd_slot_t cmd_table[CMD_MAX_ENTRIES];
static size_t cmd_count = 0U;

/* -------------------------------------------------------------------------- */
/* Existing API – kept for backward compatibility */
static const cmd_entry_t *command_table = NULL;
static size_t command_table_count = 0U;

cmd_engine_status_t cmd_engine_init(const cmd_entry_t *table, size_t count)
{
    if ((table == NULL) || (count == 0U)) {
        command_table = NULL;
        command_table_count = 0U;
        return CMD_ENGINE_ERR_INVALID_ARGUMENT;
    }
    command_table = table;
    command_table_count = count;
    return CMD_ENGINE_OK;
}

cmd_engine_status_t cmd_engine_execute(uint32_t cmd_id, void *context)
{
    if ((command_table == NULL) || (command_table_count == 0U)) {
        return CMD_ENGINE_ERR_NOT_INITIALIZED;
    }
    for (size_t i = 0U; i < command_table_count; ++i) {
        if (command_table[i].id == cmd_id) {
            if (command_table[i].handler == NULL) {
                return CMD_ENGINE_ERR_INVALID_ARGUMENT;
            }
            command_table[i].handler(context);
            return CMD_ENGINE_OK;
        }
    }
    return CMD_ENGINE_ERR_UNKNOWN_COMMAND;
}

/* -------------------------------------------------------------------------- */
/* New registration API (case‑insensitive) */
cmd_engine_status_t cmd_register(const char *name, cmd_handler_t handler)
{
    if ((name == NULL) || (handler == NULL) || (name[0] == '\0')) {
        return CMD_ENGINE_ERR_INVALID_ARGUMENT;
    }
    if (cmd_count >= CMD_MAX_ENTRIES) {
        return CMD_ENGINE_ERR_TABLE_FULL;
    }
    /* Convert to lower case for storage */
    char lowered[32];
    size_t i;
    for (i = 0; i < sizeof(lowered) - 1 && name[i] != '\0'; ++i) {
        lowered[i] = (char)tolower((unsigned char)name[i]);
    }
    lowered[i] = '\0';
    /* Check for duplicate */
    for (size_t j = 0; j < cmd_count; ++j) {
        if (strcmp(cmd_table[j].name, lowered) == 0) {
            return CMD_ENGINE_ERR_DUPLICATE_COMMAND;
        }
    }
    /* Store */
    strcpy(cmd_table[cmd_count].name, lowered);
    cmd_table[cmd_count].handler = handler;
    ++cmd_count;
    return CMD_ENGINE_OK;
}

cmd_engine_status_t cmd_engine_execute_by_name(const char *name, void *context)
{
    if ((name == NULL) || (cmd_count == 0U)) {
        return CMD_ENGINE_ERR_INVALID_ARGUMENT;
    }
    char lowered[32];
    size_t i;
    for (i = 0; i < sizeof(lowered) - 1 && name[i] != '\0'; ++i) {
        lowered[i] = (char)tolower((unsigned char)name[i]);
    }
    lowered[i] = '\0';
    for (size_t j = 0; j < cmd_count; ++j) {
        if (strcmp(cmd_table[j].name, lowered) == 0) {
            if (cmd_table[j].handler == NULL) {
                return CMD_ENGINE_ERR_INVALID_ARGUMENT;
            }
            cmd_table[j].handler(context);
            return CMD_ENGINE_OK;
        }
    }
    return CMD_ENGINE_ERR_UNKNOWN_COMMAND;
}

/* -------------------------------------------------------------------------- */
/* Helper to list all registered commands */
const char *cmd_engine_list_commands(void)
{
    static char buffer[256];
    size_t pos = 0;
    buffer[0] = '\0';
    for (size_t i = 0; i < cmd_count; ++i) {
        const char *sep = (i == 0) ? "" : ", ";
        int written = snprintf(buffer + pos, sizeof(buffer) - pos, "%s%s", sep, cmd_table[i].name);
        if (written < 0) break;
        pos += (size_t)written;
        if (pos >= sizeof(buffer)) break;
    }
    return buffer;
}

/* -------------------------------------------------------------------------- */
/* Human‑readable error strings */
const char *cmd_engine_strerror(cmd_engine_status_t status)
{
    switch (status) {
        case CMD_ENGINE_OK: return "OK";
        case CMD_ENGINE_ERR_INVALID_ARGUMENT: return "Invalid argument";
        case CMD_ENGINE_ERR_NOT_INITIALIZED: return "Engine not initialized";
        case CMD_ENGINE_ERR_UNKNOWN_COMMAND: return "Unknown command";
        case CMD_ENGINE_ERR_DUPLICATE_COMMAND: return "Duplicate command registration";
        case CMD_ENGINE_ERR_TABLE_FULL: return "Command table full";
        default: return "Unknown error";
    }
}
