#include "sd.h"

#define SDCARD  1
u8  SD_Type=0;//SD卡的类型 

spi_dev_t sdcard = {
	.hspi = &hspi1,
	.CS_Port = SD_CS_GPIO_Port,
	.CS_Pin = SD_CS_Pin
};

//static uint8_t    spi_tx_buf[256];   /**< TX buffer. */
//static uint8_t    spi_rx_buf[256];   /**< RX buffer. */





//向SD卡发送一个命令
//输入: u8 cmd   命令 
//      u32 arg  命令参数
//      u8 crc   crc校验值	   
//返回值:SD卡返回的响应															  
u8 SD_SendCmd(u8 cmd, u32 arg, u8 crc)
{
    u8 r1;	
	u8 Retry=0; 
	// SD_DisSelect();//取消上次片选
	// if(SD_Select())return 0XFF;//片选失效 
	//发送
    SPI_WriteOne(&sdcard, cmd | 0x40);//分别写入命令
    SPI_WriteOne(&sdcard, arg >> 24);
    SPI_WriteOne(&sdcard, arg >> 16);
    SPI_WriteOne(&sdcard, arg >> 8);
    SPI_WriteOne(&sdcard, arg);	  
    SPI_WriteOne(&sdcard, crc); 
	if(cmd==CMD12)SPI_WriteOne(&sdcard, 0xff);//Skip a stuff byte when stop reading
    //等待响应，或超时退出
	Retry=0X1F;
	do{
		r1=SPI_ReadOne(&sdcard);
	}while((r1&0X80) && Retry--);	 
	//返回状态值
    return r1;
}		


u8 SD_Init(void)
{
    u8 r1;      // 存放SD卡的返回值
    u16 retry;  // 用来进行超时计数
    u8 buf[4];  
	u16 i;
	//if(SD_GetResponse(0xFF)) return 1;
	SPI_SetSpeed(&sdcard, SPI_BAUDRATEPRESCALER_256);//低速
	//HAL_Delay(1);
 	for(i=0;i<10;i++)SPI_ReadOne(&sdcard);//发送最少74个脉冲
	retry=20;
	do
	{
		r1=SD_SendCmd(CMD0,0,0x95);//进入IDLE状态
	}while((r1!=0X01) && retry--);
 	SD_Type=0;//默认无卡
	if(r1==0X01)
	{
		if(SD_SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
		{
			for(i=0;i<4;i++)buf[i]=SPI_ReadOne(&sdcard);	//Get trailing return value of R7 resp
			if(buf[2]==0X01&&buf[3]==0XAA)//卡是否支持2.7~3.6V
			{
				retry=0XFFFE;
				do
				{
					SD_SendCmd(CMD55,0,0X01);	//发送CMD55
					r1=SD_SendCmd(CMD41,0x40000000,0X01);//发送CMD41
				}while(r1&&retry--);
				if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//鉴别SD2.0卡版本开始
				{
					for(i=0;i<4;i++)buf[i]=SPI_ReadOne(&sdcard);//得到OCR值
					if(buf[0]&0x40)SD_Type=SD_TYPE_V2HC;    //检查CCS
					else SD_Type=SD_TYPE_V2;   
				}
			}
		}else//SD V1.x/ MMC	V3
		{
			SD_SendCmd(CMD55,0,0X01);		//发送CMD55
			r1=SD_SendCmd(CMD41,0,0X01);	//发送CMD41
			if(r1<=1)
			{		
				SD_Type=SD_TYPE_V1;
				retry=0XFFFE;
				do //等待退出IDLE模式
				{
					SD_SendCmd(CMD55,0,0X01);	//发送CMD55
					r1=SD_SendCmd(CMD41,0,0X01);//发送CMD41
				}while(r1&&retry--);
			}else
			{
				SD_Type=SD_TYPE_MMC;//MMC V3
				retry=0XFFFE;
				do //等待退出IDLE模式
				{											    
					r1=SD_SendCmd(CMD1,0,0X01);//发送CMD1
				}while(r1&&retry--);  
			}
			if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)SD_Type=SD_TYPE_ERR;//错误的卡
		}
	}
	// SD_DisSelect();//取消片选
	SPI_SetSpeed(&sdcard, SPI_BAUDRATEPRESCALER_4);//高速
	if(SD_Type)return 0;
	else if(r1)return r1; 
	return 0xaa;//其他错误
}

//等待SD卡回应
//Response:要得到的回应值
//返回值:0,成功得到了该回应值
//    其他,得到回应值失败
u8 SD_GetResponse(u8 Response)
{
	u16 Count=0xFFFF;//等待次数	   						  
	while ((SPI_ReadOne(&sdcard)!=Response)&&Count)Count--;//等待得到准确的回应  	  
	if (Count==0)return MSD_RESPONSE_FAILURE;//得到回应失败   
	else return MSD_RESPONSE_NO_ERROR;//正确回应
}

