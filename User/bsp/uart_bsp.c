#include "stdio.h"
#include "uart_bsp.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "string.h"

#define malloc pvPortMalloc
#define free   vPortFree

#define LOG_UART_HANDLE huart1

enum
{
  _IDLE       = 0,
  _BUSY       = 1,
};

/**************************************************************************************/
/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  if(LOG_UART_HANDLE.gState != HAL_UART_STATE_BUSY_TX)
    HAL_UART_Transmit(&LOG_UART_HANDLE, (uint8_t *)&ch, 1, 0xFFFF); 

  return ch;
}

/****************************************************************************************/
static uart_obj_t uart_obj_tbl[UART_OBJ_NUM_MAX];

int uart_obj_open(uart_obj_id_t obj_id, uint32_t baudrate, uint32_t parity, uint32_t databits, uint32_t stopbits)
{
  uart_obj_t *obj = &uart_obj_tbl[obj_id];
  if(obj_id >= UART_OBJ_NUM_MAX)
    return -1;
  
  if(obj->rx_buffer == NULL)
  {
    if(obj->rx_buffer_size<64)
      obj->rx_buffer_size = 64;
    else if(obj->rx_buffer_size > 1024)
      obj->rx_buffer_size = 1024;
    obj->rx_buffer = xStreamBufferCreate( obj->rx_buffer_size, obj->rx_buffer_size/2 );
  }
  if( obj->rx_buffer == NULL )
    return -1;
  
  vSemaphoreCreateBinary(obj->rx_sema);
  xSemaphoreTake( obj->rx_sema, ( TickType_t ) 0);
  vSemaphoreCreateBinary(obj->tx_sema);

  if(obj->rs485_rxen) 
    obj->rs485_rxen();

#ifdef UART1_OBJ_EN
  if(obj_id == UART1_OBJ) 
  {
    obj->huart=&huart1;
    obj->huart->Instance = USART1;
  }
#endif
#ifdef UART2_OBJ_EN
  else if(obj_id == UART2_OBJ) 
  {
    obj->huart=&huart2;
    obj->huart->Instance = USART2;
  }
#endif
#ifdef UART3_OBJ_EN
  else if(obj_id == UART3_OBJ) 
  {
    obj->huart=&huart3;
    obj->huart->Instance = USART3;
  }
#endif
#ifdef UART4_OBJ_EN
  else if(obj_id == UART4_OBJ) 
  {
    obj->huart=&huart4;
    obj->huart->Instance = UART4;
  }
#endif
#ifdef UART5_OBJ_EN
  else if(obj_id == UART5_OBJ) 
  {
    obj->huart=&huart5;
    obj->huart->Instance = UART5;
  }
#endif
#ifdef UART6_OBJ_EN
  else if(obj_id == UART6_OBJ) 
  {
    obj->huart=&huart6;
    obj->huart->Instance = USART6;
  }
#endif
  
  obj->huart->Init.BaudRate = baudrate;
  obj->huart->Init.WordLength = databits;
  obj->huart->Init.StopBits = stopbits;
  obj->huart->Init.Parity = parity;
  obj->huart->Init.Mode = UART_MODE_TX_RX;
  obj->huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  obj->huart->Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(obj->huart) != HAL_OK)
  {
    Error_Handler();
  }
  
  HAL_UART_Receive_IT(obj->huart, &obj->rxbyte, 1);  //配置huart中断接收模式参数
  __HAL_UART_ENABLE_IT(obj->huart, UART_IT_IDLE); 
  
  return 0;
}

void uart_obj_close(uart_obj_id_t obj_id)
{
  uart_obj_t *obj = &uart_obj_tbl[obj_id];
  HAL_UART_DeInit(obj->huart);
  vStreamBufferDelete(obj->rx_buffer);
  obj->rx_buffer = NULL;
}

int uart_obj_ioctl(uart_obj_id_t obj_id, uint32_t baudrate, uint32_t parity, uint32_t databits, uint32_t stopbits)
{
  uart_obj_t *obj = &uart_obj_tbl[obj_id];
  obj->huart->Init.BaudRate = baudrate;
  obj->huart->Init.WordLength = databits;
  obj->huart->Init.StopBits = stopbits;
  obj->huart->Init.Parity = parity;
  obj->huart->Init.Mode = UART_MODE_TX_RX;
  obj->huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  obj->huart->Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(obj->huart) != HAL_OK)
  {
    Error_Handler();
  }

  return 0;
}

int uart_obj_ioctl_rs485_register(uart_obj_id_t obj_id, void(*rs485_rxen)(void), void (*rs485_txen)(void) )
{
  uart_obj_t *obj = &uart_obj_tbl[obj_id];
  if(rs485_rxen != NULL)
    obj->rs485_rxen = rs485_rxen;
  if(rs485_txen != NULL)
    obj->rs485_txen = rs485_txen;
  return 0;
}

