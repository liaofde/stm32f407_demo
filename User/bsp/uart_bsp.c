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
  if(obj_id >= UART_OBJ_NUM_MAX)
    return -1;
  
  if(uart_obj_tbl[obj_id].rx_buffer_size<256)
    uart_obj_tbl[obj_id].rx_buffer_size = 256;
  else if(uart_obj_tbl[obj_id].rx_buffer_size > 1024)
    uart_obj_tbl[obj_id].rx_buffer_size = 1024;
  
  uart_obj_tbl[obj_id].rx_buffer = xStreamBufferCreate( uart_obj_tbl[obj_id].rx_buffer_size, uart_obj_tbl[obj_id].rx_buffer_size/2 );
  if( uart_obj_tbl[obj_id].rx_buffer == NULL )
    return -1;
  
  vSemaphoreCreateBinary(uart_obj_tbl[obj_id].rx_idle_semaph);
  xSemaphoreTake( uart_obj_tbl[obj_id].rx_idle_semaph, ( TickType_t ) 0);
  vSemaphoreCreateBinary(uart_obj_tbl[obj_id].tx_cplt_semaph);

  if(uart_obj_tbl[obj_id].rs485_rxen) 
    uart_obj_tbl[obj_id].rs485_rxen();

#ifdef UART1_OBJ_EN
  if(obj_id == UART1_OBJ) 
  {
    uart_obj_tbl[obj_id].huart=&huart1;
    uart_obj_tbl[obj_id].huart->Instance = USART1;
  }
#endif
#ifdef UART2_OBJ_EN
  else if(obj_id == UART2_OBJ) 
  {
    uart_obj_tbl[obj_id].huart=&huart2;
    uart_obj_tbl[obj_id].huart->Instance = USART2;
  }
#endif
#ifdef UART3_OBJ_EN
  else if(obj_id == UART3_OBJ) 
  {
    uart_obj_tbl[obj_id].huart=&huart3;
    uart_obj_tbl[obj_id].huart->Instance = USART3;
  }
#endif
#ifdef UART4_OBJ_EN
  else if(obj_id == UART4_OBJ) 
  {
    uart_obj_tbl[obj_id].huart=&huart4;
    uart_obj_tbl[obj_id].huart->Instance = UART4;
  }
#endif
#ifdef UART5_OBJ_EN
  else if(obj_id == UART5_OBJ) 
  {
    uart_obj_tbl[obj_id].huart=&huart5;
    uart_obj_tbl[obj_id].huart->Instance = UART5;
  }
#endif
  
  uart_obj_tbl[obj_id].huart->Init.BaudRate = baudrate;
  uart_obj_tbl[obj_id].huart->Init.WordLength = databits;
  uart_obj_tbl[obj_id].huart->Init.StopBits = stopbits;
  uart_obj_tbl[obj_id].huart->Init.Parity = parity;
  uart_obj_tbl[obj_id].huart->Init.Mode = UART_MODE_TX_RX;
  uart_obj_tbl[obj_id].huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart_obj_tbl[obj_id].huart->Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(uart_obj_tbl[obj_id].huart) != HAL_OK)
  {
    Error_Handler();
  }
  
  HAL_UART_Receive_IT(uart_obj_tbl[obj_id].huart, &uart_obj_tbl[obj_id].rxbyte, 1);  //配置huart中断接收模式参数
  __HAL_UART_ENABLE_IT(uart_obj_tbl[obj_id].huart, UART_IT_IDLE); 
  
  return 0;
}

void uart_obj_close(uart_obj_id_t obj_id)
{
  HAL_UART_DeInit(uart_obj_tbl[obj_id].huart);
  vStreamBufferDelete(uart_obj_tbl[obj_id].rx_buffer);
  uart_obj_tbl[obj_id].rx_buffer = NULL;
}

int uart_obj_ioctl(uart_obj_id_t obj_id, uint32_t baudrate, uint32_t parity, uint32_t databits, uint32_t stopbits)
{
  uart_obj_tbl[obj_id].huart->Init.BaudRate = baudrate;
  uart_obj_tbl[obj_id].huart->Init.WordLength = databits;
  uart_obj_tbl[obj_id].huart->Init.StopBits = stopbits;
  uart_obj_tbl[obj_id].huart->Init.Parity = parity;
  uart_obj_tbl[obj_id].huart->Init.Mode = UART_MODE_TX_RX;
  uart_obj_tbl[obj_id].huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart_obj_tbl[obj_id].huart->Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(uart_obj_tbl[obj_id].huart) != HAL_OK)
  {
    Error_Handler();
  }

  return 0;
}

int uart_obj_ioctl_rs485_register(uart_obj_id_t obj_id, void(*rs485_rxen)(void), void (*rs485_txen)(void) )
{
  if(rs485_rxen != NULL)
    uart_obj_tbl[obj_id].rs485_rxen = rs485_rxen;
  if(rs485_txen != NULL)
    uart_obj_tbl[obj_id].rs485_txen = rs485_txen;
  return 0;
}

int uart_obj_ioctl_specific_rtx_isr_register(uart_obj_id_t obj_id, void(*specific_rx_isr)(void), void (*specific_tx_isr)(void) )
{
  if(specific_rx_isr != NULL)
    uart_obj_tbl[obj_id].specific_rx_isr = specific_rx_isr;
  if(specific_tx_isr != NULL)
    uart_obj_tbl[obj_id].specific_tx_isr = specific_tx_isr;
  return 0;
}

