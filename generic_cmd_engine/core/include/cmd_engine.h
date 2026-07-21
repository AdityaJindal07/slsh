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
    CMD_ENGINE_ERR_UNKNOWN_COMMAND = -3
} cmd_engine_status_t;

/** Registers a non-empty, caller-owned command table. */
cmd_engine_status_t cmd_engine_init(const cmd_entry_t *table, size_t count);

/** Finds cmd_id and invokes its handler with the supplied opaque context. */
cmd_engine_status_t cmd_engine_execute(uint32_t cmd_id, void *context);

#endif
