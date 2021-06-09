/**
  ******************************************************************************
  * @file    canopen_thread.c
  * @author  
  * @version 
  * @date    
  * @brief   This file provides CANOpen communication control thread functions 
  *
  ******************************************************************************
  */

#include "can_thread.h"
#include "canfestival.h"
#include <stdio.h>
#include "cmsis_os.h"
#include "utils_userdef.h"
#include "can_common_api.h"
#include "can_bsp.h"

//#include "stm32fxx_can_user_bsp.h"

extern CAN_HandleTypeDef hcan1;
static void can_dispatch_thread(const void *arg);

static 	osMailQId    m_canmsg_mail = NULL;

static  osMutexId     m_mutex = NULL;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#if 0
  #define  CAN_PRINTF(fmt, ... )  printf(fmt , ##__VA_ARGS__)
#else
  #define  CAN_PRINTF(...) //printf
#endif
/**
  * @brief  canInit
  * @param  CANx:CAN1 or CAN2 bitrate
  * @retval 0£ºSuccess
  */
void canopen_dispatch_thread_entry(void)
{		
  can_obj_open(CAN1_OBJ, CAN_KBAUD_1000K);
  can_obj_ioctl(CAN1_OBJ, CAN_KBAUD_500K);
  static osMailQDef(canmsg_mail,32,Message);
  static osThreadDef(can_dispatch,can_dispatch_thread,osPriorityHigh,1,1024);
  
  static osMutexDef(mutex);
  m_mutex = osMutexCreate(osMutex(mutex));
  YT_ASSERT(m_mutex);
  
  m_canmsg_mail = osMailCreate(osMailQ(canmsg_mail),NULL);
  YT_ASSERT(m_canmsg_mail);
  osThreadCreate(osThread(can_dispatch),NULL);

}

int can_rxmsg_event_push(CAN_RxHeaderTypeDef *rxheader, uint8_t *dat)
{
    int ret = -1;
    
    Message  *obj = (Message*)osMailAlloc(m_canmsg_mail,osWaitForever);
    if(obj)
    {
      obj->cob_id = rxheader->StdId;
      obj->rtr    = rxheader->RTR;
      obj->len    = rxheader->DLC;
      for(uint8_t i=0; i< obj->len; i++)
      {  
        obj->data[i] = dat[i];
      }
      
      if(osMailPut(m_canmsg_mail,obj) == osOK)
        ret = 0;
      else 
        osMailFree(m_canmsg_mail,obj);
    }
    
    return ret;
}
/**
  * @brief  canSend
	* @param  CANx:CAN1 or CAN2   m:can message
  * @retval 0£ºSuccess
  */
unsigned char canSend(CAN_PORT CANx, Message *m)	                
{
#if 1
  int res=0;
  
  osMutexWait(m_mutex,osWaitForever);
  
  //int can1_send_date(uint32_t cob_id, uint8_t rtr, uint8_t data_size, uint8_t* data);
  //res = can1_send_date(m->cob_id, m->rtr, m->len, m->data);
  
  can_msg_t msg;
  msg.Std_Ext_Id = m->cob_id;
  msg.RTR = m->rtr;
  msg.DLC = m->len;
  msg.IDE =0;
  memcpy(msg.BUF, m->data, m->len);
  can_obj_write(CAN1_OBJ, (uint8_t *)&msg, sizeof(msg));
  
  CAN_PRINTF("can send[%04x,len:%d]:",(m->cob_id),m->len);
  for(uint8_t i=0;i<m->len;i++)
  {
    CAN_PRINTF("%02x ",m->data[i]);
  }
  CAN_PRINTF("\r\n");
  
  osMutexRelease(m_mutex);
  
  return res==0? 0:1;
  
#else
  unsigned char i;
  
  osMutexWait(m_mutex,osWaitForever);
  
  hcan1.pTxMsg->StdId = (uint32_t)(m->cob_id);
  hcan1.pTxMsg->ExtId = 0x00;
  hcan1.pTxMsg->RTR   = m->rtr;								  
  hcan1.pTxMsg->IDE   = CAN_ID_STD;                           
  hcan1.pTxMsg->DLC   = m->len;
  
  
  for(i=0;i<m->len;i++)
  {
    hcan1.pTxMsg->Data[i] = m->data[i];
  }
  
  if(hcan1.pTxMsg->StdId == 0x602)
  {
    CAN_PRINTF("can send[%04x,len:%d]:",hcan1.pTxMsg->StdId,m->len);
    for(i=0;i<m->len;i++)
    {
      CAN_PRINTF("%02x ",m->data[i]);
    }
    CAN_PRINTF("\r\n");
  }
  
  /*##-3- Start the Transmission process ###############################*/
  if(HAL_CAN_Transmit(&hcan1, 10) != HAL_OK)
  {
    /* Transmition Error */
    osMutexRelease(m_mutex);
    return 1;	
  }
  
  osMutexRelease(m_mutex);
  return 0;	
#endif	
}

static void can_dispatch_thread(const void *arg)
{
  can_msg_t msg;
  
  while(1)
  {
    if(can_obj_read (CAN1_OBJ, (uint8_t *)&msg, sizeof(msg), osWaitForever)>0)
    {
      Message obj;

      obj.cob_id = msg.Std_Ext_Id;
      obj.rtr    = msg.RTR;
      obj.len    = msg.DLC;
      for(int i=0;i<obj.len&&i<=8;i++)
      {  
        obj.data[i]=msg.BUF[i];
      }
      
      CO_Data* d;
      CO_Data* can_open_NodeID_CO_Data_get( UNS8 nodeId);
      if((d = can_open_NodeID_CO_Data_get((obj.cob_id &0x7F))) != NULL)
      {
        canDispatch(d, &obj);
      }
    }
    
  }

}
