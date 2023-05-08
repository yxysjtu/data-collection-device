#include "sdff.h"

void FF_SD_Write(char* filename, BYTE write_buff[],uint8_t bufSize)
{
	FATFS fs;
	FIL file;
	uint8_t res=0;
	UINT Bw;	
	
	res = SD_Init();		//SD卡初始化
	
	if(res == 1)
	{
		printf("SD init fail\r\n");		
	}
	else
	{
		printf("SD init success\r\n");		
	}
	
	res=f_mount(&fs,"0:",1);		//挂载
	res = FR_NO_FILESYSTEM;
//	if(test_sd == 0)		//用于测试格式化
	if(res == FR_NO_FILESYSTEM)		//没有文件系统，格式化
	{
//		test_sd =1;				//用于测试格式化
		printf("no file system\r\n");	
        //有问题，把sd卡干烂了	
		res = f_mkfs("0:", 1, 512);		//格式化sd卡
        printf("res_mkfs:%d\r\n", res);
		if(res == FR_OK)
		{
			printf("mkfs success\r\n");	
			
			res = f_mount(NULL,"0:",1); 		//格式化后先取消挂载
            printf("res_demount:%d\r\n", res);
            res = SD_Init();
            printf("res_reinit:%d\r\n", res);
			res = f_mount(&fs,"0:",1);			//重新挂载	
            printf("res_remount:%d", res);
			if(res == FR_OK)
			{
				printf("mount OK\r\n");
			}else{
                printf("mount fail (%d)\r\n", res);
            }	
		}
		else
		{
			printf("mkfs fail\r\n");		
		}
	}
	else if(res == FR_OK)
	{
		printf("SD OK\r\n");		
	}
	else
	{
		printf("SD fail\r\n");
	}	
	
	res = f_open(&file,filename,FA_OPEN_ALWAYS |FA_WRITE);
    printf("res_open:%d\r\n", res);
	if((res & FR_DENIED) == FR_DENIED)
	{
		printf("SD no storage\r\n");		
	}
	
	f_lseek(&file, f_size(&file));//确保写词写入不会覆盖之前的数据
    printf("res_seek:%d\r\n", res);
	if(res == FR_OK)
	{
		printf("open/create file success\r\n");		
		res = f_write(&file,write_buff,bufSize,&Bw);		//写数据到SD卡
		if(res == FR_OK)
		{
			printf("write file success\r\n");			
		}
		else
		{
			printf("write file fail\r\n");
		}		
	}
	else
	{
		printf("open file fail\r\n");
	}	
	
	f_close(&file);						//关闭文件		
	f_mount(NULL,"0:",1);		 //取消挂载
	
}


void FF_SD_GetCapacity(void)
{
	FATFS FS;
	FATFS *fs;
	DWORD fre_clust,AvailableSize,UsedSize;  
	uint16_t TotalSpace;
	uint8_t res;
	
	res = SD_Init();		//SD卡初始化
	if(res == 1)
	{
		printf("sd init fail\r\n");		
	}
	else
	{
		printf("sd init success\r\n");		
	}
	
	/* 挂载 */
	res=f_mount(&FS,"0:",1);		//挂载
	if (res != FR_OK)
	{
		printf("FileSystem Mounted Failed (%d)\r\n", res);
		if(res == FR_NO_FILESYSTEM)		//没有文件系统，格式化
		{
	//		test_sd =1;				//用于测试格式化
			printf("no file system\r\n");		
			res = f_mkfs("", 0, 0);		//格式化sd卡
			if(res == FR_OK)
			{
				printf("mkfs success\r\n");		
				res = f_mount(NULL,"0:",1); 		//格式化后先取消挂载
				res = f_mount(&FS,"0:",1);			//重新挂载	
				if(res == FR_OK)
				{
					printf("mount OK\r\n");
				}else{
                    printf("mount fail (%d)\r\n", res);
                }
			}
			else
			{
				printf("mkfs fail (%d)\r\n", res);		
			}
		}
	}
	
    //HAL_Delay(10);
    printf("getfree\r\n");
	res = f_getfree("0:", &fre_clust, &fs);  /* 根目录 */
	if (res == FR_OK ) 
	{
		TotalSpace=(uint16_t)(((fs->n_fatent - 2) * fs->csize ) / 2 /1024);
		AvailableSize=(uint16_t)((fre_clust * fs->csize) / 2 /1024);
		UsedSize=TotalSpace-AvailableSize;              
		/* Print free space in unit of MB (assuming 512 bytes/sector) */
		printf("\r\n%d MB total drive space.\r\n""%d MB available.\r\n""%d MB  used.\r\n",TotalSpace, AvailableSize,UsedSize);
	}
	else 
	{
		printf("Get SDCard Capacity Failed (%d)\r\n", res);
	}		
} 