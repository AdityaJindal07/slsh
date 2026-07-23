#ifndef SHELL_PARSER_H
#define SHELL_PARSER_H

#include <stddef.h>
#include <stdint.h>

/* Maximum line length (including terminating null) */
#define SHELL_PARSER_MAX_LINE 128
/* Maximum number of arguments (including argv[0]) */
#define SHELL_PARSER_MAX_ARGS 16

/* Return codes for parser operations */
typedef enum {
    SHELL_PARSER_OK = 0,
    SHELL_PARSER_LINE_READY = 1,   /* A full line (terminated by \r or \n) is ready */
    SHELL_PARSER_ERR_OVERFLOW = -1 /* Buffer overflow occurred */
} shell_parser_status_t;

/* Parser state – all members are owned by the parser (no dynamic allocation) */
typedef struct {
    char line_buf[SHELL_PARSER_MAX_LINE];
    size_t len;                     /* Current length in line_buf */
    int argc;                       /* Argument count after tokenization */
    char *argv[SHELL_PARSER_MAX_ARGS];
} shell_parser_t;

/* Initialise a parser instance; clears buffers */
void shell_parser_init(shell_parser_t *parser);

/* Process a single character; returns status.
   Backspace (0x08) or DEL (0x7F) delete the previous character.
   CR (\r) or LF (\n) terminate the line and trigger tokenisation.
*/
shell_parser_status_t shell_parser_process_char(shell_parser_t *parser, char c);

/* Tokenise the accumulated line buffer into argc/argv. */
void shell_parser_tokenize(shell_parser_t *parser);

/* Reset parser state for next line (keeps buffer cleared). */
void shell_parser_reset(shell_parser_t *parser);

#endif /* SHELL_PARSER_H */
