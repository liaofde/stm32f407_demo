#ifndef __UART_BSP_H__
#define __UART_BSP_H__

#if defined( __cplusplus )
extern "C" {
#endif
  
#include "NuMicro.h"
#include "config_info_dct/config_info_dct.h"
#include "freertos.h"
#include "semphr.h"   
#include "stream_buffer.h"
  
#define RX_TX_BUFFER_LEN 1024

typedef enum
{
  UART0_OBJ,
  UART1_OBJ,
  UART2_OBJ,
  UART3_OBJ,
  UART4_OBJ,
  UART5_OBJ,
  UART_OBJ_NUM_MAX
}uart_obj_id_t;

typedef struct
{
  uint32_t baudrate;
  uint32_t databits;
  uint32_t parity;
  uint32_t stopbits;
}uart_var_t;

typedef struct uart_obj
{
  UART_T           *UART;
  IRQn_Type         UART_IRQ;
  uart_var_t        var;
  uint8_t           tx_idle;
  uint8_t           rx_idle;
  StreamBufferHandle_t rx_buffer;
  StreamBufferHandle_t tx_buffer;
  void (*rs485_rxen)(void);
  void (*rs485_txen)(void);
}uart_obj_t;

int uart_obj_open (uart_obj_id_t obj_id , baudrate_t baudrate, databits_t databits,  parity_t parity, stopbits_t stopbits);
int uart_obj_close(uart_obj_id_t obj_id );
int uart_obj_read (uart_obj_id_t obj_id , uint8_t *buf, int size, uint32_t waittime);
int uart_obj_write(uart_obj_id_t obj_id , uint8_t *buf, int size);
int uart_obj_ioctl(uart_obj_id_t obj_id , baudrate_t baudrate, databits_t databits,  parity_t parity, stopbits_t stopbits);
int uart_obj_ioctl_rs485(uart_obj_id_t obj_id, void(*rs485_rxen)(void), void (*rs485_txen)(void) );
int uart_obj_ioctl_is_txidle(uart_obj_id_t obj_id);
int uart_obj_ioctl_is_rxidle(uart_obj_id_t obj_id);
int uart_obj_ioctl_query(uart_obj_id_t obj_id , uint32_t **baudrate, uint32_t **databits,  uint32_t **parity, uint32_t **stopbits);

void uart_obj_isr (uart_obj_id_t obj_id );

#if defined( __cplusplus )
}
#endif

#endif
