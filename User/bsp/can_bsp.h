#ifndef __STM32FXX_CAN_USER_BSP_H
#define __STM32FXX_CAN_USER_BSP_H 

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "bsp_isr_def.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"

#define CAN1_OBJ_EN
#define xCAN2_OBJ_EN

typedef enum
{       
  CAN_KBAUD_10K   = 300,
  CAN_KBAUD_20K   = 150,
  CAN_KBAUD_50K   = 60,
  CAN_KBAUD_100K  = 30,
  CAN_KBAUD_125K  = 24,
  CAN_KBAUD_250K  = 12,
  CAN_KBAUD_500K  = 6,
  CAN_KBAUD_1000K = 3,  // 1Mbps
}CAN_Baud_t;

typedef enum
{
#ifdef CAN1_OBJ_EN
  CAN1_OBJ,
#endif
#ifdef CAN2_OBJ_EN
  CAN2_OBJ,
#endif
  CAN_OBJ_NUM,
}can_obj_id_t;
   
#pragma pack(1)
typedef struct
{
  uint32_t Std_Ext_Id;    /*标准帧11位id,0~0x7FF. */
                          /*扩展帧29位id,0~0x1FFFFFFF. */
  uint8_t IDE:1;          /*0:标准帧 1:扩展帧*/
  uint8_t RTR:1;          /*0:数据帧 1:远程帧*/
  uint8_t :2;
  uint8_t DLC:4;          /*数据长度 0~8*/
  uint8_t BUF[8];         /*数据缓冲*/
}can_msg_t;
#pragma pack ()

typedef struct _can_obj
{
  CAN_HandleTypeDef  *hcan;
  uint8_t            *obj_name;
  osMailQId          rx_osMailQid;
  uint8_t            init_flag; 
}can_obj_t;

int can_obj_open (can_obj_id_t obj_id, uint32_t can_baudrate);
int can_obj_read (can_obj_id_t obj_id, uint8_t *buf, int size, uint32_t timeout);
int can_obj_write(can_obj_id_t obj_id, uint8_t *buf, int size);
int can_obj_ioctl(can_obj_id_t obj_id, uint32_t can_baudrate);
void can_obj_close (can_obj_id_t obj_id);
#ifdef __cplusplus
}
#endif
#endif /*__ can_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
