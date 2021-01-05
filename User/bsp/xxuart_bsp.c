#include "uart_bsp.h"
#include "cmsis_os.h"
#include "freertos.h"
#include "stream_buffer.h"

enum
{
 UART_RX_BUSY_STATUS       = 0,
 UART_RX_IDLE_STATUS       = 1,
 UART_RX_CKECK_BUSY_STATUS = 2,
 UART_RX_GET_DATA_STATUS   = 5,

 UART_TX_BUSY_STATUS       = 0,
 UART_TX_IDLE_STATUS       = 1,
};

uart_obj_t uart_obj_list[UART_OBJ_NUM_MAX];

int uart_obj_open(uart_obj_id_t obj_id, baudrate_t baudrate, databits_t databits,  parity_t parity, stopbits_t stopbits)
{
  if(obj_id >= UART_OBJ_NUM_MAX || uart_obj_list[obj_id].rx_buffer != NULL)
    return -1;

  uart_obj_list[obj_id].var.baudrate = baudrate;
  uart_obj_list[obj_id].var.databits = databits;
  uart_obj_list[obj_id].var.parity   = parity;
  uart_obj_list[obj_id].var.stopbits = stopbits;
  
  uart_obj_list[obj_id].rx_buffer = xStreamBufferCreate( RX_TX_BUFFER_LEN, RX_TX_BUFFER_LEN/2 );
  
  if( uart_obj_list[obj_id].rx_buffer == NULL )
    return -1;
  
  uart_obj_list[obj_id].tx_buffer = xStreamBufferCreate( RX_TX_BUFFER_LEN, RX_TX_BUFFER_LEN/2 );
  if( uart_obj_list[obj_id].tx_buffer == NULL )
  {
    vStreamBufferDelete((StreamBufferHandle_t)uart_obj_list[obj_id].rx_buffer);
    uart_obj_list[obj_id].rx_buffer = NULL;
    return -1;
  }
  
  switch(obj_id)
  {
  case UART0_OBJ:
    uart_obj_list[obj_id].UART = UART0;
    uart_obj_list[obj_id].UART_IRQ = UART0_IRQn;
    break;
  case UART1_OBJ:
    uart_obj_list[obj_id].UART = UART1;
    uart_obj_list[obj_id].UART_IRQ = UART1_IRQn;
    break;
  case UART2_OBJ:
    uart_obj_list[obj_id].UART = UART2;
    uart_obj_list[obj_id].UART_IRQ = UART2_IRQn;
    break;
  case UART3_OBJ:
    uart_obj_list[obj_id].UART = UART3;
    uart_obj_list[obj_id].UART_IRQ = UART3_IRQn;
    break;
  case UART4_OBJ:
    uart_obj_list[obj_id].UART = UART4;
    uart_obj_list[obj_id].UART_IRQ = UART4_IRQn;
    break;
  case UART5_OBJ:
    uart_obj_list[obj_id].UART = UART5;
    uart_obj_list[obj_id].UART_IRQ = UART5_IRQn;
    break;
  }
  
  uint32_t _baudrate = (baudrate == baudrate_1200)?1200:(baudrate == baudrate_2400)?2400:\
    (baudrate == baudrate_4800)?4800:(baudrate == baudrate_9600)?9600:(baudrate == baudrate_19200)?19200:\
    (baudrate == baudrate_38400)?38400:(baudrate == baudrate_57600)?57600:(baudrate == baudrate_115200)?115200:9600;
  uint32_t _databits = (databits == databits_5)?UART_WORD_LEN_5:(databits == databits_6)?UART_WORD_LEN_6:\
    (databits == databits_7)?UART_WORD_LEN_7:UART_WORD_LEN_8;
  uint32_t _parity = (parity == parity_odd)?UART_PARITY_ODD:(parity == parity_even)?UART_PARITY_EVEN:\
    (parity == parity_mark)?UART_PARITY_MARK:(parity == parity_space)?UART_PARITY_SPACE:UART_PARITY_NONE;
  uint32_t _stopbits = (stopbits == stopbits_1)?UART_STOP_BIT_1:(stopbits == stopbits_2)?UART_STOP_BIT_2:UART_STOP_BIT_1;
  
  UART_SetLineConfig(uart_obj_list[obj_id].UART, _baudrate, _databits, _parity, _stopbits);
  //UART_Open(uart_obj_list[obj_id].UART, _baudrate);
  
  UART_SetTimeoutCnt(uart_obj_list[obj_id].UART, 0x10); // Set Rx Time-out counter

  uart_obj_list[obj_id].UART->FIFO &= ~ UART_FIFO_RFITL_Msk;
  uart_obj_list[obj_id].UART->FIFO |= UART_FIFO_RFITL_8BYTES;
  
  NVIC_SetPriority(uart_obj_list[obj_id].UART_IRQ, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
  NVIC_EnableIRQ(uart_obj_list[obj_id].UART_IRQ);
  UART_EnableInt(uart_obj_list[obj_id].UART, (UART_INTEN_RDAIEN_Msk  |  UART_INTEN_RXTOIEN_Msk | UART_INTEN_RLSIEN_Msk | UART_INTEN_BUFERRIEN_Msk));  

  uart_obj_list[obj_id].tx_idle = UART_TX_IDLE_STATUS;
  
  return 0;
}

int uart_obj_close(uart_obj_id_t obj_id)
{
  if(obj_id>=UART_OBJ_NUM_MAX)
    return -1;
  
  NVIC_DisableIRQ(uart_obj_list[obj_id].UART_IRQ);
  UART_DisableInt(uart_obj_list[obj_id].UART, (UART_INTEN_RDAIEN_Msk  |  UART_INTEN_RXTOIEN_Msk | UART_INTEN_RLSIEN_Msk | UART_INTEN_BUFERRIEN_Msk));  
  
  vStreamBufferDelete(uart_obj_list[obj_id].rx_buffer);
  vStreamBufferDelete(uart_obj_list[obj_id].tx_buffer);
  uart_obj_list[obj_id].rx_buffer = NULL;
  uart_obj_list[obj_id].tx_buffer = NULL;
  
  UART_Close(uart_obj_list[obj_id].UART);
  
  return 0;
}

int uart_obj_read(uart_obj_id_t obj_id, uint8_t *buf, int size, uint32_t waittime)
{
  int ret;
  uint16_t vaild_len;
  
  if(uart_obj_list[obj_id].rx_idle == UART_RX_BUSY_STATUS )
  {
    uart_obj_list[obj_id].rx_idle = UART_RX_CKECK_BUSY_STATUS;
  }
  else if(uart_obj_list[obj_id].rx_idle >= UART_RX_CKECK_BUSY_STATUS )
  {
    uart_obj_list[obj_id].rx_idle++;
    osDelay(10);
  }
  vaild_len = RX_TX_BUFFER_LEN/2;
  if(uart_obj_list[obj_id].rx_idle >= UART_RX_GET_DATA_STATUS || xStreamBufferBytesAvailable(uart_obj_list[obj_id].rx_buffer) >= vaild_len)
  {
    if(uart_obj_list[obj_id].rx_idle >= UART_RX_GET_DATA_STATUS)
      uart_obj_list[obj_id].rx_idle = UART_RX_IDLE_STATUS;
    else
      size = vaild_len;
    ret = xStreamBufferReceive( uart_obj_list[obj_id].rx_buffer, ( void * ) buf, size, 0 );
  }
  else
  {
    osDelay(10);
    ret = 0;
  }
  return ret;
}

int uart_obj_write(uart_obj_id_t obj_id, uint8_t *buf, int size)
{
  int ret;
  if(buf == NULL || size == 0)
    return 0;
  
  ret = xStreamBufferSend(uart_obj_list[obj_id].tx_buffer, buf, size, 0xffffffff);
  if(uart_obj_list[obj_id].tx_idle == UART_TX_IDLE_STATUS && ret)
  {
    if(uart_obj_list[obj_id].rs485_txen != NULL)
      uart_obj_list[obj_id].rs485_txen();
    uart_obj_list[obj_id].tx_idle = UART_TX_BUSY_STATUS;
    UART_EnableInt(uart_obj_list[obj_id].UART, UART_INTEN_TXENDIEN_Msk);
  }

  return ret;
}

int uart_obj_ioctl(uart_obj_id_t obj_id, baudrate_t baudrate, databits_t databits,  parity_t parity, stopbits_t stopbits)
{
  uart_obj_list[obj_id].var.baudrate = baudrate;
  uart_obj_list[obj_id].var.parity   = parity;
  uart_obj_list[obj_id].var.databits = databits;
  uart_obj_list[obj_id].var.stopbits = stopbits;
  
  UART_DisableInt(uart_obj_list[obj_id].UART, (UART_INTEN_RDAIEN_Msk  |  UART_INTEN_RXTOIEN_Msk));  
  NVIC_DisableIRQ(uart_obj_list[obj_id].UART_IRQ);
  
  uint32_t _baudrate = (baudrate == baudrate_1200)?1200:(baudrate == baudrate_2400)?2400:\
    (baudrate == baudrate_4800)?4800:(baudrate == baudrate_9600)?9600:(baudrate == baudrate_19200)?19200:\
    (baudrate == baudrate_38400)?38400:(baudrate == baudrate_57600)?57600:(baudrate == baudrate_115200)?115200:9600;
  uint32_t _databits = (databits == databits_5)?UART_WORD_LEN_5:(databits == databits_6)?UART_WORD_LEN_6:\
    (databits == databits_7)?UART_WORD_LEN_7:UART_WORD_LEN_8;
  uint32_t _parity = (parity == parity_odd)?UART_PARITY_ODD:(parity == parity_even)?UART_PARITY_EVEN:\
    (parity == parity_mark)?UART_PARITY_MARK:(parity == parity_space)?UART_PARITY_SPACE:UART_PARITY_NONE;
  uint32_t _stopbits = (stopbits == stopbits_1)?UART_STOP_BIT_1:(stopbits == stopbits_2)?UART_STOP_BIT_2:UART_STOP_BIT_1;
  
  UART_SetLineConfig(uart_obj_list[obj_id].UART, _baudrate, _databits, _parity, _stopbits);
  //UART_Open(uart_obj_list[obj_id].UART, _baudrate);
  
  UART_SetTimeoutCnt(uart_obj_list[obj_id].UART, 0x10); // Set Rx Time-out counter

  uart_obj_list[obj_id].UART->FIFO &= ~ UART_FIFO_RFITL_Msk;
  uart_obj_list[obj_id].UART->FIFO |= UART_FIFO_RFITL_8BYTES;
  

  NVIC_SetPriority(uart_obj_list[obj_id].UART_IRQ, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
  NVIC_EnableIRQ(uart_obj_list[obj_id].UART_IRQ);
  UART_EnableInt(uart_obj_list[obj_id].UART, (UART_INTEN_RDAIEN_Msk  |  UART_INTEN_RXTOIEN_Msk | UART_INTEN_RLSIEN_Msk | UART_INTEN_BUFERRIEN_Msk));  

  return 0;
}

int uart_obj_ioctl_rs485(uart_obj_id_t obj_id, void(*rs485_rxen)(void), void (*rs485_txen)(void) )
{
  uart_obj_list[obj_id].rs485_rxen   = rs485_rxen;
  uart_obj_list[obj_id].rs485_txen   = rs485_txen;
  
  return 0;
}

int uart_obj_ioctl_is_txidle(uart_obj_id_t obj_id)
{
  return uart_obj_list[obj_id].tx_idle;
}

int uart_obj_ioctl_is_rxidle(uart_obj_id_t obj_id)
{
  return uart_obj_list[obj_id].rx_idle;
}

int uart_obj_ioctl_query(uart_obj_id_t obj_id , uint32_t **baudrate, uint32_t **databits,  uint32_t **parity, uint32_t **stopbits)
{
  *baudrate = &uart_obj_list[obj_id].var.baudrate;
  *databits = &uart_obj_list[obj_id].var.databits;
  *parity   = &uart_obj_list[obj_id].var.parity;
  *stopbits = &uart_obj_list[obj_id].var.stopbits;
  
  return 0;
}

void uart_obj_isr(uart_obj_id_t obj_id)
{
  UART_T  *UART = uart_obj_list[obj_id].UART;
  uint32_t u32IntSts = UART->INTSTS;
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if(u32IntSts & UART_INTSTS_BUFERRINT_Msk)
  {
    UART_ClearIntFlag(UART, UART_INTSTS_BUFERRINT_Msk);
  }
  
  if(u32IntSts & UART_INTSTS_RLSINT_Msk)
  {
    UART_ClearIntFlag(UART, UART_INTSTS_RLSINT_Msk);
  }
  
  if((u32IntSts & UART_INTSTS_RDAINT_Msk) || (u32IntSts & UART_INTSTS_RXTOINT_Msk) || (u32IntSts & UART_INTSTS_RLSINT_Msk))//rx received data
  {
    uint8_t u8InChar;
    uint8_t buf[16];
    uint8_t size=0;
    while(UART_GET_RX_EMPTY(UART) == 0)//rx received data
    {
      /* Get the character from UART Buffer */
      u8InChar = UART_READ(UART);
      buf[size++] = u8InChar;
      uart_obj_list[obj_id].rx_idle = 0;
      if(size>=15)
      {
        break;
      }
    }
    if(size>0)
    {
      xStreamBufferSendFromISR(uart_obj_list[obj_id].rx_buffer, buf, size, &xHigherPriorityTaskWoken);
    }
  }
  
  if(u32IntSts & UART_INTSTS_TXENDIF_Msk) //tx finished
  {
    uint8_t u8OutChar;
    while(UART_GET_TX_FULL(UART)==0)
    {
      xHigherPriorityTaskWoken = pdFALSE;
      if(xStreamBufferReceiveFromISR(uart_obj_list[obj_id].tx_buffer, &u8OutChar, 1, &xHigherPriorityTaskWoken)>0)
      {
        UART_WRITE(UART, u8OutChar);
      }
      else
      {
        UART_DisableInt(UART, UART_INTEN_TXENDIEN_Msk);
        uart_obj_list[obj_id].tx_idle = UART_TX_IDLE_STATUS;
        break;
      }
    }
  }
  portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
