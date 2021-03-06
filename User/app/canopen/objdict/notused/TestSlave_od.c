
/* File generated by gen_cfile.py. Should not be modified. */

#include "TestSlave.h"

/**************************************************************************/
/* Declaration of the mapped variables                                    */
/**************************************************************************/
UNS8 SlaveMap1 = 0x0;		/* Mapped at index 0x2000, subindex 0x00 */
UNS8 SlaveMap2 = 0x0;		/* Mapped at index 0x2001, subindex 0x00 */
UNS8 SlaveMap3 = 0x0;		/* Mapped at index 0x2002, subindex 0x00 */
UNS8 SlaveMap4 = 0x0;		/* Mapped at index 0x2003, subindex 0x00 */

/**************************************************************************/
/* Declaration of the value range types                                   */
/**************************************************************************/

UNS32 TestSlave_valueRangeTest (UNS8 typeValue, void * value)
{
  switch (typeValue) {
  }
  return 0;
}

/**************************************************************************/
/* The node id                                                            */
/**************************************************************************/
/* node_id default value.*/
UNS8 TestSlave_bDeviceNodeId = 0x00;

/**************************************************************************/
/* Array of message processing information */

const UNS8 TestSlave_iam_a_slave = 1;

TIMER_HANDLE TestSlave_heartBeatTimers[1];

/*
$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

                               OBJECT DICTIONARY

$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
*/

/* index 0x1000 :   Device Type. */
                    UNS32 TestSlave_obj1000 = 0x12D;	/* 301 */
                    subindex TestSlave_Index1000[] = 
                     {
                       { RO, uint32, sizeof (UNS32), (void*)&TestSlave_obj1000 }
                     };

/* index 0x1001 :   Error Register. */
                    UNS8 TestSlave_obj1001 = 0x0;	/* 0 */
                    subindex TestSlave_Index1001[] = 
                     {
                       { RO, uint8, sizeof (UNS8), (void*)&TestSlave_obj1001 }
                     };

/* index 0x1005 :   SYNC COB ID. */
                    UNS32 TestSlave_obj1005 = 0x80;	/* 128 */
                    ODCallback_t TestSlave_Index1005_callbacks[] = 
                     {
                       NULL,
                     };
                    subindex TestSlave_Index1005[] = 
                     {
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1005 }
                     };

/* index 0x1006 :   Communication / Cycle Period. */
                    UNS32 TestSlave_obj1006 = 0x0;	/* 0 */
                    ODCallback_t TestSlave_Index1006_callbacks[] = 
                     {
                       NULL,
                     };
                    subindex TestSlave_Index1006[] = 
                     {
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1006 }
                     };

/* index 0x1010 :   Store parameters. */
                    UNS8 TestSlave_highestSubIndex_obj1010 = 4; /* number of subindex - 1*/
                    UNS32 TestSlave_obj1010_Save_All_Parameters = 0x0;	/* 0 */
                    UNS32 TestSlave_obj1010_Save_Communication_Parameters = 0x0;	/* 0 */
                    UNS32 TestSlave_obj1010_Save_Application_Parameters = 0x0;	/* 0 */
                    UNS32 TestSlave_obj1010_Save_Manufacturer_Parameters = 0x0;	/* 0 */
                    ODCallback_t TestSlave_Index1010_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                     };
                    subindex TestSlave_Index1010[] = 
                     {
                       { RO, uint8, sizeof (UNS8), (void*)&TestSlave_highestSubIndex_obj1010 },
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1010_Save_All_Parameters },
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1010_Save_Communication_Parameters },
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1010_Save_Application_Parameters },
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1010_Save_Manufacturer_Parameters }
                     };

/* index 0x1011 :   Restore Default Parameters. */
                    UNS8 TestSlave_highestSubIndex_obj1011 = 4; /* number of subindex - 1*/
                    UNS32 TestSlave_obj1011_Restore_All_Default_Parameters = 0x0;	/* 0 */
                    UNS32 TestSlave_obj1011_Restore_Communication_Default_Parameters = 0x0;	/* 0 */
                    UNS32 TestSlave_obj1011_Restore_Application_Default_Parameters = 0x0;	/* 0 */
                    UNS32 TestSlave_obj1011_Restore_Manufacturer_Default_Parameters = 0x0;	/* 0 */
                    ODCallback_t TestSlave_Index1011_callbacks[] = 
                     {
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                       NULL,
                     };
                    subindex TestSlave_Index1011[] = 
                     {
                       { RO, uint8, sizeof (UNS8), (void*)&TestSlave_highestSubIndex_obj1011 },
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1011_Restore_All_Default_Parameters },
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1011_Restore_Communication_Default_Parameters },
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1011_Restore_Application_Default_Parameters },
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1011_Restore_Manufacturer_Default_Parameters }
                     };

