#ifndef __SRAM_BSP_H__
#define __SRAM_BSP_H__

#include "fsmc.h"

#define SRAM_DEVICE_ADDR  ((uint32_t)0x68000000)
#define SRAM_DEVICE_SIZE  ((uint32_t)0x100000)  /* SRAM device size in Bytes */  

void sram_test(void);

#endif
