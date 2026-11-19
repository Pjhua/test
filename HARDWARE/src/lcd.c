#include "lcd.h"
#include "lcd_init.h"
#include "lcdfont.h"
#include "delay.h"

/******************************************************************************
	  函数说明：在指定区域填充颜色
	  入口数据：xsta,ysta   起始坐标
				xend,yend   终止坐标
									color       要填充的颜色
	  返回值：  无
	  备  注：  USE_LCD_DMA=1 时, 预先填充一行颜色缓冲, 再用DMA逐行发送, 大幅加速刷屏
******************************************************************************/
void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color)
{
#if (USE_HARDWARE_SPI && USE_LCD_DMA)
	u16 i, j;
	u16 w = xend - xsta;
	static u8 color_buf[LCD_W * 2]; // 一行颜色缓冲(最大LCD_W像素, 每像素2字节)
	if (w == 0)
		return;
	if (w > LCD_W)
		w = LCD_W; // 防越界
	/* 把一行的颜色铺到缓冲区(高字节在前, 低字节在后, 与LCD_WR_DATA顺序一致) */
	for (j = 0; j < w; j++)
	{
		color_buf[j * 2] = color >> 8;
		color_buf[j * 2 + 1] = color;
	}
	LCD_Address_Set(xsta, ysta, xend - 1, yend - 1); // 设置显示范围
	LCD_DC_Set();									 // 进入数据模式
	for (i = ysta; i < yend; i++)
	{
		LCD_Writ_Bus_DMA(color_buf, w * 2); // DMA发送一行
	}
#else
	u16 i, j;
	LCD_Address_Set(xsta, ysta, xend - 1, yend - 1); // 设置显示范围
	for (i = ysta; i < yend; i++)
	{
		for (j = xsta; j < xend; j++)
		{
			LCD_WR_DATA(color);
		}
	}
#endif
}

void LCD_Clear(u16 color)
{
	LCD_Fill(0, 0, LCD_W, LCD_H, color);
}

/******************************************************************************
	  函数说明：在指定位置画点
	  入口数据：x,y 画点坐标
				color 点的颜色
	  返回值：  无
******************************************************************************/
void LCD_DrawPoint(u16 x, u16 y, u16 color)
{
	LCD_Address_Set(x, y, x, y); // 设置光标位置
	LCD_WR_DATA(color);
}

void LCD_DrawPoint_DMA(u16 x, u16 y, u16 color)
{
	LCD_Address_Set(x, y, x, y); // 设置光标位置
	LCD_WR_DATA_DMA(color);
}
/******************************************************************************
	  函数说明：画线
	  入口数据：x1,y1   起始坐标
				x2,y2   终止坐标
				color   线的颜色
	  返回值：  无
******************************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
	u16 t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; // 计算坐标增量
	delta_y = y2 - y1;
	uRow = x1; // 画线起点坐标
	uCol = y1;
	if (delta_x > 0)
		incx = 1; // 设置单步方向
	else if (delta_x == 0)
		incx = 0; // 垂直线
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; // 水平线
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; // 选取基本增量坐标轴
	else
		distance = delta_y;
	for (t = 0; t < distance + 1; t++)
	{
		LCD_DrawPoint(uRow, uCol, color); // 画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}

/******************************************************************************
	  函数说明：画矩形
	  入口数据：x1,y1   起始坐标
				x2,y2   终止坐标
				color   矩形的颜色
	  返回值：  无
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
	LCD_DrawLine(x1, y1, x2, y1, color);
	LCD_DrawLine(x1, y1, x1, y2, color);
	LCD_DrawLine(x1, y2, x2, y2, color);
	LCD_DrawLine(x2, y1, x2, y2, color);
}

/******************************************************************************
	  函数说明：画圆
	  入口数据：x0,y0   圆心坐标
				r       半径
				color   圆的颜色
	  返回值：  无
******************************************************************************/
void Draw_Circle(u16 x0, u16 y0, u8 r, u16 color)
{
	int a, b;
	a = 0;
	b = r;
	while (a <= b)
	{
		LCD_DrawPoint(x0 - b, y0 - a, color); // 3
		LCD_DrawPoint(x0 + b, y0 - a, color); // 0
		LCD_DrawPoint(x0 - a, y0 + b, color); // 1
		LCD_DrawPoint(x0 - a, y0 - b, color); // 2
		LCD_DrawPoint(x0 + b, y0 + a, color); // 4
		LCD_DrawPoint(x0 + a, y0 - b, color); // 5
		LCD_DrawPoint(x0 + a, y0 + b, color); // 6
		LCD_DrawPoint(x0 - b, y0 + a, color); // 7
		a++;
		if ((a * a + b * b) > (r * r)) // 判断要画的点是否过远
		{
			b--;
		}
	}
}

