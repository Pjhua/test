/*
 * Copyright 2024 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "lvgl.h"
#include "custom.h"
#include "pwm.h"
#include "w2812.h"
#include "sht3x.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "adc.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Create a demo application
 */

void custom_init(lv_ui *ui)
{
    /* Add your codes here */
    /* 控制界面用到的外设初始化 */
    sg90_init();    /* 舵机：PA1 / TIM2_CH2 PWM */
    fan_pwm_init(); /* 风扇：PC8 / TIM8_CH3 PWM */
    w2812_init();   /* RGB灯：PB15 普通GPIO 软件模拟时序，不占用 SPI2/I2S2 和 DMA */

    /* 监控界面（screen_sms）用到的传感器初始化 */
    ADC_Config(); /* PC0光敏 + PC1火焰（双通道DMA） */
    Sht3x_Init(); /* SHT30 温湿度：PB6(SCL) PB7(SDA) 软件IIC */
    {
        unsigned char mpu_ret = mpu_dmp_init();
        printf("[mpu] DMP init ret=%d\r\n", mpu_ret);
        /* DMP 初始化失败（自检/固件加载等）时按裸芯片重新配置一次，
           界面会改用原始加速度计+陀螺仪数据做姿态解算，保证数据仍能更新 */
        if (mpu_ret != 0)
            Mpu6050_Init();
    }

}
