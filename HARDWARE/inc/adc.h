#ifndef _ADC_H_
#define _ADC_H_

#include "stm32f4xx.h"
#include "delay.h"

#define ADCx ADC3
#define ADCx_CLK RCC_APB2Periph_ADC3
#define ADCx_CHANNEL_GPIO_CLK RCC_AHB1Periph_GPIOC
#define GPIO_PORT GPIOC

/* 光敏电阻 GL5506 - PC0 - ADC3_Ch10 */
#define LIGHT_GPIO_PIN GPIO_Pin_0
#define LIGHT_ADC_CHANNEL ADC_Channel_10

/* 火焰传感器 F5 - PC1 - ADC3_Ch11 */
#define FLAME_GPIO_PIN GPIO_Pin_1
#define FLAME_ADC_CHANNEL ADC_Channel_11

#define DMA_CHANNELx DMA_Channel_2
#define DMA_STREAMx DMA2_Stream0

#define V_DARK_MV 3200     /* 暗环境电压(mV)，对应0%光照 */
#define V_BRIGHT_MV 1000   /* 强光电压(mV)，对应100%光照 */
#define V_NO_FLAME_MV 3200 /* 无火焰电压(mV) */
#define V_FLAME_MV 1000    /* 有火焰电压(mV) */

#define ADC_USE_DMA 1 // 1 使用DMA, 0 不使用DMA

/* DMA数组中的通道索引 */
#define LIGHT_CH_IDX 0
#define FLAME_CH_IDX 1

extern volatile u16 uhADCxConvertedValue[2]; // [0]=通道10(PC0/光照) [1]=通道11(PC1/火焰)

void ADC_Config(void);
u8 Get_LightIntensity(u32 voltage_mV);
u8 Get_FlameIntensity(u32 voltage_mV);
u16 ADC_Filter(u8 n, u8 value);
uint16_t ADC_GetValue(u8 id);

#endif
