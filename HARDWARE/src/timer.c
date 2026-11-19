#include "timer.h"
#include "key.h"

static u32 time6_us = 0;

datetime_t g_datetime = {0};

/* 平年每个月的天数，下标0对应1月 */
static const u8 days_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void timer6_init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_TimeBaseInitStruct.TIM_Period = arr - 1;
    TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);

    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM6_DAC_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);

    // TIM_Cmd(TIM6, ENABLE);
}

void timer7_init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

    TIM_TimeBaseInitStruct.TIM_Period = arr - 1;
    TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStruct);

    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM7, ENABLE);
}




void timer2_init(u32 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitStruct.TIM_Period = arr - 1;
    TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM2,ENABLE);
}

u32 tim7_tick_cnt = 0;
void TIM7_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    {
        tim7_tick_cnt++;
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    }
}

void TIM6_DAC_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        time6_us--;
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}

void delay_us_tim6(u32 us)
{
    time6_us = us;
    TIM_Cmd(TIM6, ENABLE);
    while (time6_us)
        ;
    TIM_Cmd(TIM6, DISABLE);
}
void delay_ms_tim6(u32 ms)
{
    uint32_t repeat = ms / 30; /*  这里用30,是考虑到可能有超频应用 */
    uint32_t remain = ms % 30;
    while (repeat)
    {
        delay_us_tim6(30 * 1000); /* 利用delay_us 实现 1000ms 延时 */
        repeat--;
    }
    if (remain)
    {
        delay_us_tim6(remain * 1000); /* 利用delay_us, 把尾数延时(remain ms)给做了 */
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        if (key_flag == 1 && GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
        {
            TIM_Cmd(TIM2, DISABLE);
            key_val = 1;
        }
        else if (key_flag == 3 && GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
        {
            TIM_Cmd(TIM2, DISABLE);
            key_val = 3;
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}




/* ---------- 软件时钟 ---------- */
u8 time_is_leap_year(u16 year) /* 判断闰年：1闰年 0平年 */
{
    if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0)
    {
        return 1;
    }
    return 0;
}
u8 time_days_in_month(u16 year, u8 month) /* 取某年某月的天数 */
{
    if (month < 1 || month > 12)
    {
        month = 1;
        return 31;
    }
    if (month == 2 && time_is_leap_year(year) == 1)
    {
        return 29;
    }
    return days_table[month - 1];
}
void time_set(u8 weekday, u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec)
{
    g_datetime.year = year;
    g_datetime.month = (month >= 1 && month <= 12) ? month : 1;
    g_datetime.day = (day >= 1 && day <= time_days_in_month(year, month)) ? day : 1;
    g_datetime.hour = (hour <= 23) ? hour : 0;
    g_datetime.min = (min <= 59) ? min : 0;
    g_datetime.sec = (sec <= 59) ? sec : 0;
}
u8 time_set_from_str(const char *str, const char *week) /* "2026-09-22 10:42:04" */
{
    int year, month, day, hour, min, sec, weekday;
    if (sscanf(str, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec) != 6)
    {
        return 0;
    }
    if (sscanf(week, "%d", &weekday) != 1)
    {
        return 0;
    }
#if RTC_USE_TIMER
    time_set(weekday, year, month, day, hour, min, sec);
#else
    rtc_set_time(weekday, year, month, day, hour, min, sec);
#endif
    return 1;
}
void time_tick_1s(void) /* 时间走1秒，处理进位 */
{
    if (++g_datetime.sec < 60)
    {
        return;
    }
    g_datetime.sec = 0;
    if (++g_datetime.min < 60)
    {
        return;
    }
    g_datetime.min = 0;
    if (++g_datetime.hour < 24)
    {
        return;
    }
    g_datetime.hour = 0;
    if (++g_datetime.day <= time_days_in_month(g_datetime.year, g_datetime.month))
    {
        return;
    }
    g_datetime.day = 1;
    if (++g_datetime.month <= 12)
    {
        return;
    }
    g_datetime.month = 1;

    g_datetime.year += 1;
}
void time_handle(void) /* 放在主循环里调用，处理中断攒下的秒数 */
{
    u16 ticks;
    ticks = tim7_tick_cnt;
    tim7_tick_cnt = 0;

    while (ticks--)
    {
        time_tick_1s(); /* 时间走1秒：秒到60分加1，分到60时加1…… */
    }
}

void time_get_str(char *buf, u16 len) /* 格式化成字符串 */
{
    if (buf == NULL || len < 20)
    {
        return;
    }
    snprintf(buf, len, "%04d-%02d-%02d %02d:%02d:%02d", g_datetime.year, g_datetime.month, g_datetime.day, g_datetime.hour, g_datetime.min, g_datetime.sec);
}


#include "lvgl.h"

void timer1_lvgl_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    // 使能定时器1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 100 - 1;
    TIM_TimeBaseInitStruct.TIM_Prescaler = 1680 - 1; //1ms 溢出
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);

    /* Enable the TIM6 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 先清除标志再启动中断，不然会触发一次定时器的中断服务函数
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM1, ENABLE);
}

void TIM1_UP_TIM10_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
        lv_tick_inc(1);
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}




