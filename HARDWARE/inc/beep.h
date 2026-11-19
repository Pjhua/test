#ifndef _BEEP_H
#define _BEEP_H

#include "stm32f4xx.h"
#include "io_bit.h"

#define BEEP PDout(15)

void beep_init(void);
void BEEP_ON(void);
void BEEP_OFF(void);
void BEEP_TURN(void);

#endif