/* index 0x1016 :   Consumer Heartbeat Time */
                    UNS8 TestSlave_highestSubIndex_obj1016 = 0;
                    UNS32 TestSlave_obj1016[]={0};

/* index 0x1017 :   Producer Heartbeat Time. */
                    UNS16 TestSlave_obj1017 = 0x3E8;	/* 1000 */
                    subindex TestSlave_Index1017[] = 
                     {
                       { RW, uint16, sizeof (UNS16), (void*)&TestSlave_obj1017 }
                     };

/* index 0x1018 :   Identity. */
                    UNS8 TestSlave_highestSubIndex_obj1018 = 4; /* number of subindex - 1*/
                    UNS32 TestSlave_obj1018_Vendor_ID = 0x0;	/* 0 */
                    UNS32 TestSlave_obj1018_Product_Code = 0x0;	/* 0 */
                    UNS32 TestSlave_obj1018_Revision_Number = 0x0;	/* 0 */
                    UNS32 TestSlave_obj1018_Serial_Number = 0x0;	/* 0 */
                    subindex TestSlave_Index1018[] = 
                     {
                       { RO, uint8, sizeof (UNS8), (void*)&TestSlave_highestSubIndex_obj1018 },
                       { RO, uint32, sizeof (UNS32), (void*)&TestSlave_obj1018_Vendor_ID },
                       { RO, uint32, sizeof (UNS32), (void*)&TestSlave_obj1018_Product_Code },
                       { RO, uint32, sizeof (UNS32), (void*)&TestSlave_obj1018_Revision_Number },
                       { RO, uint32, sizeof (UNS32), (void*)&TestSlave_obj1018_Serial_Number }
                     };

/* index 0x1200 :   Server SDO Parameter. */
                    UNS8 TestSlave_highestSubIndex_obj1200 = 2; /* number of subindex - 1*/
                    UNS32 TestSlave_obj1200_COB_ID_Client_to_Server_Receive_SDO = 0x601;	/* 1537 */
                    UNS32 TestSlave_obj1200_COB_ID_Server_to_Client_Transmit_SDO = 0x581;	/* 1409 */
                    subindex TestSlave_Index1200[] = 
                     {
                       { RO, uint8, sizeof (UNS8), (void*)&TestSlave_highestSubIndex_obj1200 },
                       { RO, uint32, sizeof (UNS32), (void*)&TestSlave_obj1200_COB_ID_Client_to_Server_Receive_SDO },
                       { RO, uint32, sizeof (UNS32), (void*)&TestSlave_obj1200_COB_ID_Server_to_Client_Transmit_SDO }
                     };

/* index 0x1800 :   Transmit PDO 1 Parameter. */
                    UNS8 TestSlave_highestSubIndex_obj1800 = 5; /* number of subindex - 1*/
                    UNS32 TestSlave_obj1800_COB_ID_used_by_PDO = 0x182;	/* 386 */
                    UNS8 TestSlave_obj1800_Transmission_Type = 0x0;	/* 0 */
                    UNS16 TestSlave_obj1800_Inhibit_Time = 0x0;	/* 0 */
                    UNS16 TestSlave_obj1800_Compatibility_Entry = 0x0;	/* 0 */
                    UNS16 TestSlave_obj1800_Event_Timer = 0x0;	/* 0 */
                    subindex TestSlave_Index1800[] = 
                     {
                       { RO, uint8, sizeof (UNS8), (void*)&TestSlave_highestSubIndex_obj1800 },
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1800_COB_ID_used_by_PDO },
                       { RW, uint8, sizeof (UNS8), (void*)&TestSlave_obj1800_Transmission_Type },
                       { RW, uint16, sizeof (UNS16), (void*)&TestSlave_obj1800_Inhibit_Time },
                       { RW, uint16, sizeof (UNS16), (void*)&TestSlave_obj1800_Compatibility_Entry },
                       { RW, uint16, sizeof (UNS16), (void*)&TestSlave_obj1800_Event_Timer }
                     };

