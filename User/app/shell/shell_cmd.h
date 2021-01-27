#ifndef __SHELL_CMD_H__
#define __SHELL_CMD_H__

typedef uint16_t (*log_act_t)(void*, char *help_info);

typedef struct {
	char *log_cmd;
	log_act_t act;
        char *help_info;
}shell_cmd_t;

uint16_t shell_cmd_handler(char *data, uint16_t len);

#endif
