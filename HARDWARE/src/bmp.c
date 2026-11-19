#include "bmp.h"
#include "lcd.h"
#include "lcd_init.h"
#include "ff.h"

#define BMP_BUF_SIZE 4096           /* 位图原始数据缓冲：一次连续读多行，凑够多扇区，减少SD卡命令次数 */
#define BMP_LINE_BUF_SIZE (240 * 2) /* 转换后的RGB565行缓冲：240像素，每像素2字节 */

u8 bmp_buff[BMP_BUF_SIZE];                 /* 位图原始数据缓冲区 */
static u8 bmp_line_buf[BMP_LINE_BUF_SIZE]; /* 转换后的RGB565行缓冲(高字节在前) */

/******************************************************************************
      函数说明：把一行BMP原始像素数据转换成LCD需要的RGB565字节流
      入口数据：src       BMP原始行数据(含行填充字节)
                dst       转换结果，每像素2字节，高字节在前(与LCD_WR_DATA发送顺序一致)
                width     该行像素个数
                bit_count 位深：16/24/32
                is_555    仅16位有效：1=5:5:5，0=5:6:5
      返回值：  无
      备  注：  先在RAM里整行完成颜色转换，再整行DMA发送，
                避免原来逐像素调用LCD_DrawPoint(每点都要重设一次显示窗口)
******************************************************************************/
static void bmp_line_convert(u8 *src, u8 *dst, u16 width, u16 bit_count, u8 is_555)
{
    u16 x, color;

    for (x = 0; x < width; x++)
    {
        if (bit_count == 16)
        {
            if (is_555)
            {
                color = (src[x * 2] & 0X1F);       /* B */
                color |= (src[x * 2] & 0XE0) << 1; /* G */
                color |= src[x * 2 + 1] << 9;      /* R */
            }
            else
            { /* 5:6:5 */
                color = src[x * 2] | (src[x * 2 + 1] << 8);
            }
        }
        else if (bit_count == 24)
        {
            color = (src[x * 3] >> 3) | ((src[x * 3 + 1] >> 2) << 5) | ((src[x * 3 + 2] >> 3) << 11); /* RGB888转565 */
        }
        else /* 32位 */
        {
            color = (src[x * 4] >> 3) | ((src[x * 4 + 1] >> 2) << 5) | ((src[x * 4 + 2] >> 3) << 11);
        }

        dst[x * 2] = color >> 8;
        dst[x * 2 + 1] = color;
    }
}