/* index 0x1801 :   Transmit PDO 2 Parameter. */
                    UNS8 TestSlave_highestSubIndex_obj1801 = 5; /* number of subindex - 1*/
                    UNS32 TestSlave_obj1801_COB_ID_used_by_PDO = 0x282;	/* 642 */
                    UNS8 TestSlave_obj1801_Transmission_Type = 0x0;	/* 0 */
                    UNS16 TestSlave_obj1801_Inhibit_Time = 0x0;	/* 0 */
                    UNS16 TestSlave_obj1801_Compatibility_Entry = 0x0;	/* 0 */
                    UNS16 TestSlave_obj1801_Event_Timer = 0x0;	/* 0 */
                    subindex TestSlave_Index1801[] = 
                     {
                       { RO, uint8, sizeof (UNS8), (void*)&TestSlave_highestSubIndex_obj1801 },
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1801_COB_ID_used_by_PDO },
                       { RW, uint8, sizeof (UNS8), (void*)&TestSlave_obj1801_Transmission_Type },
                       { RW, uint16, sizeof (UNS16), (void*)&TestSlave_obj1801_Inhibit_Time },
                       { RW, uint16, sizeof (UNS16), (void*)&TestSlave_obj1801_Compatibility_Entry },
                       { RW, uint16, sizeof (UNS16), (void*)&TestSlave_obj1801_Event_Timer }
                     };

/* index 0x1A00 :   Transmit PDO 1 Mapping. */
                    UNS8 TestSlave_highestSubIndex_obj1A00 = 2; /* number of subindex - 1*/
                    UNS32 TestSlave_obj1A00[] = 
                    {
                      0x20000008,	/* 536870920 */
                      0x20010008	/* 536936456 */
                    };
                    subindex TestSlave_Index1A00[] = 
                     {
                       { RW, uint8, sizeof (UNS8), (void*)&TestSlave_highestSubIndex_obj1A00 },
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1A00[0] },
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1A00[1] }
                     };

/* index 0x1A01 :   Transmit PDO 2 Mapping. */
                    UNS8 TestSlave_highestSubIndex_obj1A01 = 2; /* number of subindex - 1*/
                    UNS32 TestSlave_obj1A01[] = 
                    {
                      0x20020008,	/* 537001992 */
                      0x20030008	/* 537067528 */
                    };
                    subindex TestSlave_Index1A01[] = 
                     {
                       { RW, uint8, sizeof (UNS8), (void*)&TestSlave_highestSubIndex_obj1A01 },
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1A01[0] },
                       { RW, uint32, sizeof (UNS32), (void*)&TestSlave_obj1A01[1] }
                     };

/* index 0x2000 :   Mapped variable SlaveMap1 */
                    subindex TestSlave_Index2000[] = 
                     {
                       { RW, uint8, sizeof (UNS8), (void*)&SlaveMap1 }
                     };

/* index 0x2001 :   Mapped variable SlaveMap2 */
                    subindex TestSlave_Index2001[] = 
                     {
                       { RW, uint8, sizeof (UNS8), (void*)&SlaveMap2 }
                     };

/* index 0x2002 :   Mapped variable SlaveMap3 */
                    subindex TestSlave_Index2002[] = 
                     {
                       { RW, uint8, sizeof (UNS8), (void*)&SlaveMap3 }
                     };

/* index 0x2003 :   Mapped variable SlaveMap4 */
                    subindex TestSlave_Index2003[] = 
                     {
                       { RW, uint8, sizeof (UNS8), (void*)&SlaveMap4 }
                     };

