#ifndef _CST816S_H_
#define _CST816S_H_
 
#include "lcd_init.h"
#include "CTIIC.h"
#include "lcd.h"

#define CTP_INT PDin(1)		// 触摸笔中断

#define CST_WR_CMD 0x2A  //写命令地址  0010 1010 
#define CST_RD_CMD 0x2B  //读数据地址  0010 1011 

#define  FT_DEVIDE_MODE 0x00	   // CST816模式控制寄存器
#define  GT_ADDR_REG    0x02     //触摸状态寄存器
#define  CST816S_ID     0xA7     //CST816SID寄存器

/* 触摸滑动方向枚举，由 cst816s_get_swipe() 返回 */
typedef enum
{
	SLI_UP=-2,
	SLI_DOWN=2,
	SLI_LEFT=1,
	SLI_RIGHT=-1,
	SLI_NULL=0
} SwipeDir;

u8 CST816S_WR_DATA(u16 addr,u8 data);
u8 CST816S_RD_DATA(u16 addr,u8 len,u8 *value);
void  CST816S_Init(void);
u8 CST816S_Read(u16 *x, u16 *y);
u8 CST816S_GetPoint(u16 *x, u16 *y); // 不检查INT引脚，直接I2C读取：有手指返回0，无手指返回1，出错返回2
SwipeDir cst816s_get_swipe(void);
#endif
