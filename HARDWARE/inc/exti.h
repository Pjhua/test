#ifndef _EXTI_H
#define _EXTI_H

#include "stm32f4xx.h"
#include "led.h"
#include "beep.h"

extern u8 key_flag;
extern u8 key_val;

void EXTI_Config(void);

#endif
