#include "Flash.h"

uint8_t Flash_Write(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len){
	uint32_t n,i;
	HAL_FLASH_Unlock();           //解锁flash
	static FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = FLASH_START_ADDR+ blk_addr*FLASH_BLK_SIZ;
	EraseInitStruct.NbPages = blk_len;
	uint32_t PageError = 0;
	if(	HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)   //擦除数据
	{
	   return 1;		
	}
		
    for(i=0;i<blk_len*FLASH_BLK_SIZ;i+=4)                        //以四字节写入falsh
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,FLASH_START_ADDR +                     
        blk_addr*FLASH_BLK_SIZ + i , *(uint32_t *)(&buf[i]));
	}
	HAL_FLASH_Lock();
	return 0;
}
uint8_t Flash_Read(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len){
	uint8_t *s = (uint8_t *)(FLASH_START_ADDR);  
	uint8_t *p = buf;
	s+=(blk_addr*FLASH_BLK_SIZ);
	for(int i=0; i<blk_len*FLASH_BLK_SIZ ;i++)       //读取FLash地址中的数据
	{
		*(buf++) = *(s++ );
	}
	return 0;
}