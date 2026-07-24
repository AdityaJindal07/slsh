#ifndef BUILTIN_CMDS_H
#define BUILTIN_CMDS_H

#include "cmd_engine.h"

/** Registers basic built-in demonstration commands: help, ping, echo. */
cmd_engine_status_t register_builtin_commands(void);

#endif /* BUILTIN_CMDS_H */
