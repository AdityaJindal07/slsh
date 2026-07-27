#ifndef FREERTOS_UART_ADAPTER_H
#define FREERTOS_UART_ADAPTER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Queue handle abstraction compatible with FreeRTOS QueueHandle_t */
typedef struct freertos_queue_s freertos_queue_t;

/** Portable FreeRTOS queue API wrapper */
freertos_queue_t *freertos_queue_create(size_t depth, size_t item_size);
bool freertos_queue_send(freertos_queue_t *queue, const void *item, uint32_t wait_ticks);
bool freertos_queue_receive(freertos_queue_t *queue, void *item, uint32_t wait_ticks);
void freertos_queue_delete(freertos_queue_t *queue);

/** FreeRTOS UART Queue Adapter Initialization */
bool freertos_uart_init(size_t rx_depth, size_t tx_depth);
void freertos_uart_deinit(void);

/** Helper methods for simulation / ISR queue pushing */
bool freertos_uart_push_rx(uint8_t ch);
bool freertos_uart_pop_tx(uint8_t *out_ch);

/**
 * @brief FreeRTOS Task entry point for Command Shell processing.
 *        Reads characters from UART Rx Queue, parses lines, and invokes Command Engine.
 * @param pvParameters Optional task parameters (NULL or subsystem context).
 */
void freertos_cmd_shell_task(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif /* FREERTOS_UART_ADAPTER_H */
