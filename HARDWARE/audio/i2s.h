#ifndef __I2S_H
#define __I2S_H

#include "io_bit.h"    									

extern void (*i2s_tx_callback)(void);		
void I2S2_Init(unsigned short I2S_Standard ,unsigned short I2S_Mode,unsigned short I2S_Clock_Polarity,unsigned short I2S_DataFormat);  
unsigned char I2S2_SampleRate_Set(unsigned int samplerate);
void I2S2_TX_DMA_Init(unsigned char* buf0,unsigned char *buf1,unsigned short num); 

#endif





















