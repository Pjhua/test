#ifndef _TIMER_H
#define _TIMER_H

#include "stm32f4xx.h"
#include "stdio.h"
#include "esp8266.h"
#include "rtc.h"

#define RTC_USE_TIMER 0

extern u32 time6_us;
extern u32 tim7_tick_cnt;


/* 年 月 日 时 分 秒 */
typedef struct
{
    u16 year; /* 年，如 2026 */
    u8 month; /* 月，1~12    */
    u8 day;   /* 日，1~31    */
    u8 hour;  /* 时，0~23    */
    u8 min;   /* 分，0~59    */
    u8 sec;   /* 秒，0~59    */
} datetime_t;

extern datetime_t g_datetime; /* 当前的年月日时分秒 */
extern u8 key_flag;
extern u8 key_val;

void timer6_init(u16 arr, u16 psc);
void timer7_init(u16 arr, u16 psc);
void timer2_init(u32 arr, u16 psc);
void delay_us_tim6(u32 us);
void delay_ms_tim6(u32 ms);

/* ---------- 软件时钟 ---------- */
u8 time_is_leap_year(u16 year);            /* 判断闰年：1闰年 0平年 */
u8 time_days_in_month(u16 year, u8 month); /* 取某年某月的天数 */
void time_set(u8 weekday, u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec);
u8 time_set_from_str(const char *str, const char *week); /* "2026-09-22 10:42:04" */
void time_tick_1s(void);                                 /* 时间走1秒，处理进位 */
void time_handle(void);                                  /* 放在主循环里调用，处理中断攒下的秒数 */
void time_get_str(char *buf, u16 len);                   /* 格式化成字符串 */

void timer1_lvgl_init(void);

#endif
