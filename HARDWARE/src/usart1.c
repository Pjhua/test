#include "usart1.h"

Usart1 usart1 = {0};

void usart1_init(u32 baud)
{
    GPIO_InitTypeDef gpio_struct; // 定义结构体
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    // 初始化iO
    gpio_struct.GPIO_Mode = GPIO_Mode_AF;            // 复用
    gpio_struct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; // 引脚
    gpio_struct.GPIO_OType = GPIO_OType_PP;          // 推挽
    gpio_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;        // 无上下拉
    gpio_struct.GPIO_Speed = GPIO_Speed_50MHz;       // 速度
    GPIO_Init(GPIOA, &gpio_struct);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1); // 选择复用功能
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    // 初始化串口
    USART_InitStruct.USART_BaudRate = baud;                                      // 波特率
    USART_InitStruct.USART_StopBits = USART_StopBits_1;                          // 停止位
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;                     // 数据位
    USART_InitStruct.USART_Parity = USART_Parity_No;                             // 校验位
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 模式
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流
    USART_Init(USART1, &USART_InitStruct);

#if USART1_RECV_USE_DMA
    dma_init_flashToUsart1Rx();
#else
    // 开启串口接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
#endif
    // 开启串口空闲中断
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

    // 初始化NVIC
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART1, ENABLE); // 使能串口
}

// 发送一个字节
void usart1_send_type(u8 data)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        ;
    USART_SendData(USART1, data);
}

// 发送一串字符串
void usart1_send_string(u8 *str)
{
    while (*str != '\0')
    {
        usart1_send_type(*str);
        str++;
    }
}

// 接收一个字节
u8 usart1_recv_type(void)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
        ;
    return USART_ReceiveData(USART1);
}

// 发送一包数据
void usart1_send_pack(u8 arr[], u8 len)
{
    u8 i;
    for (i = 0; i < len; i++)
    {
        usart1_send_type(arr[i]);
    }
}

void USART1_IRQHandler(void) // USART1中断服务函数
{
#if !USART1_RECV_USE_DMA
    u8 data;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        data = usart1_recv_type();
        if (usart1.recv_len < USART1_RECV_MAX - 1)
        {
            usart1.recv_buff[usart1.recv_len++] = data;
        }
    }
#endif
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        usart1.recv_len = USART1_RECV_MAX - DMA_GetCurrDataCounter(DMA2_Stream5);
        usart1.recv_buff[usart1.recv_len] = '\0';
        DMA_Cmd(DMA2_Stream5, DISABLE);
        DMA_SetCurrDataCounter(DMA2_Stream5, USART1_RECV_MAX);
        DMA_Cmd(DMA2_Stream5, ENABLE);
        usart1.recv_flag = 1;
        USART1->SR;
        USART1->DR;
    }
}

void usart1_handle(void)
{
    if (usart1.recv_flag)
    {
        printf("len:%d,data:%s\r\n", usart1.recv_len, usart1.recv_buff);
        // strcasecmp--不区分大小写
        if (strcasecmp((char *)usart1.recv_buff, "led1on") == 0)
        {
            LED1_ON();
        }
        // 区分大小写
        if (strcmp((char *)usart1.recv_buff, "led1off") == 0)
        {
            LED1_OFF();
        }
        // strncmp--前n位,区分大小写
        if (strncmp((char *)usart1.recv_buff, "led2on", 6) == 0)
        {
            LED2_ON();
        }
        // strncasecmp--前n位,不区分大小写
        if (strncasecmp((char *)usart1.recv_buff, "led2off", 7) == 0)
        {
            LED2_OFF();
        }
        if (strcasecmp((char *)usart1.recv_buff, "led3on") == 0)
        {
            LED3_ON();
        }
        if (strcasecmp((char *)usart1.recv_buff, "led3off") == 0)
        {
            LED3_OFF();
        }
        if (strcasecmp((char *)usart1.recv_buff, "jdqon") == 0)
        {
            RELAY = 1;
        }
        if (strcasecmp((char *)usart1.recv_buff, "jdqoff") == 0)
        {
            RELAY = 0;
        }
        if (strcasecmp((char *)usart1.recv_buff, "fson") == 0)
        {
            FAN = 1;
        }
        if (strcasecmp((char *)usart1.recv_buff, "fsoff") == 0)
        {
            FAN = 0;
        }
        if (strcasecmp((char *)usart1.recv_buff, "beepon") == 0)
        {
            BEEP = 1;
        }
        if (strcasecmp((char *)usart1.recv_buff, "beepoff") == 0)
        {
            BEEP = 0;
        }
        memset(&usart1, 0, sizeof(usart1));
    }
}

// 加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
// 标准库需要的支持函数
struct __FILE
{
    int handle;
};

FILE __stdout;
// 定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
void _ttywrch(int ch)
{
    ch = ch;
}
// 重定义fputc函数
int fputc(int ch, FILE *f)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        ; // 循环发送,直到发送完毕
    USART_SendData(USART1, ch);
    return ch;
}
#endif