/******************************************************************************
	  函数说明：显示汉字串
	  入口数据：x,y显示坐标
				*s 要显示的汉字串
				fc 字的颜色
				bc 字的背景色
				sizey 字号 可选 16 24 32
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_ShowChinese(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	while (*s != 0)
	{
		if (sizey == 12)
			LCD_ShowChinese12x12(x, y, s, fc, bc, sizey, mode);
		else if (sizey == 16)
			LCD_ShowChinese16x16(x, y, s, fc, bc, sizey, mode);
		else if (sizey == 24)
			LCD_ShowChinese24x24(x, y, s, fc, bc, sizey, mode);
		else if (sizey == 32)
			LCD_ShowChinese32x32(x, y, s, fc, bc, sizey, mode);
		else
			return;
		s += 3;
		x += sizey;
	}
}

/******************************************************************************
	  函数说明：显示单个12x12汉字
	  入口数据：x,y显示坐标
				*s 要显示的汉字
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_ShowChinese12x12(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 i, j, m = 0;
	u16 k;
	u16 HZnum;		 // 汉字数目
	u16 TypefaceNum; // 一个字符所占字节大小
	u16 x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;

	HZnum = sizeof(tfont12) / sizeof(typFNT_GB12); // 统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont12[k].Index[0] == *(s)) && (tfont12[k].Index[1] == *(s + 1)) && (tfont12[k].Index[2] == *(s + 2)))
		{
			LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont12[k].Msk[i] & (0x01 << j))
							LCD_WR_DATA(fc);
						else
							LCD_WR_DATA(bc);
						m++;
						if (m % sizey == 0)
						{
							m = 0;
							break;
						}
					}
					else // 叠加方式
					{
						if (tfont12[k].Msk[i] & (0x01 << j))
							LCD_DrawPoint(x, y, fc); // 画一个点
						x++;
						if ((x - x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}

/******************************************************************************
	  函数说明：显示单个16x16汉字
	  入口数据：x,y显示坐标
				*s 要显示的汉字
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_ShowChinese16x16(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 i, j, m = 0;
	u16 k;
	u16 HZnum;		 // 汉字数目
	u16 TypefaceNum; // 一个字符所占字节大小
	u16 x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	HZnum = sizeof(tfont16) / sizeof(typFNT_GB16); // 统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont16[k].Index[0] == *(s)) && (tfont16[k].Index[1] == *(s + 1)) && (tfont16[k].Index[2] == *(s + 2)))
		{
			LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont16[k].Msk[i] & (0x01 << j))
							LCD_WR_DATA(fc);
						else
							LCD_WR_DATA(bc);
						m++;
						if (m % sizey == 0)
						{
							m = 0;
							break;
						}
					}
					else // 叠加方式
					{
						if (tfont16[k].Msk[i] & (0x01 << j))
							LCD_DrawPoint(x, y, fc); // 画一个点
						x++;
						if ((x - x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}

/******************************************************************************
	  函数说明：显示单个24x24汉字
	  入口数据：x,y显示坐标
				*s 要显示的汉字
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_ShowChinese24x24(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 i, j, m = 0;
	u16 k;
	u16 HZnum;		 // 汉字数目
	u16 TypefaceNum; // 一个字符所占字节大小
	u16 x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	HZnum = sizeof(tfont24) / sizeof(typFNT_GB24); // 统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont24[k].Index[0] == *(s)) && (tfont24[k].Index[1] == *(s + 1)) && (tfont24[k].Index[2] == *(s + 2)))
		{
			LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont24[k].Msk[i] & (0x01 << j))
							LCD_WR_DATA(fc);
						else
							LCD_WR_DATA(bc);
						m++;
						if (m % sizey == 0)
						{
							m = 0;
							break;
						}
					}
					else // 叠加方式
					{
						if (tfont24[k].Msk[i] & (0x01 << j))
							LCD_DrawPoint(x, y, fc); // 画一个点
						x++;
						if ((x - x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}

/******************************************************************************
	  函数说明：显示单个32x32汉字
	  入口数据：x,y显示坐标
				*s 要显示的汉字
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_ShowChinese32x32(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 i, j, m = 0;
	u16 k;
	u16 HZnum;		 // 汉字数目
	u16 TypefaceNum; // 一个字符所占字节大小
	u16 x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	HZnum = sizeof(tfont32) / sizeof(typFNT_GB32); // 统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont32[k].Index[0] == *(s)) && (tfont32[k].Index[1] == *(s + 1)) && (tfont32[k].Index[2] == *(s + 2)))
		{
			LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont32[k].Msk[i] & (0x01 << j))
							LCD_WR_DATA(fc);
						else
							LCD_WR_DATA(bc);
						m++;
						if (m % sizey == 0)
						{
							m = 0;
							break;
						}
					}
					else // 叠加方式
					{
						if (tfont32[k].Msk[i] & (0x01 << j))
							LCD_DrawPoint(x, y, fc); // 画一个点
						x++;
						if ((x - x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}

/******************************************************************************
	  函数说明：显示单个字符
	  入口数据：x,y显示坐标
				num 要显示的字符
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_ShowChar(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 temp, sizex, t, m = 0;
	u16 i, TypefaceNum; // 一个字符所占字节大小
	u16 x0 = x;
	sizex = sizey / 2;
	TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
	num = num - ' ';									 // 得到偏移后的值
	LCD_Address_Set(x, y, x + sizex - 1, y + sizey - 1); // 设置光标位置
	for (i = 0; i < TypefaceNum; i++)
	{
		if (sizey == 12)
			temp = ascii_1206[num][i]; // 调用6x12字体
		else if (sizey == 16)
			temp = ascii_1608[num][i]; // 调用8x16字体
		else if (sizey == 24)
			temp = ascii_2412[num][i]; // 调用12x24字体
		else if (sizey == 32)
			temp = ascii_3216[num][i]; // 调用16x32字体
		else
			return;
		for (t = 0; t < 8; t++)
		{
			if (!mode) // 非叠加模式
			{
				if (temp & (0x01 << t))
					LCD_WR_DATA(fc);
				else
					LCD_WR_DATA(bc);
				m++;
				if (m % sizex == 0)
				{
					m = 0;
					break;
				}
			}
			else // 叠加模式
			{
				if (temp & (0x01 << t))
					LCD_DrawPoint(x, y, fc); // 画一个点
				x++;
				if ((x - x0) == sizex)
				{
					x = x0;
					y++;
					break;
				}
			}
		}
	}
}

/******************************************************************************
	  函数说明：显示字符串
	  入口数据：x,y显示坐标
				*p 要显示的字符串
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_ShowString(u16 x, u16 y, const u8 *p, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	while (*p != '\0')
	{
		LCD_ShowChar(x, y, *p, fc, bc, sizey, mode);
		x += sizey / 2;
		p++;
	}
}

/******************************************************************************
	  函数说明：显示数字
	  入口数据：m底数，n指数
	  返回值：  无
******************************************************************************/
u32 mypow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}

