#ifndef SHT30_H
#define SHT30_H

#include "io_bit.h"

#define SHT3X_ADDR  0X44			//从机地址
#define SHT3X_READ_TEMP_HUM 0x240b	//非时钟延伸，中等重复性

typedef struct
{
	float temperature;
	float humidity;
}Sht3x_TypeDef; 
 
void Sht3x_Init(void);
int Sht3x_Read_TemperatureHumidity(Sht3x_TypeDef *HT);
 
#endif
