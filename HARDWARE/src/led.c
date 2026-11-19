#include "led.h"

void led_init(void)
{
	GPIO_InitTypeDef gpio_struct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	gpio_struct.GPIO_Mode = GPIO_Mode_OUT;							// 模式
	gpio_struct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14; // 引脚
	gpio_struct.GPIO_Speed = GPIO_Speed_50MHz;						// 速度
	gpio_struct.GPIO_OType = GPIO_OType_PP;							// 推挽
	gpio_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;						// 无上下拉

	GPIO_Init(GPIOD, &gpio_struct);

	GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
}

void LED1_ON(void)
{
	LED1 = 0;
}

void LED1_OFF(void)
{
	LED1 = 1;
}

void LED1_TURN(void)
{
	LED1 = !LED1;
}

void LED2_ON(void)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_13);
}

void LED2_OFF(void)
{
	GPIO_SetBits(GPIOD, GPIO_Pin_13);
}

void LED2_TURN(void)
{
	GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
}

void LED3_ON(void)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_14);
}

void LED3_OFF(void)
{
	GPIO_SetBits(GPIOD, GPIO_Pin_14);
}

void LED3_TURN(void)
{
	GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
}

void LED_ON_ALL(void)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
}

void LED_OFF_ALL(void)
{
	GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
}

void LED_TURN_ALL(void)
{
	GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
}
