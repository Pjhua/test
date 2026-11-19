#ifndef _USART2_H
#define _USART2_H

#include "stm32f4xx.h"
#include <string.h>

#define USART2_RECV_MAX 2048
#define USART2_IDLE_USE 0 // 1使用空闲中断，0使用定时器中断

typedef struct Usart2
{
	u8 recv_buff[USART2_RECV_MAX];
	u16 recv_len;
	u8 recv_flag;
} Usart2;

extern Usart2 usart2;

void usart2_init(u32 baud);
void usart2_send_type(u8 data);
void usart2_send_string(u8 *str);
u8 usart2_recv_type(void);
void usart2_send_arr(u8 arr[], u16 len);
void timer12_init(u16 arr, u16 psc);

#endif
