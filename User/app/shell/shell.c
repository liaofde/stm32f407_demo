#include "shell.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "stdbool.h"
#include "usrapi.h"

osThreadId ShellTaskHandle;

void shell_uart_obj_init(void)
{
  uart_obj_open(UART1_OBJ, 115200, UART_PARITY_NONE, UART_WORDLENGTH_8B, UART_STOPBITS_1);
}

static const uint8_t key_up[3]    ={0x1b, 0x5b ,0x41};//up
static const uint8_t key_down[3]  ={0x1b, 0x5b ,0x42};//down
static const uint8_t key_right[3] ={0x1b, 0x5b ,0x43};//right
static const uint8_t key_left[3]  ={0x1b, 0x5b ,0x44};//left
//static const uint8_t key_f1[3]    ={0x1b, 0x4f ,0x50};//f1
//static const uint8_t key_f2[3]    ={0x1b, 0x4f ,0x51};//f2
//static const uint8_t key_f3[3]    ={0x1b, 0x4f ,0x52};//f3
//static const uint8_t key_f4[3]    ={0x1b, 0x4f ,0x53};//f4

#define SHELL_PROMPT ">"

static bool shell_handle_history(struct shell *shell)
{
#if defined(_WIN32)
  int i;
  shell_printf("\r");
  
  for (i = 0; i <= 60; i++)
    putchar(' ');
  shell_printf("\r");
  
#else
  shell_printf("\033[2K\r");
#endif
  shell_printf("%s%s", SHELL_PROMPT, shell->line);
  return false;
}

static void shell_push_history(struct shell *shell)
{
  if (shell->line_position != 0)
  {
    /* push history */
    if (shell->history_count >= SHELL_HISTORY_LINES)
    {
      /* if current cmd is same as last cmd, don't push */
      if (memcmp(&shell->cmd_history[SHELL_HISTORY_LINES - 1], shell->line, SHELL_CMD_SIZE))
      {
        /* move history */
        int index;
        for (index = 0; index < SHELL_HISTORY_LINES - 1; index ++)
        {
          memcpy(&shell->cmd_history[index][0],
                 &shell->cmd_history[index + 1][0], SHELL_CMD_SIZE);
        }
        memset(&shell->cmd_history[index][0], 0, SHELL_CMD_SIZE);
        memcpy(&shell->cmd_history[index][0], shell->line, shell->line_position);
        
        /* it's the maximum history */
        shell->history_count = SHELL_HISTORY_LINES;
      }
    }
    else
    {
      /* if current cmd is same as last cmd, don't push */
      if (shell->history_count == 0 || memcmp(&shell->cmd_history[shell->history_count - 1], shell->line, SHELL_CMD_SIZE))
      {
        shell->current_history = shell->history_count;
        memset(&shell->cmd_history[shell->history_count][0], 0, SHELL_CMD_SIZE);
        memcpy(&shell->cmd_history[shell->history_count][0], shell->line, shell->line_position);
        
        /* increase count and set current history position */
        shell->history_count ++;
      }
    }
  }
  shell->current_history = shell->history_count;
}

extern log_item_t log_item[];
static void shell_auto_complete(struct shell *shell)
{
  log_item_t *pt = log_item;
  log_item_t *item=NULL;
  
  if (shell->line_position != 0)
  {
    uint16_t i=0;
    while(pt->log_cmd)
    {
      if(strstr(pt->log_cmd, shell->line) == pt->log_cmd)
      {
        i++;
        if(i==1)
          item = pt;
        else 
          break;
      }
      pt++;
    }
    if(i==1)
    {
      uint8_t len = strlen(item->log_cmd);
      shell_printf("%s", &item->log_cmd[shell->line_curpos]);
      strcpy(shell->line, item->log_cmd);
      shell->line_position = shell->line_curpos = len;
    }
    else if(i>1)
    {
      shell_printf("\r\n");
      pt = log_item;
      while(pt->log_cmd)
      {
        if(strstr(pt->log_cmd, shell->line) == pt->log_cmd)
        {
          shell_printf("%s\t", pt->log_cmd);
        }
        pt++;
      }
      shell_printf("\r\n"SHELL_PROMPT);
      shell_printf("%s",shell->line);
    }
  }
}

