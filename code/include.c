#include "include.h"

 
//重写这个函数,重定向printf函数到串口
 
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1 , (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
}
