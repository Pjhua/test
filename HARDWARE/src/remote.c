#include "remote.h"

void Remote_Init(void)
{
    GPIO_InitTypeDef gpio_struct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);

    gpio_struct.GPIO_Mode = GPIO_Mode_AF;
    gpio_struct.GPIO_Pin = GPIO_Pin_6;
    gpio_struct.GPIO_Speed = GPIO_Speed_100MHz; // 速度
    gpio_struct.GPIO_OType = GPIO_OType_PP;     // 推挽
    gpio_struct.GPIO_PuPd = GPIO_PuPd_DOWN;     // 无上下拉
    GPIO_Init(GPIOE, &gpio_struct);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_TIM9);

    TIM_TimeBaseStructure.TIM_Period = 10000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 168 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);

    // 初始化TIM9输入捕获参数
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;                // CC1S=01 	选择输入端 IC1映射到TI1上
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;     // 上升沿捕获
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 映射到TI1上
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;           // 配置输入分频,不分频
    TIM_ICInitStructure.TIM_ICFilter = 0x03;                        //  配置输入滤波器
    TIM_ICInit(TIM9, &TIM_ICInitStructure);

    TIM_ITConfig(TIM9, TIM_IT_Update | TIM_IT_CC2, ENABLE);  // 允许更新中断 ,允许CC1IE捕获中断
    TIM_ClearITPendingBit(TIM9, TIM_IT_CC2 | TIM_IT_Update); // 清除中断标志位
    TIM_Cmd(TIM9, ENABLE);                                   // 使能定时器11

    NVIC_InitStruct.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStruct);
}

// 遥控器接收状态
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留
//[4]:标记上升沿是否已经被捕获
//[3:0]:溢出计时器
u8 RmtSta = 0;
u16 Dval;       // 下降沿时计数器的值
u32 RmtRec = 0; // 红外接收到的数据
u8 RmtCnt = 0;  // 按键按下的次数

void TIM1_BRK_TIM9_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM9, TIM_IT_Update) != RESET) // 溢出
    {
        TIM_ClearITPendingBit(TIM9, TIM_IT_Update); // 清除中断标志位
        if (RmtSta & 0x80)                          // 上次有数据被接收到了
        {
            RmtSta &= ~0X10; // 取消上升沿已经被捕获标记
            if ((RmtSta & 0X0F) == 0X00)
                RmtSta |= 1 << 6; // 标记已经完成一次按键的键值信息采集
            if ((RmtSta & 0X0F) < 14)
                RmtSta++;
            else
            {
                RmtSta &= ~(1 << 7); // 清空引导标识
                RmtSta &= 0XF0;      // 清空计数器
            }
        }
    }
    if (TIM_GetITStatus(TIM9, TIM_IT_CC2) != RESET) // 捕获1发生捕获事件
    {
        TIM_ClearITPendingBit(TIM9, TIM_IT_CC2); // 清除中断标志位
        if (RDATA)
        {
            TIM_SetCounter(TIM9, 0);
            TIM_OC1PolarityConfig(TIM9, TIM_ICPolarity_Falling);
            RmtSta |= 0x10;
        }
        else
        {
            Dval = TIM_GetCapture2(TIM9);
            TIM_OC1PolarityConfig(TIM9, TIM_ICPolarity_Rising);
            if (RmtSta & 0x10)
            {
                if (RmtSta & 0x80)
                {
                    if (Dval > 300 && Dval < 800)
                    {
                        RmtRec <<= 1;
                        RmtRec |= 0;
                    }
                    else if (Dval > 1400 && Dval < 1800)
                    {
                        RmtRec <<= 1;
                        RmtRec |= 1;
                    }
                    else if (Dval > 2200 && Dval < 2600)
                    {
                        RmtCnt++;
                        RmtSta &= 0xF0;
                    }
                }
                else if (Dval > 4200 && Dval < 4700)
                {
                    RmtSta |= 1 << 7;
                    RmtCnt = 0;
                }
            }
            RmtSta &= ~0x10;
        }
    }
}

// 处理红外键盘
// 返回值:
//	 0,没有任何按键按下
// 其他,按下的按键键值.
u8 Remote_Scan(void)
{
    u8 sta = 0;
    u8 t1, t2;
    if (RmtSta & (1 << 6)) // 得到一个按键的所有信息了
    {
        t1 = RmtRec >> 24;                      // 得到地址码
        t2 = (RmtRec >> 16) & 0xff;             // 得到地址反码
        if ((t1 == (u8)~t2) && t1 == REMOTE_ID) // 检验遥控识别码(ID)及地址
        {
            t1 = RmtRec >> 8;
            t2 = RmtRec;
            if (t1 == (u8)~t2)
                sta = t1; // 键值正确
        }
        if ((sta == 0) || ((RmtSta & 0X80) == 0)) // 按键数据错误/遥控已经没有按下了
        {
            RmtSta &= ~(1 << 6); // 清除接收到有效按键标识
            RmtCnt = 0;          // 清除按键次数计数器
        }
    }
    return sta;
}
