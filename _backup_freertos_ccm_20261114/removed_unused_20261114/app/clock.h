#ifndef __CLOCK_H_
#define __CLOCK_H_

#include "stm32f4xx.h"

/* 指针时钟，圆心(120,130)，半径58，配合timer.c里的软件时钟g_datetime使用 */
void Clock_DrawFace(void);     /* 画表盘：外圆、刻度、圆内1~12数字，进入时钟界面时画一次 */
void Clock_UpdateHands(void);  /* 刷新时分秒指针，每秒调用一次 */

#endif
