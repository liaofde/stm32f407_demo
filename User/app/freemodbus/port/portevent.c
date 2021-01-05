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
//static struct rt_event     xSlaveOsEvent;
static xQueueHandle xQueueHdl;


/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortEventInit( void )
{
  //rt_event_init(&xSlaveOsEvent,"slave event",RT_IPC_FLAG_PRIO);
  BOOL bStatus = FALSE;
  if( 0 != ( xQueueHdl = xQueueCreate( 1, sizeof( eMBEventType ) ) ) )
  {
    bStatus = TRUE;
  }
  return bStatus;
}

void
vMBPortEventClose( void )
{
  if( 0 != xQueueHdl )
  {
    vQueueDelete( xQueueHdl );
    xQueueHdl = 0;
  }
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
  BOOL bStatus = TRUE;
  if( __get_IPSR() != 0)
  {
    ( void )xQueueSendFromISR( xQueueHdl, ( const void * )&eEvent, pdFALSE );
  }
  else
  {
    ( void )xQueueSend( xQueueHdl, ( const void * )&eEvent, pdFALSE );
  }
  
  return bStatus;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent )
{
  BOOL xEventHappened = FALSE;
  
  if( pdTRUE == xQueueReceive( xQueueHdl, eEvent, 0 ) )
  {
    xEventHappened = TRUE;
  }
  return xEventHappened;
}
