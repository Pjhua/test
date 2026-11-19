#ifndef _USART1_H
#define _USART1_H

#include "stm32f4xx.h"
#include "led.h"
#include "beep.h"
#include "relay.h"
#include "fan.h"
#include <string.h>
#include "dma.h"

#define USART1_RECV_MAX 64
#define USART1_RECV_USE_DMA 1 // 串口接收是否使用DMA 1使用 0不使用

typedef struct Usart1
{
	u8 recv_buff[USART1_RECV_MAX];
	u8 recv_len;
	u8 recv_flag;
} Usart1;

extern Usart1 usart1;

void usart1_init(u32 baud);
void usart1_send_type(u8 data);
void usart1_send_string(u8 *str);
u8 usart1_recv_type(void);
// void usart1_recv_string(void);
void usart1_handle(void);
void usart1_send_pack(u8 arr[], u8 len);

#endif
