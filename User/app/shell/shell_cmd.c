#include "cmsis_os.h"
#include "freertos.h"
#include "shell_cmd.h"
#include "main.h"

#define CMD_EXECUTE_ECHO "Command is executing"

uint16_t motor_opt(void *data, char *help_info);
uint16_t help_opt(void *data, char *help_info);
uint16_t system_opt(void *data, char *help_info);
uint16_t reboot_opt(void *data, char *help_info);
uint16_t restore_opt(void *data, char *help_info);

const shell_cmd_t shell_cmd[]=
{
/*************不可更改区域 开始 ********************/
  {"?",         help_opt,       "?"},
  {"system",    system_opt,     "system"},
  {"reboot",    reboot_opt,     "reboot"},
/*************不可更改区域 结束 ********************/
  {"restore",   restore_opt,    "restore"},
  {"motor",     motor_opt,      "motor -s horiz -d (0~360.0\\left\\right) -v (1~100)\r\n\t\t eg:motor -s horiz -d 45 -v 50"},
  {NULL,}
};


uint16_t system_opt(void *data, char *help_info)
{
  uint16_t ret = 0;
  
  ret =  sprintf((char *)data, "\r\nfirmware version %s_r%s bulid %s %s\r\nrun ticks:%d\r\nFreeHeapSize:%d\r\n",FW_VERSION, "10", __DATE__, __TIME__, HAL_GetTick(), xPortGetFreeHeapSize());
  ret +=  sprintf((char *)data+ret,"=================================================\r\n");
  ret +=  sprintf((char *)data+ret,"task_name\tstate\tprior\trtack\tId\r\n");
  osThreadList((uint8_t *)data+ret);
  ret = strlen(data);
  
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

uint16_t restore_opt(void *data, char *help_info)
{
  uint16_t ret;
  
  //reboot_flag = 1;
  ret = sprintf(data, "restore ok\r\n");
  return ret;
}

uint16_t help_opt(void *data, char *help_info)
{
  shell_cmd_t *item = (shell_cmd_t  *)shell_cmd;
  uint16_t ret = 0;
  
  ret +=  sprintf((char *)data+ret, "firmware version %s_r%s bulid %s %s\r\nrun ticks:%d\r\nFreeHeapSize:%d\r\n",FW_VERSION, "10", __DATE__, __TIME__, HAL_GetTick(), xPortGetFreeHeapSize());
  item ++;
  ret += sprintf((char *)data+ret, "cmd\r\n----\r\n");
  while(item->log_cmd != NULL)
  {
    ret += sprintf((char *)data+ret, "%s\r\n", item->log_cmd);
    item ++;
  }
  
  return ret;
}

uint16_t shell_cmd_handler(char *data, uint16_t len)
{
  shell_cmd_t *item = (shell_cmd_t *)shell_cmd;
  int res = -1;
  int str_len;
  uint16_t ret = 0;
  uint8_t help=0;
  
  if(len>0)
  {
    while(item->log_cmd != NULL)
    {
      str_len = strlen(item->log_cmd);
      if(memcmp(data, item->log_cmd, str_len) == 0 && (data[str_len] == ' ' || data[str_len] == '\0' || data[str_len] == '\r' ||data[str_len] == '\n'))
      {
        res = 0;
        int i=sscanf(data, "%*s %c", &help);
        if(i==1 && help == '?')
          ret = sprintf(data+ret, "%s\r\n", item->help_info!=NULL?item->help_info:"");
        else if(item->act)
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