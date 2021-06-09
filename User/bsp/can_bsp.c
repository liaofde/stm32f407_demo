#include "main.h"
#include "can_bsp.h" 

extern CAN_HandleTypeDef hcan1;

//CAN_TxHeaderTypeDef TxHeader;
//CAN_RxHeaderTypeDef RxHeader;

uint8_t TxData[8];
uint8_t RxData[8];
uint32_t TxMailbox;

can_obj_t can_obj[CAN_OBJ_NUM];

#define CAN_HEARTBEAT_ID  0x700
#define CAN_SDO_TX_ID     0x600
#define CAN_SDO_RX_ID     0x580

static osMailQDef(can_msg,32,can_msg_t);

int can_obj_open (can_obj_id_t obj_id, uint32_t baudrate)
{
  if(can_obj[obj_id].init_flag)
    return 0;
  
  CAN_HandleTypeDef *hcan;
#ifdef CAN1_OBJ_EN
  if(obj_id == CAN1_OBJ)
  {
    can_obj[obj_id].hcan = &hcan1;
    can_obj[obj_id].hcan->Instance = CAN1;
  }
#endif
#ifdef CAN2_OBJ_EN
  else if(obj_id == CAN1_OBJ)
  {
    can_obj[obj_id].hcan = &hcan2;
    can_obj[obj_id].hcan->Instance = CAN2;
  }
#endif
  else
    return -1;
  
  hcan = can_obj[obj_id].hcan;
  hcan->Init.Prescaler = baudrate;
  hcan->Init.Mode = CAN_MODE_NORMAL;
  hcan->Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan->Init.TimeSeg1 = CAN_BS1_9TQ;
  hcan->Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan->Init.TimeTriggeredMode = DISABLE;
  hcan->Init.AutoBusOff = ENABLE;
  hcan->Init.AutoWakeUp = DISABLE;
  hcan->Init.AutoRetransmission = DISABLE;
  hcan->Init.ReceiveFifoLocked = DISABLE;
  hcan->Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(hcan) != HAL_OK)
  {
    //Error_Handler();
    return -1;
  }
  
  CAN_FilterTypeDef  sFilterConfig;
  HAL_StatusTypeDef  HAL_Status;
  
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0;
  sFilterConfig.FilterIdLow = 0;
  sFilterConfig.FilterMaskIdHigh = 0;
  sFilterConfig.FilterMaskIdLow = 0;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;
  
  HAL_Status = HAL_CAN_ConfigFilter(hcan, &sFilterConfig);
  if (HAL_Status != HAL_OK)
  {
    /* Error */
    return -1;
  }
  
  HAL_Status = HAL_CAN_Start(hcan);
  if (HAL_Status != HAL_OK)
  {
    /* Error */
    return -1;
  }
  
  HAL_Status = HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING |CAN_IT_TX_MAILBOX_EMPTY);
  if (HAL_Status != HAL_OK){
    /* Error */
    return -1;
  }
  
  if(can_obj[obj_id].rx_osMailQid == NULL)
    can_obj[obj_id].rx_osMailQid = osMailCreate(osMailQ(can_msg),NULL);
  if(can_obj[obj_id].rx_osMailQid == NULL)
    return -1;
  
  can_obj[obj_id].init_flag = 1;
  return 0;
}

int can_obj_read (can_obj_id_t obj_id, uint8_t *buf, int size, uint32_t timeout)
{
  if(obj_id >= CAN_OBJ_NUM || can_obj[obj_id].hcan == NULL || can_obj[obj_id].init_flag == 0)
    return -1;
  
  int ret = 0;
  osEvent evt;
  can_obj_t *obj =  &can_obj[obj_id];
  
  evt = osMailGet(obj->rx_osMailQid, timeout);
  if(evt.status == osEventMail)
  {
    can_msg_t *msg = (can_msg_t*)evt.value.p;
    ret = 5 + msg->DLC;
    memcpy(buf,msg,ret);
    osMailFree(obj->rx_osMailQid, msg);
  }
  
  return ret;
}

