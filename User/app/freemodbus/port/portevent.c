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
 * File: $Id: portevent.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
   
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <cmsis_os.h>

/* ----------------------- Variables ----------------------------------------*/
#define USE_OS_MESSAGE

#ifdef USE_OS_MESSAGE
osMessageQId mb_slave_message  =  NULL;
osMessageQDef(mb_slave_message, 2, eMBEventType);
#else
static xQueueHandle xQueueHdl;
#endif

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortEventInit( void )
{
  BOOL bStatus = FALSE;
  
#ifdef USE_OS_MESSAGE
  if((mb_slave_message = osMessageCreate(osMessageQ(mb_slave_message),NULL)) != NULL)
#else
  if( 0 != ( xQueueHdl = xQueueCreate( 1, sizeof( eMBEventType ) ) ) )
#endif
  {
    bStatus = TRUE;
  }
  return bStatus;
}

void
vMBPortEventClose( void )
{
#ifdef USE_OS_MESSAGE
  if(mb_slave_message)
  {
    osMessageDelete(mb_slave_message);
  }
#else
  if( 0 != xQueueHdl )
  {
    vQueueDelete( xQueueHdl );
    xQueueHdl = 0;
  }
#endif
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
  BOOL bStatus = TRUE;
  
#ifdef USE_OS_MESSAGE
  if(osOK != osMessagePut(mb_slave_message, (uint32_t)eEvent , 0))
    bStatus = FALSE; 
#else
  if( __get_IPSR() != 0)
  {
    ( void )xQueueSendFromISR( xQueueHdl, ( const void * )&eEvent, NULL );
  }
  else
  {
    ( void )xQueueSend( xQueueHdl, ( const void * )&eEvent, 0 );
  }
#endif
  return bStatus;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent )
{
  BOOL xEventHappened = FALSE;
  
#ifdef USE_OS_MESSAGE
  osEvent os_event;
  os_event = osMessageGet(mb_slave_message, 0);
  if(os_event.status == osEventMessage)
  {
    *eEvent =  (eMBEventType)os_event.value.v;
    xEventHappened = TRUE;
  }
#else
  if( pdTRUE == xQueueReceive( xQueueHdl, eEvent, 0 ) )
  {
    xEventHappened = TRUE;
  }
#endif
  return xEventHappened;
}
