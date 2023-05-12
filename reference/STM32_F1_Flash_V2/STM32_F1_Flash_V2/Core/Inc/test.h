

#ifndef	__MY_FLASH_H__
#define	__MY_FLASH_H__


#include "stm32f1xx_hal.h" 
#include "stm32f1xx_hal_flash.h" 

unsigned int flash_eraser_write(unsigned int* DATA_32, unsigned int num);

unsigned int flash_read(unsigned int* DATA_32, unsigned int num);

#endif

