/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-06-21     flybreak     first version
 */

#include "mb.h"
#include "user_mb_app.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "uart_bsp.h"


#define SLAVE_ADDR      0x01
#define PORT_NUM        UART2_OBJ
#define PORT_BAUDRATE   19200
#define PORT_PARITY     MB_PAR_EVEN

#define MB_POLL_THREAD_PRIORITY  10
#define MB_SEND_THREAD_PRIORITY  RT_THREAD_PRIORITY_MAX - 1

#define MB_POLL_CYCLE_MS 200

extern USHORT usSRegHoldBuf[S_REG_HOLDING_NREGS];

osThreadId mb_slave_TaskHandle;


static void mb_slave_poll(void const *parameter)
{
    eMBInit(MB_RTU, SLAVE_ADDR, PORT_NUM, PORT_BAUDRATE, PORT_PARITY);
    eMBEnable();
    while (1)
    {
        usSRegHoldBuf[0]=0x52b8;
        usSRegHoldBuf[1]=0x48c1;
        usSRegHoldBuf[2]=0x52b8;
        usSRegHoldBuf[3]=0x48c1;
        eMBPoll();
        void mb_seriral_hdl(void);
        mb_seriral_hdl();
    }
}

void mb_slave_thread_entry(void)
{
  osThreadDef(mb_slave, mb_slave_poll, osPriorityNormal, 0, 512);
  mb_slave_TaskHandle = osThreadCreate(osThread(mb_slave), NULL);
}

