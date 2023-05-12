

#include "flash.h"

/*
0x08000000是stm32系列处理器的flash起始地址
小容量产品：主存储块1-32KB，每页1KB
中容量产品：主存储块64-128KB，每页1KB
大容量产品：主存储块256KB以上，每页2KB
互联型产品：主存储块256KB以上，每页2KB
*/

/* Base address of the Flash sectors */
//#define FLASH_PAGE_SIZE		(0x400)			//it has defined in xx_hal_flash_ex.h
#define	ADDR_FLASH_PAGE_0		((uint32_t)0x08000000) 
#define ADDR_FLASH_PAGE(n)	(ADDR_FLASH_PAGE_0 + ((uint32_t)(n)*FLASH_PAGE_SIZE))

#define FLASH_USER_PAGE_NUM 	2		/*define num pages for user */
#define FLASH_USER_START_ADDR  ADDR_FLASH_PAGE(64-2)			/* Start @ of user Flash area */
#define FLASH_USER_END_ADDR 	(FLASH_USER_START_ADDR + FLASH_USER_PAGE_NUM*FLASH_PAGE_SIZE)   /* End @ of user Flash area FLASH_PAGE_SIZE */


unsigned int flash_eraser_write(unsigned int * DATA_32, unsigned int num)
{
	unsigned int	i	= 0;
	unsigned int	Address = 0;
	unsigned int	PageError = 0;
	FLASH_EraseInitTypeDef EraseInitStruct;

	if (num == 0)
		return 0;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Erase the user Flash area
	  (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
	EraseInitStruct.NbPages = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) 
															/ FLASH_PAGE_SIZE;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
	{
		HAL_FLASH_Lock();
		return 0;
	}

	/* Program the user Flash area word by word
	  (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
	Address = FLASH_USER_START_ADDR;
	i = 0;
	while ((Address < FLASH_USER_END_ADDR) && (i < num))
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, DATA_32[i++]) == HAL_OK)
		{
			Address = Address + 4;
		}
		else 
		{
			HAL_FLASH_Lock();
			return 0;
		}
	}

	/* Lock the Flash to disable the flash control register access (recommended
	   to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();
	return i;
}


unsigned int flash_read(unsigned int * DATA_32, unsigned int num)
{
	unsigned int	i	= 0;
	unsigned int	Address = 0;

	if (num == 0)
		return 0;

	/* Check if the programmed data is OK
		MemoryProgramStatus = 0: data programmed correctly
		MemoryProgramStatus != 0: number of words not programmed correctly ******/
	Address = FLASH_USER_START_ADDR;
	i = 0;

	while ((Address < FLASH_USER_END_ADDR) && (i < num))
	{
		DATA_32[i++] = *(volatile uint32_t *)Address;
		Address= Address + 4;
	}
	return i;
}


