#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cmd_engine.h"
#include "builtin_cmds.h"
#include "freertos_uart_adapter.h"

int main(void)
{
    printf("Starting FreeRTOS UART Queue Adapter Unit Test...\n");

    /* Initialize FreeRTOS Rx & Tx Queues */
    assert(freertos_uart_init(128, 128) == true);

    /* Register built-in commands */
    assert(register_builtin_commands() == CMD_ENGINE_OK);

    /* Push "ping\n" into Rx Queue */
    const char *cmd = "ping\n";
    for (size_t i = 0; i < strlen(cmd); ++i) {
        assert(freertos_uart_push_rx((uint8_t)cmd[i]) == true);
    }

    /* Execute FreeRTOS Task processing loop */
    freertos_cmd_shell_task(NULL);

    /* Verify output in Tx Queue */
    char tx_buf[64];
    size_t idx = 0;
    uint8_t ch;
    while (freertos_uart_pop_tx(&ch)) {
        if (idx < sizeof(tx_buf) - 1) {
            tx_buf[idx++] = (char)ch;
        }
    }
    tx_buf[idx] = '\0';

    printf("Tx Queue Captured Output:\n%s\n", tx_buf);
    assert(strstr(tx_buf, "pong") != NULL);

    freertos_uart_deinit();
    printf("FreeRTOS UART Queue Adapter Test PASSED!\n");
    return 0;
}
