#include "w25q128.h"


uint8_t w25q128_busy = 0;
/* Nicky ******************************************************************* */
//器件使能
void W25Q128_Enable()
{
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, 0); // Chip select
}
 
/* Nicky ******************************************************************* */
//器件失能
void W25Q128_Disable()
{
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, 1); // Chip disselect
}
 
/* Nicky ******************************************************************* */
//SPI2 发送 1 个字节数据
void spi2_Transmit_one_byte(uint8_t TxData)
{
	uint8_t Rxdata;
	HAL_SPI_TransmitReceive(&hspi1, &TxData, &Rxdata, 1, 1000);  
}
 
/* Nicky ******************************************************************* */
//SPI2 接收 1 个字节数据
uint8_t spi2_Receive_one_byte()
{
	uint8_t Rxdata, TxData = 0xff;
	HAL_SPI_TransmitReceive(&hspi1, &TxData, &Rxdata, 1, 1000);       
 	return Rxdata; 
}
 
/* Nicky ******************************************************************* */
//W25Q128写使能,将WEL置1 
void W25Q128_Write_Enable()   
{
	W25Q128_Enable();                            //使能器件   
    spi2_Transmit_one_byte(0x06); 
	W25Q128_Disable();                            //取消片选     	      
}
 
/* Nicky ******************************************************************* */
//W25Q128写失能,将WEL置0 
void W25Q128_Write_Disable()   
{
	W25Q128_Enable();                            //使能器件   
    spi2_Transmit_one_byte(0x04); 
	W25Q128_Disable();                            //取消片选     	      
}
 
/* Nicky ******************************************************************* */
//读取寄存器状态
uint8_t W25Q128_ReadSR(void)   
{  
	uint8_t byte=0;   
	W25Q128_Enable();                            //使能器件   
	spi2_Transmit_one_byte(0x05);    //发送读取状态寄存器命令
	byte=spi2_Receive_one_byte();             //读取一个字节
	W25Q128_Disable();                           //取消片选     
	return byte;   
} 
 
/* Nicky ******************************************************************* */
//等待空闲
void W25Q128_Wait_Busy()   
{   
	while((W25Q128_ReadSR()&0x01)==0x01);   // 等待BUSY位清空
}
 
/* Nicky ******************************************************************* */
//擦除地址所在的一个扇区
void Erase_one_Sector(uint32_t Address)
{
	W25Q128_Write_Enable();                  //SET WEL 	 
	W25Q128_Wait_Busy(); 		
	W25Q128_Enable();                            //使能器件 
	spi2_Transmit_one_byte(0x20);      //发送扇区擦除指令 
	spi2_Transmit_one_byte((uint8_t)((Address)>>16));  //发送24bit地址    
	spi2_Transmit_one_byte((uint8_t)((Address)>>8));   
	spi2_Transmit_one_byte((uint8_t)Address);  
	W25Q128_Disable();                            //取消片选     	      
	W25Q128_Wait_Busy(); 				   //等待擦除完成
}
 
 
/* Nicky ******************************************************************* */
//擦除地址所在的扇区
void Erase_Write_data_Sector(uint32_t Address,uint32_t Write_data_NUM)   
{
	//总共4096个扇区
	//计算 写入数据开始的地址 + 要写入数据个数的最后地址 所处的扇区	
	uint16_t Star_Sector,End_Sector,Num_Sector;
	Star_Sector = Address / 4096;						//数据写入开始的扇区
	End_Sector = (Address + Write_data_NUM) / 4096;		//数据写入结束的扇区
	Num_Sector = End_Sector - Star_Sector;  			//数据写入跨几个扇区
 
	//开始擦除扇区
	for(uint16_t i=0;i <= Num_Sector;i++)
	{
		Erase_one_Sector(Address);
		Address += 4095;
	}
 
}
 