/******************************************************************************
	  函数说明：显示整数变量
	  入口数据：x,y显示坐标
				num 要显示整数变量
				len 要显示的位数
				fc 字的颜色
				bc 字的背景色
				sizey 字号
	  返回值：  无
******************************************************************************/
void LCD_ShowIntNum(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc, u8 sizey)
{
	u8 t, temp;
	u8 enshow = 0;
	u8 sizex = sizey / 2;
	for (t = 0; t < len; t++)
	{
		temp = (num / mypow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				LCD_ShowChar(x + t * sizex, y, ' ', fc, bc, sizey, 0);
				continue;
			}
			else
				enshow = 1;
		}
		LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
	}
}

/******************************************************************************
	  函数说明：显示两位小数变量
	  入口数据：x,y显示坐标
				num 要显示小数变量
				len 要显示的位数
				fc 字的颜色
				bc 字的背景色
				sizey 字号
	  返回值：  无
******************************************************************************/
void LCD_ShowFloatNum1(u16 x, u16 y, float num, u8 len, u16 fc, u16 bc, u8 sizey)
{
	u8 t, temp, sizex;
	u16 num1;
	sizex = sizey / 2;
	num1 = num * 100;
	for (t = 0; t < len; t++)
	{
		temp = (num1 / mypow(10, len - t - 1)) % 10;
		if (t == (len - 2))
		{
			LCD_ShowChar(x + (len - 2) * sizex, y, '.', fc, bc, sizey, 0);
			t++;
			len += 1;
		}
		LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
	}
}

