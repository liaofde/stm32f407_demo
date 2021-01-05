#include "stdio.h"
#include "string.h"
#include "config_info_storage_dct.h"

unsigned char eeprom_buf[1024];

int eeprom_write(uint16_t address, char *date, uint16_t size)
{
  memcpy(&eeprom_buf[address], date, size);
  return 0;
}

int eeprom_read(uint16_t address, char *date, uint16_t size)
{
  memcpy(date, &eeprom_buf[address], size);
  return 0;
}

void dct_write(char *date, uint16_t offset, uint16_t size)
{
    eeprom_write(offset, date, size);
}

void dct_read(char *date, uint16_t offset, uint16_t size)
{
    eeprom_read(offset, date, size);
}

/*
void main(void)
{
  unsigned char buf[8];
  int i=0;
  memset(eeprom_buf, 255, 1024);
  for(i=0; i<1024; i++)
  {
    if(i%32==0)
      printf("\n");
    printf("%02x ", eeprom_buf[i]);
  }
  while(1)
  {
    printf("\ninput ip:");
    scanf("%d.%d.%d.%d",&buf[0],&buf[1],&buf[2],&buf[3]);
    config_info_dct_write(buf, ip);
    memset(buf,0,4);
    config_info_dct_read(buf, ip);
    printf("ip:%d.%d.%d.%d\n",buf[0],buf[1],buf[2],buf[3]);
    
    printf("\ninput mask:");
    scanf("%d.%d.%d.%d",&buf[0],&buf[1],&buf[2],&buf[3]);
    config_info_dct_write(buf, mask);
    memset(buf,0,4);
    config_info_dct_read(buf, mask);
    printf("mask:%d.%d.%d.%d\n",buf[0],buf[1],buf[2],buf[3]);
    
    printf("\ninput gw:");
    scanf("%d.%d.%d.%d",&buf[0],&buf[1],&buf[2],&buf[3]);
    config_info_dct_write(buf, gw);
    memset(buf,0,4);
    config_info_dct_read(buf, gw);
    printf("gw:%d.%d.%d.%d\n",buf[0],buf[1],buf[2],buf[3]);
    
    printf("\ninput mac:");
    scanf("%x.%x.%x.%x.%x.%x",&buf[0],&buf[1],&buf[2],&buf[3],&buf[4],&buf[5]);
    config_info_dct_write(buf, mac);
    memset(buf,0,4);
    config_info_dct_read(buf, mac);
    printf("mac:%02x.%02x.%02x.%02x.%02x.%02x\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
    
    for(i=0; i<1024; i++)
    {
      if(i%32==0)
        printf("\n");
      printf("%02x ", eeprom_buf[i]);
    }
  }
}
*/