#include "usart3.h"

struct Usart3 usart3;

void usart3_init(u32 baud)
{
    GPIO_InitTypeDef gpio_struct; // 定义结构体
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); // 开启时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    // 初始化iO
    gpio_struct.GPIO_Mode = GPIO_Mode_AF;           // 复用
    gpio_struct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8; // 引脚
    gpio_struct.GPIO_OType = GPIO_OType_PP;         // 推挽
    gpio_struct.GPIO_PuPd = GPIO_PuPd_UP;           // 上拉：接收脚悬空时保持空闲高电平
    gpio_struct.GPIO_Speed = GPIO_Speed_50MHz;      // 速度
    GPIO_Init(GPIOD, &gpio_struct);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3); // 选择复用功能
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);

    // 初始化串口
    USART_InitStruct.USART_BaudRate = baud;                                      // 波特率
    USART_InitStruct.USART_StopBits = USART_StopBits_1;                          // 停止位
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;                     // 数据位
    USART_InitStruct.USART_Parity = USART_Parity_No;                             // 校验位
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 模式
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流
    USART_Init(USART3, &USART_InitStruct);

    // 开启串口接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    // 开启串口空闲中断
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);

    // 初始化NVIC
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART3, ENABLE); // 使能串口
}

// 发送一个字节
void usart3_send_type(u8 data)
{
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        ;
    USART_SendData(USART3, data);
}

// 发送一串字符串
void usart3_send_string(u8 *str)
{
    while (*str != '\0')
    {
        usart3_send_type(*str);
        str++;
    }
}

// 接收一个字节
u8 usart3_recv_type(void)
{
    while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET)
        ;
    return USART_ReceiveData(USART3);
}

// 发送数组数据
void usart3_send_arr(u8 arr[], u8 len)
{
    u8 i;
    for (i = 0; i < len; i++)
    {
        usart3_send_type(arr[i]);
    }
}

void USART3_IRQHandler(void) // usart3中断服务函数
{
    u8 data;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        data = (u8)USART_ReceiveData(USART3);
        usart3.rx_count++;
        if (usart3.recv_len < USART3_RECV_MAX - 1)
        {
            usart3.recv_buff[usart3.recv_len++] = data;
        }
    }
    /* 溢出/噪声错误：读 DR 清除，否则接收会彻底卡死（波特率不对时常见） */
    if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) == SET)
    {
        (void)USART_ReceiveData(USART3);
    }
    if (USART_GetITStatus(USART3, USART_IT_IDLE) == SET)
    {
        usart3.recv_flag = 1;
        (void)USART3->SR;
        (void)USART3->DR;
    }
}


