#include "freertos_uart_adapter.h"
#include "shell_io.h"
#include "shell_parser.h"
#include "cmd_engine.h"
#include "linux_console.h"
/*
#ifdef _WIN32
#include <windows.h>
#define shell_sleep(ms) Sleep(ms)
#else
#include <unistd.h>
#define shell_sleep(ms) usleep((ms) * 1000)
#endif
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
//#include <unistd.h>
/* -------------------------------------------------------------------------- */
/* Portable Queue Implementation for Host & Embedded targets                 */
/* -------------------------------------------------------------------------- */
struct freertos_queue_s {
    uint8_t *buffer;
    size_t depth;
    size_t item_size;
    size_t head;
    size_t tail;
    size_t count;
};

freertos_queue_t *freertos_queue_create(size_t depth, size_t item_size)
{
    if (depth == 0 || item_size == 0) return NULL;
    freertos_queue_t *q = (freertos_queue_t *)malloc(sizeof(freertos_queue_t));
    if (!q) return NULL;
    q->buffer = (uint8_t *)malloc(depth * item_size);
    if (!q->buffer) {
        free(q);
        return NULL;
    }
    q->depth = depth;
    q->item_size = item_size;
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    return q;
}

bool freertos_queue_send(freertos_queue_t *queue, const void *item, uint32_t wait_ticks)
{
    (void)wait_ticks;
    if (!queue || !item) return false;
    if (queue->count >= queue->depth) return false;

    memcpy(&queue->buffer[queue->head * queue->item_size], item, queue->item_size);
    queue->head = (queue->head + 1) % queue->depth;
    queue->count++;
    return true;
}

bool freertos_queue_receive(freertos_queue_t *queue, void *item, uint32_t wait_ticks)
{
    (void)wait_ticks;
    if (!queue || !item) return false;
    if (queue->count == 0) return false;

    memcpy(item, &queue->buffer[queue->tail * queue->item_size], queue->item_size);
    queue->tail = (queue->tail + 1) % queue->depth;
    queue->count--;
    return true;
}

void freertos_queue_delete(freertos_queue_t *queue)
{
    if (!queue) return;
    if (queue->buffer) free(queue->buffer);
    free(queue);
}

/* -------------------------------------------------------------------------- */
/* Static FreeRTOS Rx & Tx Queues                                            */
/* -------------------------------------------------------------------------- */
static freertos_queue_t *s_rx_queue = NULL;
static freertos_queue_t *s_tx_queue = NULL;

bool freertos_uart_init(size_t rx_depth, size_t tx_depth)
{
    freertos_uart_deinit();
    s_rx_queue = freertos_queue_create(rx_depth, sizeof(uint8_t));
    s_tx_queue = freertos_queue_create(tx_depth, sizeof(uint8_t));
    return (s_rx_queue != NULL && s_tx_queue != NULL);
}

void freertos_uart_deinit(void)
{
    if (s_rx_queue) {
        freertos_queue_delete(s_rx_queue);
        s_rx_queue = NULL;
    }
    if (s_tx_queue) {
        freertos_queue_delete(s_tx_queue);
        s_tx_queue = NULL;
    }
}

bool freertos_uart_push_rx(uint8_t ch)
{
    return freertos_queue_send(s_rx_queue, &ch, 0);
}

bool freertos_uart_pop_tx(uint8_t *out_ch)
{
    return freertos_queue_receive(s_tx_queue, out_ch, 0);
}

/* -------------------------------------------------------------------------- */
/* HAL Bindings for FreeRTOS Adapter                                          */
/* -------------------------------------------------------------------------- */
int shell_getchar(void)
{
    uint8_t ch;
    if (freertos_queue_receive(s_rx_queue, &ch, 0)) {
        return (int)ch;
    }
    return EOF;
}

int shell_putchar(int ch)
{
    uint8_t byte = (uint8_t)ch;
    if (freertos_queue_send(s_tx_queue, &byte, 0)) {
        return ch;
    }
    return EOF;
}

int shell_puts(const char *str)
{
    if (!str) return EOF;
    int count = 0;
    while (*str) {
        if (shell_putchar((int)*str++) == EOF) return EOF;
        count++;
    }
    return count;
}

int shell_printf(const char *fmt, ...)
{
    if (!fmt) return -1;
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (len > 0) {
        shell_puts(buffer);
    }
    return len;
}

/* -------------------------------------------------------------------------- */
/* FreeRTOS Task Implementation                                               */
/* -------------------------------------------------------------------------- */
void freertos_cmd_shell_task(void *pvParameters)
{
    (void)pvParameters;
    shell_parser_t parser;
    shell_parser_init(&parser);

    while (1) {
        int ch = shell_getchar();
        if (ch == EOF) {
            // No data yet – yield briefly and keep looping
   
    //shell_sleep(10);      // Linux: microseconds (10 ms)

            continue;
        }

        shell_parser_status_t status = shell_parser_process_char(&parser, (char)ch);
        if (status == SHELL_PARSER_LINE_READY) {
            if (parser.argc > 0) {
                cmd_engine_status_t cmd_st = cmd_engine_execute_by_name(parser.argv[0], &parser);
                if (cmd_st != CMD_ENGINE_OK) {
                    shell_printf("Error (%s): Unknown command '%s'\r\n",
                                 cmd_engine_strerror(cmd_st), parser.argv[0]);
                }
            }
            shell_parser_reset(&parser);
            // Show prompt for next command
            linux_console_print_prompt();
        }
    }
}
