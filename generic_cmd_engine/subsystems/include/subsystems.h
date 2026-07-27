#ifndef SUBSYSTEMS_H
#define SUBSYSTEMS_H

#include "cmd_engine.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/* Subsystem Registration Functions                                           */
/* -------------------------------------------------------------------------- */

/** Registers OBDH-specific telemetry & system commands */
cmd_engine_status_t obdh_register_commands(void);

/** Registers EPS-specific power management commands */
cmd_engine_status_t eps_register_commands(void);

/** Registers TTC-specific radio & beacon commands */
cmd_engine_status_t ttc_register_commands(void);

#ifdef __cplusplus
}
#endif

#endif /* SUBSYSTEMS_H */
