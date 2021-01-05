/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

#include "port_.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "uart_dev.h"
/* ----------------------- Static variables ---------------------------------*/

/* ----------------------- Defines ------------------------------------------*/
/* serial transmit event */
#define EVENT_SERIAL_TRANS_START    (1<<0)

uint8_t rx_idle_cnt=5;
uint8_t rx_enable=1;
uint8_t tx_enable=0;
/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);

uart_dev_t mb_uart_dev={
  .specific_rx_isv = prvvUARTRxISR,
  .specific_tx_isv = prvvUARTTxReadyISR,
};

static UART_InitTypeDef initInfo = {
  .BaudRate = 19200,
  .WordLength = UART_WORDLENGTH_8B,
  .StopBits = UART_STOPBITS_1,
  .Parity = UART_PARITY_EVEN,
  .Mode = UART_MODE_TX_RX,
  .HwFlowCtl = UART_HWCONTROL_NONE,
  .OverSampling = UART_OVERSAMPLING_16,
};

uart_dev_arg_t mb_uart_dev_arg = {
  .huart = &huart3,
  .USART = USART3,
  .InitInfo = &initInfo,
  .rx_ringbuff_size = 16,
};

void mb_uart_dev_init( ULONG ulBaudRate, UCHAR ucDataBits,eMBParity eParity)
{
  initInfo.BaudRate = ulBaudRate;
  if(eParity!=MB_PAR_NONE)
    initInfo.WordLength = UART_WORDLENGTH_9B;
  initInfo.Parity = (eParity==MB_PAR_NONE)?UART_PARITY_NONE:(eParity==MB_PAR_ODD)?UART_PARITY_ODD:UART_PARITY_EVEN;
  uart_dev_open(&mb_uart_dev, &mb_uart_dev_arg);
}

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
        eMBParity eParity)
{
    mb_uart_dev_init(ulBaudRate,ucDataBits,eParity);
    return TRUE;
}

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
  rx_enable=xRxEnable;
  tx_enable=xTxEnable;
}


BOOL xMBPortSerialPutByte(CHAR ucByte)
{
    uart_dev_write(&mb_uart_dev, (uint8_t *)&ucByte,1);
    return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{
    *pucByte = mb_uart_dev_arg.rxbyte;
    return TRUE;
}

/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvUARTTxReadyISR(void)
{
    pxMBFrameCBTransmitterEmpty();
}

/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */

void prvvUARTRxISR(void)
{
    rx_idle_cnt = 1;
    if(rx_enable)
      pxMBFrameCBByteReceived();
}

void mb_waitidle(void)
{
    osDelay(5);
    if(rx_enable && rx_idle_cnt>0 && rx_idle_cnt++>2)
    {
      rx_idle_cnt = 0;
      extern void prvvTIMERExpiredISR(void);
      prvvTIMERExpiredISR();
    }
    if(tx_enable)
    {
      tx_enable = 0;
      prvvUARTTxReadyISR();
    }
}