int uart_obj_ioctl_specific_rtx_isr_register(uart_obj_id_t obj_id, void(*specific_rx_isr)(void), void (*specific_tx_isr)(void) )
{
  uart_obj_t *obj = &uart_obj_tbl[obj_id];
  if(specific_rx_isr != NULL)
    obj->specific_rx_isr = specific_rx_isr;
  if(specific_tx_isr != NULL)
    obj->specific_tx_isr = specific_tx_isr;
  return 0;
}

int uart_obj_ioctl_get_info(uart_obj_id_t obj_id, uint32_t *baudrate, uint32_t *parity, uint32_t *databits, uint32_t *stopbits)
{
  uart_obj_t *obj = &uart_obj_tbl[obj_id];
  *baudrate = obj->huart->Init.BaudRate;
  *databits = obj->huart->Init.WordLength;
  *stopbits = obj->huart->Init.StopBits;
  *parity   = obj->huart->Init.Parity;
  
  return 0;
}

int uart_obj_ioctl_alloc_rx_buffer(uart_obj_id_t obj_id, uint16_t size)
{
  uart_obj_t *obj = &uart_obj_tbl[obj_id];
  taskENTER_CRITICAL();
  if(obj->rx_buffer)
  {
    vStreamBufferDelete(obj->rx_buffer);
  }
  obj->rx_buffer_size = size;
  obj->rx_buffer = xStreamBufferCreate( obj->rx_buffer_size, obj->rx_buffer_size/2 );
  taskEXIT_CRITICAL();
  
  if( obj->rx_buffer == NULL )
    return -1;
  
  return 0;
}

int uart_obj_read(uart_obj_id_t obj_id, uint8_t *buf, int size, uint32_t timeout)
{
  uart_obj_t *obj = &uart_obj_tbl[obj_id];
  int ret = 0;
  SemaphoreHandle_t xSemaphore = obj->rx_sema;
  
  if( xSemaphore != NULL )
  {
    if(__get_IPSR() != 0)
    {
      //xSemaphoreTakeFromISR( xSemaphore, NULL);
      ret = xStreamBufferReceiveFromISR( obj->rx_buffer, ( void * ) buf, size, 0 );
    }
    else
    {
      if(xSemaphoreTake( xSemaphore, ( TickType_t ) timeout ) == pdTRUE)
      {  
        ret = xStreamBufferReceive( obj->rx_buffer, ( void * ) buf, size, 0 );
        if(xStreamBufferBytesAvailable( obj->rx_buffer ))
          xSemaphoreGive( obj->rx_sema);
      }
    }
  }
  return ret;
}

