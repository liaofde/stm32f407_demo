/**
******************************************************************************
* @file    CANOpenObjDictConfig.c
* @author  Zhenglin R&D Driver Software Team
* @version V1.0.0
* @date    26/04/2015
* @brief   This file is CANOpenObjDictConfig file.
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "CANOpenObjDictConfig.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*Froward declaration*/
#define LOG printf


/**
* @brief  Master_heartbeatError.
* @param  d:CANOpen object dictionary. heartbeatID:Node id
* @retval None
*/
void Master_heartbeatError(CO_Data* d, UNS8 heartbeatID)
{
  LOG("Master_heartbeatError %d\r\n", heartbeatID);
}

/**
* @brief  Master_initialisation.
* @param  d:CANOpen object dictionary.
* @retval None
*/
void Master_initialisation(CO_Data* d)
{
  LOG("Master_initialisation\r\n");
#if 1
  UNS32 TPDO1_COBID = 0x0181; 
  UNS32 RPDO1_COBID = 0x201;
  UNS32 TPDO2_COBID = 0x0182; 
  UNS32 RPDO2_COBID = 0x202;
  UNS32 size = sizeof(UNS32); 
  UNS8 unsize = sizeof(unsize); 
  UNS8  TpdoTransmissionType = 0;
  
  TpdoTransmissionType = TpdoTransmissionType;
  LOG("Master_initialisation\r\n");
  
  /*****************************************
  * Define RPDOs to match slave ID=2 TPDOs*
  *****************************************/
  writeLocalDict( d, /*CO_Data* d*/
                 0x1400, /*UNS16 index*/
                 0x01, /*UNS8 subind*/ 
                 &RPDO1_COBID, /*void * pSourceData,*/ 
                 &size, /* UNS8 * pExpectedSize*/
                 RW);  /* UNS8 checkAccess */
  TpdoTransmissionType = TRANS_SYNC_ACYCLIC;	
  writeLocalDict( d, /*CO_Data* d*/
                 0x1400, /*UNS16 index*/
                 0x02, /*UNS8 subind*/ 
                 &TpdoTransmissionType, /*void * pSourceData,*/ 
                 (UNS32*)&unsize, /* UNS8 * pExpectedSize*/
                 RW);  /* UNS8 checkAccess */
  
  writeLocalDict( d, /*CO_Data* d*/
                 0x1401, /*UNS16 index*/
                 0x01, /*UNS8 subind*/ 
                 &RPDO2_COBID, /*void * pSourceData,*/ 
                 &size, /* UNS8 * pExpectedSize*/
                 RW);  /* UNS8 checkAccess */
  TpdoTransmissionType = TRANS_SYNC_ACYCLIC;	
  writeLocalDict( d, /*CO_Data* d*/
                 0x1401, /*UNS16 index*/
                 0x02, /*UNS8 subind*/ 
                 &TpdoTransmissionType, /*void * pSourceData,*/ 
                 (UNS32*)&unsize, /* UNS8 * pExpectedSize*/
                 RW);  /* UNS8 checkAccess */
  
  writeLocalDict( d, /*CO_Data* d*/
                 0x1800, /*UNS16 index*/
                 0x01, /*UNS8 subind*/ 
                 &TPDO1_COBID, /*void * pSourceData,*/ 
                 &size, /* UNS8 * pExpectedSize*/
                 RW);  /* UNS8 checkAccess */
  TpdoTransmissionType = TRANS_SYNC_MIN;	
  writeLocalDict( d, /*CO_Data* d*/
                 0x1800, /*UNS16 index*/
                 0x02, /*UNS8 subind*/ 
                 &TpdoTransmissionType, /*void * pSourceData,*/ 
                 (UNS32*)&unsize, /* UNS8 * pExpectedSize*/
                 RW);  /* UNS8 checkAccess */
  
  writeLocalDict( d, /*CO_Data* d*/
                 0x1801, /*UNS16 index*/
                 0x01, /*UNS8 subind*/ 
                 &TPDO2_COBID, /*void * pSourceData,*/ 
                 &size, /* UNS8 * pExpectedSize*/
                 RW);  /* UNS8 checkAccess */
  TpdoTransmissionType = TRANS_SYNC_MIN;	
  writeLocalDict( d, /*CO_Data* d*/
                 0x1801, /*UNS16 index*/
                 0x02, /*UNS8 subind*/ 
                 &TpdoTransmissionType, /*void * pSourceData,*/ 
                 (UNS32*)&unsize, /* UNS8 * pExpectedSize*/
                 RW);  /* UNS8 checkAccess */
#endif
  
}

