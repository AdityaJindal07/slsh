#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "linux_console.h"

#include "freertos_uart_adapter.h"
#include "builtin_cmds.h"

// Forward declaration of the shell task function defined in freertos_uart_adapter.c
extern void freertos_cmd_shell_task(void *pvParameters);

static DWORD WINAPI shell_task_thread(LPVOID lpParam) {
    (void)lpParam;
    freertos_cmd_shell_task(NULL);
    return 0;
}

int main(void) {
    uint8_t out;

    // Initialize UART queues with reasonable depths
    if (!freertos_uart_init(64, 64)) {
        fprintf(stderr, "Failed to initialize FreeRTOS UART adapter\n");
        return EXIT_FAILURE;
    }
    // Register built‑in commands (help, ping, echo)
    if (register_builtin_commands() != CMD_ENGINE_OK) {
        fprintf(stderr, "Failed to register built‑in commands\n");
        freertos_uart_deinit();
        return EXIT_FAILURE;
    }
    // Show initial prompt
    linux_console_print_prompt();
    // Drain any queued prompt output to stdout

    while (freertos_uart_pop_tx(&out)) {
        putchar(out);
        fflush(stdout);
    }

    // Start the command shell task in a separate thread
    HANDLE hThread = CreateThread(NULL, 0, shell_task_thread, NULL, 0, NULL);
    if (hThread == NULL) {
        fprintf(stderr, "Failed to create shell task thread\n");
        freertos_uart_deinit();
        return EXIT_FAILURE;
    }

    // Main loop: read from stdin, push to RX queue, and forward any TX output to stdout
    int ch;
    while ((ch = getchar()) != EOF) {
        // Push user input into the RX queue for the shell task
        freertos_uart_push_rx((uint8_t)ch);
        // Immediately drain any pending TX output produced by the command processing

        while (freertos_uart_pop_tx(&out)) {
            putchar(out);
            fflush(stdout);
        }
    }

    // After EOF, wait for the shell thread to finish processing any remaining input.
    WaitForSingleObject(hThread, INFINITE);
    // Drain any leftover TX output.

    while (freertos_uart_pop_tx(&out)) {
        putchar(out);
    }
    CloseHandle(hThread);

    freertos_uart_deinit();
    return EXIT_SUCCESS;
}
