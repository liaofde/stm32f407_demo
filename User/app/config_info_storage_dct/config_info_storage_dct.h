#ifndef __CONFIG_INFO_STORAGE_DCT_H__
#define __CONFIG_INFO_STORAGE_DCT_H__

#include "stdint.h"

#define OFFSETOF( type, member )  ( (uintptr_t)&((type *)0)->member )
#define SIZEOF( type, member )  ( sizeof((type *)0)->member )

extern unsigned char eeprom_buf[1024];

/*define config_dct member*/
typedef struct
{
  uint8_t ip[4];
  uint8_t mask[4];
  uint8_t gw[4];
  uint8_t mac[6];
}config_dct_t;

#define OFFSET_OF_DCT_MEMBER( member )  OFFSETOF( config_dct_t, member )
#define SIZE_OF_DCT_MEMBER( member)     SIZEOF( config_dct_t, member )//sizeof(((config_dct_t *)0)->member )

//void dct_write(char *date, uint16_t offset, uint16_t size);
//void dct_read(char *date, uint16_t offset, uint16_t size);

#define config_info_dct_write(data, member) dct_write(data, OFFSET_OF_DCT_MEMBER(member), SIZE_OF_DCT_MEMBER(member))
#define config_info_dct_read(data, member) dct_read(data, OFFSET_OF_DCT_MEMBER(member), SIZE_OF_DCT_MEMBER(member))

#endif