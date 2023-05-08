#include "myspi.h"

#define CS(x) HAL_GPIO_WritePin(dev->CS_Port, dev->CS_Pin, x)

uint8_t SPI_ReadWriteByte(spi_dev_t* dev, uint8_t TxData)//发送一个字节，并从寄存器返回一个字节
{
	uint8_t Rxdata;
	HAL_SPI_TransmitReceive(dev->hspi, &TxData, &Rxdata, 1, 1000);       
 	return Rxdata;          		    
}

void SPI_SetSpeed(spi_dev_t* dev, uint32_t speed){
    dev->hspi->Init.BaudRatePrescaler = speed;
    if (HAL_SPI_Init(dev->hspi) != HAL_OK)
    {
        Error_Handler();
    }

}
uint8_t SPI_ReadOne(spi_dev_t* dev){
    //CS(0);
    uint8_t data = SPI_ReadWriteByte(dev, 0xff);
    //CS(1);
    return data;
}

void SPI_WriteOne(spi_dev_t* dev, uint8_t data){
    //CS(0);
    SPI_ReadWriteByte(dev, data);
    //CS(1);
}

void SPI_Read(spi_dev_t* dev, uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)   
{ 
	uint16_t i; 
	
	CS(0);
	SPI_ReadWriteByte(dev, (uint8_t)((ReadAddr) >> 16));   //·写入目标地址   
	SPI_ReadWriteByte(dev, (uint8_t)((ReadAddr) >> 8));   
	SPI_ReadWriteByte(dev, (uint8_t)ReadAddr);   
	for (i = 0; i < NumByteToRead; i++)
	{ 
		pBuffer[i]=SPI_ReadWriteByte(dev, 0XFF);    //循环读入字节数据  
	}	
	CS(1);
} 

void SPI_Write(spi_dev_t* dev, uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
 	uint16_t i; 
 
    CS(0);
    SPI_ReadWriteByte(dev, (uint8_t)((WriteAddr) >> 16)); //写入的目标地址   
    SPI_ReadWriteByte(dev, (uint8_t)((WriteAddr) >> 8));   
    SPI_ReadWriteByte(dev, (uint8_t)WriteAddr);   
    for (i = 0; i < NumByteToWrite; i++)
	SPI_ReadWriteByte(dev, pBuffer[i]);//循环写入字节数据  
	CS(1);
}

