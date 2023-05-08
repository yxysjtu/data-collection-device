#include "include.h"

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
float db = 30;

//sd card
int sd_state = 0;
uint32_t sd_sector_count;
uint8_t block_w[512] = {0};
uint8_t block_r[512] = {0};
const char* txt = "HELLOWORLD2";

//FatFs
char SD_FileName[] = "hello.txt";
uint8_t WriteBuffer[] = "01 write buff to sd \r\n";
//uint8_t test_sd =0;	//用于测试格式化
uint8_t write_cnt =0;	//写SD卡次数


int tf = 0;

void setup(){
	SHT30_Reset();
	SHT30_Init();
	
	sd_state = SD_Init();
	sd_sector_count = SD_GetSectorCount();
	
	FF_SD_Write("a.txt", (BYTE*)txt, 12);
	//snprintf((char *)block_w, 12, txt);
	//sd_state = SD_WriteSector(block_w, 0, 1);
	/*for(tf = 0; tf < 150; tf++){
		sd_state = SD_WriteSector(block_w, tf, 1);
		printf("w %d \r\n", sd_state);
	}
	sd_state = SD_Init();
	printf("init %d\r\n", sd_state);
	int timeout = 5;
	while(sd_state && timeout--){
		sd_state = SD_Init();
		printf("init %d\r\n", sd_state);
	}
	for(tf = 0; tf < 300; tf++){
		sd_state = SD_ReadSector(block_r, tf, 1);
		printf("r %d\r\n", sd_state);
	}
	*/

}

void loop(){
	
	sht_state = SHT30_Read_Dat(dat);
	SHT30_Dat_To_Float(dat, &temperature, &humidity);

	//not working very great
	/*sound_v = 0;
	for(int i = 0; i < 100; i++){
		sound_v += ADC_GetVal(ADC_CHANNEL_0);
	}
	sound_v = sound_v / 100;
	sound_level = 0;
	for(int i = 0; i < 100; i++){
		raw_sound = ADC_GetVal(ADC_CHANNEL_0) - sound_v;
		if(raw_sound < 0) raw_sound = -raw_sound;
		sound_level += raw_sound;
	}
	sound_level /= 100;
	db = db * 0.9 + (float)sound_level * 0.1;*/
	
	
	bh1750_state = BH1750_Send_Cmd(ONCE_H_MODE);
	HAL_Delay(200);
	bh1750_state = BH1750_Read_Dat(dat);
	light = BH1750_Dat_To_Lux(dat);	
	
	
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	HAL_Delay(200);
	
}
