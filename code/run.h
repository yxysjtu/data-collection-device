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
float db = 30, k = 10, offset = 0;

//flash
uint8_t txbuf[2048];
uint8_t rxbuf[2048];


//fatfs
char* path= "0:";
FATFS fs;
FRESULT fr;
FIL file;
UINT br;

//RTC
RTC_TimeTypeDef time={0};
RTC_DateTypeDef date={0};

int testt = 0;

uint32_t getsize(char *s){
	uint32_t i = 0;
	while(s[i] != '\n'){
		i++;
	}
	return i + 1;
}

FRESULT fs_write(){
    uint32_t size = 0;
    char buf[50];

    fr = f_mount(&fs, path, 1);
    if(fr == FR_NO_FILESYSTEM){
        fr = f_mkfs(path, 0, 0);
        f_mount(NULL, path, 1);
        fr = f_mount(&fs, path, 1);
    }

    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);  
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);  

    memset(buf,0,sizeof(buf));
    sprintf((char *)buf,"0:FC%02d%02d%02d.csv",(date.Year+23),date.Month,date.Date);//按日期进行数据存储
    fr = f_open(&file,(const char *)buf, FA_OPEN_ALWAYS | FA_WRITE);
    if(fr == FR_OK){
        size = f_size(&file);
        if(size == 0){ //create
            memset(buf,0,sizeof(buf));
            sprintf((char *)buf,"%s,%s,%s,%s,%s\r\n","Time","temperature","humidity","light","sound");
            fr= f_write(&file, buf, getsize(buf),(void *)&br);
            if(fr != FR_OK){
                printf("Title write process error code is %d...\r\n",fr);  
            } 
        }else{
            fr = f_lseek(&file, size);
            memset(buf,0,sizeof(buf));
            sprintf((char *)buf,"%d:%d,%.1f,%.1f,%d,%.1f\r\n", 
                        time.Hours,
                        time.Minutes,
                        temperature,
                        humidity,
                        light,
                        db);
            fr = f_write(&file, buf, getsize(buf),(void *)&br);
            if(fr != FR_OK){
                printf("Data write process error code is %d...\r\n",fr);  
            }
        }
    }else{
        printf("File Open failed,error code is %d...\r\n",fr);  
        return fr;
    }
    fr = f_close(&file);
    if(fr != FR_OK){
        printf("f_close error code is %d...\r\n",fr);  
    } 

    return fr;
}
	
void setup(){
	SHT30_Reset();
	SHT30_Init();
	
	/*date.Month = 5;
	date.Date = 19;
	time.Hours = 14;
	time.Minutes = 30;
	HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
	HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);*/
	
	//sprintf((char *)txbuf, "helloworld");
	//Flash_Write(txbuf, 0, 1);
	//Flash_Read(rxbuf, 0, 1);
	
	
	
}

void loop(){
	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);  
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);  
	
	sht_state = SHT30_Read_Dat(dat);
	SHT30_Dat_To_Float(dat, &temperature, &humidity);

	for(int j = 0; j < 10; j++){
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
	}
	
	
	bh1750_state = BH1750_Send_Cmd(ONCE_H_MODE);
	HAL_Delay(200);
	bh1750_state = BH1750_Read_Dat(dat);
	light = BH1750_Dat_To_Lux(dat);	
	
	
	if(testt++ < 5) fs_write();
	
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	HAL_Delay(200);
	
	//I2C restart if no reply
}
