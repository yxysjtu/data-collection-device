/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
 #include "rtc.h"
/* USER CODE END Header */
#include "fatfs.h"

uint8_t retUSER;    /* Return value for USER */
char USERPath[4];   /* USER logical drive path */
FATFS USERFatFS;    /* File system object for USER logical drive */
FIL USERFile;       /* File object for USER */

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the USER driver ###########################*/
  retUSER = FATFS_LinkDriver(&USER_Driver, USERPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
	RTC_TimeTypeDef time={0};
	RTC_DateTypeDef date={0};
	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);  
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);  
    return ((DWORD)(date.Year + 2023 - 1980) << 25) /* Year 2015 */
		| ((DWORD)date.Month << 21) /* Month 1 */
		| ((DWORD)date.WeekDay << 16) /* Mday 1 */
		| ((DWORD)time.Hours << 11) /* Hour 0 */
		| ((DWORD)time.Minutes << 5) /* Min 0 */
		| ((DWORD)time.Seconds >> 1); /* Sec 0 */
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */

/* USER CODE END Application */
