#include "beep.h"

void beep_init(void)
{
	GPIO_InitTypeDef gpio_struct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	gpio_struct.GPIO_Mode = GPIO_Mode_OUT;
	gpio_struct.GPIO_Pin = GPIO_Pin_15;
	gpio_struct.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_struct.GPIO_OType = GPIO_OType_PP;	  // 推挽
	gpio_struct.GPIO_PuPd = GPIO_PuPd_NOPULL; // 无上下拉

	GPIO_Init(GPIOD, &gpio_struct);

	GPIO_ResetBits(GPIOD, GPIO_Pin_15);
}

void BEEP_ON(void)
{
	BEEP = 1;
}

void BEEP_OFF(void)
{
	BEEP = 0;
}

void BEEP_TURN(void)
{
	BEEP = !BEEP;
}
