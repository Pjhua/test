#include "fan.h"

void fan_init(void)
{
	GPIO_InitTypeDef gpio_struct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	gpio_struct.GPIO_Mode = GPIO_Mode_OUT;	   // 模式
	gpio_struct.GPIO_Pin = GPIO_Pin_8;		   // 引脚
	gpio_struct.GPIO_Speed = GPIO_Speed_50MHz; // 速度
	gpio_struct.GPIO_OType = GPIO_OType_PP;	   // 推挽
	gpio_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;  // 无上下拉

	GPIO_Init(GPIOC, &gpio_struct);

	GPIO_ResetBits(GPIOC, GPIO_Pin_8);
}

void FAN_ON(void)
{
	FAN = 1;
}

void FAN_OFF(void)
{
	FAN = 0;
}

void FAN_TURN(void)
{
	FAN = !FAN;
}
