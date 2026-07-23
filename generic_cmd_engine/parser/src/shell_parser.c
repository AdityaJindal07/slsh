#include "shell_parser.h"
#include <string.h>

void shell_parser_init(shell_parser_t *parser) {
    if (!parser) return;
    parser->len = 0;
    parser->argc = 0;
    parser->line_buf[0] = '\0';
    memset(parser->argv, 0, sizeof(parser->argv));
}

static void shell_parser_backspace(shell_parser_t *parser) {
    if (parser->len > 0) {
        parser->len--;
        parser->line_buf[parser->len] = '\0';
    }
}

static void shell_parser_finalize_line(shell_parser_t *parser) {
    parser->line_buf[parser->len] = '\0';
    shell_parser_tokenize(parser);
}

shell_parser_status_t shell_parser_process_char(shell_parser_t *parser, char c) {
    if (!parser) return SHELL_PARSER_ERR_OVERFLOW;
    switch (c) {
        case '\b':
        case 0x7F: /* DEL */
            shell_parser_backspace(parser);
            return SHELL_PARSER_OK;
        case '\r':
        case '\n':
            /* If we already just processed \r and current is \n, ignore duplicate */
            if (parser->len == 0 && parser->argc == 0) {
                // empty line, treat as ready but no tokens
                return SHELL_PARSER_LINE_READY;
            }
            shell_parser_finalize_line(parser);
            return SHELL_PARSER_LINE_READY;
        default:
            if ((unsigned char)c < 0x20) {
                /* Non-printable control chars are ignored */
                return SHELL_PARSER_OK;
            }
            if (parser->len + 1 >= SHELL_PARSER_MAX_LINE) {
                /* Buffer would overflow */
                return SHELL_PARSER_ERR_OVERFLOW;
            }
            parser->line_buf[parser->len++] = c;
            parser->line_buf[parser->len] = '\0';
            return SHELL_PARSER_OK;
    }
}

void shell_parser_tokenize(shell_parser_t *parser) {
    if (!parser) return;
    parser->argc = 0;
    char *ptr = parser->line_buf;
    while (*ptr != '\0' && parser->argc < SHELL_PARSER_MAX_ARGS) {
        /* Skip leading whitespace */
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        if (*ptr == '\0') break;
        parser->argv[parser->argc++] = ptr;
        /* Scan until next whitespace or end */
        while (*ptr != '\0' && *ptr != ' ' && *ptr != '\t') ptr++;
        if (*ptr != '\0') {
            *ptr = '\0';
            ptr++;
        }
    }
    /* Ensure remaining argv entries are NULL */
    for (int i = parser->argc; i < SHELL_PARSER_MAX_ARGS; ++i) {
        parser->argv[i] = NULL;
    }
}

void shell_parser_reset(shell_parser_t *parser) {
    if (!parser) return;
    parser->len = 0;
    parser->argc = 0;
    parser->line_buf[0] = '\0';
    memset(parser->argv, 0, sizeof(parser->argv));
}
