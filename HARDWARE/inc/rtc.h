#ifndef _RTC_H
#define _RTC_H

#include "stm32f4xx.h"
#include "esp8266.h"
#include "timer.h"

extern u8 rtc_alarm;
extern u8 rtc_wakup_flag;
extern RTC_DateTypeDef RTC_DateStruct;
extern RTC_TimeTypeDef RTC_TimeStruct;

void rtc_config(void);
void rtc_init(void);
void rtc_set_time(u8 weekday,u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec);
void rtc_read_datetime(void); /* 读硬件 RTC → 同步到 g_datetime */
void RTC_AlarmConfig(void);


#endif