int uart_obj_read(uart_obj_id_t obj_id, uint8_t *buf, int size, uint32_t timeout)
{
  int ret = -1;
  SemaphoreHandle_t xSemaphore = uart_obj_tbl[obj_id].rx_idle_semaph;
  
  if( xSemaphore != NULL )
  {
    if(__get_IPSR() != 0)
    {
      xSemaphoreTakeFromISR( xSemaphore, NULL);
      xStreamBufferReceiveFromISR( uart_obj_tbl[obj_id].rx_buffer, ( void * ) buf, size, 0 );
    }
    else
    {
      xSemaphoreTake( xSemaphore, ( TickType_t ) timeout );
      ret = xStreamBufferReceive( uart_obj_tbl[obj_id].rx_buffer, ( void * ) buf, size, 0 );
    }
  }
  return ret;
}

int uart_obj_write(uart_obj_id_t obj_id, uint8_t *buf, int size)
{
  int ret = 0;
  SemaphoreHandle_t xSemaphore = uart_obj_tbl[obj_id].tx_cplt_semaph;
  uint8_t *tx_buff =  NULL;
  
  if( xSemaphore != NULL )
  {
    if(__get_IPSR() != 0)
    {
      if( xSemaphoreTakeFromISR( xSemaphore, NULL ) == pdTRUE )
      {
        tx_buff = uart_obj_tbl[obj_id].tx_buff;
        if(tx_buff == NULL)
          tx_buff = buf;
        else
          memcpy(tx_buff,buf,uart_obj_tbl[obj_id].tx_buff_len>=size? size : uart_obj_tbl[obj_id].tx_buff_len);
      }
    }
    else
    {
      if( xSemaphoreTake( xSemaphore, ( TickType_t ) osWaitForever ) == pdTRUE )
      {
        tx_buff = uart_obj_tbl[obj_id].tx_buff;
        if(tx_buff != NULL && uart_obj_tbl[obj_id].tx_buff_len < size)
        {
          free(tx_buff);
          tx_buff = NULL;
        }
        
        if(tx_buff == NULL)
        {
          uart_obj_tbl[obj_id].tx_buff = tx_buff = malloc(size);
          uart_obj_tbl[obj_id].tx_buff_len = size;
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
    if(uart_obj_tbl[obj_id].rs485_txen) 
      uart_obj_tbl[obj_id].rs485_txen();
    uart_obj_tbl[obj_id].tx_busy = 1;
    ret = HAL_UART_Transmit_IT(uart_obj_tbl[obj_id].huart, tx_buff, size);
  }
  else 
    ret=-1;

  if(ret == -1)
    size = 0;
  

  return size;
}

uint8_t uart_obj_read_is_busy(uart_obj_id_t obj_id)
{
  return uart_obj_tbl[obj_id].rx_busy;
}

uint8_t uart_obj_write_is_busy(uart_obj_id_t obj_id)
{
  return uart_obj_tbl[obj_id].tx_busy;
}

void uart_obj_rxcplt_isr(uart_obj_id_t obj_id)
{
  BaseType_t xHigherPriorityTaskWoken=pdFALSE;
  SemaphoreHandle_t xSemaphore;

  uart_obj_tbl[obj_id].rx_busy = 1;
  xStreamBufferSendFromISR(uart_obj_tbl[obj_id].rx_buffer, &uart_obj_tbl[obj_id].rxbyte, 1, &xHigherPriorityTaskWoken);
  HAL_UART_Receive_IT(uart_obj_tbl[obj_id].huart, &uart_obj_tbl[obj_id].rxbyte, 1);
  if(xStreamBufferBytesAvailable(uart_obj_tbl[obj_id].rx_buffer) >= (uart_obj_tbl[obj_id].rx_buffer_size>>1))
  {
    xSemaphore = uart_obj_tbl[obj_id].rx_idle_semaph;
    if(xSemaphore != NULL)
      xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
  }
  if(uart_obj_tbl[obj_id].specific_rx_isr)
    uart_obj_tbl[obj_id].specific_rx_isr();
  
  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void uart_obj_rxidle_isr(uart_obj_id_t obj_id)
{
  BaseType_t xHigherPriorityTaskWoken=pdFALSE;
  SemaphoreHandle_t xSemaphore;
  
  HAL_UART_Receive_IT(uart_obj_tbl[obj_id].huart, &uart_obj_tbl[obj_id].rxbyte, 1);
  xSemaphore = uart_obj_tbl[obj_id].rx_idle_semaph;
  if(xSemaphore != NULL)
    xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
  uart_obj_tbl[obj_id].rx_busy = 0;
  
  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void uart_obj_txcplt_isr(uart_obj_id_t obj_id)
{
  BaseType_t xHigherPriorityTaskWoken=pdFALSE;
  SemaphoreHandle_t xSemaphore;
  
  if(uart_obj_tbl[obj_id].rs485_rxen) 
    uart_obj_tbl[obj_id].rs485_rxen();
  /* Unblock the task by releasing the semaphore. */
  uart_obj_tbl[obj_id].tx_busy = 0;
  xSemaphore = uart_obj_tbl[obj_id].tx_cplt_semaph;
  if(xSemaphore != NULL)
    xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
  if(uart_obj_tbl[obj_id].specific_tx_isr)
    uart_obj_tbl[obj_id].specific_tx_isr();

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
#ifdef UART2_OBJ_EN
  else if(huart->Instance == USART3)
    uart_obj_rxcplt_isr(UART3_OBJ);
#endif
#ifdef UART5_OBJ_EN
  else if(huart->Instance == UART4)
    uart_obj_rxcplt_isr(UART4_OBJ);
#endif
#ifdef UART5_OBJ_EN
  else if(huart->Instance == UART5)
    uart_obj_rxcplt_isr(UART5_OBJ);
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
}
