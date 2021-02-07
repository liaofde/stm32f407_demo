#ifndef __UART_BSP_H__
#define __UART_BSP_H__

#include "usart.h"
#include "bsp_isr_def.h"
#include "ring_buffer.h"
#include "dlist.h"
#include "FreeRTOS.h"
#include "semphr.h"   
#include "stream_buffer.h"

#define UART1_OBJ_EN
#define UART2_OBJ_EN
#define UART3_OBJ_EN
#define UART4_OBJ_EN
#define xUART5_OBJ_EN
#define xUART6_OBJ_EN

typedef enum
{
#ifdef UART1_OBJ_EN
  UART1_OBJ,
#endif
#ifdef UART2_OBJ_EN
  UART2_OBJ,
#endif
#ifdef UART3_OBJ_EN
  UART3_OBJ,
#endif
#ifdef UART4_OBJ_EN
  UART4_OBJ,
#endif
#ifdef UART5_OBJ_EN
  UART5_OBJ,
#endif
#ifdef UART6_OBJ_EN
  UART6_OBJ,
#endif
  UART_OBJ_NUM_MAX
}uart_obj_id_t;

typedef struct _uart_obj
{
  UART_HandleTypeDef *huart;
  uint8_t            *obj_name;
  StreamBufferHandle_t rx_buffer;
  uint8_t            *tx_buff;
  uint16_t            tx_buff_len;
  uint16_t            rx_buffer_size;
  void (*rs485_rxen)(void);
  void (*rs485_txen)(void);
  void (*specific_rx_isr)(void);
  void (*specific_tx_isr)(void);
  SemaphoreHandle_t   rx_sema;
  SemaphoreHandle_t   tx_sema;
  uint8_t             rx_stat:1;
  uint8_t             tx_stat:1;
  uint8_t             rxbyte;
}uart_obj_t;

typedef struct
{
  uint32_t baudrate;
  uint32_t parity;
  uint32_t databits;
  uint32_t stopbits;
}uart_var_t;

void uart_obj_close(uart_obj_id_t obj_id);
int uart_obj_open(uart_obj_id_t obj_id, uint32_t baudrate, uint32_t parity, uint32_t databits, uint32_t stopbits);
int uart_obj_read(uart_obj_id_t obj_id, uint8_t *buf, int size, uint32_t timeout);
int uart_obj_write(uart_obj_id_t obj_id, uint8_t *buf, int size);
int uart_obj_ioctl(uart_obj_id_t obj_id, uint32_t baudrate, uint32_t parity, uint32_t databits, uint32_t stopbits);
int uart_obj_ioctl_rs485_register(uart_obj_id_t obj_id, void(*rs485_rxen)(void), void (*rs485_txen)(void) );
int uart_obj_ioctl_specific_rtx_isr_register(uart_obj_id_t obj_id, void(*specific_rx_isr)(void), void (*specific_tx_isr)(void) );
int uart_obj_ioctl_get_info(uart_obj_id_t obj_id, uint32_t *baudrate, uint32_t *parity, uint32_t *databits, uint32_t *stopbits);
int uart_obj_ioctl_alloc_rx_buffer(uart_obj_id_t obj_id, uint16_t size);
int uart_obj_ioctl_is_tx_busy(uart_obj_id_t obj_id);
int uart_obj_ioctl_is_rx_busy(uart_obj_id_t obj_id);

#endif