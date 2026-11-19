#include "CST816S.h"
#include "CTIIC.h"
#include "lcd.h"
#include "delay.h"

u8 ID;

// 写数据
u8 CST816S_WR_DATA(u16 addr, u8 data)
{
	u8 reg;
	CTP_IIC_Start();		  // IIC起始信号
	CTP_SendByte(CST_WR_CMD); // 发送写指令
	reg = CTP_WaitAck();
	CTP_SendByte(addr);
	reg = CTP_WaitAck();
	CTP_SendByte(data); // 写入数据
	reg = CTP_WaitAck();
	CTP_IIC_Stop(); // IIC结束信号
	return reg;
}

// 读数据
u8 CST816S_RD_DATA(u16 addr, u8 len, u8 *value)
{
	u8 reg, i;
	CTP_IIC_Start();
	CTP_SendByte(CST_WR_CMD); // 发送写指令
	reg = CTP_WaitAck();
	CTP_SendByte(addr);
	reg = CTP_WaitAck();
	CTP_IIC_Start();
	CTP_SendByte(CST_RD_CMD);
	reg = CTP_WaitAck();
	for (i = 0; i < len; i++)
	{
		if (i == (len - 1))
		{
			value[i] = CTP_ReadByte(0x00);
		}
		else
		{
			value[i] = CTP_ReadByte(0x01);
		}
	}
	CTP_IIC_Stop();
	return reg;
}

void CST816S_Init(void)
{
	CTP_GPIOInit();

	CTP_RES_Clr();
	CTP_INT_Clr();
	delay_ms(50);
	CTP_INT_Set();
	delay_ms(50);
	CTP_RES_Set();
	delay_ms(50);
	CTP_INT_IN();
	delay_ms(20);
	CST816S_WR_DATA(0xfd, 0x0);

	CTP_RES_Clr();
	CTP_INT_Clr();
	delay_ms(50);
	CTP_INT_Set();
	delay_ms(50);
	CTP_RES_Set();
	delay_ms(50);
	CTP_INT_IN();
	delay_ms(20);
	CST816S_RD_DATA(CST816S_ID, 1, &ID);
	printf("touch ID:%x\r\n", ID);
}

u8 gesture = 0;
// 读取数据
//u8 CST816S_Read(u16 *x, u16 *y)
//{
//	u8 buf[4] = {0};
//	u8 mode = 0;
//	if (CTP_INT == 0)
//	{
//		delay_ms(20);
//		if (CST816S_RD_DATA(0x02, 1, &mode)) // 读取手指个数
//			return 1;
//		printf("手指数量：%d\r\n", mode);
//		if (mode)
//		{
//			if (CST816S_RD_DATA(0x01, 1, &mode)) // 读取手势
//				return 2;
//			gesture = mode;
//			printf("手势：%d\r\n", mode);
//			if (mode)
//			{
//				if (CST816S_RD_DATA(0x03, 4, buf)) // 读取坐标
//					return 3;
//				*x = ((buf[0] & 0x0F) << 8) | buf[1];
//				*y = ((buf[2] & 0x0F) << 8) | buf[3];
//			}
//		}
//		return 0;
//	}
//	return 1;
//}


u8 CST816S_Read(u16 *x, u16 *y)
{
	u8 buf[4] = {0};
	u8 num = 0; // 手指个数
	u8 ges = 0; // 手势

	if (CTP_INT == 0)
	{
		delay_ms(20);
		if (CST816S_RD_DATA(0x02, 1, &num)) // 读取手指个数失败
			return 1;
		if (num == 0) // 手指已抬起：本次事件作废，不用旧坐标
			return 1;
		if (CST816S_RD_DATA(0x03, 4, buf)) // 读取坐标失败
			return 2;
		*x = ((buf[0] & 0x0F) << 8) | buf[1];
		*y = ((buf[2] & 0x0F) << 8) | buf[3];

		if (!CST816S_RD_DATA(0x01, 1, &ges)) // 手势单独读、单独存（供以后做双击/长按），不再拦截坐标
			gesture = ges;
		return 0;
	}
	return 1;
}


// 不检查INT引脚，直接通过I2C读取触摸点。返回0有手指，1无手指，2读取出错
u8 CST816S_GetPoint(u16 *x, u16 *y)
{
	u8 buf[4] = {0};
	u8 mode = 0;

	if (CST816S_RD_DATA(0x02, 1, &mode)) // 读取手指个数
		return 2;
	if (mode) // 有手指，读取坐标
	{
		if (CST816S_RD_DATA(0x03, 4, buf))
			return 2;
		*x = ((buf[0] & 0x0F) << 8) | buf[1];
		*y = ((buf[2] & 0x0F) << 8) | buf[3];
		return 0;
	}
	return 1;
}

/* ================= 触摸屏滑动检测 ================= */
#define SWIPE_THRESHOLD 50 // 判定为滑动的最小位移(像素)，太小容易把轻点误判成滑动
#define _ABS(x) ((x) >= 0 ? (x) : -(x))

/* 返回值约定：
 *   1 = 左滑    -1 = 右滑
 *   2 = 上滑    -2 = 下滑
 *   0 = 无滑动（位移太小或无触摸）
 * 按下时记住起点，抬手时结算水平和垂直位移，取主方向返回
 */
SwipeDir cst816s_get_swipe(void)
{
    static u8 touching = 0;    // 当前是否处于按下状态
    static u16 start_x = 0;    // 按下时的x坐标
    static u16 start_y = 0;    // 按下时的y坐标
    static u16 last_x = 0;     // 最近一次的x坐标
    static u16 last_y = 0;     // 最近一次的y坐标
    u16 x, y;
    s16 dx, dy;
    s8 dir = 0;

    if (CST816S_GetPoint(&x, &y) == 0) // 有手指按下
    {
        if (!touching) // 刚按下：记录起点
        {
            touching = 1;
            start_x = x;
            start_y = y;
        }
        last_x = x; // 持续更新最后的坐标
        last_y = y;
    }
    else if (touching) // 刚抬手：结算本次滑动
    {
        touching = 0;
        dx = (s16)last_x - (s16)start_x;        // 正值=向右，负值=向左
        dy = (s16)start_y - (s16)last_y;        // 正值=向上，负值=向下

        if ((_ABS(dx) >= _ABS(dy)) && (_ABS(dx) >= SWIPE_THRESHOLD))
        {
            /* 水平为主 */
            dir = (dx < 0) ? SLI_LEFT : SLI_RIGHT;            // 1=左滑，-1=右滑
        }
        else if ((_ABS(dy) > _ABS(dx)) && (_ABS(dy) >= SWIPE_THRESHOLD))
        {
            /* 垂直为主 */
            dir = (dy > 0) ? SLI_UP : SLI_DOWN;            // 2=上滑，-2=下滑
        }
    }
    return dir;
}












