#ifndef __SHELL_CMD_H__
#define __SHELL_CMD_H_

typedef uint16_t (*log_act_t)(void*, char *help_info);

typedef struct {
	char *log_cmd;
	log_act_t act;
        char *help_info;
}log_item_t;

uint16_t shell_cmd_handler(char *data, uint16_t len);

#endif