/******************************************************************************
	  函数说明：显示图片
	  入口数据：x,y起点坐标
				length 图片长度
				width  图片宽度
				pic[]  图片数组
	  返回值：  无
	  备  注：  USE_LCD_DMA=1 时, 逐行用DMA发送(每行width*2字节), 避免单次DMA超过65535字节限制
******************************************************************************/
void LCD_ShowPicture(u16 x, u16 y, u16 length, u16 width, const u8 pic[])
{
#if (USE_HARDWARE_SPI && USE_LCD_DMA)
	u16 i;
	LCD_Address_Set(x, y, x + length - 1, y + width - 1);
	LCD_DC_Set(); // 进入数据模式
	for (i = 0; i < length; i++)
	{
		/* 第i行起始字节偏移 = i * width * 2, 每行发送 width*2 字节 */
		LCD_Writ_Bus_DMA((u8 *)(pic + (u32)i * width * 2), width * 2);
	}
#else
	u16 i, j;
	u32 k = 0;
	LCD_Address_Set(x, y, x + length - 1, y + width - 1);
	for (i = 0; i < length; i++)
	{
		for (j = 0; j < width; j++)
		{
			LCD_WR_DATA8(pic[k * 2]);
			LCD_WR_DATA8(pic[k * 2 + 1]);
			k++;
		}
	}
#endif
}

/******************************************************************************
	  函数说明：显示图片
	  入口数据：x,y起点坐标
				 pic_addr  图片烧录到Flash的起始地址
	  返回值：  无
	  备  注：  USE_LCD_DMA=1 时, 每行先从W25Qxx读到缓冲, 再用DMA发送到LCD,
				相比原来逐像素读Flash+逐字节发送, 速度大幅提升
******************************************************************************/
void LCD_ShowPictureFlash(u16 x, u16 y, u32 pic_addr)
{
	u32 i;
	u16 width, high;
	u8 pic_head[8] = {0};
#if (USE_HARDWARE_SPI && USE_LCD_DMA)
	u8 *row_buf;
#else
	u8 pic_data[2] = {0};
#endif
	// 从Flash 中读取图片头数据
	W25qxx_ReadDatas(pic_head, pic_addr, 8);
	pic_addr += 8;
	width = pic_head[2] << 8 | pic_head[3];
	high = pic_head[4] << 8 | pic_head[5];
	// printf("width:%d,high:%d\r\n",width,high);
	if (width > LCD_W || high > LCD_H)
	{
		printf("图片尺寸超出LCD范围\r\n");
		return;
	}
	LCD_Address_Set(x, y, x + width - 1, y + high - 1);
#if (USE_HARDWARE_SPI && USE_LCD_DMA)
	row_buf = (u8 *)malloc((u32)width * 2); // 一行像素数据缓冲
	LCD_DC_Set();							// 进入数据模式
	for (i = 0; i < high; i++)
	{
		W25qxx_ReadDatas(row_buf, pic_addr, width * 2);
		pic_addr += width * 2;
		LCD_Writ_Bus_DMA(row_buf, width * 2); // DMA发送一行
	}
	free(row_buf);
#else
	for (i = 0; i < width * high; i++)
	{
		W25qxx_ReadDatas(pic_data, pic_addr, 2);
		pic_addr += 2;
		LCD_WR_DATA8(pic_data[0]);
		LCD_WR_DATA8(pic_data[1]);
	}
#endif
}

