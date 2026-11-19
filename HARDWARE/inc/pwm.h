#ifndef _PWM_H
#define _PWM_H

#include "stm32f4xx.h"
#include "lcd_init.h"

#define BL_MAX_LEVEL 100 /* 亮度档位 0~100，对应占空比 0%~100% */
#define BL_STEP 1        /* 每次按键加2档，即亮度加10% */

extern u8 led_flag;
extern u16 bright;
extern volatile u8 bl_level;

void sg90_init(void);
void sg90_set_angle(u8 angle);
u8 sg90_get_angle(void); /* 反算当前角度（0~180），供界面显示 */
void fan_pwm_init(void);
void fan_set_speed(u32 speed);
void fan_set_gear(u8 gear);
u8 fan_get_gear(void); /* 反算当前档位（0~3），供界面显示 */
void led_pwm_init(void);
void led_set_bright(u8 id, u8 bright);
void brea_light(void);

void bl_pwm_init(void);
u8 bl_level_next(void);

#endif
