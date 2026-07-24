#ifndef CMD_ENGINE_H
#define CMD_ENGINE_H

#include <stddef.h>
#include <stdint.h>

/** Callback invoked for a registered command. The caller owns context. */
typedef void (*cmd_handler_t)(void *context);

/** Immutable mapping between a source-independent command ID and a callback. */
typedef struct {
    uint32_t id;
    cmd_handler_t handler;
} cmd_entry_t;

typedef enum {
    CMD_ENGINE_OK = 0,
    CMD_ENGINE_ERR_INVALID_ARGUMENT = -1,
    CMD_ENGINE_ERR_NOT_INITIALIZED = -2,
    CMD_ENGINE_ERR_UNKNOWN_COMMAND = -3,
    CMD_ENGINE_ERR_DUPLICATE_COMMAND = -4,
    CMD_ENGINE_ERR_TABLE_FULL = -5
} cmd_engine_status_t;

/** Registers a non-empty, caller-owned command table. */
cmd_engine_status_t cmd_engine_init(const cmd_entry_t *table, size_t count);

/** Finds cmd_id and invokes its handler with the supplied opaque context. */
cmd_engine_status_t cmd_engine_execute(uint32_t cmd_id, void *context);

/** Register a command name (case‑insensitive) with its handler. */
cmd_engine_status_t cmd_register(const char *name, cmd_handler_t handler);

/** Alias for cmd_register as required by specifications. */
#define command_register cmd_register

/** Execute a command by its name (case‑insensitive). */
cmd_engine_status_t cmd_engine_execute_by_name(const char *name, void *context);

/** Returns a comma‑separated list of all registered command names. */
const char *cmd_engine_list_commands(void);

/** Returns a human‑readable string for a status code. */
const char *cmd_engine_strerror(cmd_engine_status_t status);

#endif