/* Nicky ******************************************************************* */
//擦除整个芯片 等待时间超长... 10-20S
void Erase_W25Q128_Chip(void)   
{                                   
    W25Q128_Write_Enable();                  //SET WEL 
    W25Q128_Wait_Busy();   
  	W25Q128_Enable();                            //使能器件   
    spi2_Transmit_one_byte(0x60);        //发送片擦除命令  
	W25Q128_Disable();                            //取消片选     	      
	W25Q128_Wait_Busy();   				   //等待芯片擦除结束
} 
 
/* Nicky ******************************************************************* */
//读取W25Q128数据
void Read_W25Q128_data(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i=0;   										    
	W25Q128_Enable();                     //使能器件   
    spi2_Transmit_one_byte(0x03);         //发送读取命令   
    spi2_Transmit_one_byte((uint8_t)((ReadAddr)>>16));  //发送24bit地址    
    spi2_Transmit_one_byte((uint8_t)((ReadAddr)>>8));   
    spi2_Transmit_one_byte((uint8_t)ReadAddr);   
    for(;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=spi2_Receive_one_byte();   //循环读数  
    }
	W25Q128_Disable(); 				    	      
}
 
/* Nicky ******************************************************************* */
//写字，一次最多一页
void Write_Word(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
 	uint16_t i; 
 
	W25Q128_Write_Enable();                  //SET WEL
	W25Q128_Enable();                            //使能器件
	spi2_Transmit_one_byte(0x02);
    spi2_Transmit_one_byte((uint8_t)((WriteAddr) >> 16)); //写入的目标地址   
    spi2_Transmit_one_byte((uint8_t)((WriteAddr) >> 8));   
    spi2_Transmit_one_byte((uint8_t)WriteAddr);   
    for (i = 0; i < NumByteToWrite; i++)
		spi2_Transmit_one_byte(pBuffer[i]);//循环写入字节数据  
	W25Q128_Disable();
	W25Q128_Wait_Busy();		//写完之后需要等待芯片操作完。
}
 
/* Nicky ******************************************************************* */
//定位到页
void Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{
	uint16_t Word_remain;
	Word_remain=256-WriteAddr%256; 	//定位页剩余的字数	
	
	if(NumByteToWrite <= Word_remain)
		Word_remain=NumByteToWrite;		//定位页能一次写完
	while(1)
	{
		Write_Word(pBuffer,WriteAddr,Word_remain);	
		if(NumByteToWrite==Word_remain)
		{
			break;	//判断写完就 break
		}	
	 	else //没写完，翻页了
		{
			pBuffer += Word_remain;		//直针后移当页已写字数
			WriteAddr += Word_remain;	
			NumByteToWrite -= Word_remain;	//减去已经写入了的字数
			if(NumByteToWrite>256)
				Word_remain=256; 		//一次可以写入256个字
			else 
				Word_remain=NumByteToWrite; 	//不够256个字了
		}
	}	    
} 

void W25Q128_Writeblk(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len){
	for(uint32_t blk = blk_addr; blk < blk_addr + blk_len; blk++){
		Erase_one_Sector(blk * 4096);
		Write_Page(buf + (blk - blk_addr) * 4096, blk * 4096, 4096);
	}
}

void W25Q128_Readblk(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len){
	for(uint32_t blk = blk_addr; blk < blk_addr + blk_len; blk++){
		Read_W25Q128_data(buf + (blk - blk_addr) * 4096, blk * 4096, 4096);
	}
}
 
/* Nicky ******************************************************************* */
// 读取 ID 测试 OK 0xEF 0X17
uint16_t read_W25Q128_ID()
{
	uint8_t _RxData[2]={0x00};
	W25Q128_Enable();
	
	//发送指令
    spi2_Transmit_one_byte(0x90);
	spi2_Transmit_one_byte(0x00);
	spi2_Transmit_one_byte(0x00);
	spi2_Transmit_one_byte(0x00);
	
	//接收数据
	_RxData[0] = spi2_Receive_one_byte();
	_RxData[1] = spi2_Receive_one_byte();
	
    W25Q128_Disable();
    
	return *(uint16_t *)_RxData;
}
 
