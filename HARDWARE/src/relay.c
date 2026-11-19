#include "relay.h"

void relay_init(void)
{
	GPIO_InitTypeDef gpio_struct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	gpio_struct.GPIO_Mode = GPIO_Mode_OUT;	   // 模式
	gpio_struct.GPIO_Pin = GPIO_Pin_0;		   // 引脚
	gpio_struct.GPIO_Speed = GPIO_Speed_50MHz; // 速度
	gpio_struct.GPIO_OType = GPIO_OType_PP;	   // 推挽
	gpio_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;  // 无上下拉

	GPIO_Init(GPIOE, &gpio_struct);

	GPIO_ResetBits(GPIOE, GPIO_Pin_0);
}

void RELAY_ON(void)
{
	RELAY = 1;
}

void RELAY_OFF(void)
{
	RELAY = 0;
}

void RELAY_TURN(void)
{
	RELAY = !RELAY;
}
