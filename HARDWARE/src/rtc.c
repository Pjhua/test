#include "rtc.h"

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

void RTC_AlarmConfig(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    RTC_AlarmTypeDef RTC_AlarmStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_Minutes | RTC_AlarmMask_DateWeekDay;
    RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
    RTC_AlarmStructure.RTC_AlarmDateWeekDay = 15;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = 9;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 10;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 30;

    RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
    RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
    RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
    RTC_ITConfig(RTC_IT_ALRA, ENABLE);
}

void RTC_WakupConfig(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the RTC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* EXTI configuration */
    EXTI_ClearITPendingBit(EXTI_Line22);
    EXTI_InitStructure.EXTI_Line = EXTI_Line22;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Configure the RTC Wakeup Clock source and Counter (Wakeup event each 1 second) */
    RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
    RTC_SetWakeUpCounter(0x7FF);

    /* Enable the Wakeup Interrupt */
    RTC_ITConfig(RTC_IT_WUT, ENABLE);

    /* Enable Wakeup Counter */
    RTC_WakeUpCmd(ENABLE);
}

void rtc_config(void)
{
    RTC_InitTypeDef RTC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    PWR_BackupAccessCmd(ENABLE);
    RCC_BackupResetCmd(ENABLE);
    RCC_BackupResetCmd(DISABLE);

    RCC_LSEConfig(RCC_LSE_ON);
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }

    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForSynchro();

    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    RTC_InitStructure.RTC_SynchPrediv = 0xFF;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);

    RTC_DateStruct.RTC_Year = 26;
    RTC_DateStruct.RTC_Month = 11;
    RTC_DateStruct.RTC_Date = 10;
    RTC_DateStruct.RTC_WeekDay = 2;
    RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);

#if 0
    RTC_TimeStruct.RTC_H12 = RTC_H12_AM;
    RTC_TimeStruct.RTC_Hours = 0x10;
    RTC_TimeStruct.RTC_Minutes = 0x35;
    RTC_TimeStruct.RTC_Seconds = 0x24;
    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStruct);
#else
    RTC_TimeStruct.RTC_H12 = RTC_H12_AM;
    RTC_TimeStruct.RTC_Hours = 10;
    RTC_TimeStruct.RTC_Minutes = 20;
    RTC_TimeStruct.RTC_Seconds = 24;
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
#endif
}

u8 rtc_alarm = 0;

void RTC_Alarm_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_ALRA) != RESET)
    {
        RTC_ClearITPendingBit(RTC_IT_ALRA);
        rtc_alarm = 1;
    }
    EXTI_ClearITPendingBit(EXTI_Line17);
}

u8 rtc_wakup_flag = 0;
RTC_DateTypeDef RTC_DateStruct;
RTC_TimeTypeDef RTC_TimeStruct;

void RTC_WKUP_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_WUT) != RESET)
    {
        rtc_wakup_flag = 1;
        RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
        RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
        RTC_ClearITPendingBit(RTC_IT_WUT);
        EXTI_ClearITPendingBit(EXTI_Line22);
    }
}

void rtc_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x1235)
    {
        rtc_config();
        RTC_WriteBackupRegister(RTC_BKP_DR0, 0x1235);
    }
    else
    {
        EXTI_ClearITPendingBit(EXTI_Line17);
        RTC_ClearITPendingBit(RTC_IT_ALRA);
        RTC_ClearITPendingBit(RTC_IT_WUT);
        EXTI_ClearITPendingBit(EXTI_Line22);
    }
    RTC_AlarmConfig();

    // RTC_ITConfig(RTC_IT_ALRA, ENABLE);
    RTC_WakupConfig();
}

void rtc_set_time(u8 weekday, u16 year, u8 month, u8 day, u8 hour, u8 min, u8 sec)
{
    RTC_DateStruct.RTC_Year = year - 2000;
    RTC_DateStruct.RTC_Month = month;
    RTC_DateStruct.RTC_Date = day;
    RTC_DateStruct.RTC_WeekDay = weekday;
    RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);
    RTC_TimeStruct.RTC_H12 = RTC_H12_AM;
    RTC_TimeStruct.RTC_Hours = hour;
    RTC_TimeStruct.RTC_Minutes = min;
    RTC_TimeStruct.RTC_Seconds = sec;
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
}

/* 把硬件 RTC 当前时间同步到 g_datetime（timer.c 的软件时钟）。
   rtc_init() 之后上电调一次即可，之后由 TIM7 + time_handle() 走时。 */
void rtc_read_datetime(void)
{
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);

    g_datetime.year  = 2000 + RTC_DateStruct.RTC_Year;
    g_datetime.month = RTC_DateStruct.RTC_Month;
    g_datetime.day   = RTC_DateStruct.RTC_Date;
    g_datetime.hour  = RTC_TimeStruct.RTC_Hours;
    g_datetime.min   = RTC_TimeStruct.RTC_Minutes;
    g_datetime.sec   = RTC_TimeStruct.RTC_Seconds;
}
