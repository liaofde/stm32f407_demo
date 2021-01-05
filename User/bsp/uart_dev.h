#ifndef __UART_DEV_H__
#define __UART_DEV_H__

#include "FreeRTOS.h"
#include "semphr.h"   
#include "uart_bsp.h"
#include "ring_buffer.h"
#include "dlist.h"

typedef struct _uart_dev_arg
{
  UART_HandleTypeDef *huart;
  USART_TypeDef      *USART;
  UART_InitTypeDef   *InitInfo;
  void (*rs485_rxen)(void);
  void (*rs485_txen)(void);
  uint16_t            rx_ringbuff_size;
  uint8_t             rxbyte;
}uart_dev_arg_t;

typedef struct _uart_dev
{
  UART_HandleTypeDef *huart;
  uart_dev_arg_t     *arg;
  RINGBUFF_T         *rx_ringbuff;
  uint8_t            *tx_buff;
  uint16_t            tx_buff_len;
  void (*rs485_rxen)(void);
  void (*rs485_txen)(void);
  void (*specific_rx_isv)(void);
  void (*specific_tx_isv)(void);
  struct list_head    node_list;
  uint8_t            *dev_name;
  SemaphoreHandle_t   rx_idle_semaph;
  SemaphoreHandle_t   tx_cplt_semaph;
  uint8_t             tx_busy:1;
  uint8_t             rx_busy:1;
}uart_dev_t;

void uart_dev_close(uart_dev_t* dev);
int uart_dev_open(uart_dev_t* dev, uart_dev_arg_t *arg);
int uart_dev_read(uart_dev_t* dev, uint8_t *buf, int size, uint32_t timeout);
int uart_dev_write(uart_dev_t* dev, uint8_t *buf, int size);
int uart_dev_ioctl(uart_dev_t* dev, uart_dev_arg_t *arg);
uint8_t uart_dev_write_is_busy(uart_dev_t* dev);
uint8_t uart_dev_read_is_busy(uart_dev_t* dev);

#endif