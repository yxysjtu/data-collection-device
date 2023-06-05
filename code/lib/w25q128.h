#include "spi.h"
#include "usart.h"
#include <stdio.h>

extern uint8_t w25q128_busy;

uint16_t read_W25Q128_ID();
void W25Q128_Writeblk(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
void W25Q128_Readblk(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);

void Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void Write_Word(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void Read_W25Q128_data(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);
void Erase_Write_data_Sector(uint32_t Address,uint32_t Write_data_NUM);
void Erase_one_Sector(uint32_t Address);