#include <stdio.h>
#include <string.h>
#include "subsystems.h"
#include "builtin_cmds.h"
#include "shell_parser.h"
#include "shell_io.h"

/* -------------------------------------------------------------------------- */
/* OBDH Subsystem Command Callbacks                                           */
/* -------------------------------------------------------------------------- */
static void obdh_status_cmd(void *ctx)
{
    (void)ctx;
    shell_printf("[OBDH] CPU: 12%% | Memory: 42KB Free | Uptime: 1420s\n");
}

static void obdh_telemetry_dump_cmd(void *ctx)
{
    (void)ctx;
    shell_printf("[OBDH] Telemetry Log: [OK] Sensor1=24.5C, Sensor2=3.30V, Packets=1042\n");
}

cmd_engine_status_t obdh_register_commands(void)
{
    cmd_engine_status_t st = cmd_register("obdh_status", obdh_status_cmd);
    if (st != CMD_ENGINE_OK) return st;
    return cmd_register("telemetry_dump", obdh_telemetry_dump_cmd);
}

/* -------------------------------------------------------------------------- */
/* EPS Subsystem Command Callbacks                                            */
/* -------------------------------------------------------------------------- */
static void eps_vbat_cmd(void *ctx)
{
    (void)ctx;
    shell_printf("[EPS] Battery Voltage: 8.24V | State of Charge: 94%%\n");
}

static void eps_rail_enable_cmd(void *ctx)
{
    const shell_parser_t *parser = (const shell_parser_t *)ctx;
    if (parser && parser->argc > 1) {
        shell_printf("[EPS] Power Rail '%s' ENABLED\n", parser->argv[1]);
    } else {
        shell_printf("[EPS] Usage: rail_enable <rail_name>\n");
    }
}

cmd_engine_status_t eps_register_commands(void)
{
    cmd_engine_status_t st = cmd_register("vbat", eps_vbat_cmd);
    if (st != CMD_ENGINE_OK) return st;
    return cmd_register("rail_enable", eps_rail_enable_cmd);
}

/* -------------------------------------------------------------------------- */
/* TTC Subsystem Command Callbacks                                            */
/* -------------------------------------------------------------------------- */
static void ttc_beacon_cmd(void *ctx)
{
    (void)ctx;
    shell_printf("[TTC] Radio Beacon Transmitted: 'BEACON_CUBESAT_01_OK'\n");
}

static void ttc_rssi_cmd(void *ctx)
{
    (void)ctx;
    shell_printf("[TTC] RX RSSI: -88 dBm | SNR: 14 dB\n");
}

cmd_engine_status_t ttc_register_commands(void)
{
    cmd_engine_status_t st = cmd_register("beacon", ttc_beacon_cmd);
    if (st != CMD_ENGINE_OK) return st;
    return cmd_register("rssi", ttc_rssi_cmd);
}

/* -------------------------------------------------------------------------- */
/* Subsystem Simulation Main Entry                                           */
/* -------------------------------------------------------------------------- */
static void process_sim_command(const char *subsystem_name, const char *cmd_line)
{
    shell_parser_t parser;
    shell_parser_init(&parser);

    for (size_t i = 0; i < strlen(cmd_line); ++i) {
        shell_parser_process_char(&parser, cmd_line[i]);
    }
    shell_parser_process_char(&parser, '\n');

    if (parser.argc > 0) {
        shell_printf("[%s Execution] Executing '%s': ", subsystem_name, parser.argv[0]);
        cmd_engine_status_t st = cmd_engine_execute_by_name(parser.argv[0], &parser);
        if (st != CMD_ENGINE_OK) {
            shell_printf("Error (%s)\n", cmd_engine_strerror(st));
        }
    }
}

#include <string.h>

static void run_demo(void) {
    /* Simulate Telecommands for OBDH */
    process_sim_command("OBDH", "obdh_status");
    process_sim_command("OBDH", "telemetry_dump");

    /* Simulate Telecommands for EPS */
    process_sim_command("EPS", "vbat");
    process_sim_command("EPS", "rail_enable 3V3_PAYLOAD");

    /* Simulate Telecommands for TTC */
    process_sim_command("TTC", "beacon");
    process_sim_command("TTC", "rssi");

    /* Common built-in */
    process_sim_command("GENERIC", "ping");

    shell_printf("\nSimulation Completed Successfully!\n");
}

int main(int argc, char *argv[]) {
    shell_printf("=====================================================\n");
    shell_printf("  SAT-OS Multi-Subsystem Command Engine Simulation   \n");
    shell_printf("=====================================================\n\n");

    /* Register built-ins and all subsystem commands */
    register_builtin_commands();
    obdh_register_commands();
    eps_register_commands();
    ttc_register_commands();

    shell_printf("Registered Commands: %s\n\n", cmd_engine_list_commands());

    // If "demo" argument is provided, run the original automated demo sequence.
    if (argc > 1 && strcmp(argv[1], "demo") == 0) {
        run_demo();
        return 0;
    }

    // Interactive shell mode
    linux_console_print_prompt();
    shell_parser_t parser;
    shell_parser_init(&parser);
    while (1) {
        int ch = linux_console_getchar();
        if (ch == EOF) break; // End of input (Ctrl+D)
        shell_parser_status_t status = shell_parser_process_char(&parser, (char)ch);
        if (status == SHELL_PARSER_LINE_READY) {
            printf("\n");
            if (parser.argc > 0) {
                cmd_engine_status_t cmd_st = cmd_engine_execute_by_name(parser.argv[0], &parser);
                if (cmd_st != CMD_ENGINE_OK) {
                    printf("Error (%s): Unknown or invalid command '%s'\n", cmd_engine_strerror(cmd_st), parser.argv[0]);
                }
            }
            shell_parser_reset(&parser);
            linux_console_print_prompt();
        }
    }
    return 0;
}