int uart_obj_write(uart_obj_id_t obj_id, uint8_t *buf, int size)
{
  uart_obj_t *obj = &uart_obj_tbl[obj_id];
  int ret = 0;
  SemaphoreHandle_t xSemaphore = obj->tx_sema;
  uint8_t *tx_buff =  NULL;
  
  if( xSemaphore != NULL )
  {
    if(__get_IPSR() != 0)
    {
      if( xSemaphoreTakeFromISR( xSemaphore, NULL ) == pdTRUE )
      {
        tx_buff = obj->tx_buff;
        if(tx_buff == NULL)
          tx_buff = buf;
        else
          memcpy(tx_buff,buf,obj->tx_buff_len>=size? size : obj->tx_buff_len);
      }
    }
    else
    {
      if( xSemaphoreTake( xSemaphore, ( TickType_t ) osWaitForever ) == pdTRUE )
      {
        tx_buff = obj->tx_buff;
        if(tx_buff != NULL && obj->tx_buff_len < size)
        {
          free(tx_buff);
          tx_buff = NULL;
        }
        
        if(tx_buff == NULL)
        {
          obj->tx_buff = tx_buff = malloc(size);
          obj->tx_buff_len = size;
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
    if(obj->rs485_txen) 
      obj->rs485_txen();
    obj->tx_stat = _BUSY;
    ret = HAL_UART_Transmit_IT(obj->huart, tx_buff, size);
  }
  else 
    ret=-1;

  if(ret == -1)
    size = 0;

  return size;
}

int uart_obj_ioctl_is_rx_busy(uart_obj_id_t obj_id)
{
  uart_obj_t *obj = &uart_obj_tbl[obj_id];
  return obj->rx_stat;
}

int uart_obj_ioctl_is_tx_busy(uart_obj_id_t obj_id)
{
  uart_obj_t *obj = &uart_obj_tbl[obj_id];
  return obj->tx_stat;
}

void uart_obj_rxcplt_isr(uart_obj_id_t obj_id)
{
  uart_obj_t *obj = &uart_obj_tbl[obj_id];
  BaseType_t xHigherPriorityTaskWoken=pdFALSE;
  SemaphoreHandle_t xSemaphore;

  obj->rx_stat = _IDLE;
  xStreamBufferSendFromISR(obj->rx_buffer, &obj->rxbyte, 1, &xHigherPriorityTaskWoken);
  HAL_UART_Receive_IT(obj->huart, &obj->rxbyte, 1);
  if(xStreamBufferBytesAvailable(obj->rx_buffer) >= (obj->rx_buffer_size>>1))
  {
    xSemaphore = obj->rx_sema;
    if(xSemaphore != NULL)
      xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
  }
  if(obj->specific_rx_isr)
    obj->specific_rx_isr();
  
  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void uart_obj_rxidle_isr(uart_obj_id_t obj_id)
{
  uart_obj_t *obj = &uart_obj_tbl[obj_id];
  BaseType_t xHigherPriorityTaskWoken=pdFALSE;
  SemaphoreHandle_t xSemaphore;
  
  HAL_UART_Receive_IT(obj->huart, &obj->rxbyte, 1);
  xSemaphore = obj->rx_sema;
  if(xSemaphore != NULL)
    xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
  obj->rx_stat = _BUSY;
  
  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void uart_obj_txcplt_isr(uart_obj_id_t obj_id)
{
  uart_obj_t *obj = &uart_obj_tbl[obj_id];
  BaseType_t xHigherPriorityTaskWoken=pdFALSE;
  SemaphoreHandle_t xSemaphore;
  
  if(obj->rs485_rxen) 
    obj->rs485_rxen();
  /* Unblock the task by releasing the semaphore. */
  obj->tx_stat = _IDLE;
  xSemaphore = obj->tx_sema;
  if(xSemaphore != NULL)
    xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
  if(obj->specific_tx_isr)
    obj->specific_tx_isr();

  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
#ifdef UART1_OBJ_EN
  if(huart->Instance == USART1)
    uart_obj_rxcplt_isr(UART1_OBJ);
#endif
#ifdef UART2_OBJ_EN
  else if(huart->Instance == USART2)
    uart_obj_rxcplt_isr(UART2_OBJ);
#endif
#ifdef UART3_OBJ_EN
  else if(huart->Instance == USART3)
    uart_obj_rxcplt_isr(UART3_OBJ);
#endif
#ifdef UART4_OBJ_EN
  else if(huart->Instance == UART4)
    uart_obj_rxcplt_isr(UART4_OBJ);
#endif
#ifdef UART5_OBJ_EN
  else if(huart->Instance == UART5)
    uart_obj_rxcplt_isr(UART5_OBJ);
#endif
#ifdef UART6_OBJ_EN
  else if(huart->Instance == USART6)
    uart_obj_rxcplt_isr(UART6_OBJ);
#endif
}

void Usr_UART_RxIdleCallback(UART_HandleTypeDef *huart)
{
  if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET)
  {
    __HAL_UART_CLEAR_IDLEFLAG(huart);

#ifdef UART1_OBJ_EN
    if(huart->Instance == USART1)
      uart_obj_rxidle_isr(UART1_OBJ);
#endif
#ifdef UART2_OBJ_EN
    else if(huart->Instance == USART2)
      uart_obj_rxidle_isr(UART2_OBJ);
#endif
#ifdef UART3_OBJ_EN
    else if(huart->Instance == USART3)
      uart_obj_rxidle_isr(UART3_OBJ);
#endif
#ifdef UART4_OBJ_EN
    else if(huart->Instance == UART4)
      uart_obj_rxidle_isr(UART4_OBJ);
#endif
#ifdef UART5_OBJ_EN
    else if(huart->Instance == UART5)
      uart_obj_rxidle_isr(UART5_OBJ);
#endif
#ifdef UART6_OBJ_EN
    else if(huart->Instance == USART6)
      uart_obj_rxidle_isr(UART6_OBJ);
#endif
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
#ifdef UART1_OBJ_EN
    if(huart->Instance == USART1)
      uart_obj_txcplt_isr(UART1_OBJ);
#endif
#ifdef UART2_OBJ_EN
    else if(huart->Instance == USART2)
      uart_obj_txcplt_isr(UART2_OBJ);
#endif
#ifdef UART3_OBJ_EN
    else if(huart->Instance == USART3)
      uart_obj_txcplt_isr(UART3_OBJ);
#endif
#ifdef UART4_OBJ_EN
    else if(huart->Instance == UART4)
      uart_obj_txcplt_isr(UART4_OBJ);
#endif
#ifdef UART5_OBJ_EN
    else if(huart->Instance == UART5)
      uart_obj_txcplt_isr(UART5_OBJ);
#endif
#ifdef UART6_OBJ_EN
    else if(huart->Instance == USART6)
      uart_obj_txcplt_isr(UART6_OBJ);
#endif
}
