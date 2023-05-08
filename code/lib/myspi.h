#ifndef MYSPI_H
#define MYSPI_H

#include "spi.h"

typedef struct
{
    SPI_HandleTypeDef* hspi;
    GPIO_TypeDef* CS_Port;
    uint16_t CS_Pin;
} spi_dev_t;

uint8_t SPI_ReadWriteByte(spi_dev_t* dev, uint8_t TxData);
void SPI_SetSpeed(spi_dev_t* dev, uint32_t speed);

uint8_t SPI_ReadOne(spi_dev_t* dev);
void SPI_WriteOne(spi_dev_t* dev, uint8_t data);
void SPI_Read(spi_dev_t* dev, uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void SPI_Write(spi_dev_t* dev, uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

#endif
