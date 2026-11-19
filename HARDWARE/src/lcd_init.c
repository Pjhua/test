#include "lcd_init.h"
#include "delay.h"

void LCD_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD, ENABLE); // 使能PORTB,PORTD时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       // 上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);             // 初始化
    GPIO_SetBits(GPIOB, GPIO_Pin_4);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       // 上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);             // 初始化
    GPIO_SetBits(GPIOD, GPIO_Pin_3 | GPIO_Pin_4);

#if USE_HARDWARE_SPI // 使用硬件SPI
    SPI_InitTypeDef SPI_InitStruct;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       // 复用
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       // 上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);             // 初始化
    GPIO_SetBits(GPIOB, GPIO_Pin_3 | GPIO_Pin_5);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3); // 选择复用功能
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3);

    // 开启SPI3时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    /* 全双工 */
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    /* 主模式*/
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    /* 数据位 */
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    /*  时 钟 极 性*/
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
    /* 时 钟 相 位  */
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    /*  从器件管理(是否NSS 硬 件 管 理）*/
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    /* 波特率 */
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    /* 先传高位还是低位 */
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    /* Initialize the SPI_CRCPolynomial member */
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI3, &SPI_InitStruct);

    SPI_Cmd(SPI3, ENABLE);

#else // 使用IO模拟SPI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       // 上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);             // 初始化
    GPIO_SetBits(GPIOB, GPIO_Pin_3 | GPIO_Pin_5);
#endif

#if USE_LCD_DMA
    LCD_DMA_Init();
#endif
}

/******************************************************************************
      函数说明：LCD串行数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_Writ_Bus(u8 dat)
{
    LCD_CS_Clr();
#if USE_HARDWARE_SPI
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET)
        ;
    SPI_SendData(SPI3, dat);

    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET)
        ;
    SPI_ReceiveData(SPI3);
#else
    u8 i;
    for (i = 0; i < 8; i++)
    {
        LCD_SCLK_Clr();
        if (dat & 0x80)
        {
            LCD_MOSI_Set();
        }
        else
        {
            LCD_MOSI_Clr();
        }
        LCD_SCLK_Set();
        dat <<= 1;
    }
#endif
    LCD_CS_Set();
}

/******************************************************************************
      函数说明：LCD批量数据写入函数(DMA方式)
      入口数据：buf  待发送数据缓冲区
                len  数据长度(字节数)
      返回值：  无
      备  注：  调用前由调用者设置好 DC 电平(数据模式LCD_DC_Set/命令模式LCD_DC_Clr)
                本函数完成: CS拉低 -> SPI3+DMA发送整段数据 -> CS拉高
                USE_LCD_DMA=1 时使用DMA搬运(不占用CPU); =0时回退为逐字节CPU搬运
******************************************************************************/
void LCD_Writ_Bus_DMA(u8 *buf, u16 len)
{

    LCD_CS_Clr(); // 拉低片选
#if (USE_HARDWARE_SPI && USE_LCD_DMA)
    LCD_SPI3_DMA_Send(buf, len); // SPI3 + DMA 批量发送
#else
    u16 i;
    for (i = 0; i < len; i++)
    {
        LCD_Writ_Bus(buf[i]); // 逐字节发送(CPU搬运)
    }
#endif
    LCD_CS_Set(); // 拉高片选
}

/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
    LCD_Writ_Bus(dat);
}

/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
    LCD_Writ_Bus(dat >> 8);
    LCD_Writ_Bus(dat);
}

void LCD_WR_DATA_DMA(u16 dat)
{
    u8 buf[2];
    buf[0] = (dat >> 8) & 0xFF; // 高字节
    buf[1] = dat & 0xFF;        // 低字节
    LCD_Writ_Bus_DMA(buf, 2);
    while (DMA1_Stream7->CR & DMA_SxCR_EN)
        ;
}
/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
    LCD_DC_Clr(); // 写命令
    LCD_Writ_Bus(dat);
    LCD_DC_Set(); // 写数据
}

/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
    y1 += 20;
    y2 += 20;
    if (USE_HORIZONTAL == 0)
    {
        LCD_WR_REG(0x2a); // 列地址设置
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b); // 行地址设置
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c); // 储存器写
    }
    else if (USE_HORIZONTAL == 1)
    {
        LCD_WR_REG(0x2a); // 列地址设置
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b); // 行地址设置
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c); // 储存器写
    }
    else if (USE_HORIZONTAL == 2)
    {
        LCD_WR_REG(0x2a); // 列地址设置
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b); // 行地址设置
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c); // 储存器写
    }
    else
    {
        LCD_WR_REG(0x2a); // 列地址设置
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b); // 行地址设置
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c); // 储存器写
    }
}

void LCD_Init(void)
{
    LCD_GPIO_Init(); // 初始化GPIO

    //    LCD_RES_Clr();//复位
    //    delay_ms(100);
    //    LCD_RES_Set();
    //    delay_ms(100);

    LCD_BLK_Set(); // 打开背光
    delay_ms(100);

    //************* Start Initial Sequence **********//
    LCD_WR_REG(0x11); // Sleep out
    delay_ms(120);    // Delay 120ms
    //************* Start Initial Sequence **********//
    LCD_WR_REG(0x36);
    if (USE_HORIZONTAL == 0)
        LCD_WR_DATA8(0x00);
    else if (USE_HORIZONTAL == 1)
        LCD_WR_DATA8(0xC0);
    else if (USE_HORIZONTAL == 2)
        LCD_WR_DATA8(0x70);
    else
        LCD_WR_DATA8(0xA0);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x05);

    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(0xB7);
    LCD_WR_DATA8(0x35);

    LCD_WR_REG(0xBB);
    LCD_WR_DATA8(0x32); // Vcom=1.35V

    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x01);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x15); // GVDD=4.8V  颜色深度

    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x20); // VDV, 0x20:0v

    LCD_WR_REG(0xC6);
    LCD_WR_DATA8(0x0F); // 0x0F:60Hz

    LCD_WR_REG(0xD0);
    LCD_WR_DATA8(0xA4);
    LCD_WR_DATA8(0xA1);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);
    LCD_WR_REG(0x21);

    LCD_WR_REG(0x29);
}
