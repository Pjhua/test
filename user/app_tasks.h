#ifndef __APP_TASKS_H
#define __APP_TASKS_H

#include "stm32f4xx.h"

/* 测量采集任务对外共享的数据快照。
   字段都是 4 字节以内的简单值，任务间直接拷贝即可（都对齐，读不会撕裂） */
typedef struct
{
    float temperature; /* SHT30 温度 ℃ */
    float humidity;    /* SHT30 湿度 % */
    u16   light_mv;    /* 光照 ADC 电压 mV */
    u8    light_pct;   /* 光照强度 0~100% */
    float pitch;       /* MPU6050 俯仰角 */
    float roll;        /* 横滚角 */
    float yaw;         /* 航向角 */
} app_measure_t;

/* custom_init()（SHT30/ADC/MPU DMP 初始化）完成标志。
   测量采集任务等它置 1 后再开始读传感器，避免与初始化抢同一路软件 I2C */
extern volatile u8 g_app_hw_ready;

/* 创建“开始任务”，由开始任务统一创建其它所有任务并自删除 */
void app_tasks_start(void);

/* 取最近一次测量采集结果（UI 用，避免界面线程直接读传感器） */
void app_measure_get(app_measure_t *out);

#endif
