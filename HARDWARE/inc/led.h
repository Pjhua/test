#ifndef _LED_H
#define _LED_H

#include "stm32f4xx.h"
#include "io_bit.h"

#define LED1 PDout(12)
#define LED2 PDout(13)
#define LED3 PDout(14)


void led_init(void);
void LED1_ON(void);
void LED1_OFF(void);
void LED1_TURN(void);
void LED2_ON(void);
void LED2_OFF(void);
void LED2_TURN(void);
void LED3_ON(void);
void LED3_OFF(void);
void LED3_TURN(void);
void LED_ON_ALL(void);
void LED_OFF_ALL(void);
void LED_TRUN_ALL(void);

#endif