u8 SD_WriteSector(u8* buf,u32 sector,u32 cnt){
	u32 i = 0;
	u8 state = 0;
	if(SD_GetResponse(0xFF)) return 1;
	if(cnt == 1){
		SD_SendCmd(CMD24, sector, 0x01);//写单个扇区
		state = SD_SendData(buf, 0xFE);//写512个字节	   
	}
	else{
		SD_SendCmd(CMD55,0,0x01);	
		SD_SendCmd(CMD23,cnt,0x01);   //设置多扇区写入前预先擦除N个block
 		SD_SendCmd(CMD25,sector,0x01);//写多个扇区
		for(i = 0; i < cnt; i++){
			SD_SendData(buf,0xFC);//写512个字节	 
			buf += 512;  
		}
		state = SD_SendData(0,0xFD);//写结束指令
	}
	HAL_Delay(1);
	return state;
}

u8 SD_ReadSector(u8* buf, u32 sector, u32 cnt){
	u32 i = 0;
	u8 state = 0;
	if(SD_GetResponse(0xFF)) return 1;
	if(cnt == 1){
		SD_SendCmd(CMD17,sector,0x01);//读扇区
		state = SD_RecvData(buf, 512);			//接收512个字节	   
	}else{
		SD_SendCmd(CMD18,sector,0x01);//连续读命令
		for(i = 0; i < cnt; i++){
			state = SD_RecvData(buf,512);//接收512个字节	 
			buf += 512;  
		}
		SD_SendCmd(CMD12,0,0x01);	//停止数据传输
	}   
	HAL_Delay(1);
	return state;

}
u8 SD_SendData(u8* buf, u8 cmd){
	u16 t;		  	  
	if(SD_GetResponse(0xFF)) return 1;
	SPI_WriteOne(&sdcard, cmd);
	// 2. 发送(开始传输)/(结束传输)的标志
	// 写一个扇区的情况下发送0xFE开始传输数据。
	// 写多个扇区的情况下发送0xFC开始传输数据。
	if(cmd != 0xFD)//不是结束指令
	{
		for(t=0;t<512;t++) SPI_WriteOne(&sdcard, buf[t]);//提高速度,减少函数传参时间
	    SPI_WriteOne(&sdcard, 0xFF); //忽略crc
	    SPI_WriteOne(&sdcard, 0xFF);
		SPI_WriteOne(&sdcard, 0xFF); //接收响应								  					    
	}						 									  					    
  	return 0;//写入成功
}

//从sd卡读取一个数据包的内容
//buf:数据缓存区
//len:要读取的数据长度.
//返回值:0,成功;其他,失败;	
u8 SD_RecvData(u8*buf, u16 len)
{			  	  
	if(SD_GetResponse(0xFE)) return 1;//等待SD卡发回数据起始令牌0xFE
    while(len--)//开始接收数据
    {
        *buf = SPI_ReadOne(&sdcard);
        buf++;
    }
    //下面是2个伪CRC（dummy CRC）
    SPI_WriteOne(&sdcard, 0xFF);
    SPI_WriteOne(&sdcard, 0xFF);									  					    
    return 0;//读取成功
}

//获取SD卡的CSD信息，包括容量和速度信息
//输入:u8 *cid_data(存放CID的内存，至少16Byte）	    
//返回值:0：NO_ERR
//		 1：错误														   
u8 SD_GetCSD(u8 *csd_data)
{
    u8 r1;	 
    r1=SD_SendCmd(CMD9,0,0x01);//发CMD9命令，读CSD
    if(r1==0)
	{
    	r1=SD_RecvData(csd_data, 16);//接收16个字节的数据 
    }
	// SD_DisSelect();//取消片选
	if(r1)return 1;
	else return 0;
}  

//获取SD卡的总扇区数（扇区数）   
//返回值:0： 取容量出错 
//       其他:SD卡的容量(扇区数/512字节)
//每扇区的字节数必为512，因为如果不是512，则初始化不能通过.														  
u32 SD_GetSectorCount(void)
{
    u8 csd[16];
    u32 Capacity;  
    u8 n;
	u16 csize;  					    
	//取CSD信息，如果期间出错，返回0
    if(SD_GetCSD(csd)!=0) return 0;	    
    //如果为SDHC卡，按照下面方式计算
    if((csd[0]&0xC0)==0x40)	 //V2.00的卡
    {	
		csize = csd[9] + ((u16)csd[8] << 8) + 1;
		Capacity = (u32)csize << 10;//得到扇区数	 		   
    }else//V1.XX的卡
    {	
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
		Capacity= (u32)csize << (n - 9);//得到扇区数   
    }
    return Capacity >> 5;
}

/********************************************END FILE*******************************************/
