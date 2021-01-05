#ifndef __USER_MB_APP__
#define __USER_MB_APP__

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbutils.h"

//#define PKG_MODBUS_SLAVE_RTU 1

/* -----------------------Slave Defines -------------------------------------*/
#define S_DISCRETE_INPUT_START                    0
#define S_DISCRETE_INPUT_NDISCRETES               16
#define S_COIL_START                              0
#define S_COIL_NCOILS                             64
#define S_REG_INPUT_START                         0
#define S_REG_INPUT_NREGS                         100
#define S_REG_HOLDING_START                       0
#define S_REG_HOLDING_NREGS                       100
/* salve mode: holding register's all address */
#define          S_HD_RESERVE                     0
/* salve mode: input register's all address */
#define          S_IN_RESERVE                     0
/* salve mode: coil's all address */
#define          S_CO_RESERVE                     0
/* salve mode: discrete's all address */
#define          S_DI_RESERVE                     0


#endif
