#ifndef _FREERTOS_H
#define _FREERTOS_H

#include "stm32f4xx.h"
#include "stdio.h"

typedef struct
{
    float hum;
    float temp;
    short gx;
    short gy;
    short gz;
} buf_t;

void low_task(void *acg);
void middle_task(void *acg);
void high_task(void *acg);
void start_task(void *acg);
void freertos_init(void);
void measure_task(void *acg);
void lcd_task(void *acg);
void wifi_task(void *acg);
void high_task(void *arg);
void middle_task(void *arg);
void low_task(void *arg);

#endif
