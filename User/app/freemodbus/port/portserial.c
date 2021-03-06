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
#include "uart_bsp.h"
/* -----------------------  variables ---------------------------------*/
volatile uint8_t rx_idle_cnt=0;
volatile uint8_t rx_enable=1;
volatile uint8_t tx_enable=0;
volatile UCHAR  RTUBuf[256];

#define xSPECIFIC_ISR_ENABLE     

#ifndef SPECIFIC_ISR_ENABLE 
uint16_t rxlen=0;
#endif
uint16_t txlen=0;

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);

static void rs485_rxen(void)
{
  UART2_RS485_RE;
}

static void rs485_txen(void)
{
  UART2_RS485_TE;
}

void mb_uart_obj_init( ULONG ulBaudRate, UCHAR ucDataBits,eMBParity eParity)
{
  uint32_t WordLength;
  uint32_t Parity;
  
  if(eParity!=MB_PAR_NONE)
    WordLength = UART_WORDLENGTH_9B;
  else
    WordLength = UART_WORDLENGTH_8B;
  Parity = (eParity==MB_PAR_NONE)?UART_PARITY_NONE:(eParity==MB_PAR_ODD)?UART_PARITY_ODD:UART_PARITY_EVEN;
  uart_obj_open(UART2_OBJ, ulBaudRate, Parity , WordLength,  UART_STOPBITS_1);
  uart_obj_ioctl_rs485_register(UART2_OBJ, rs485_rxen, rs485_txen);
#ifdef SPECIFIC_ISR_ENABLE
  uart_obj_ioctl_specific_rtx_isr_register(UART2_OBJ, prvvUARTRxISR, prvvUARTTxReadyISR);
#endif
}

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
        eMBParity eParity)
{
    mb_uart_obj_init(ulBaudRate,ucDataBits,eParity);
    return TRUE;
}

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
  rx_enable=xRxEnable;
  tx_enable=xTxEnable;
}


BOOL xMBPortSerialPutByte(CHAR ucByte)
{
    RTUBuf[txlen++]=ucByte;
    return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{  
#ifdef SPECIFIC_ISR_ENABLE
    uart_obj_read(UART2_OBJ, (uint8_t *)pucByte, 1, 0);
#else
    *pucByte = RTUBuf[rxlen++];
#endif
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
    rx_idle_cnt = 0;
    if(rx_enable)
      pxMBFrameCBByteReceived();
}

void mb_seriral_hdl(void)
{
    extern uint8_t mb_time_enable;
    
#ifdef SPECIFIC_ISR_ENABLE
    osDelay(5);
    if(mb_time_enable && ++rx_idle_cnt>=3)
    {
      rx_idle_cnt = 0;
      extern void prvvTIMERExpiredISR(void);
      prvvTIMERExpiredISR();
    }
#else
    uint16_t len=0;
    len=uart_obj_read(UART2_OBJ, (uint8_t *)RTUBuf, 256, 5);
    if(len>0 && rx_enable)
    {
      int i=0;
      for(i=0,rxlen = 0;i<len;i++)
        prvvUARTRxISR();
    }
    if(mb_time_enable)
    {
      extern void prvvTIMERExpiredISR(void);
      prvvTIMERExpiredISR();
    }
#endif
    txlen=0;
    while(tx_enable)
    {
      prvvUARTTxReadyISR();
    }
    if(txlen)
    {
      uart_obj_write(UART2_OBJ, (uint8_t *)RTUBuf,txlen);
    }
}
