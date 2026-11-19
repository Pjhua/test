#include "iic.h"
#include "sht3x.h"
#include "delay.h"

static unsigned char crc8(const unsigned char *data, int len);

/*********************************************************************************************************
 * 函 数 名 : Sht3x_Init
 * 功能说明 : 初始化SHT3x端口
 * 形    参 : 无
 * 返 回 值 : 无
 * 备    注 : IIC_SCL：PB6，IIC_SDA：PB7
 *********************************************************************************************************/
void Sht3x_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	 // 通用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;	 // 开漏
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 速度配置
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // 浮空（无上下拉）
	GPIO_Init(GPIOB, &GPIO_InitStructure);			 // 初始化

	IIC_SCL = 1;
	IIC_SDA = 1;
}
/*********************************************************************************************************
 * 函 数 名 : Sht3x_Read_TemperatureHumidity
 * 功能说明 : 读取SHT3x温湿度数据
 * 形    参 : HT：存储温湿度的结构体指针
 * 返 回 值 : 无
 * 备    注 : 无
 *********************************************************************************************************/
int Sht3x_Read_TemperatureHumidity(Sht3x_TypeDef *temp)
{
	unsigned char buff[6] = {0};
	unsigned char i = 0;

	IIC_Start();								// 起止信号
	IIC_WriteByte(SHT3X_ADDR << 1 | IIC_WRITE); // 器件地址+写
	if (IIC_Wait_ACK() == ACK)
	{											 // 对方应答了，继续执行
		IIC_WriteByte(SHT3X_READ_TEMP_HUM >> 8); // 命令的高位
		if (IIC_Wait_ACK() == ACK)
		{											   // 对方应答了，继续执行
			IIC_WriteByte(SHT3X_READ_TEMP_HUM & 0xff); // 命令的低位
			if (IIC_Wait_ACK() == ACK)
			{ // 对方应答了，继续执行
				do
				{
					if (++i > 20)
					{
						return 4;
					}
					IIC_Stop();
					delay_ms(2); // 等待对方采集好数据
					IIC_Start();
					IIC_WriteByte(SHT3X_ADDR << 1 | IIC_READ); // 询问是否采集好了数据
				} while (IIC_Wait_ACK() != ACK); // 如果条件成立，表示传感器采集数据未完成

				for (i = 0; i < 5; i++)
				{
					buff[i] = IIC_ReadByte(ACK);
				}
				buff[5] = IIC_ReadByte(NACK);
				IIC_Stop();

				// 转换温湿度
				if (crc8(buff, 2) == buff[2] && crc8(&buff[3], 2) == buff[5])
				{
					temp->temperature = (buff[0] << 8 | buff[1]) / 65535.0 * 175 - 45;
					temp->humidity = (buff[3] << 8 | buff[4]) / 65535.0 * 100;
				}
				else
					return 5;
			}
			else
				return 3;
		}
		else
			return 2;
	}
	else
		return 1;

	return 0;
}
/*
 *
 * CRC-8 formula from page 14 of SHT spec pdf
 *
 * Test data 0xBE, 0xEF should yield 0x92
 *
 * Initialization data 0xFF
 * Polynomial 0x31 (x8 + x5 +x4 +1)
 * Final XOR 0x00
 */
unsigned char crc8(const unsigned char *data, int len)
{
	const unsigned char POLYNOMIAL = 0x31;
	unsigned char crc = 0xFF;
	int j, i;

	for (j = len; j; --j)
	{
		crc ^= *data++;
		for (i = 8; i; --i)
		{
			crc = (crc & 0x80)
					  ? (crc << 1) ^ POLYNOMIAL
					  : (crc << 1);
		}
	}

	return crc;
}
