#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32f4xx.h" 

#define SYS_SUPPORT_OS	1
void delay_init(unsigned char clk);         /* 初始化延迟函数 */
void delay_us(unsigned int us);             /* 延时ums */
void delay_ms(u32 ms);
#endif

