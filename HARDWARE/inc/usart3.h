#ifndef _USART3_H
#define _USART3_H

#include "stm32f4xx.h"
#include <string.h>

#define USART3_RECV_MAX 64

typedef struct Usart3
{
	u8 recv_buff[USART3_RECV_MAX];
	volatile u8 recv_len;
	volatile u8 recv_flag;
	volatile u32 rx_count; /* 累计收到的字节数（诊断用，只增不清） */
} Usart3;

extern Usart3 usart3;

void usart3_init(u32 baud);
void usart3_send_type(u8 data);
void usart3_send_string(u8 *str);
u8 usart3_recv_type(void);
void usart3_send_arr(u8 arr[], u8 len);

#endif
