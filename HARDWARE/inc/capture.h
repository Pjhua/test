#ifndef _CAPTURE_H
#define _CAPTURE_H

#include "stm32f4xx.h"
#include "delay.h"

/* ========== TIM5 按键输入捕获 ========== */
extern u8 tim5ch1_capture_sta;
extern u32 tim5ch1_capture_val;
void key_capture_init(void);

/* ========== SR04 超声波测距 (TIM11 输入捕获) ========== */
// SR04 引脚定义
#define SR04_TRIG_PORT GPIOB
#define SR04_TRIG_PIN GPIO_Pin_8
#define SR04_ECHO_PORT GPIOB
#define SR04_ECHO_PIN GPIO_Pin_9
extern u8 sr04_capture_sta;    // SR04 捕获状态
extern u32 sr04_capture_val;   // SR04 捕获值
void sr04_init(void);          // SR04 初始化（TRIG: PB8, ECHO: PB9）
void sr04_trig(void);          // 发送触发脉冲
float sr04_get_distance(void); // 获取距离（单位 cm）

#endif