/******************************************************************************
      函数说明：BMP解码显示核心
      入口数据：xpos,ypos   图片左上角显示坐标
                clear_screen 1=图片没铺满整屏时先清屏  0=不清屏(用于图标叠加)
                file_path 带路径的文件名
      返回值：  1 成功，0 失败
      备  注：  提速关键点——
                1) 每行只设置一次显示窗口(原来每画一个点就设置一次，约11字节/点)
                2) 颜色转换在RAM完成，整行用SPI3+DMA发送(每行一次DMA，不占CPU搬字节)
                3) 文件数据按4096字节分块连续读取，减少FatFs/SD卡的单扇区读开销
******************************************************************************/
static u8 bmp_decode_core(u16 xpos, u16 ypos, u8 clear_screen, const char *file_path)
{
    UINT br;
    FIL fp;
    FRESULT res;
    u16 y, i;
    u16 width, height;
    u16 line_bytes;
    u8 add_bytes = 0;
    u8 is_555 = 0;
    u16 rows, rows_per_chunk;
    u8 *raw_line;
    BITMAPFILEHEADER bmpfileheader;
    BITMAPINFOHEADER bmpinfoheader;

    res = f_open(&fp, file_path, FA_READ); /* 打开file_path路径上的源文件，FA_READ->指定读访问对象。 可以从文件中读取数据 */
    if (res != FR_OK)
    {
        printf("打开文件%s出错,错误码：%d\r\n", file_path, res);
        return 0;
    }
    /* 读取文件头信息 */
    res = f_read(&fp, &bmpfileheader, sizeof(bmpfileheader), &br);
    if (res != FR_OK)
    {
        f_close(&fp);
        return 0;
    }
    res = f_read(&fp, &bmpinfoheader, sizeof(bmpinfoheader), &br);
    if (res != FR_OK)
    {
        f_close(&fp);
        return 0;
    }

    width = bmpinfoheader.ciWidth;
    height = bmpinfoheader.ciHeight;
    if (width == 0 || height == 0 ||
        (bmpinfoheader.ciBitCount != 16 && bmpinfoheader.ciBitCount != 24 && bmpinfoheader.ciBitCount != 32))
    {
        printf("不支持的BMP格式:宽%d 高%d 位深%d\r\n", width, height, bmpinfoheader.ciBitCount);
        f_close(&fp);
        return 0;
    }
    /* 行缓冲和窗口都是按LCD尺寸申请的，超出范围直接拒绝，防止缓冲越界 */
    if ((u32)xpos + width > LCD_W || (u32)ypos + height > LCD_H)
    {
        printf("图片尺寸超出LCD范围\r\n");
        f_close(&fp);
        return 0;
    }

    if ((width * (bmpinfoheader.ciBitCount / 8)) % 4 == 0) /* 能整除 */
    {
        add_bytes = 0;
    }
    else
    {
        add_bytes = 4 - (width * (bmpinfoheader.ciBitCount / 8)) % 4;
    }
    line_bytes = width * (bmpinfoheader.ciBitCount / 8) + add_bytes;

    if (bmpinfoheader.ciBitCount == 16) /* 16位时用压缩标志区分5:5:5和5:6:5，与原逻辑一致 */
    {
        is_555 = (bmpinfoheader.ciCompress == 0);
    }

    /* 图片没铺满整屏时才清屏：满屏图片每行逐点覆盖，清屏纯属白刷一遍，省掉一次全屏填充 */
    if (clear_screen && (width < LCD_W || height < LCD_H))
    {
        LCD_Clear(WHITE);
    }

    /* 每次连续读取的行数：让单次读取尽量占满缓冲区(接近整扇区)，减少每次f_read的固定开销 */
    rows_per_chunk = BMP_BUF_SIZE / line_bytes;
    if (rows_per_chunk == 0)
        rows_per_chunk = 1;
    if (rows_per_chunk > height)
        rows_per_chunk = height;

    for (y = 0; y < height;)
    {
        rows = height - y;
        if (rows > rows_per_chunk)
            rows = rows_per_chunk;

        res = f_read(&fp, bmp_buff, (UINT)rows * line_bytes, &br);
        if (res != FR_OK || br < (UINT)rows * line_bytes)
            break; /* 读取失败或文件数据不全，停止 */

        for (i = 0; i < rows; i++)
        {
            raw_line = bmp_buff + (u32)i * line_bytes;
            bmp_line_convert(raw_line, bmp_line_buf, width, bmpinfoheader.ciBitCount, is_555);

            /* BMP数据自下而上存放：文件第y行为图像最底部一行，对应显示到 ypos+height-1-y 行 */
            LCD_Address_Set(xpos, ypos + height - 1 - (y + i),
                            xpos + width - 1, ypos + height - 1 - (y + i));
            LCD_DC_Set();                              /* 进入数据模式 */
            LCD_Writ_Bus_DMA(bmp_line_buf, width * 2); /* DMA整行发送 */
        }
        y += rows;
    }

    f_close(&fp);
    return 1;
}

// const char *file_path带路径的文件名
int bmp_decode(const char *file_path)
{
    return bmp_decode_core(0, 0, 1, file_path);
}

// const char *file_path带路径的文件名
u8 bmp_decode_mini(u16 xpos, u16 ypos, const char *file_path)
{
    return bmp_decode_core(xpos, ypos, 0, file_path);
}
