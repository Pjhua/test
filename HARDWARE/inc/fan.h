#ifndef _FAN_H
#define _FAN_H

#include "stm32f4xx.h"
#include "io_bit.h"

#define FAN PCout(8)

void fan_init(void);
void FAN_ON(void);
void FAN_OFF(void);
void FAN_TURN(void);

#endif