static int init_step = 0;
static void ConfigureSlaveNode(CO_Data* d, UNS8 nodeId);

/**/
static void CheckSDOAndContinue(CO_Data* d, UNS8 nodeId)
{
	UNS32 abortCode;

	if(getWriteResultNetworkDict (d, nodeId, &abortCode) != SDO_FINISHED)
		printf("Master : Failed in initializing slave %2.2x, step %d, AbortCode :%4.4x \n", nodeId, init_step, abortCode);

	/* Finalise last SDO transfer with this node */
	closeSDOtransfer(d, nodeId, SDO_CLIENT);

	ConfigureSlaveNode(d, nodeId);
}

static void ConfigureSlaveNode(CO_Data* d, UNS8 nodeId)
{
	UNS8 res;
	printf("Master : ConfigureSlaveNode %2.2x\n", nodeId);
	printf("nodeid slave=%x\n",nodeId);
	switch(++init_step){
		case 1: 
		{	/*disable Slave's TPDO 1 */
			UNS32 TPDO_COBId = 0x80000180 + nodeId;
			
			printf("Master : disable slave %2.2x TPDO 1 \n", nodeId);
			res = writeNetworkDictCallBack (d, /*CO_Data* d*/
					/**TestSlave_Data.bDeviceNodeId, UNS8 nodeId*/
					nodeId, /*UNS8 nodeId*/
					0x1800, /*UNS16 index*/
					0x01, /*UNS8 subindex*/
					4, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&TPDO_COBId,/*void *data*/
					CheckSDOAndContinue, /*SDOCallback_t Callback*/
                    0); /* use block mode */
        }			
		break;

		case 2: 
		{	/*setup Slave's TPDO 1 to be transmitted on SYNC*/
			UNS8 Transmission_Type = 0x01;
			
			printf("Master : set slave %2.2x TPDO 1 transmit type\n", nodeId);
			res = writeNetworkDictCallBack (d, /*CO_Data* d*/
					/**TestSlave_Data.bDeviceNodeId, UNS8 nodeId*/
					nodeId, /*UNS8 nodeId*/
					0x1800, /*UNS16 index*/
					0x02, /*UNS8 subindex*/
					1, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&Transmission_Type,/*void *data*/
					CheckSDOAndContinue, /*SDOCallback_t Callback*/
                    0); /* use block mode */
		}			
		break;

		case 3: 
		{	/*re-enable Slave's TPDO 1 */
			UNS32 TPDO_COBId = 0x00000180 + nodeId;
			
			printf("Master : re-enable slave %2.2x TPDO 1\n", nodeId);
			res = writeNetworkDictCallBack (d, /*CO_Data* d*/
					/**TestSlave_Data.bDeviceNodeId, UNS8 nodeId*/
					nodeId, /*UNS8 nodeId*/
					0x1800, /*UNS16 index*/
					0x01, /*UNS8 subindex*/
					4, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&TPDO_COBId,/*void *data*/
					CheckSDOAndContinue, /*SDOCallback_t Callback*/
                    0); /* use block mode */
		}			
		break;
					
		case 4: 
		{	/*disable Slave's TPDO 2 */
			UNS32 TPDO_COBId = 0x80000200 + nodeId;
			
			printf("Master : disable slave %2.2x RPDO 1\n", nodeId);
			res = writeNetworkDictCallBack (d, /*CO_Data* d*/
					/**TestSlave_Data.bDeviceNodeId, UNS8 nodeId*/
					nodeId, /*UNS8 nodeId*/
					0x1400, /*UNS16 index*/
					0x01, /*UNS8 subindex*/
					4, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&TPDO_COBId,/*void *data*/
					CheckSDOAndContinue, /*SDOCallback_t Callback*/
                    0); /* use block mode */
		}			
		break;

					
		case 5:
		{	
			UNS8 Transmission_Type = 0x01;
			
			printf("Master : set slave %2.2x RPDO 1 receive type\n", nodeId);
			res = writeNetworkDictCallBack (d, /*CO_Data* d*/
					/**TestSlave_Data.bDeviceNodeId, UNS8 nodeId*/
					nodeId, /*UNS8 nodeId*/
					0x1400, /*UNS16 index*/
					0x02, /*UNS8 subindex*/
					1, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&Transmission_Type,/*void *data*/
					CheckSDOAndContinue, /*SDOCallback_t Callback*/
                    0); /* use block mode */
		}	
		break;

		case 6: 
		{	/*re-enable Slave's TPDO 1 */
			UNS32 TPDO_COBId = 0x00000200 + nodeId;
			
			printf("Master : re-enable %2.2x RPDO 1\n", nodeId);
			res = writeNetworkDictCallBack (d, /*CO_Data* d*/
					/**TestSlave_Data.bDeviceNodeId, UNS8 nodeId*/
					nodeId, /*UNS8 nodeId*/
					0x1400, /*UNS16 index*/
					0x01, /*UNS8 subindex*/
					4, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&TPDO_COBId,/*void *data*/
					CheckSDOAndContinue, /*SDOCallback_t Callback*/
                    0); /* use block mode */
		}			
		break;
		
		case 7:	
		{
			UNS16 Heartbeat_Producer_Time = 0x03E8; 
			printf("Master : set slave %2.2x heartbeat producer time \n", nodeId);
			res = writeNetworkDictCallBack (d, /*CO_Data* d*/
					/**TestSlave_Data.bDeviceNodeId, UNS8 nodeId*/
					nodeId, /*UNS8 nodeId*/
					0x1017, /*UNS16 index*/
					0x00, /*UNS8 subindex*/
					2, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&Heartbeat_Producer_Time,/*void *data*/
					CheckSDOAndContinue, /*SDOCallback_t Callback*/
                    0); /* use block mode */
		}			
		break;

		case 8: 
		{	/*disable Slave's TPDO 2 */
			UNS32 TPDO_COBId = 0x80000280 + nodeId;
			
			printf("Master : disable slave %2.2x TPDO 2 \n", nodeId);
			res = writeNetworkDictCallBack (d, /*CO_Data* d*/
					/**TestSlave_Data.bDeviceNodeId, UNS8 nodeId*/
					nodeId, /*UNS8 nodeId*/
					0x1801, /*UNS16 index*/
					0x01, /*UNS8 subindex*/
					4, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&TPDO_COBId,/*void *data*/
					CheckSDOAndContinue, /*SDOCallback_t Callback*/
                    0); /* use block mode */
		}			
		break;

		case 9: 
		{	/*disable Slave's TPDO 3 */
			UNS32 TPDO_COBId = 0x80000380 + nodeId;
			
			printf("Master : disable slave %2.2x TPDO 3 \n", nodeId);
			res = writeNetworkDictCallBack (d, /*CO_Data* d*/
					/**TestSlave_Data.bDeviceNodeId, UNS8 nodeId*/
					nodeId, /*UNS8 nodeId*/
					0x1802, /*UNS16 index*/
					0x01, /*UNS8 subindex*/
					4, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&TPDO_COBId,/*void *data*/
					CheckSDOAndContinue, /*SDOCallback_t Callback*/
                    0); /* use block mode */
		}
		break;			

		case 10: 
		{	/*disable Slave's TPDO 2 */
			UNS32 TPDO_COBId = 0x80000480 + nodeId;
			
			printf("Master : disable slave %2.2x TPDO 4 \n", nodeId);
			res = writeNetworkDictCallBack (d, /*CO_Data* d*/
					/**TestSlave_Data.bDeviceNodeId, UNS8 nodeId*/
					nodeId, /*UNS8 nodeId*/
					0x1803, /*UNS16 index*/
					0x01, /*UNS8 subindex*/
					4, /*UNS8 count*/
					0, /*UNS8 dataType*/
					&TPDO_COBId,/*void *data*/
					CheckSDOAndContinue, /*SDOCallback_t Callback*/
                    0); /* use block mode */
		}			
		break;			
		
		case 11:
			/* Put the master in operational mode */
			setState(d, Operational);
			  
			/* Ask slave node to go in operational mode */
			masterSendNMTstateChange (d, nodeId, NMT_Start_Node);
	}
			
}
/**
* @brief  Master_preOperational.
* @param  d:CANOpen object dictionary.
* @retval None
*/
void Master_preOperational(CO_Data* d)
{
  ConfigureSlaveNode(d, *d->bDeviceNodeId);
  LOG( "Master_preOperational\r\n");
}
/**
* @brief  Master_preOperational.
* @param  d:CANOpen object dictionary.
* @retval None
*/
void Master_operational(CO_Data* d)
{
#if 0
  UNS32 SyncTimePeriod = 1000;
  UNS32 size = sizeof(SyncTimePeriod);
  
  writeLocalDict( d, /*CO_Data* d*/
                 0x1006, /*UNS16 index*/
                 0x00, /*UNS8 subind*/ 
                 &SyncTimePeriod, /*void * pSourceData,*/ 
                 &size, /* UNS8 * pExpectedSize*/
                 RW);  /* UNS8 checkAccess */
  stopSYNC(d);
  startSYNC(d);
  setTimer(100);
#endif
  LOG( "Master_operational\n");
}
/**
* @brief  Master_stopped.
* @param  d:CANOpen object dictionary.
* @retval None
*/
void Master_stopped(CO_Data* d)
{
  LOG( "Master_stopped\n");
}