const indextable TestSlave_objdict[] = 
{
  { (subindex*)TestSlave_Index1000,sizeof(TestSlave_Index1000)/sizeof(TestSlave_Index1000[0]), 0x1000},
  { (subindex*)TestSlave_Index1001,sizeof(TestSlave_Index1001)/sizeof(TestSlave_Index1001[0]), 0x1001},
  { (subindex*)TestSlave_Index1005,sizeof(TestSlave_Index1005)/sizeof(TestSlave_Index1005[0]), 0x1005},
  { (subindex*)TestSlave_Index1006,sizeof(TestSlave_Index1006)/sizeof(TestSlave_Index1006[0]), 0x1006},
  { (subindex*)TestSlave_Index1010,sizeof(TestSlave_Index1010)/sizeof(TestSlave_Index1010[0]), 0x1010},
  { (subindex*)TestSlave_Index1011,sizeof(TestSlave_Index1011)/sizeof(TestSlave_Index1011[0]), 0x1011},
  { (subindex*)TestSlave_Index1017,sizeof(TestSlave_Index1017)/sizeof(TestSlave_Index1017[0]), 0x1017},
  { (subindex*)TestSlave_Index1018,sizeof(TestSlave_Index1018)/sizeof(TestSlave_Index1018[0]), 0x1018},
  { (subindex*)TestSlave_Index1200,sizeof(TestSlave_Index1200)/sizeof(TestSlave_Index1200[0]), 0x1200},
  { (subindex*)TestSlave_Index1800,sizeof(TestSlave_Index1800)/sizeof(TestSlave_Index1800[0]), 0x1800},
  { (subindex*)TestSlave_Index1801,sizeof(TestSlave_Index1801)/sizeof(TestSlave_Index1801[0]), 0x1801},
  { (subindex*)TestSlave_Index1A00,sizeof(TestSlave_Index1A00)/sizeof(TestSlave_Index1A00[0]), 0x1A00},
  { (subindex*)TestSlave_Index1A01,sizeof(TestSlave_Index1A01)/sizeof(TestSlave_Index1A01[0]), 0x1A01},
  { (subindex*)TestSlave_Index2000,sizeof(TestSlave_Index2000)/sizeof(TestSlave_Index2000[0]), 0x2000},
  { (subindex*)TestSlave_Index2001,sizeof(TestSlave_Index2001)/sizeof(TestSlave_Index2001[0]), 0x2001},
  { (subindex*)TestSlave_Index2002,sizeof(TestSlave_Index2002)/sizeof(TestSlave_Index2002[0]), 0x2002},
  { (subindex*)TestSlave_Index2003,sizeof(TestSlave_Index2003)/sizeof(TestSlave_Index2003[0]), 0x2003},
};

const indextable * TestSlave_scanIndexOD (UNS16 wIndex, UNS32 * errorCode, ODCallback_t **callbacks)
{
	int i;
	*callbacks = NULL;
	switch(wIndex){
		case 0x1000: i = 0;break;
		case 0x1001: i = 1;break;
		case 0x1005: i = 2;*callbacks = TestSlave_Index1005_callbacks; break;
		case 0x1006: i = 3;*callbacks = TestSlave_Index1006_callbacks; break;
		case 0x1010: i = 4;*callbacks = TestSlave_Index1010_callbacks; break;
		case 0x1011: i = 5;*callbacks = TestSlave_Index1011_callbacks; break;
		case 0x1017: i = 6;break;
		case 0x1018: i = 7;break;
		case 0x1200: i = 8;break;
		case 0x1800: i = 9;break;
		case 0x1801: i = 10;break;
		case 0x1A00: i = 11;break;
		case 0x1A01: i = 12;break;
		case 0x2000: i = 13;break;
		case 0x2001: i = 14;break;
		case 0x2002: i = 15;break;
		case 0x2003: i = 16;break;
		default:
			*errorCode = OD_NO_SUCH_OBJECT;
			return NULL;
	}
	*errorCode = OD_SUCCESSFUL;
	return &TestSlave_objdict[i];
}

/* To count at which received SYNC a PDO must be sent.
 * Even if no pdoTransmit are defined, at least one entry is computed
 * for compilations issues.
 */
UNS8 TestSlave_count_sync[2] = {0,};

quick_index TestSlave_firstIndex = {
  8, /* SDO_SVR */
  0, /* SDO_CLT */
  0, /* PDO_RCV */
  0, /* PDO_RCV_MAP */
  9, /* PDO_TRS */
  11 /* PDO_TRS_MAP */
};

quick_index TestSlave_lastIndex = {
  8, /* SDO_SVR */
  0, /* SDO_CLT */
  0, /* PDO_RCV */
  0, /* PDO_RCV_MAP */
  10, /* PDO_TRS */
  12 /* PDO_TRS_MAP */
};

UNS16 TestSlave_ObjdictSize = sizeof(TestSlave_objdict)/sizeof(TestSlave_objdict[0]); 

CO_Data TestSlave_Data = CANOPEN_NODE_DATA_INITIALIZER(TestSlave);

