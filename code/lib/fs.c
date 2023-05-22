#include "fs.h"

char *path = "0:";

FRESULT fs_init(){
    FATFS fs;
    FRESULT fr;
    fr = f_mount(&fs, path, 1);
    if(fr == FR_NO_FILESYSTEM){
        fr = f_mkfs(path, 0, 0);
    }
    f_mount(NULL, path, 1);
    return fr;
}

FRESULT fs_write(){
    FATFS fs;
    FRESULT fr;
    FIL file;
    UINT br;
    uint32_t size = 0;
    char buf[50];

    fr = f_mount(&fs, path, 1);
    if(fr == FR_NO_FILESYSTEM){
        fr = f_mkfs(path, 0, 0);
        f_mount(NULL, path, 1);
        fr = f_mount(&fs, path, 1);
    }

    RTC_TimeTypeDef time={0};
    RTC_DateTypeDef date={0};
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);  
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);  

    memset(buf,0,sizeof(buf));
    sprintf((char *)buf,"0:FC%02d%02d%02d.csv",(date.Year+23),date.Month,date.Date);//按日期进行数据存储
    fr = f_open (&file,(const char *)buf, FA_OPEN_ALWAYS | FA_WRITE);
    if(fr == FR_OK){
        size = f_size(&file);
        if(size == 0){ //create
            memset(buf,0,sizeof(buf));
            sprintf((char *)buf,"%s,%s,%s\r\n","Time","temperature","humidity","light","sound");
            fr= f_write(&file, buf, sizeof(buf),(void *)&br);
            if(fr != FR_OK){
                printf("Title write process error code is %d...\r\n",fr);  
            } 
        }else{
            fr = f_lseek(&file, size);
            memset(buf,0,sizeof(buf));
            sprintf((char *)buf,"%d:%d,%.1f,%.1f,%d,%.1f\r\n", 
                        time.Hours,
                        time.Minutes
                        temperature,
                        humidity,
                        light,
                        sound);
            fr = f_write(&fileobj, buf, sizeof(buf),(void *)&br);
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