#include "main.h"
#include "time_bsp.h"
#include "can_bsp.h"
#include "gpio.h"
#include "can.h"
#include "fsmc.h"
#include "spi.h"
//#include "tim.h"
#include "usart.h"
//#include "adc.h"
//#include "dma.h"

void bsp_inition(void)
{
  MX_GPIO_Init();
  //MX_USART1_UART_Init();
  MX_UART4_Init();
  //MX_CAN1_Init();
  MX_FSMC_Init();
  MX_USART6_UART_Init();
  MX_SPI1_Init();
  //MX_USART3_UART_Init();
  //MX_USART2_UART_Init();
}