void LCD_ShowChinese_Flash(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode) // flash显示汉字串
{
	while (*s != 0)
	{
		if (sizey == 12)
			LCD_ShowChinese12x12_Flash(x, y, s, fc, bc, sizey, mode);
		else if (sizey == 16)
			LCD_ShowChinese16x16_Flash(x, y, s, fc, bc, sizey, mode);
		else if (sizey == 24)
			LCD_ShowChinese24x24_Flash(x, y, s, fc, bc, sizey, mode);
		else if (sizey == 32)
			LCD_ShowChinese32x32_Flash(x, y, s, fc, bc, sizey, mode);
		else
			return;
		s += 3;
		x += sizey;
	}
}

void LCD_ShowChinese12x12_Flash(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 i, j, m = 0;
	u16 TypefaceNum;
	u16 x0 = x;
	u8 *readBuf;
	u8 gbk[8];
	u8 *p = s;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	readBuf = (u8 *)malloc(TypefaceNum);

	// 判断此文件是否是UTF-8的格式，如果是，if成立
	if ((s[0] & 0xF0) == 0xE0 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80)
	{
		if (SwitchToGbk(s, 3, gbk) == 0)
			p = gbk;
	}
	W25qxx_ReadDatas(readBuf, ((p[0] - 0xA1) * 94 + (p[1] - 0xA1)) * TypefaceNum + GB2312_12_FLASH_ADDR, TypefaceNum);
	LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
	for (i = 0; i < TypefaceNum; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (!mode)
			{
				if (readBuf[i] & (0x01 << j))
					LCD_WR_DATA(fc);
				else
					LCD_WR_DATA(bc);
				m++;
				if (m % sizey == 0)
				{
					m = 0;
					break;
				}
			}
			else
			{
				if (readBuf[i] & (0x01 << j))
					LCD_DrawPoint(x, y, fc);
				x++;
				if ((x - x0) == sizey)
				{
					x = x0;
					y++;
					break;
				}
			}
		}
	}
	free(readBuf);
}

void LCD_ShowChinese16x16_Flash(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 i, j, m = 0;
	u16 TypefaceNum;
	u16 x0 = x;
	u8 *readBuf;
	u8 gbk[8];
	u8 *p = s;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	readBuf = (u8 *)malloc(TypefaceNum);
	// 判断此文件是否是UTF-8的格式，如果是，if成立
	if ((s[0] & 0xF0) == 0xE0 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80)
	{
		if (SwitchToGbk(s, 3, gbk) == 0)
			p = gbk;
	}
	W25qxx_ReadDatas(readBuf, ((p[0] - 0xA1) * 94 + (p[1] - 0xA1)) * TypefaceNum + GB2312_16_FLASH_ADDR, TypefaceNum);
	LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
	for (i = 0; i < TypefaceNum; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (!mode)
			{
				if (readBuf[i] & (0x01 << j))
					LCD_WR_DATA(fc);
				else
					LCD_WR_DATA(bc);
				m++;
				if (m % sizey == 0)
				{
					m = 0;
					break;
				}
			}
			else
			{
				if (readBuf[i] & (0x01 << j))
					LCD_DrawPoint(x, y, fc);
				x++;
				if ((x - x0) == sizey)
				{
					x = x0;
					y++;
					break;
				}
			}
		}
	}
	free(readBuf);
}

void LCD_ShowChinese24x24_Flash(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 i, j, m = 0;
	u16 TypefaceNum;
	u16 x0 = x;
	u8 *readBuf;
	u8 gbk[8];
	u8 *p = s;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	readBuf = (u8 *)malloc(TypefaceNum);
	// 判断此文件是否是UTF-8的格式，如果是，if成立
	if ((s[0] & 0xF0) == 0xE0 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80)
	{
		if (SwitchToGbk(s, 3, gbk) == 0)
			p = gbk;
	}
	W25qxx_ReadDatas(readBuf, ((p[0] - 0xA1) * 94 + (p[1] - 0xA1)) * TypefaceNum + GB2312_24_FLASH_ADDR, TypefaceNum);
	LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
	for (i = 0; i < TypefaceNum; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (!mode)
			{
				if (readBuf[i] & (0x01 << j))
					LCD_WR_DATA(fc);
				else
					LCD_WR_DATA(bc);
				m++;
				if (m % sizey == 0)
				{
					m = 0;
					break;
				}
			}
			else
			{
				if (readBuf[i] & (0x01 << j))
					LCD_DrawPoint(x, y, fc);
				x++;
				if ((x - x0) == sizey)
				{
					x = x0;
					y++;
					break;
				}
			}
		}
	}
	free(readBuf);
}

