#ifndef __SHT30_H_
#define __SHT30_H_
 
#include "i2c.h"
 
 void SHT30_Reset(void);
 uint8_t SHT30_Init(void);
 uint8_t SHT30_Read_Dat(uint8_t* dat);
 uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value);
 uint8_t SHT30_Dat_To_Float(uint8_t* const dat, float* temperature, float* humidity);
 
 
 #endif