/**
* @brief  Master_post_sync.
* @param  d:CANOpen object dictionary.
* @retval None
*/
void Master_post_sync(CO_Data* d)
{
  
  //Controlword       = 0x0f;
  
  LOG("Master_post_sync\r\n");
}

/**
* @brief  Master_post_emcy.
* @param  d:CANOpen object dictionary.
* @retval None
*/
__weak void Master_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg, const UNS8 errSpec[5])
{
  LOG("Master received EMCY message. Node: %2.2x  ErrorCode: %4.4x  ErrorRegister: %2.2x\r\n", nodeID, errCode, errReg);
  
  //  if(errCode != 0x6300)
  //  {	
  //    if(nodeID == 1)
  //    {
  //      //set_fault_state(eFAULT_DRIVER_FAULT,errCode);
  //      //motor_brake(BRAKE_ON);
  //    }
  //    else if(nodeID == 2)
  //    {
  //      //lifter_brake(BRAKE_ON);
  //      //if(is_lifter_zero())
  //      //masterSendNMTstateChange(&LifterObjDict_Data, nodeID, NMT_Reset_Node);
  //    }
  //    
  //  }
}
/**
* @brief  Master_post_TPDO.
* @param  d:CANOpen object dictionary.
* @retval None
*/
void Master_post_TPDO(CO_Data* d)
{
  LOG("Master_post_TPDO\r\n");
}
/**
* @brief  Master_post_SlaveBootup.
* @param  d:CANOpen object dictionary.nodeId:Node id
* @retval None
*/
void Master_post_SlaveBootup(CO_Data* d, UNS8 nodeid)
{
  masterSendNMTstateChange (d, nodeid, NMT_Start_Node);
  LOG("Master_post_SlaveBootup %x\r\n", nodeid);
}
/**
* @brief  Master_post_SlaveBootup.
* @param  d:CANOpen object dictionary.nodeId:Node id newNodeState:Slave change state
* @retval None
*/
void Master_post_SlaveStateChange(CO_Data* d, UNS8 nodeId, e_nodeState newNodeState)
{
  static UNS8 NodeIdNum = 0;
  if (newNodeState == Operational)
  {
    NodeIdNum++;
    if (NodeIdNum == MAX_CAN_BUS_ID)
    {
      NodeIdNum = 0;
      setState(d, Operational);
    }
  }
}
/**
* @brief  CANOpenMasterInit.
* @param  d:CANOpen object dictionary.
* @retval None
*/
void CANOpenMasterInit(CO_Data* d)
{
  d->heartbeatError = Master_heartbeatError;
  d->initialisation = Master_initialisation;
  d->preOperational = Master_preOperational;
  d->operational = Master_operational;
  d->stopped = Master_stopped;
  d->post_sync = Master_post_sync;
  d->post_TPDO = Master_post_TPDO;
  d->post_emcy = Master_post_emcy;
  d->post_SlaveBootup = Master_post_SlaveBootup;
  d->post_SlaveStateChange = Master_post_SlaveStateChange;
}

/******************* (C) COPYRIGHT 2015 Personal Electronics *****END OF FILE****/