void LCD_ShowChinese32x32_Flash(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 i, j, m = 0;
	u16 TypefaceNum;
	u16 x0 = x;
	u8 *readBuf;
	u8 gbk[8];
	u8 *p = s;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	readBuf = (u8 *)malloc(TypefaceNum);
	// 判断此文件是否是UTF-8的格式，如果是，if成立
	if ((s[0] & 0xF0) == 0xE0 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80)
	{
		if (SwitchToGbk(s, 3, gbk) == 0)
			p = gbk;
	}
	W25qxx_ReadDatas(readBuf, ((p[0] - 0xA1) * 94 + (p[1] - 0xA1)) * TypefaceNum + GB2312_32_FLASH_ADDR, TypefaceNum);
	LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
	for (i = 0; i < TypefaceNum; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (!mode)
			{
				if (readBuf[i] & (0x01 << j))
					LCD_WR_DATA(fc);
				else
					LCD_WR_DATA(bc);
				m++;
				if (m % sizey == 0)
				{
					m = 0;
					break;
				}
			}
			else
			{
				if (readBuf[i] & (0x01 << j))
					LCD_DrawPoint(x, y, fc);
				x++;
				if ((x - x0) == sizey)
				{
					x = x0;
					y++;
					break;
				}
			}
		}
	}
	free(readBuf);
}

/******************************************************************************
	  函数说明：从flash显示单个字符
	  入口数据：x,y显示坐标
				num 要显示的字符
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_ShowChar_Flash(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 temp, sizex, t, m = 0;
	u16 i, TypefaceNum; // 一个字符所占字节大小
	u16 x0 = x;
	u8 *readbuf;
	u32 baseaddr;
	sizex = sizey / 2;
	TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
	readbuf = (u8 *)malloc(TypefaceNum);
	if (sizey == 12)
		baseaddr = ASCII12_FLASH_ADDR;
	else if (sizey == 16)
		baseaddr = ASCII16_FLASH_ADDR;
	else if (sizey == 24)
		baseaddr = ASCII24_FLASH_ADDR;
	else if (sizey == 32)
		baseaddr = ASCII32_FLASH_ADDR;
	else
	{
		free(readbuf);
		return;
	}
	W25qxx_ReadDatas(readbuf, baseaddr + num * TypefaceNum, TypefaceNum);
	LCD_Address_Set(x, y, x + sizex - 1, y + sizey - 1); // 设置光标位置
	for (i = 0; i < TypefaceNum; i++)
	{
		temp = readbuf[i];
		for (t = 0; t < 8; t++)
		{
			if (!mode) // 非叠加模式
			{
				if (temp & (0x01 << t))
					LCD_WR_DATA(fc);
				else
					LCD_WR_DATA(bc);
				m++;
				if (m % sizex == 0)
				{
					m = 0;
					break;
				}
			}
			else // 叠加模式
			{
				if (temp & (0x01 << t))
					LCD_DrawPoint(x, y, fc); // 画一个点
				x++;
				if ((x - x0) == sizex)
				{
					x = x0;
					y++;
					break;
				}
			}
		}
	}
	free(readbuf);
}

/******************************************************************************
	  函数说明：显示字符串
	  入口数据：x,y显示坐标
				*p 要显示的字符串
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_ShowString_Flash(u16 x, u16 y, const u8 *p, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	while (*p != '\0')
	{
		LCD_ShowChar_Flash(x, y, *p, fc, bc, sizey, mode);
		x += sizey / 2;
		p++;
	}
}

/******************************************************************************
	  函数说明：显示整数变量
	  入口数据：x,y显示坐标
				num 要显示整数变量
				len 要显示的位数
				fc 字的颜色
				bc 字的背景色
				sizey 字号
	  返回值：  无
******************************************************************************/
void LCD_ShowIntNum_Flash(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc, u8 sizey)
{
	u8 t, temp;
	u8 enshow = 0;
	u8 sizex = sizey / 2;
	for (t = 0; t < len; t++)
	{
		temp = (num / mypow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				LCD_ShowChar_Flash(x + t * sizex, y, ' ', fc, bc, sizey, 0);
				continue;
			}
			else
				enshow = 1;
		}
		LCD_ShowChar_Flash(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
	}
}

