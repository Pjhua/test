#ifndef _RELAY_H
#define _RELAY_H

#include "stm32f4xx.h"
#include "io_bit.h"

#define RELAY PEout(0)

void relay_init(void);
void RELAY_ON(void);
void RELAY_OFF(void);
void RELAY_TURN(void);

#endif