void ShellTask(void const * argument)
{
  /* USER CODE BEGIN ShellTask */
  uint8_t buf[64];
  uint8_t len;
  shell_t shell_obj;
  shell_uart_obj_init();
  shell_t *shell = &shell_obj;
  uint8_t offset;
  uint8_t ch;
  
  shell->echo_mode = 1;
  shell_printf("\r\n"SHELL_PROMPT);

  while(1)
  {
    len = uart_obj_read(UART1_OBJ, buf,64, 0xffffffff);
    offset = 0;
    while(offset < len)
    {
      if(buf[offset] == 0x1b)
      {
        if(memcmp(key_up, &buf[offset], 3) == 0)
        {
          /* prev history */
          if (shell->current_history > 0)
            shell->current_history --;
          else
          {
            shell->current_history = 0;
            offset += 3;
            continue;
          }
          
          /* copy the history command */
          memcpy(shell->line, &shell->cmd_history[shell->current_history][0],
                 SHELL_CMD_SIZE);
          shell->line_curpos = shell->line_position = strlen(shell->line);
          shell_handle_history(shell);
        }
        else if(memcmp(key_down, &buf[offset], 3) == 0)
        {
          if (shell->current_history < shell->history_count - 1)
            shell->current_history ++;
          else
          {
            /* set to the end of history */
            if (shell->history_count != 0)
              shell->current_history = shell->history_count - 1;
            else
            {
              offset += 3;
              continue;
            }
          }
          
          memcpy(shell->line, &shell->cmd_history[shell->current_history][0],
                 SHELL_CMD_SIZE);
          shell->line_curpos = shell->line_position = strlen(shell->line);
          shell_handle_history(shell);
        }
        else if(memcmp(key_right, &buf[offset], 3) == 0)
        {
          if (shell->line_curpos < shell->line_position)
          {
            shell_printf("%c", shell->line[shell->line_curpos]);
            shell->line_curpos ++;
          }
        }
        else if(memcmp(key_left, &buf[offset], 3) == 0)
        {
          if(shell->line_curpos)
          {
            shell_printf("\b");
            shell->line_curpos --;
          }
        }
        offset += 3;
        continue;
      }
      else if(buf[offset] == 0x08 || buf[offset] == 0x7f)
      {
        /* note that shell->line_curpos >= 0 */
        if (shell->line_curpos == 0)
        {  
          offset += 1;
          continue;
        }
        shell->line_position--;
        shell->line_curpos--;
        
        if (shell->line_position > shell->line_curpos)
        {
          int i;
          
          memmove(&shell->line[shell->line_curpos],
                  &shell->line[shell->line_curpos + 1],
                  shell->line_position - shell->line_curpos);
          shell->line[shell->line_position] = 0;
          
          shell_printf("\b%s  \b", &shell->line[shell->line_curpos]);
          
          /* move the cursor to the origin position */
          for (i = shell->line_curpos; i <= shell->line_position; i++)
            shell_printf("\b");
        }
        else
        {
          shell_printf("\b \b");
          shell->line[shell->line_position] = 0;
        }
        offset += 1;
        continue;
      }
      else if(buf[offset] == '\r' || buf[offset] == '\n')
      {
        shell_push_history(shell);
        
        uint16_t len = strlen(shell->line);
        if(len > 0)
        {
          char *msg = NULL;
          msg = (char *)pvPortMalloc(512);
          if(msg != NULL)
          {
            memcpy(msg, shell->line, len+1);
            len = shell_cmd_handler(msg, len);
            if(len)
              shell_printf("\r\n%s\r\n", msg);
            vPortFree(msg);
          }
        }
        
        memset(shell->line, 0, sizeof(shell->line));
        shell->line_curpos = shell->line_position = 0;
        
        shell_printf("\r\n"SHELL_PROMPT);
        if(buf[offset++] == '\n')
          offset ++;
        offset ++;
        continue;
      }
      else if(buf[offset] == 0x09)//table
      {
        offset++;
        shell_auto_complete(shell);
      }
      else if(buf[offset] == 0x03)//ctrl+c
      {
        offset++;
        memset(shell->line, 0, shell->line_curpos);
        shell->line_curpos=0;
        shell->line_position = 0;
        shell_printf("^C\r\n"SHELL_PROMPT);
      }
      else
      {
        ch = buf[offset];
        /* it's a large line, discard it */
        if (shell->line_position >= SHELL_CMD_SIZE)
          shell->line_position = 0;
        
        /* normal character */
        if (shell->line_curpos < shell->line_position)
        {
          int i;
          
          memmove(&shell->line[shell->line_curpos + 1],
                  &shell->line[shell->line_curpos],
                  shell->line_position - shell->line_curpos);
          shell->line[shell->line_curpos] = ch;
          if (shell->echo_mode)
            shell_printf("%s", &shell->line[shell->line_curpos]);
          
          /* move the cursor to new position */
          for (i = shell->line_curpos; i < shell->line_position; i++)
            shell_printf("\b");
        }
        else
        {
          shell->line[shell->line_position] = ch;
          if (shell->echo_mode)
            shell_printf("%c", ch);
        }
        
        ch = 0;
        shell->line_position ++;
        shell->line_curpos++;
        if (shell->line_position >= SHELL_CMD_SIZE)
        {
          /* clear command line */
          shell->line_position = 0;
          shell->line_curpos = 0;
          memset(shell->line, 0, sizeof(shell->line));
        }
        offset++;
        continue;
      }
    }
  }
}

int shell_thread_entry(void)
{
  osThreadDef(shell, ShellTask, osPriorityNormal, 0, 512);
  ShellTaskHandle = osThreadCreate(osThread(shell), NULL);
  
  return 0;
}

THREAD_ENTRY_EXPORT(shell_thread_entry,0);