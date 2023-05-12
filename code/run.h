#include "include.h"
#include "math.h"

//sht30
float temperature = 0, humidity = 0;
int sht_state;
uint8_t dat[6]={0};

//bh1750
uint16_t light = 0;
int bh1750_state;

//mic
uint32_t sound_v;
int raw_sound;
int32_t sound_level = 0;
float db = 30, k = 20, offset = 0;

//flash
const u8 TEXT_Buffer[]={"STM32F103 FLASH TEST"};
#define SIZE sizeof(TEXT_Buffer)		//数组长度
u8 datatemp[SIZE];

//fatfs
char path[4]= {"0:"};
FRESULT FATFS_Status;
	
void setup(){
	SHT30_Reset();
	SHT30_Init();
	
	//FATFS_Status = f_mount(&USERFatFS, path, 1);
    //f_mkdir("0:/FW");

	//STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)TEXT_Buffer,SIZE);//第一次写读
	//STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,SIZE);
}

void loop(){
	sht_state = SHT30_Read_Dat(dat);
	SHT30_Dat_To_Float(dat, &temperature, &humidity);

	/*for(int j = 0; j < 10; j++){
		sound_v = 0;
		for(int i = 0; i < 1000; i++){
			sound_v += ADC_GetVal(ADC_CHANNEL_1);
		}
		sound_v = sound_v / 1000;
		sound_level = 0;
		for(int i = 0; i < 1000; i++){
			raw_sound = ADC_GetVal(ADC_CHANNEL_1) - sound_v;
			if(raw_sound < 0) raw_sound = -raw_sound;
			sound_level += raw_sound;
		}
		sound_level /= 1000;
		db = db * 0.9 + (k * log((double)sound_level) + offset) * 0.1;
	}*/
	
	
	bh1750_state = BH1750_Send_Cmd(ONCE_H_MODE);
	HAL_Delay(200);
	bh1750_state = BH1750_Read_Dat(dat);
	light = BH1750_Dat_To_Lux(dat);	
	
	
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	HAL_Delay(200);
	
	
}
