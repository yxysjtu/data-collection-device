#ifndef __SHT3x_H
#define __SHT3x_H


void SHT3x_Init(void);
unsigned char ReadSHT3x(float *Hum,float *Temp);

#endif
