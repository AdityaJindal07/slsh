#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <pthread.h>
#endif

#include "linux_console.h"
#include "freertos_uart_adapter.h"
#include "builtin_cmds.h"

extern void freertos_cmd_shell_task(void *pvParameters);

#ifdef _WIN32

static DWORD WINAPI shell_task_thread(LPVOID lpParam)
{
    (void)lpParam;
    freertos_cmd_shell_task(NULL);
    return 0;
}

static void sleep_ms(unsigned int ms)
{
    Sleep(ms);
}

#else

static void *shell_task_thread(void *arg)
{
    (void)arg;
    freertos_cmd_shell_task(NULL);
    return NULL;
}

static void sleep_ms(unsigned int ms)
{
    sleep(ms * 1000);
}

#endif

int main(void)
{
    uint8_t out;

    /* Initialize queues */
    if (!freertos_uart_init(64, 64)) {
        fprintf(stderr, "Failed to initialize FreeRTOS UART adapter\n");
        return EXIT_FAILURE;
    }

    /* Register commands */
    if (register_builtin_commands() != CMD_ENGINE_OK) {
        fprintf(stderr, "Failed to register built-in commands\n");
        freertos_uart_deinit();
        return EXIT_FAILURE;
    }

    /* Print initial prompt */
    linux_console_print_prompt();

    while (freertos_uart_pop_tx(&out))
        putchar(out);

    fflush(stdout);

#ifdef _WIN32

    HANDLE hThread = CreateThread(
        NULL,
        0,
        shell_task_thread,
        NULL,
        0,
        NULL);

    if (hThread == NULL) {
        fprintf(stderr, "Failed to create shell thread\n");
        freertos_uart_deinit();
        return EXIT_FAILURE;
    }

#else

    pthread_t hThread;

    if (pthread_create(&hThread, NULL, shell_task_thread, NULL) != 0) {
        fprintf(stderr, "Failed to create shell thread\n");
        freertos_uart_deinit();
        return EXIT_FAILURE;
    }

#endif

    /* Interactive console */
    int ch;

    while ((ch = getchar()) != EOF) {

        /* Send character to simulated UART RX */
        freertos_uart_push_rx((uint8_t)ch);

        /* Give shell thread time to process it */
        sleep_ms(1);

        /* Print everything currently in TX queue */
        while (freertos_uart_pop_tx(&out)) {
            putchar(out);
        }

        fflush(stdout);
    }

    /* Give shell task a chance to flush remaining output */
    sleep_ms(20);

    while (freertos_uart_pop_tx(&out)) {
        putchar(out);
    }

    fflush(stdout);

#ifdef _WIN32
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
#else
    pthread_cancel(hThread);
    pthread_join(hThread, NULL);
#endif

    freertos_uart_deinit();

    return EXIT_SUCCESS;
}