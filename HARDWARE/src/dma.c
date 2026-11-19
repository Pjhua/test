#include "dma.h"

u8 dma_ITflag = 0;
const uint32_t aSRC_Const_Buffer[BUFFER_SIZE] = {
    0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
    0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,
    0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
    0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,
    0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
    0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,
    0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
    0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80};
uint32_t aDST_Buffer[BUFFER_SIZE];

const u8 aTx_Buffer[] = "hello,world!!!!";

void dma_init_flashToRam(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    __IO uint32_t Timeout = TIMEOUT_MAX;

    RCC_AHB1PeriphClockCmd(DMA_STREAM_CLOCK, ENABLE);

    DMA_DeInit(DMA_STREAM);

    while (DMA_GetCmdStatus(DMA_STREAM) != DISABLE)
    {
    }

    DMA_InitStructure.DMA_Channel = DMA_CHANNEL;                            // 通道选择
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)aSRC_Const_Buffer; // 外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)aDST_Buffer;          // 存储器地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;                     // 传输方向
    DMA_InitStructure.DMA_BufferSize = (uint32_t)BUFFER_SIZE;               // 传输数量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;         // 增量
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 增量
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // 传输宽度
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;         // 传输宽度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                     // 高优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  // 不使用fifo
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;           //
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;             // 单次
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     // 单次
    DMA_Init(DMA_STREAM, &DMA_InitStructure);

    DMA_ITConfig(DMA_STREAM, DMA_IT_TC, ENABLE);

    DMA_Cmd(DMA_STREAM, ENABLE);

    Timeout = TIMEOUT_MAX;
    while ((DMA_GetCmdStatus(DMA_STREAM) != ENABLE) && (Timeout-- > 0))
    {
    }
    if (Timeout == 0)
    {
        while (1)
        {
        }
    }
    NVIC_InitStructure.NVIC_IRQChannel = DMA_STREAM_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void dma_init_flashToUsart1Tx(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    __IO uint32_t Timeout = TIMEOUT_MAX;

    DMA_DeInit(DMA2_Stream7);

    while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE)
    {
    }

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                          // 选择通道
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)aTx_Buffer;           // 存储器地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR));   // 外设地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                 // 传输模式选择
    DMA_InitStructure.DMA_BufferSize = sizeof(aTx_Buffer);                  // 传输数量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 非增量
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 增量
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 传输宽度
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 传输宽度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                     // 高优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  // 不使用fifo
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;   //
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;             // 单次
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     // 单次

    DMA_Init(DMA2_Stream7, &DMA_InitStructure);

    Timeout = TIMEOUT_MAX;
    while ((DMA_GetCmdStatus(DMA2_Stream7) != ENABLE) && (Timeout-- > 0))
    {
    }
    if (Timeout == 0)
    {
        while (1)
        {
        }
    }

    // USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);

    DMA_Cmd(DMA2_Stream7, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}

void dma_init_flashToUsart1Rx(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    __IO uint32_t Timeout = TIMEOUT_MAX;

    DMA_DeInit(DMA2_Stream5);

    while (DMA_GetCmdStatus(DMA2_Stream5) != DISABLE)
    {
    }

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                          // 选择通道
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)usart1.recv_buff;     // 存储器地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR));   // 外设地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                 // 传输模式选择
    DMA_InitStructure.DMA_BufferSize = USART1_RECV_MAX;                     // 传输数量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 非增量
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 增量
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 传输宽度
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 传输宽度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                         // 循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                     // 高优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  // 不使用fifo
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;   //
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;             // 单次
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     // 单次

    DMA_Init(DMA2_Stream5, &DMA_InitStructure);

    Timeout = TIMEOUT_MAX;
    while ((DMA_GetCmdStatus(DMA2_Stream5) != ENABLE) && (Timeout-- > 0))
    {
    }
    if (Timeout == 0)
    {
        while (1)
        {
        }
    }

    DMA_Cmd(DMA2_Stream5, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
}

u8 Buffercmp(const uint32_t *pBuffer, uint32_t *pBuffer1, uint16_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer != *pBuffer1)
        {
            return 1;
        }
        pBuffer++;
        pBuffer1++;
    }

    return 0;
}

void DMA2_Stream0_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA_STREAM, DMA_IT_TCIF))
    {
        DMA_ClearITPendingBit(DMA_STREAM, DMA_IT_TCIF);
        dma_ITflag = 1;
    }
}

void LCD_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(LCD_DMA_STREAM);
    while (DMA_GetCmdStatus(LCD_DMA_STREAM) != DISABLE)
    {
    }
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    DMA_InitStructure.DMA_Channel = LCD_DMA_CHANNEL;                        // 选择通道
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;                    // 存储器地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI3->DR;         // 外设地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                 // 传输模式选择
    DMA_InitStructure.DMA_BufferSize = 1;                                   // 传输数量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 非增量
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 增量
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 传输宽度
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 传输宽度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                     // 高优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  // 不使用fifo
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;       //
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;             // 单次
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     // 单次
    DMA_Init(LCD_DMA_STREAM, &DMA_InitStructure);
}

/******************************************************************************
  函数说明：通过SPI3+DMA发送缓冲数据到LCD
  入口数据：buf  待发送数据缓冲区首地址
            len  数据长度(字节数)
  返回值：  无
  备  注：  查询方式等待传输完成; 调用前需先完成 LCD_DMA_Init() 初始化
            调用者负责 DC(数据/命令) 和 CS(片选) 的电平控制
******************************************************************************/
void LCD_SPI3_DMA_Send(u8 *buf, u16 len)
{
    if (len == 0)
        return;

    /* 关闭DMA数据流 (NDTR/M0AR 只能在EN=0时写入) */
    DMA_Cmd(LCD_DMA_STREAM, DISABLE);
    while (DMA_GetCmdStatus(LCD_DMA_STREAM) != DISABLE)
    {
    }

    /*重设内存地址与传输数量 */
    DMA_MemoryTargetConfig(LCD_DMA_STREAM, (uint32_t)buf, DMA_Memory_0);
    DMA_SetCurrDataCounter(LCD_DMA_STREAM, len);

    /*清除标志 */
    DMA_ClearFlag(LCD_DMA_STREAM, LCD_DMA_TC_FLAG);

    /* 先使能SPI3的DMA发送请求, 再使能DMA数据流 */
    SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(LCD_DMA_STREAM, ENABLE);

    /*等待DMA传输完成 (所有数据已写入SPI3->DR) */
    while (DMA_GetFlagStatus(LCD_DMA_STREAM, LCD_DMA_TC_FLAG) == RESET)
    {
    }

    /*等待SPI发送缓冲区为空 且 硬件不忙 (确保最后一字节真正移位发出) */
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET)
    {
    }
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET)
    {
    }

    /* 先关闭DMA数据流, 再关闭SPI3的DMA发送请求(ST官方例程顺序) */
    DMA_Cmd(LCD_DMA_STREAM, DISABLE);
    while (DMA_GetCmdStatus(LCD_DMA_STREAM) != DISABLE)
    {
    }
    SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, DISABLE);

    /*  清除DMA传输完成标志, 供下次使用 */
    DMA_ClearFlag(LCD_DMA_STREAM, LCD_DMA_TC_FLAG);
    SPI3->SR;
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == SET)
    {
        SPI_I2S_ReceiveData(SPI3);
    }
}
