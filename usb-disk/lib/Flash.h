#ifndef _FLASH_H_
#define _FLASH_H_
#include "main.h"


#define FLASH_START_ADDR      		  0x08010000    // 设置flahs起始地址
#define FLASH_BLK_NBR                  224
#define FLASH_BLK_SIZ                  2048

uint8_t Flash_Write(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
uint8_t Flash_Read(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);


#endif

