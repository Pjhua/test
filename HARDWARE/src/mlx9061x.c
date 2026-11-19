#include "mlx9061x.h"
#include "delay.h"
#include "iic.h"

static unsigned char PEC_cal(unsigned char pec[], int n);

/*********************************************************************************************************
 * 函 数 名 : Mlx90614_Init
 * 功能说明 : Mlx90614端口初始化
 * 形    参 : 无
 * 返 回 值 : 无
 * 备    注 : IIC_SCL：PB6，IIC_SDA：PB7
 *********************************************************************************************************/
void Mlx9061x_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;	 // 开漏
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 速度配置
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // 浮空（无上下拉）
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	IIC_SCL = 1;
	IIC_SDA = 1;
}
/*********************************************************************************************************
 * 函 数 名 : Mlx9061x_ReadTemperature
 * 功能说明 : 从RAM中读取温度值，并转换后返回
 * 形    参 : 出口参数--温度
 * 返 回 值 : 0 成功，其他失败
 * 备    注 : 无
 *********************************************************************************************************/
int Mlx9061x_ReadTemperature(float *temp)
{
	unsigned char DataL;
	unsigned char DataH;
	unsigned char PEC;
	unsigned char arr[6] = {
		0,
		0,
		0,
		MLX9061X_ADDR << 1 | IIC_READ,
		MLX9061X_RAM_ACCESS | MLX9061X_RAM_TOBJ1,
		MLX9061X_ADDR << 1 | IIC_WRITE,
	};

	IIC_Start();
	IIC_WriteByte(MLX9061X_ADDR << 1 | IIC_WRITE);
	if (IIC_Wait_ACK() == ACK)
	{
		IIC_WriteByte(MLX9061X_RAM_ACCESS | MLX9061X_RAM_TOBJ1);
		if (IIC_Wait_ACK() == ACK)
		{
			IIC_SCL = 0; // 针对性的处理
			delay_us(5);
			IIC_Start();
			IIC_WriteByte(MLX9061X_ADDR << 1 | IIC_READ);
			if (IIC_Wait_ACK() == ACK)
			{
				DataL = IIC_ReadByte(ACK);
				DataH = IIC_ReadByte(ACK);
				PEC = IIC_ReadByte(NACK);
				IIC_Stop();

				arr[2] = DataL;
				arr[1] = DataH;

				if (PEC_cal(arr, 6) == PEC)
				{
					*temp = (DataH << 8 | DataL) * 0.02 - 273.15;
				}
				else
					return 4;
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
//----------------------------------------------------------------------------------------------------------------------------------------//
// CALCULATE THE PEC PACKET
//----------------------------------------------------------------------------------------------------------------------------------------//
unsigned char PEC_cal(unsigned char pec[], int n)
{
	unsigned char crc[6];
	unsigned char Bitposition = 47;
	unsigned char shift;
	unsigned char i;
	unsigned char j;
	unsigned char temp;

	do
	{
		crc[5] = 0; // Load CRC value 0x000000000107
		crc[4] = 0;
		crc[3] = 0;
		crc[2] = 0;
		crc[1] = 0x01;
		crc[0] = 0x07;
		Bitposition = 47; // Set maximum bit position at 47
		shift = 0;		  // Find first 1 in the transmitted bytes
		i = 5;			  // Set highest index (package byte index)
		j = 0;			  // Byte bit index, from lowest
		while ((pec[i] & (0x80 >> j)) == 0 && (i > 0))
		{
			Bitposition--;
			if (j < 7)
			{
				j++;
			}
			else
			{
				j = 0x00;
				i--;
			}
		} // the position of highest "1" bit in Bitposition is calculated
		shift = Bitposition - 8; // Get shift value for CRC value

		while (shift)
		{
			for (i = 5; i < 0xFF; i--)
			{
				if ((crc[i - 1] & 0x80) && (i > 0))
				{ // Check if the MSB of the byte lower is "1"
					// Yes - current byte + 1
					temp = 1; // No - current byte + 0
				}
				else
				{ // So that "1" can shift between bytes
					temp = 0;
				}
				crc[i] <<= 1;
				crc[i] += temp;
			}
			shift--;
		}
		// Exclusive OR between pec and crc
		for (i = 0; i <= 5; i++)
		{
			pec[i] ^= crc[i];
		}
	} while (Bitposition > 8);

	return pec[0];
}
