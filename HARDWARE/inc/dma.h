#ifndef _DMA_H
#define _DMA_H

#include "stm32f4xx.h"
#include "usart1.h"

#define DMA_STREAM DMA2_Stream0
#define DMA_CHANNEL DMA_Channel_1
#define DMA_STREAM_CLOCK RCC_AHB1Periph_DMA2
#define DMA_STREAM_IRQ DMA2_Stream0_IRQn
#define DMA_IT_TCIF DMA_IT_TCIF0
#define BUFFER_SIZE 32
#define TIMEOUT_MAX 10000

#define LCD_DMA_STREAM DMA1_Stream5    /* LCD DMA 数据流: SPI3_TX */
#define LCD_DMA_CHANNEL DMA_Channel_0  /* SPI3_TX 对应通道0 */
#define LCD_DMA_TC_FLAG DMA_FLAG_TCIF5 /* 传输完成标志(Stream5) */

extern u8 dma_ITflag, dma_usart_ITflag;
extern const uint32_t aSRC_Const_Buffer[BUFFER_SIZE];
extern uint32_t aDST_Buffer[BUFFER_SIZE];

u8 Buffercmp(const uint32_t *pBuffer, uint32_t *pBuffer1, uint16_t BufferLength);
void dma_init_flashToRam(void);
void dma_init_flashToUsart1Tx(void);
void dma_init_flashToUsart1Rx(void);
void LCD_DMA_Init(void);
void LCD_SPI3_DMA_Send(u8 *buf, u16 len);

#endif
