#ifndef SDFF_H
#define SDFF_H

#include "ff.h"
#include "stdio.h"
#include "sd.h"

void FF_SD_Write(char* filename, BYTE write_buff[],uint8_t bufSize);
void FF_SD_GetCapacity(void);

#endif
