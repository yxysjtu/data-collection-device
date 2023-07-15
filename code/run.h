#include "include.h"
#include "math.h"

//#define TEST_HARDWARE

//sht30
float temperature = 0, humidity = 0;
int sht_state;
uint8_t dat[6]={0};

//bh1750
int32_t light = 0;
int bh1750_state;

//mic
#define SAMPLE_N 1000
uint32_t sound_v, vref;
float vbat;
int16_t raw_sound[SAMPLE_N];

int32_t sound_level = 0, sound_level0 = 0;
float db = 30, k = 22.3, offset = -32, db0, vn;

//flash
//uint8_t txbuf[4096];
//uint8_t rxbuf[4096];
uint16_t flash_id = 0;
extern int usb_write;
int stat_read = 0;

//fatfs
char* path= "0:";
FATFS fs;
FRESULT fr, cfg_stat;
FIL file;
UINT br;
FILINFO info, info2;

//oled
char str[50]={'\0'};
uint8_t oled_init = 0;

//RTC
uint32_t rtc_cnt;
int wsec,wmin,whour,wday,wmon,wyear;
uint32_t time_sep = 3600, reset_time = 0;
extern uint8_t usb_state;

uint8_t usb_on = 0;

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
    /*if(fr == FR_NO_FILESYSTEM){
        fr = f_mkfs(path, 0, 0);
        f_mount(NULL, path, 1);
        fr = f_mount(&fs, path, 1);
    }*/

 
	RTC_Get();

    memset(buf,0,sizeof(buf));
    sprintf((char *)buf,"0:FC%02d%02d%02d.csv",calendar.w_year%2000,calendar.w_month,calendar.w_date);//按日期进行数据存储
    fr = f_open(&file,(const char *)buf, FA_OPEN_ALWAYS | FA_WRITE);
    if(fr == FR_OK){
        size = f_size(&file);
        if(size == 0){ //create
            memset(buf,0,sizeof(buf));
            sprintf((char *)buf,"%s,%s,%s,%s,%s,%s\r\n","Time","Temperature","Humidity","Light","Sound","VBAT");
            fr= f_write(&file, buf, getsize(buf),(void *)&br);
            if(fr != FR_OK){
                printf("Title write process error code is %d...\r\n",fr);  
            } 
        }else{
            fr = f_lseek(&file, size);
            memset(buf,0,sizeof(buf));
			vbat = 4096/(float)vref * 1.5;
			float vbat_percent = (vbat - 2.5)/(3.2 - 2.5) * 100;
			if(vbat_percent < 0) vbat_percent = 0;
			else if(vbat_percent > 100) vbat_percent = 100;
            sprintf((char *)buf,"%d:%d,%.1f,%.1f,%d,%.1f,%.0f%%\r\n", 
                        calendar.hour,
                        calendar.min,
                        temperature,
                        humidity,
                        light,
                        db,
						vbat_percent);
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
int write_n = 0;
void write_data(int n){
	char buf[10];
	uint32_t size = 0;
	fr = f_mount(&fs, path, 1);
	sprintf((char *)buf,"0:data.txt");
	fr = f_open(&file,buf, FA_OPEN_ALWAYS | FA_WRITE);
	size = f_size(&file);
	//if(size > 0 && n > 1)
		fr = f_lseek(&file, size);
	for(int i = 0; i < SAMPLE_N; i++){
		memset(buf,0,sizeof(buf));
        sprintf((char *)buf,"%d,\r\n", raw_sound[i]);
        fr = f_write(&file, buf, getsize(buf),(void *)&br);
	}
	fr = f_close(&file);
	
}

void led_fastblink(){
	for(int i = 0; i < 4; i++){
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		HAL_Delay(100);
	}
}

void read_sensor(){
	temperature = 0;
	humidity = 0;
	int try_n = 10;
	while(temperature <= 0 && try_n > 0){
		sht_state = SHT30_Read_Dat(dat);
		SHT30_Dat_To_Float(dat, &temperature, &humidity);
		try_n--;
	}
	
	bh1750_state = BH1750_Send_Cmd(ONCE_H_MODE);
	HAL_Delay(300);
	bh1750_state = BH1750_Read_Dat(dat);
	light = BH1750_Dat_To_Lux(dat);
	//light += (uint32_t)(1.1887 * (float)BH1750_Dat_To_Lux(dat) - 5.068);	
	if(bh1750_state == 1) light = -1;
	
	db = 0;
	for(int j = 0; j < 1; j++){
		for(int k = 0; k < 5; k++)
			vref += ADC_GetVal(ADC_CHANNEL_VREFINT);
		vref /= 5;
		
		for(int i = 0; i < SAMPLE_N; i++){
			raw_sound[i] = ADC_GetVal(ADC_CHANNEL_1);
		}
		
		//filter
		for(int i = 0; i < SAMPLE_N - 1; i++){
			raw_sound[i] = (raw_sound[i] + raw_sound[i + 1]) / 2;
		}
		//calculate dc
		sound_v = 0;
		for(int i = 0; i < SAMPLE_N; i++){
			sound_v += raw_sound[i];
		}
		sound_v /= SAMPLE_N;
		db0 = 0;
		for(int i = 0; i < SAMPLE_N; i++){
			raw_sound[i] -= sound_v;
			//energy
			db0 += (float)raw_sound[i] * raw_sound[i];
		}
		db += 4.047*log(db0 / vref / vref * 2000)+29.4084;
	}
	db /= 1;
	db = db * 1.2592 - 5.9167;
}

void oled_show(){
	read_sensor();
	
	sprintf(str, "T:%.1f H:%.1f", temperature, humidity);
	OLED_ShowStr(0,0,(uint8_t*)str,2);
	sprintf(str, "light:%d", light);
	OLED_ShowStr(0,2,(uint8_t*)str,2);
	sprintf(str, "sound:%.1f", db);
	OLED_ShowStr(0,4,(uint8_t*)str,2);
	sprintf(str, "vref:%d", vref);
	OLED_ShowStr(0,6,(uint8_t*)str,2);
}

void setup(){
	#ifdef TEST_HARDWARE
	while(1){
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		HAL_Delay(500);	
	}
	#endif
	
	SHT30_Reset();
	SHT30_Init();

	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)){
		//read vbat
		fr = f_mount(&fs, path, 1);
		fr = f_open(&file,"0:device.inf", FA_OPEN_ALWAYS | FA_READ);
		uint8_t numofread;
		fr = f_read(&file, &vref, 4, (UINT*)&numofread);
		fr = f_close(&file);
		vbat = 4096/(float)vref * 1.5;
	}
	while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)){
		usb_on = 1;
		RTC_Get();
		if(vbat <= 2.5){
			led_fastblink();
		}
		//HAL_Delay(500);
		//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		read_sensor();
		//oled_show();
	}
	//RTC_Set(2023,6,1,23,12,0);
	
	//read configuration
	stat_read = 1;
	fr = f_mount(&fs, path, 1);
	cfg_stat = f_stat("0:device.cfg", &info2);
	fr = f_open(&file,"0:device.cfg", FA_OPEN_ALWAYS | FA_READ);
	uint8_t numofread;
	uint8_t buf[8];
	fr = f_read(&file, &buf, 8, (UINT*)&numofread);
	if(fr == FR_OK){
		time_sep = *(uint32_t*)(buf+0);
		reset_time = *(uint32_t*)(buf+4);
	}
	fr = f_close(&file);
	
	if(!usb_on){
		//write battery voltage
		for(int k = 0; k < 5; k++)
			vref += ADC_GetVal(ADC_CHANNEL_VREFINT);
		vref /= 5;
		fr = f_open(&file,"0:device.inf", FA_OPEN_ALWAYS | FA_WRITE);
		fr = f_write(&file, &vref, 4, (UINT*)&numofread);
		fr = f_close(&file);
	}
	
	//write data into csv file
	for(int i = 0; i < 2; i++){
	read_sensor();}
	fs_write();
	//}db=0;fs_write();
		
	if(reset_time == 0xffffffff){//new configuration flag
		if(cfg_stat == FR_OK){
			led_fastblink();
			wsec = info2.ftime&0x1f;
			wmin = (info2.ftime>>5)&0x3f;
			whour = (info2.ftime>>11)&0x1f;
			wday = (info2.fdate)&0x1f;
			wmon = (info2.fdate>>5)&0x0f;
			wyear = (info2.fdate>>9) + 1980;
			RTC_Set(wyear,wmon,wday,whour,wmin,wsec);
			
			reset_time = 0;
			fr = f_mount(&fs, path, 1);
			fr = f_open(&file,"0:device.cfg", FA_OPEN_ALWAYS | FA_WRITE);
			fr = f_write(&file, &time_sep, 4, (UINT*)&numofread);
			fr = f_write(&file, &reset_time, 4, (UINT*)&numofread);
			fr = f_close(&file);
		}
	}
	stat_read = 0;
	
	while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)){
		RTC_Get();
		//HAL_Delay(500);
		//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		read_sensor();
		//oled_show();
	}
	
	RTC_SetAlarm(time_sep);
	//enter standby mode
    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);//禁用所有使用的唤醒源:PWR_WAKEUP_PIN1 connected to PA.00
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);//清除所有相关的唤醒标志
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);//启用连接到PA.00的WakeUp Pin
	HAL_PWR_EnterSTANDBYMode();	
	
}

void loop(){  
	rtc_cnt = (RTC->CNTH << 16) + RTC->CNTL;
	RTC_Get();
	read_sensor();
	
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	HAL_Delay(500);
	
	//I2C restart if no reply
}
