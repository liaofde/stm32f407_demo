#include "uart_dev.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "string.h"

#define malloc pvPortMalloc
#define free   vPortFree


LIST_HEAD(uart_dev_list);

int uart_dev_open(uart_dev_t* dev, uart_dev_arg_t *arg)
{
  if(dev == NULL || arg == NULL)
    return -1;
  dev->arg = arg;
  
  if(arg == NULL || arg->huart == NULL || arg->USART == NULL)
    return -1;
  
  dev->huart = arg->huart;
  dev->rx_ringbuff = malloc(sizeof(RINGBUFF_T));
  dev->rs485_rxen = arg->rs485_rxen;
  dev->rs485_txen = arg->rs485_txen;
  
  if(NULL == dev->rx_ringbuff)
  {
    return -1;
  }
  if(arg->rx_ringbuff_size<64)
    arg->rx_ringbuff_size = 64;
  else if(arg->rx_ringbuff_size > 2048)
    arg->rx_ringbuff_size = 2048;
  uint8_t *pool  = malloc(arg->rx_ringbuff_size);
  if(NULL == pool)
  {
    free(dev->rx_ringbuff);
    return -1;
  }
  ringbuffer_init(dev->rx_ringbuff, pool, arg->rx_ringbuff_size);
 
  list_add(&(dev->node_list), &uart_dev_list);
  
  vSemaphoreCreateBinary(dev->rx_idle_semaph);
  xSemaphoreTake( dev->rx_idle_semaph, ( TickType_t ) 0);
  vSemaphoreCreateBinary(dev->tx_cplt_semaph);

  if(dev->rs485_rxen) 
    dev->rs485_rxen();
  bsp_uart_init(arg->huart, arg->USART, arg->InitInfo, &(dev->arg->rxbyte));
  return 0;
}

void uart_dev_close(uart_dev_t* dev)
{
  struct list_head *list;

  bsp_uart_deInit(dev->huart);
  free(dev->rx_ringbuff->buffer_ptr);
  free(dev->rx_ringbuff);
  
  list_for_each(list, &uart_dev_list)
  {
    if(list == &dev->node_list)
    {
      list_del(list);
      break;
    }
  }
}

int uart_dev_ioctl(uart_dev_t *dev, uart_dev_arg_t *arg)
{
  bsp_uart_init(arg->huart, arg->USART, arg->InitInfo, &(dev->arg->rxbyte));
  return 0;
}

int uart_dev_read(uart_dev_t* dev, uint8_t *buf, int len, uint32_t timeout)
{
  int ret = -1;
  SemaphoreHandle_t xSemaphore = dev->rx_idle_semaph;
  
  if( xSemaphore != NULL )
  {
    if(__get_IPSR() != 0)
    {
      xSemaphoreTakeFromISR( xSemaphore, NULL);
    }
    else
    {
      xSemaphoreTake( xSemaphore, ( TickType_t ) timeout );
    }
  }
  ret = ringbuffer_get(dev->rx_ringbuff, buf, len);
  return ret;
}

int uart_dev_write(uart_dev_t* dev, uint8_t *buf, int size)
{
  int ret = 0;
  SemaphoreHandle_t xSemaphore = dev->tx_cplt_semaph;
  uint8_t *tx_buff =  NULL;
  
  if( xSemaphore != NULL )
  {
    if(__get_IPSR() != 0)
    {
      if( xSemaphoreTakeFromISR( xSemaphore, NULL ) == pdTRUE )
      {
        tx_buff = dev->tx_buff;
        if(tx_buff == NULL)
          tx_buff = buf;
        else
          memcpy(tx_buff,buf,dev->tx_buff_len>=size? size : dev->tx_buff_len);
      }
    }
    else
    {
      if( xSemaphoreTake( xSemaphore, ( TickType_t ) osWaitForever ) == pdTRUE )
      {
        tx_buff = dev->tx_buff;
        if(tx_buff != NULL && dev->tx_buff_len < size)
        {
          free(tx_buff);
          tx_buff = NULL;
        }
        
        if(tx_buff == NULL)
        {
          dev->tx_buff = tx_buff = malloc(size);
          dev->tx_buff_len = size;
        }
        
        if(tx_buff != NULL)
          memcpy(tx_buff,buf,size);
        
      }
    }
  }
  else
  {
    tx_buff = buf;
  }
  
  if(tx_buff)
  {
    if(dev->rs485_txen) 
      dev->rs485_txen();
    dev->tx_busy = 1;
    ret = bsp_uart_send(dev->huart, tx_buff, size);
    
  }
  else 
    ret=-1;

  if(ret == -1)
    size = 0;
  

  return size;
}

uint8_t uart_dev_read_is_busy(uart_dev_t* dev)
{
  return dev->rx_busy;
}

uint8_t uart_dev_write_is_busy(uart_dev_t* dev)
{
  return dev->tx_busy;
}

#if UART_ISR_METHOD == 1
void uart_dev_rxcplt_isr(UART_HandleTypeDef *huart)
{
  struct list_head *list;
  uart_dev_t* dev;
  BaseType_t xHigherPriorityTaskWoken=pdFALSE;
  SemaphoreHandle_t xSemaphore;
  
  list_for_each(list, &uart_dev_list)
  {
    dev = list_entry(list, uart_dev_t , node_list);
    if(huart == dev->huart)
    {
      if(dev->specific_rx_isv)
        dev->specific_rx_isv();
      dev->rx_busy = 1;
      ringbuffer_putchar(dev->rx_ringbuff, dev->arg->rxbyte);
      bsp_uart_rcv_start(dev->huart, &dev->arg->rxbyte);
      if(ringbuffer_data_len(dev->rx_ringbuff) >= (dev->rx_ringbuff->buffer_size>>1))
      {
        xSemaphore = dev->rx_idle_semaph;
        if(xSemaphore != NULL)
          xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
      }
      
      break;
    }
  }
}
#endif


void uart_dev_rxidle_isr(UART_HandleTypeDef *huart)
{
  struct list_head *list;
  uart_dev_t* dev;
  BaseType_t xHigherPriorityTaskWoken=pdFALSE;
  SemaphoreHandle_t xSemaphore;
  
  list_for_each(list, &uart_dev_list)
  {
    dev = list_entry(list, uart_dev_t , node_list);
    if(huart == dev->huart)
    {
        /* Unblock the task by releasing the semaphore. */
      bsp_uart_rcv_start(dev->huart, &dev->arg->rxbyte);
      xSemaphore = dev->rx_idle_semaph;
      if(xSemaphore != NULL)
        xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
      dev->rx_busy = 0;
      break;
    }
  }
  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void uart_dev_txcplt_isr(UART_HandleTypeDef *huart)
{
  struct list_head *list;
  uart_dev_t* dev;
  BaseType_t xHigherPriorityTaskWoken=pdFALSE;
  SemaphoreHandle_t xSemaphore;
  
  list_for_each(list, &uart_dev_list)
  {
    dev = list_entry(list, uart_dev_t , node_list);
    if(huart == dev->huart)
    {
      if(dev->rs485_rxen) 
        dev->rs485_rxen();
        /* Unblock the task by releasing the semaphore. */
      dev->tx_busy = 0;
      xSemaphore = dev->tx_cplt_semaph;
      if(xSemaphore != NULL)
        xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
      if(dev->specific_tx_isv)
        dev->specific_tx_isv();
      break;
    }
  }
  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

