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
float db = 30, k = 8, offset = 0;

//flash
uint8_t txbuf[4096];
uint8_t rxbuf[4096];
uint16_t flash_id = 0;


//fatfs
char* path= "0:";
FATFS fs;
FRESULT fr;
FIL file;
UINT br;

//RTC
//RTC_TimeTypeDef time = {0};
//RTC_DateTypeDef date = {0};
//RTC_AlarmTypeDef alarm = {0};
uint32_t rtc_cnt;
extern uint8_t usb_state;

int testt = 0;

uint32_t getsize(char *s){
	uint32_t i = 0;
	while(s[i] != '\n'){
		i++;
	}
	return i + 1;
}

//TODO write err message in txt file
//TODO if wrong data write NULL
//TODO read config file(auto calibrate time script)

FRESULT fs_write(){
    uint32_t size = 0;
    char buf[50];

    fr = f_mount(&fs, path, 1);
    if(fr == FR_NO_FILESYSTEM){
        fr = f_mkfs(path, 0, 0);
        f_mount(NULL, path, 1);
        fr = f_mount(&fs, path, 1);
    }

    //HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);  
	//HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);  
	RTC_Get();

    memset(buf,0,sizeof(buf));
    sprintf((char *)buf,"0:FC%02d%02d%02d.csv",calendar.w_year%2000,calendar.w_month,calendar.w_date);//按日期进行数据存储
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
                        calendar.hour,
                        calendar.min,
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

void read_sensor(){
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
		db = k * log((double)sound_level) + offset;
	}
	
	
	bh1750_state = BH1750_Send_Cmd(ONCE_H_MODE);
	HAL_Delay(200);
	bh1750_state = BH1750_Read_Dat(dat);
	light = BH1750_Dat_To_Lux(dat);	
}
	
void setup(){
	//RTC_Set(2023,6,1,23,12,0);
	//rtc_cnt = (RTC->CNTH << 16) + RTC->CNTL;
	//date.Month = 5;
	//date.Date = 23;
	//time.Hours = 23;
	//time.Minutes = 59;
	//time.Seconds = 50;
	//HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
	//HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
	
	SHT30_Reset();
	SHT30_Init();
	
	//test flash
	/*flash_id = read_W25Q128_ID();
	for(uint32_t i = 0; i < 4096; i++){
		txbuf[i] = i % 256;
	}*/
	//sprintf((char*)txbuf, "Helloworld!!!");
	
	//W25Q128_Writeblk(txbuf, 4096, 1);
	//W25Q128_Readblk(rxbuf, 4096, 1);
	
	
	
	
	read_sensor();
	read_sensor();
	read_sensor();
	read_sensor();
	read_sensor();
	
	fs_write();
	
	//HAL_GPIO_WritePin(VCONT_GPIO_Port, VCONT_Pin, 1);
	
	
	/*HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);  
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN); 

	time.Seconds += 10;
	if(time.Seconds >= 60){
		time.Seconds = 0;
		time.Minutes++;
	}
	if(time.Minutes >= 60){
		time.Minutes = 0;
		time.Hours++;
	}
	if(time.Hours >= 24){
		time.Hours = 0;
	}
	RTC_AlarmTypeDef alarm = {time, RTC_ALARM_A};
	HAL_RTC_SetAlarm(&hrtc, &alarm, RTC_FORMAT_BIN);*/
	
	while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)){}
	RTC_SetAlarm(60*60);
	
    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);//禁用所有使用的唤醒源:PWR_WAKEUP_PIN1 connected to PA.00
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);//清除所有相关的唤醒标志
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);//启用连接到PA.00的WakeUp Pin
	HAL_PWR_EnterSTANDBYMode();
	
	
	//sprintf((char *)txbuf, "helloworld");
	//Flash_Write(txbuf, 0, 1);
	//Flash_Read(rxbuf, 0, 1);
	
	
	
}

void loop(){ 
	//HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);  
	//HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN); 
	rtc_cnt = (RTC->CNTH << 16) + RTC->CNTL;
	RTC_Get();
	read_sensor();
	
	
	//if(testt++ < 5) fs_write();
	
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	HAL_Delay(500);
	//HAL_GPIO_WritePin(DONE_GPIO_Port, DONE_Pin, 1);
	
	//I2C restart if no reply
}
