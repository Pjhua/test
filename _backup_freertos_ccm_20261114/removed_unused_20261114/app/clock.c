#include "clock.h"
#include "lcd.h"
#include "lcd_init.h"
#include "timer.h"
#include "delay.h"
#include <math.h>
#include <stdio.h>
#include "rtc.h"

/* 度转弧度 */
#define ANGLE (3.1415926f / 180.0f)

/* 表盘位置和大小 */
#define CLOCK_CX 120
#define CLOCK_CY 130
#define CLOCK_R 58

/* 三根指针的长度，都不超过刻度内沿(52)，擦除时不会破坏刻度；数字每秒重画 */
#define HAND_SEC 50
#define HAND_MIN 44
#define HAND_HOUR 32

static u8 clock_inited = 0; /* 第一次只画新指针，不擦除 */
static u8 old_sec, old_min, old_hour;

/* 画1~12数字，排在圆内半径40的位置 */
static void clock_draw_numbers(void)
{
    u8 i;
    float a;
    u16 nx, ny;
    char buf[4];

    for (i = 1; i <= 12; i++)
    {
        a = i * 30 * ANGLE;
        nx = CLOCK_CX + sinf(a) * 40;
        ny = CLOCK_CY - cosf(a) * 40;
        if (i >= 10)
            nx -= 8; /* 两位数宽16，居中左移8 */
        else
            nx -= 4; /* 一位数宽8，居中左移4 */
        ny -= 8;
        sprintf(buf, "%d", i);
        LCD_ShowString_Flash(nx, ny, (u8 *)buf, YELLOW, BLACK, 16, 0);
    }
}

/* 画一根指针：angle角度（0度指向12点，顺时针），len长度，width粗细，color颜色 */
static void clock_draw_hand(u16 angle, u8 len, u8 width, u16 color)
{
    float rad;
    float dx, dy, px, py;
    s8 w, off;

    rad = angle * ANGLE;
    dx = sinf(rad) * len; /* 指针末端相对圆心的位置 */
    dy = -cosf(rad) * len;
    px = cosf(rad); /* 垂直于指针的方向，用来画粗 */
    py = sinf(rad);

    for (w = 0; w < width; w++)
    {
        off = (s8)w - (s8)(width / 2);
        LCD_DrawLine((u16)(CLOCK_CX + px * off), (u16)(CLOCK_CY + py * off),
                     (u16)(CLOCK_CX + dx + px * off), (u16)(CLOCK_CY + dy + py * off), color);
    }
}

/* 画表盘，整屏黑底，刻度，数字放在圆内 */
void Clock_DrawFace(void)
{
    u8 i;
    float a;
    int x1, y1, x2, y2;

    LCD_Fill(0, 0, LCD_W - 1, LCD_H - 1, BLACK);

    /* 外圆，画两遍显得粗一点 */
    Draw_Circle(CLOCK_CX, CLOCK_CY, CLOCK_R, BLUE);
    Draw_Circle(CLOCK_CX, CLOCK_CY, CLOCK_R - 1, BLUE);

    /* 60个刻度，整点刻度加粗 */
    for (i = 0; i < 60; i++)
    {
        a = i * 6 * ANGLE;
        x1 = (int)(CLOCK_CX + sinf(a) * (CLOCK_R - 6));
        y1 = (int)(CLOCK_CY - cosf(a) * (CLOCK_R - 6));
        x2 = (int)(CLOCK_CX + sinf(a) * CLOCK_R);
        y2 = (int)(CLOCK_CY - cosf(a) * CLOCK_R);
        LCD_DrawLine(x1, y1, x2, y2, BLUE);
        if (i % 5 == 0)
        {
            LCD_DrawLine(x1 + 1, y1, x2 + 1, y2, BLUE);
        }
    }

    /* 1~12数字，在圆内 */
    clock_draw_numbers();
}

/* 刷新指针：先擦掉旧指针，再补画被擦到的数字，最后画新指针 */
void Clock_UpdateHands(void)
{
    u8 hour_a;

    if (clock_inited)
    {
        clock_draw_hand(old_sec * 6, HAND_SEC, 1, BLACK);
        clock_draw_hand(old_min * 6, HAND_MIN, 2, BLACK);
        hour_a = (old_hour % 12) * 30 + old_min / 2;
        clock_draw_hand(hour_a, HAND_HOUR, 3, BLACK);

        clock_draw_numbers(); /* 旧指针可能扫到数字，重画一遍 */
    }

    // old_sec = g_datetime.sec;
    // old_min = g_datetime.min;
    // old_hour = g_datetime.hour;

    old_sec = RTC_TimeStruct.RTC_Seconds;
    old_min = RTC_TimeStruct.RTC_Minutes;
    old_hour = RTC_TimeStruct.RTC_Hours;

    clock_draw_hand(old_sec * 6, HAND_SEC, 1, RED);   /* 秒针：红色细 */
    clock_draw_hand(old_min * 6, HAND_MIN, 2, GREEN); /* 分针：绿色 */
    hour_a = (old_hour % 12) * 30 + old_min / 2;
    clock_draw_hand(hour_a, HAND_HOUR, 3, WHITE); /* 时针：白色粗 */

    LCD_Fill(CLOCK_CX - 2, CLOCK_CY - 2, CLOCK_CX + 2, CLOCK_CY + 2, RED); /* 中心圆点 */
    clock_inited = 1;
}