int can_obj_write(can_obj_id_t obj_id, uint8_t *buf, int size)
{ 
  if(obj_id >= CAN_OBJ_NUM || can_obj[obj_id].hcan == NULL || can_obj[obj_id].init_flag == 0)
    return -1;
  
  can_obj_t *obj =  &can_obj[obj_id];
  can_msg_t *msg = (can_msg_t *)buf;
  
  if(obj->hcan == NULL || size > 13 || size < 5 || msg->DLC > 8)
    return -1;
  
  CAN_HandleTypeDef *hcan=obj->hcan;
  CAN_TxHeaderTypeDef TxHeader;
  TxHeader.StdId = msg->IDE==0?msg->Std_Ext_Id:0;//CAN_SDO_TX_ID + nodeid;
  TxHeader.ExtId = msg->IDE==0?0:msg->Std_Ext_Id;
  TxHeader.DLC = msg->DLC;
  TxHeader.RTR = msg->RTR==0?CAN_RTR_DATA:CAN_RTR_REMOTE;
  TxHeader.IDE = msg->IDE==0?CAN_ID_STD:CAN_ID_EXT;
  
  uint8_t TxData[8];
  for (int i = 0; i <msg->DLC; i++)
  {
    TxData[i] = msg->BUF[i];
  }
  
  // check whether there is available mailbox
  if (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0)
  {
    return -1;
  }
  
  if(HAL_CAN_AddTxMessage(hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
    return -1;
  
  return size;
}

int can_obj_ioctl(can_obj_id_t obj_id, uint32_t baudrate)
{
  if(obj_id >= CAN_OBJ_NUM || can_obj[obj_id].hcan == NULL || can_obj[obj_id].init_flag == 0)
    return -1;
  
  CAN_HandleTypeDef *hcan;
  hcan = can_obj[obj_id].hcan;
  
  HAL_CAN_Stop(hcan);
    
  hcan->Init.Prescaler = baudrate;
  hcan->Init.Mode = CAN_MODE_NORMAL;
  hcan->Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan->Init.TimeSeg1 = CAN_BS1_9TQ;
  hcan->Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan->Init.TimeTriggeredMode = DISABLE;
  hcan->Init.AutoBusOff = ENABLE;
  hcan->Init.AutoWakeUp = DISABLE;
  hcan->Init.AutoRetransmission = DISABLE;
  hcan->Init.ReceiveFifoLocked = DISABLE;
  hcan->Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(hcan) != HAL_OK)
  {
    //Error_Handler();
    return -1;
  }
  HAL_StatusTypeDef  HAL_Status;
  HAL_Status = HAL_CAN_Start(hcan);
  if (HAL_Status != HAL_OK)
  {
    /* Error */
    return -1;
  }

  return 0;
}

void can_obj_close (can_obj_id_t obj_id)
{
  if(obj_id > CAN_OBJ_NUM || can_obj[obj_id].hcan == NULL)
    return;
  
  HAL_CAN_DeInit(can_obj[obj_id].hcan);
  can_obj[obj_id].init_flag = 0;
}

void user_can_RxFifoMsgPendingCallback(CAN_HandleTypeDef * hcan, uint32_t CAN_RX_FIFO)
{
  can_obj_t *obj = NULL;
  
  for(int i=0; i<CAN_OBJ_NUM; i++)
  {
    if(hcan == can_obj[i].hcan)
    {
       obj = &can_obj[i];
    }
  }
  if(obj == NULL)
    return;
  
  CAN_RxHeaderTypeDef RxHeader;
  uint8_t RxData[8];
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO, &RxHeader, RxData) == HAL_OK)
  {
    can_msg_t  *msg = (can_msg_t*)osMailAlloc(obj->rx_osMailQid,osWaitForever);
    if(msg)
    {
      msg->RTR    = RxHeader.RTR==CAN_RTR_DATA?0:1;
      msg->DLC    = RxHeader.DLC;
      if(RxHeader.IDE == CAN_ID_STD)
      {
        msg->IDE = 0;
        msg->Std_Ext_Id = RxHeader.StdId;
      }
      else
      {
        msg->IDE = 1;
        msg->Std_Ext_Id = RxHeader.ExtId;
      }
      for(uint8_t i=0; i< msg->DLC; i++)
      {  
        msg->BUF[i] = RxData[i];
      }
      
      if(osMailPut(obj->rx_osMailQid,msg) == osOK)
        ;
      else 
        osMailFree(obj->rx_osMailQid,msg);
    }
    
  }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef * hcan)
{
  /* Get RX message */
  user_can_RxFifoMsgPendingCallback(hcan, CAN_RX_FIFO0);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef * hcan)
{
  /* Get RX message */
  user_can_RxFifoMsgPendingCallback(hcan, CAN_RX_FIFO1);
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
  HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY);
}


