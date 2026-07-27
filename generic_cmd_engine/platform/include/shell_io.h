#ifndef SHELL_IO_H
#define SHELL_IO_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reads a single character from the input stream/source.
 *        Blocks until a character is available.
 * @return The character read as an unsigned char cast to an int, or EOF on end-of-file/error.
 */
int shell_getchar(void);

/**
 * @brief Writes a single character to the output stream/sink.
 * @param ch Character to write.
 * @return The character written as an unsigned char cast to an int, or EOF on error.
 */
int shell_putchar(int ch);

/**
 * @brief Writes a null-terminated string to the output stream.
 * @param str Null-terminated string to write.
 * @return Non-negative value on success, or EOF on error.
 */
int shell_puts(const char *str);

/**
 * @brief Formatted print through shell_putchar.
 * @param fmt Format string (printf-style).
 * @return Number of characters printed.
 */
int shell_printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* SHELL_IO_H */