/******************************************************************************
	  函数说明：显示两位小数变量
	  入口数据：x,y显示坐标
				num 要显示小数变量
				len 要显示的位数
				fc 字的颜色
				bc 字的背景色
				sizey 字号
	  返回值：  无
******************************************************************************/
void LCD_ShowFloatNum1_Flash(u16 x, u16 y, float num, u8 len, u16 fc, u16 bc, u8 sizey)
{
	u8 t, temp, sizex;
	u16 num1;
	sizex = sizey / 2;
	num1 = num * 100;
	for (t = 0; t < len; t++)
	{
		temp = (num1 / mypow(10, len - t - 1)) % 10;
		if (t == (len - 2))
		{
			LCD_ShowChar_Flash(x + (len - 2) * sizex, y, '.', fc, bc, sizey, 0);
			t++;
			len += 1;
		}
		LCD_ShowChar_Flash(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
	}
}



/******************************************************************************
      函数说明：显示混合了 ASCII / UTF-8汉字 / GBK汉字 的字符串
      入口数据：x,y显示坐标
                *s 要显示的字符串（ASCII单字节 / UTF-8三字节 / GBK双字节 自动识别）
                fc 字的颜色
                bc 字的背景色
                sizey 字号 可选 12 16 24 32
                mode:  0非叠加模式  1叠加模式
      返回值：  无
      备  注：ASCII 每个字符宽 sizey/2；中文宽 sizey
                字模全部从 W25Q64 Flash 读取（allResource.bin）
******************************************************************************/
void LCD_ShowMixed_Flash(u16 x, u16 y, const u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	while (*s != '\0')
	{
		if (s[0] < 0x80)                                    /* ASCII 单字节 */
		{
			LCD_ShowChar_Flash(x, y, *s, fc, bc, sizey, mode);
			x += sizey / 2;
			s++;
		}
		else if ((s[0] & 0xF0) == 0xE0 &&
		         (s[1] & 0xC0) == 0x80 &&
		         (s[2] & 0xC0) == 0x80)                   /* UTF-8 三字节汉字 */
		{
			/*  必须调单汉字版本 (LCD_ShowChinese{N}x{N}_Flash)，
			 *  不能调 LCD_ShowChinese_Flash —— 它内部有 while(*s!=0) 扫全串，
			 *  遇到 ASCII 会错误地当成 GBK 双字节处理，画出杂点！ */
			if      (sizey == 12) LCD_ShowChinese12x12_Flash(x, y, (u8 *)s, fc, bc, sizey, mode);
			else if (sizey == 16) LCD_ShowChinese16x16_Flash(x, y, (u8 *)s, fc, bc, sizey, mode);
			else if (sizey == 24) LCD_ShowChinese24x24_Flash(x, y, (u8 *)s, fc, bc, sizey, mode);
			else if (sizey == 32) LCD_ShowChinese32x32_Flash(x, y, (u8 *)s, fc, bc, sizey, mode);
			x += sizey;
			s += 3;
		}
		else if (s[0] >= 0x81 && s[1] >= 0x40)              /* GBK 双字节汉字 */
		{
			if      (sizey == 12) LCD_ShowChinese12x12_Flash(x, y, (u8 *)s, fc, bc, sizey, mode);
			else if (sizey == 16) LCD_ShowChinese16x16_Flash(x, y, (u8 *)s, fc, bc, sizey, mode);
			else if (sizey == 24) LCD_ShowChinese24x24_Flash(x, y, (u8 *)s, fc, bc, sizey, mode);
			else if (sizey == 32) LCD_ShowChinese32x32_Flash(x, y, (u8 *)s, fc, bc, sizey, mode);
			x += sizey;
			s += 2;
		}
		else                                                /* 非法字节，跳过 */
		{
			s++;
		}
	}
}
