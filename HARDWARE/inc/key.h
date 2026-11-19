#ifndef _KEY_H
#define _KEY_H

#include "stm32f4xx.h"
#include "delay.h"
#include "io_bit.h"

#define KEY1 PAin(0)
#define KEY2 PBin(0)
#define KEY3 PBin(1)

#define KEY1_PRE 1
#define KEY2_PRE 2
#define KEY3_PRE 3


typedef enum
{
	KEY_SHORT_PRESS = 0X00,	//短按
	KEY_LONG_PRESS  = 0X80,	//长按
	
	KEY1_SHORT = 0x01,
	KEY2_SHORT = 0x02,
	KEY3_SHORT = 0x04,
	
	KEY1_LONG  = 0x81,
	KEY2_LONG  = 0x82,
	KEY3_LONG  = 0x84,
	
	KEY_FREE   = 10,			//空闲
	KEY_SHAKE  = 11,			//抖动
	KEY_PRESS  = 12,			//按下
	KEY_LOOSEN = 13,			//松开

}KEY_STATE;



void key_init(void);
u8 key_scan(void);
u8 Scanf_Key(void);


#endif
