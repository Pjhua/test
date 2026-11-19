#include "adc.h"

volatile u16 uhADCxConvertedValue[2] = {0, 0}; // ADC3 转换结果数组：[0]=通道10(PC0/光照) [1]=通道11(PC1/火焰)

void ADC_Config(void)
{
  ADC_InitTypeDef ADC_InitStructure;             // ADC 配置结构体（规则组参数）
  ADC_CommonInitTypeDef ADC_CommonInitStructure; // ADC 公共配置结构体（ADC1/2/3 共享参数）
#if ADC_USE_DMA
  DMA_InitTypeDef DMA_InitStructure; // DMA 配置结构体（仅使用 DMA 时需要）
#endif
  GPIO_InitTypeDef GPIO_InitStructure; // GPIO 配置结构体（模拟输入引脚配置）

/* 使能外设时钟 **************************************************************/
#if ADC_USE_DMA
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); // 使能 DMA2 时钟（ADC3 的 DMA 请求挂在 DMA2 上）
#endif
  RCC_AHB1PeriphClockCmd(ADCx_CHANNEL_GPIO_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(ADCx_CLK, ENABLE);

#if ADC_USE_DMA
  /* DMA2 流0 通道2：把 ADC3 转换结果自动搬运进数组 *************************/
  DMA_InitStructure.DMA_Channel = DMA_CHANNELx;                               // 选 DMA 通道2（ADC3 对应 DMA2_Stream0_Channel2）
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC3->DR;             // 外设地址：ADC3 数据寄存器
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)uhADCxConvertedValue;     // 内存地址：结果数组
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                     // 传输方向：外设→内存（读 ADC 结果）
  DMA_InitStructure.DMA_BufferSize = 2;                                       // 搬运次数：2 个半字（两个通道各 1 个结果）
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            // 外设地址不递增（始终读同一个 DR）
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     // 内存地址递增：结果依次写入 [0]、[1]
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 外设数据宽度 16 位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         // 内存数据宽度 16 位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             // 循环模式：搬完 2 个又从 [0] 开始，持续刷新
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         // DMA 优先级：高
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                      // 不使用 FIFO（直通传输）
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;           // FIFO 阈值（未用 FIFO，此值不生效）
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                 // 内存突发模式：单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         // 外设突发模式：单次传输
  DMA_Init(DMA_STREAMx, &DMA_InitStructure);                                  // 将上述配置写入 DMA2 流0 寄存器
  DMA_Cmd(DMA_STREAMx, ENABLE);                                               // 使能 DMA2 流0，开始搬运
#endif
  /* 配置 PC0/PC1 为模拟输入（接光敏/火焰传感器的模拟输出） ******************/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;                   // 模式：模拟输入（ADC 采集专用）
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;               // 无上下拉（模拟输入不需要）
  GPIO_InitStructure.GPIO_Pin = LIGHT_GPIO_PIN | FLAME_GPIO_PIN; // 引脚：PC0 → 光敏电阻（ADC3 通道10）引脚：PC1 → 火焰传感器（ADC3 通道11）
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* ADC 公共配置（三个 ADC 共用的寄存器） **********************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                     // 独立模式：ADC3 单独工作
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;                  // ADC 时钟分频：84MHz ÷ 2 = 42MHz
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;      // 非双ADC模式，无需 DMA 访问方式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; // 双ADC采样间隔（单ADC时无效）
  ADC_CommonInit(&ADC_CommonInitStructure);                                    // 写入 ADC 公共寄存器

  /* ADC3 规则组配置 *********************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; // 分辨率 12 位（结果范围 0~4095）

#if ADC_USE_DMA
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;       // 扫描模式：按序列依次转换 2 个通道
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // 连续转换：一轮结束后立即开始下一轮
#else
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // 单次转换：等待软件触发
#endif
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // 无外部触发（用软件启动）
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;       // 外部触发源选择（未启用触发，此值无效）
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                      // 数据右对齐（常规 12 位读数）

#if ADC_USE_DMA
  ADC_InitStructure.ADC_NbrOfConversion = 2; // 转换序列长度：2 个通道
#else
  ADC_InitStructure.ADC_NbrOfConversion = 1;
#endif

  ADC_Init(ADCx, &ADC_InitStructure); // 将上述配置写入 ADC3

  /* ADC3 规则组转换序列：rank1=通道10(PC0/光照) → [0]，rank2=通道11(PC1/火焰) → [1] **/
  ADC_RegularChannelConfig(ADCx, LIGHT_ADC_CHANNEL, 1, ADC_SampleTime_3Cycles); // 序列第1个：通道10(PC0)，采样3周期
#if ADC_USE_DMA
  ADC_RegularChannelConfig(ADCx, FLAME_ADC_CHANNEL, 2, ADC_SampleTime_3Cycles); // 序列第2个：通道11(PC1)，采样3周期
#endif

#if ADC_USE_DMA
  /* 使能 ADC3 的 DMA 请求（仅 DMA 模式需要） *******************************/
  ADC_DMARequestAfterLastTransferCmd(ADCx, ENABLE); // 每轮最后一次转换结束后才发 DMA 请求
  ADC_DMACmd(ADCx, ENABLE);                         // 使能 ADC3 的 DMA 请求，允许结果被 DMA 搬走
#endif
  /* 使能 ADC3 */
  ADC_Cmd(ADCx, ENABLE); // 使能 ADC3，开始工作
  
  ADC_SoftwareStartConv(ADCx);
}

uint16_t ADC_GetValue(u8 id)
{
#if ADC_USE_DMA
  return uhADCxConvertedValue[id];
#else
  u8 adc_ch = (ch == LIGHT_CH_IDX) ? LIGHT_ADC_CHANNEL : FLAME_ADC_CHANNEL;
  ADC_RegularChannelConfig(ADCx, adc_ch, 1, ADC_SampleTime_3Cycles);
  ADC_SoftwareStartConv(ADCx);
  while (!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC))
    ;
  return ADC_GetConversionValue(ADCx);
#endif
}

u8 Get_LightIntensity(u32 voltage_mV)
{
  u8 light;
  if (voltage_mV <= V_BRIGHT_MV)
    return 100;
  if (voltage_mV >= V_DARK_MV)
    return 0;
  light = (V_DARK_MV - voltage_mV) * 100 / (V_DARK_MV - V_BRIGHT_MV);
  return light;
}

u8 Get_FlameIntensity(u32 voltage_mV)
{
  u8 flame;
  if (voltage_mV <= V_FLAME_MV)
    return 100;
  if (voltage_mV >= V_NO_FLAME_MV)
    return 0;
  flame = (V_NO_FLAME_MV - voltage_mV) * 100 / (V_NO_FLAME_MV - V_FLAME_MV);
  return flame;
}

/*
 * 中值平均滤波法
 * 采样n次，去掉最大值和最小值，对剩余n-2个值求平均
 * 形参n: 采样次数（至少3次）id 索引
 * 返回值: 滤波后的ADC原始值
 */
u16 ADC_Filter(u8 n, u8 id)
{
  u8 i;
  u16 Vol_max = 0, Vol_min = 0xFFFF;
  u32 val, sum = 0;

  if (n < 3)
    return ADC_GetValue(id);
  for (i = 0; i < n; i++)
  {
    val = ADC_GetValue(id);
    sum += val;
    if (val > Vol_max)
      Vol_max = val;
    if (val < Vol_min)
      Vol_min = val;
  }
  return (sum - Vol_max - Vol_min) / (n - 2);
}
