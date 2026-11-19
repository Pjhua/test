#ifndef __SPI_H
#define __SPI_H

#include "io_bit.h"

void Spi1_Init(void);
void Spi2_Init(void);
void Spi3_Init(void);

unsigned char Spi1_WriteReadByte(unsigned char data);
unsigned char Spi2_WriteReadByte(unsigned char data);
unsigned char Spi3_WriteReadByte(unsigned char data);

/* SPI1 总线互斥锁：SD 卡(sd_driver)与 W25QXX(w25qxx) 共用 SPI1，
   多任务并发访问时必须串行化整笔事务（CS 拉低到拉高为一个事务），
   否则两个任务的字节流会在总线上交错，数据错乱甚至总线卡死 */
void Spi1_BusLock(void);
void Spi1_BusUnlock(void);

#endif
