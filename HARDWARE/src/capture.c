#include "capture.h"

void key_capture_init(void)
{
    GPIO_InitTypeDef gpio_struct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM5_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); // TIM5时钟使能

    gpio_struct.GPIO_Mode = GPIO_Mode_AF;
    gpio_struct.GPIO_Pin = GPIO_Pin_0;
    gpio_struct.GPIO_Speed = GPIO_Speed_50MHz; // 速度
    gpio_struct.GPIO_OType = GPIO_OType_PP;    // 推挽
    gpio_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;  // 无上下拉
    GPIO_Init(GPIOA, &gpio_struct);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5);

    TIM_TimeBaseStructure.TIM_Period = 0xFFFFFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    // 初始化TIM5输入捕获参数
    TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1;                // CC1S=01 	选择输入端 IC1映射到TI1上
    TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;     // 上升沿捕获
    TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 映射到TI1上
    TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;           // 配置输入分频,不分频
    TIM5_ICInitStructure.TIM_ICFilter = 0x00;                        // IC1F=0000 配置输入滤波器 不滤波
    TIM_ICInit(TIM5, &TIM5_ICInitStructure);

    TIM_ITConfig(TIM5, TIM_IT_Update | TIM_IT_CC1, ENABLE);  // 允许更新中断 ,允许CC1IE捕获中断
    TIM_ClearITPendingBit(TIM5, TIM_IT_CC1 | TIM_IT_Update); // 清除中断标志位
    TIM_Cmd(TIM5, ENABLE);                                   // 使能定时器5

    NVIC_InitStruct.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);
}

// 捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数
u8 tim5ch1_capture_sta = 0;  // 输入捕获状态
u32 tim5ch1_capture_val = 0; // 输入捕获值(TIM2/TIM5是32位)

// 定时器5中断服务程序
// void TIM5_IRQHandler(void)
// {

//     if ((tim5ch1_capture_sta & 0X80) == 0) // 还未成功捕获
//     {
//         if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) // 溢出
//         {
//             if (tim5ch1_capture_sta & 0X40) // 已经捕获到高电平了
//             {
//                 if ((tim5ch1_capture_sta & 0X3F) == 0X3F) // 高电平太长了
//                 {
//                     tim5ch1_capture_sta |= 0X80; // 标记成功捕获了一次
//                     tim5ch1_capture_val = 0XFFFFFFFF;
//                 }
//                 else
//                     tim5ch1_capture_sta++;
//             }
//         }
//         if (TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET) // 捕获1发生捕获事件
//         {
//             if (tim5ch1_capture_sta & 0X40) // 捕获到一个下降沿
//             {
//                 tim5ch1_capture_sta |= 0X80;                        // 标记成功捕获到一次高电平脉宽
//                 tim5ch1_capture_val = TIM_GetCapture1(TIM5);        // 获取当前的捕获值.
//                 TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Rising); // CC1P=0 设置为上升沿捕获
//             }
//             else // 还未开始,第一次捕获上升沿
//             {
//                 tim5ch1_capture_sta = 0; // 清空
//                 tim5ch1_capture_val = 0;
//                 tim5ch1_capture_sta |= 0X40; // 标记捕获到了上升沿
//                 TIM_Cmd(TIM5, DISABLE);      // 关闭定时器5
//                 TIM_SetCounter(TIM5, 0);
//                 TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Falling); // CC1P=1 设置为下降沿捕获
//                 TIM_Cmd(TIM5, ENABLE);                               // 使能定时器5
//             }
//         }
//     }
//     TIM_ClearITPendingBit(TIM5, TIM_IT_CC1 | TIM_IT_Update); // 清除中断标志位
// }

