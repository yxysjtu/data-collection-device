#ifndef MYRTC_H
#define MYRTC_H

#include "main.h"

typedef struct 
{
	volatile uint8_t hour;
	volatile uint8_t min;
	volatile uint8_t sec;			
	//公历日月年周
	volatile uint16_t w_year;
	volatile uint8_t  w_month;
	volatile uint8_t  w_date;
	volatile uint8_t  week;	
}_calendar_obj;					 
extern _calendar_obj calendar;				//日历结构体
												    
//uint8_t Is_Leap_Year(uint16_t year);					//平年,闰年判断
uint8_t RTC_Get(void);         					//获取时间   
uint8_t RTC_Get_Week(uint16_t year,uint8_t month,uint8_t day);
uint8_t RTC_Set(uint16_t syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec);		//设置时间	
uint8_t RTC_Alarm_Set(uint16_t syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec);	//设置闹钟	
void RTC_SetAlarm(uint32_t sec);

#endif
