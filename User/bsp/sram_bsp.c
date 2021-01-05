#include "sram_bsp.h"

void sram_test(void)
{
  uint32_t i;
  volatile uint32_t * address = (volatile uint32_t *)SRAM_DEVICE_ADDR;
  for(i=0; i<1024; i++)
  {
    *(volatile uint32_t *)(address++) = i;
  }
}
