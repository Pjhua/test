#include "usart2.h"

Usart2 usart2 = {0};

void usart2_init(u32 baud)
{
    GPIO_InitTypeDef gpio_struct; // 定义结构体
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // 开启时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // 初始化iO
    gpio_struct.GPIO_Mode = GPIO_Mode_AF;           // 复用
    gpio_struct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // 引脚
    gpio_struct.GPIO_OType = GPIO_OType_PP;         // 推挽
    gpio_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;       // 无上下拉
    gpio_struct.GPIO_Speed = GPIO_Speed_50MHz;      // 速度
    GPIO_Init(GPIOA, &gpio_struct);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); // 选择复用功能
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    // 初始化串口
    USART_InitStruct.USART_BaudRate = baud;                                      // 波特率
    USART_InitStruct.USART_StopBits = USART_StopBits_1;                          // 停止位
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;                     // 数据位
    USART_InitStruct.USART_Parity = USART_Parity_No;                             // 校验位
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 模式
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流
    USART_Init(USART2, &USART_InitStruct);

    // 开启串口接收中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
#if USART2_IDLE_USE
    // 开启串口空闲中断
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
#else
    timer12_init(100, 8400);
#endif
    // 初始化NVIC
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART2, ENABLE); // 使能串口
}

// 发送一个字节
void usart2_send_type(u8 data)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
        ;
    USART_SendData(USART2, data);
}

// 发送一串字符串
void usart2_send_string(u8 *str)
{
    while (*str != '\0')
    {
        usart2_send_type(*str);
        str++;
    }
}

// 接收一个字节
u8 usart2_recv_type(void)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
        ;
    return USART_ReceiveData(USART2);
}

// 发送数组数据
void usart2_send_arr(u8 arr[], u16 len)
{
    u8 i;
    for (i = 0; i < len; i++)
    {
        usart2_send_type(arr[i]);
    }
}

void USART2_IRQHandler(void) // usart2中断服务函数
{
    u8 data;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        data = usart2_recv_type();
        if (usart2.recv_len < USART2_RECV_MAX - 1)
        {
            usart2.recv_buff[usart2.recv_len++] = data;
        }
        TIM_SetCounter(TIM12, 0);
        TIM_Cmd(TIM12, ENABLE);
    }
#if USART2_IDLE_USE
    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        usart2.recv_buff[usart2.recv_len] = '\0';
        usart2.recv_flag = 1;
        USART2->SR;
        USART2->DR;
    }
#endif
}

#if !USART2_IDLE_USE
void timer12_init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);

    TIM_TimeBaseInitStruct.TIM_Period = arr - 1;
    TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(TIM12, &TIM_TimeBaseInitStruct);

    TIM_ITConfig(TIM12, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);
}
#endif

void TIM8_BRK_TIM12_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM12, TIM_IT_Update) != RESET)
    {
        TIM_Cmd(TIM12, DISABLE);
        usart2.recv_buff[usart2.recv_len] = '\0';
        usart2.recv_flag = 1;
        TIM_ClearITPendingBit(TIM12, TIM_IT_Update);
    }
}