void sr04_init(void)
{
    GPIO_InitTypeDef gpio_struct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM11_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE); // TIM11时钟使能

    gpio_struct.GPIO_Mode = GPIO_Mode_OUT;
    gpio_struct.GPIO_Pin = SR04_TRIG_PIN;
    gpio_struct.GPIO_Speed = GPIO_Speed_100MHz; // 速度
    gpio_struct.GPIO_OType = GPIO_OType_PP;     // 推挽
    gpio_struct.GPIO_PuPd = GPIO_PuPd_DOWN;     // 无上下拉
    GPIO_Init(SR04_TRIG_PORT, &gpio_struct);
    GPIO_ResetBits(SR04_TRIG_PORT, SR04_TRIG_PIN);

    gpio_struct.GPIO_Mode = GPIO_Mode_AF;
    gpio_struct.GPIO_Pin = SR04_ECHO_PIN;
    gpio_struct.GPIO_Speed = GPIO_Speed_100MHz; // 速度
    gpio_struct.GPIO_OType = GPIO_OType_PP;     // 推挽
    gpio_struct.GPIO_PuPd = GPIO_PuPd_DOWN;     // 无上下拉
    GPIO_Init(SR04_ECHO_PORT, &gpio_struct);
    GPIO_PinAFConfig(SR04_ECHO_PORT, GPIO_PinSource9, GPIO_AF_TIM11);

    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = 168 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(TIM11, &TIM_TimeBaseStructure);

    // 初始化TIM11输入捕获参数
    TIM11_ICInitStructure.TIM_Channel = TIM_Channel_1;                // CC1S=01 	选择输入端 IC1映射到TI1上
    TIM11_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;     // 上升沿捕获
    TIM11_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 映射到TI1上
    TIM11_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;           // 配置输入分频,不分频
    TIM11_ICInitStructure.TIM_ICFilter = 0x00;                        // IC1F=0000 配置输入滤波器 不滤波
    TIM_ICInit(TIM11, &TIM11_ICInitStructure);

    TIM_ITConfig(TIM11, TIM_IT_Update | TIM_IT_CC1, ENABLE);  // 允许更新中断 ,允许CC1IE捕获中断
    TIM_ClearITPendingBit(TIM11, TIM_IT_CC1 | TIM_IT_Update); // 清除中断标志位
    TIM_Cmd(TIM11, ENABLE);                                   // 使能定时器11

    NVIC_InitStruct.NVIC_IRQChannel = TIM1_TRG_COM_TIM11_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);
}

u8 sr04_capture_sta = 0;  // SR04 捕获状态
u32 sr04_capture_val = 0; // SR04 捕获值

void sr04_trig(void)
{
    GPIO_SetBits(SR04_TRIG_PORT, SR04_TRIG_PIN);
    delay_us(20);
    GPIO_ResetBits(SR04_TRIG_PORT, SR04_TRIG_PIN);
}
float sr04_get_distance(void)
{
    float distance = 0;

    // 获取测量之前重置捕获状态
    sr04_capture_sta = 0;
    sr04_capture_val = 0;

    TIM_OC1PolarityConfig(TIM11, TIM_ICPolarity_Rising);
    TIM_SetCounter(TIM11, 0);
    TIM_ClearITPendingBit(TIM11, TIM_IT_CC1 | TIM_IT_Update);

    // 发送触发脉冲
    sr04_trig();

    // 等待捕获完成（轮询等待，超时保护约 300ms）
    u32 timeout = 300000; // 300ms / 1us = 300000
    while ((sr04_capture_sta & 0X80) == 0)
    {
        delay_us(1);
        if (timeout-- == 0)
        {
            return -1.0f; // 超时，返回失败标志
        }
    }

    distance = (float)sr04_capture_val / 58.0f;

    // 清除捕获状态，准备下一次测量
    sr04_capture_sta = 0;
    sr04_capture_val = 0;

    return distance;
}
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{

    if ((sr04_capture_sta & 0X80) == 0) // 还未成功捕获
    {
        if (TIM_GetITStatus(TIM11, TIM_IT_Update) != RESET) // 溢出
        {
            if (sr04_capture_sta & 0X40) // 已经捕获到高电平了
            {
                if ((sr04_capture_sta & 0X3F) == 0X3F) // 高电平太长了
                {
                    sr04_capture_sta |= 0X80; // 标记成功捕获了一次
                    sr04_capture_val = 0XFFFF;
                }
                else
                    sr04_capture_sta++;
            }
        }
        if (TIM_GetITStatus(TIM11, TIM_IT_CC1) != RESET) // 捕获1发生捕获事件
        {
            if (sr04_capture_sta & 0X40) // 捕获到一个下降沿
            {
                sr04_capture_sta |= 0X80;                                                           // 标记成功捕获到一次高电平脉宽
                sr04_capture_val = TIM_GetCapture1(TIM11) | ((u32)(sr04_capture_sta & 0X3F) << 16); // 组合溢出计数
                TIM_OC1PolarityConfig(TIM11, TIM_ICPolarity_Rising);                                // CC1P=0 设置为上升沿捕获
            }
            else // 还未开始,第一次捕获上升沿
            {
                sr04_capture_sta = 0; // 清空
                sr04_capture_val = 0;
                sr04_capture_sta |= 0X40; // 标记捕获到了上升沿
                TIM_Cmd(TIM11, DISABLE);  // 关闭定时器11
                TIM_SetCounter(TIM11, 0);
                TIM_OC1PolarityConfig(TIM11, TIM_ICPolarity_Falling); // CC1P=1 设置为下降沿捕获
                TIM_Cmd(TIM11, ENABLE);                               // 使能定时器11
            }
        }
    }
    TIM_ClearITPendingBit(TIM11, TIM_IT_CC1 | TIM_IT_Update); // 清除中断标志位
}
