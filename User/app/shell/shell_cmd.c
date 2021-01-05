#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "cmsis_os.h"
#include "freertos.h"
#include "shell_cmd.h"
#include "main.h"

#define CMD_EXECUTE_ECHO "Command is executing"

typedef uint16_t (*log_act_t)(void*, char *help_info);
typedef struct {
	char *log_cmd;
	log_act_t act;
        char *help_info;
}log_item_t;

uint16_t motor_opt(void *data, char *help_info);
uint16_t help_opt(void *data, char *help_info);
uint16_t system_opt(void *data, char *help_info);
uint16_t reboot_opt(void *data, char *help_info);

const log_item_t log_item[]=
{
  {"motor",     motor_opt,      "motor -s horiz -d (0~360.0\\left\\right) -v (1~100)\r\n\t\t eg:motor -s horiz -d 45 -v 50"},
  {"system",    system_opt,     "system \r\n\t\t eg:system"},
  {"reboot",    reboot_opt,     "reboot \r\n\t\t eg:reboot"},
  {"?",         help_opt,       "? \r\n\t\t eg:?"},
  {NULL,}
};


uint16_t system_opt(void *data, char *help_info)
{
  uint16_t ret = 0;
  
  ret =  sprintf((char *)data, "firmware version %s_r%s bulid %s %s run ticks:%d\r\nFreeHeapSize:%d\r\n",FW_VERSION, "10", __DATE__, __TIME__, HAL_GetTick(), xPortGetFreeHeapSize());

  return ret;
}

uint16_t motor_opt(void *data, char *help_info)
{
  uint16_t ret = 0;
  char argv[2][16];
  //float  fv = 0;
  int32_t degree = 0;
  int32_t speed = -1;
  int err=1;
  int i;
  
  if((sscanf(data, "%*s -s %s -d %d -v %d", argv[0], &degree, &speed)) == 3)
  {
    if(strcmp("horiz",(char const *)argv[0]) == 0)
    {
      err = 0;
      if(speed>100)
        speed = 100;
      if(degree >360)
        degree = degree%360;
      int ptz_horiz_set_speed_degree_ctrl_event_send( int16_t speed ,float degree);
      ptz_horiz_set_speed_degree_ctrl_event_send(speed, degree);
    }
  }
  else if((i=sscanf(data, "%*s -s %s -d %s -v %d", argv[0], argv[1], &speed)) == 3 || i==2)
  {
    if(strcmp("horiz",(char const *)argv[0]) == 0)
    {
      if(strcmp("left",(char const *)argv[1]) == 0 && i == 3)
      {
        err = 0;
        int ptz_horiz_mv_left_ctrl_event_send(uint16_t speed);
        ptz_horiz_mv_left_ctrl_event_send(speed);
      }
      else if(strcmp("right",(char const *)argv[1]) == 0 && i == 3)
      {
        err = 0;
        int ptz_horiz_mv_right_ctrl_event_send(uint16_t speed);
        ptz_horiz_mv_right_ctrl_event_send(speed);
      }
      else if(strcmp("stop",(char const *)argv[1]) == 0)
      {
        err = 0;
        int ptz_horiz_stop_ctrl_event_send(void);
        ptz_horiz_stop_ctrl_event_send();
      }
    }
  }
  
  if(!err)
  {
    ret = sprintf(data, CMD_EXECUTE_ECHO);
  }
  else
  {
    ret = sprintf(data, "err! %s", help_info);
  }

  return ret;
}

uint16_t log_opt(void *data, char *help_info)
{
  uint16_t ret = 0;
//  char argv[32];
//  int  v = 0;
//  sscanf(data, "log %s %d", argv, &v);
//  
//  if(strcmp("-t",argv) == 0 && v > 0)
//  {
//    set_tcplog_enable(v>60?60:v);
//  }
//  else
//  {
//    set_tcplog_enable(10);
//    ret = sprintf(data, "eg:log -t 20\r\n");
//  }
//  
  return ret;
}

uint16_t reboot_opt(void *data, char *help_info)
{
  uint16_t ret;
  
  //reboot_flag = 1;
  ret = sprintf(data, "System reboot...\r\n");
  return ret;
}

uint16_t help_opt(void *data, char *help_info)
{
  log_item_t *item = (log_item_t  *)log_item;
  uint16_t ret = 0;
  
  ret += sprintf((char *)data+ret, "cmd\t\tformat\r\n------------------------\r\n");
  while(item->log_cmd != NULL)
  {
    ret += sprintf((char *)data+ret, "%s\t\t%s\r\n", item->log_cmd, item->help_info);
    item ++;
  }
  
  return ret;
}

uint16_t shell_cmd_handler(char *data, uint16_t len)
{
  log_item_t *item = (log_item_t *)log_item;
  int res = -1;
  int str_len;
  uint16_t ret = 0;
  
  if(len>0)
  {
    while(item->log_cmd != NULL)
    {
      str_len = strlen(item->log_cmd);
      if(memcmp(data, item->log_cmd, str_len) == 0 && (data[str_len] == ' ' || data[str_len] == '\0' || data[str_len] == '\r' ||data[str_len] == '\n'))
      {
        res = 0;
        if(item->act)
          ret = item->act(data, item->help_info);
        break;
      }
      else 
      {
        item++;
      }
    }
    if(res == -1)
    {
      ret=sprintf(data, "command not found! enter \"?\" for help\r\n");
    }
  }
  //ret += sprintf(data+ret, "\r\n%s", CMDLINE);
  return ret;
}