#ifndef __MLX9061x_H
#define __MLX9061x_H

#include "io_bit.h"

#define MLX90614

#if defined (MLX90614) 
	#define MLX9061X_ADDR 			0X5A
	#define MLX9061X_RAM_ACCESS		0x00 
	#define MLX9061X_RAM_TOBJ1		0x07 
#endif

#if defined (MLX90615) 
	#define MLX9061X_ADDR 			0X5B
	#define MLX9061X_RAM_ACCESS		0x00 
	#define MLX9061X_RAM_TOBJ1		0x27 
#endif

void Mlx9061x_Init(void);
int Mlx9061x_ReadTemperature(float *temp);

#endif
