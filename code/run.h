#include "include.h"
//#include "stm32f1xx_hal_i2c.h"

//sht30
float temperature = 0, humidity = 0;
int sht_state;
uint8_t dat[6]={0};

//bh1750
uint16_t light = 0;
int bh1750_state;



void setup(){
	SHT30_Reset();
	SHT30_Init();
	
}

void loop(){
	sht_state = SHT30_Read_Dat(dat);
	SHT30_Dat_To_Float(dat, &temperature, &humidity);
	
	bh1750_state = BH1750_Send_Cmd(ONCE_H_MODE);
	HAL_Delay(200);
	bh1750_state = BH1750_Read_Dat(dat);
	light = BH1750_Dat_To_Lux(dat);
	
	
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	HAL_Delay(200);
	
}