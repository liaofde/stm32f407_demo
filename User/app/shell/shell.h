#ifndef __SHELL_UART_DEV__

#include "usart.h"
#include "shell_cmd.h"
#include "uart_bsp.h"

#define shell_printf          printf

#define SHELL_HISTORY_LINES   3
#define SHELL_CMD_SIZE        80

typedef struct shell
{
    uint8_t echo_mode;
    uint8_t current_history;
    uint8_t history_count;
    char cmd_history[SHELL_HISTORY_LINES][SHELL_CMD_SIZE];
    char line[SHELL_CMD_SIZE];
    uint16_t line_position;
    uint16_t line_curpos;
}shell_t;

void shell_uart_dev_init(void);

#endif //__SHELL_UART_DEV__
