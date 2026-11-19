#include "ebook.h"
#include "ff.h"
#include "lcd.h"
#include "lcd_init.h"
#include "key.h"
#include "w25qxx.h"
#include "delay.h"
#include "usart1.h"
#include <stdio.h>
#include "CST816S.h"

/************************************* 显示布局（竖屏 240x280） *************************************
 * 正文区域：x 8~231，y 8~241，行高 18（16 点阵 + 2 像素间距），每屏 13 行
 * 页脚状态：显示当前页码、阅读进度百分比或 END（已到最后一页）
 ***********************************************************************************************/
#define TEXT_X0 8             // 正文左边距
#define TEXT_X1 (LCD_W - 8)   // 正文右边界（不含）
#define TEXT_Y0 8             // 正文顶边距
#define LINE_H 18             // 行高
#define LINES_PER_PAGE 13     // 每屏行数
#define STATUS_Y (LCD_H - 40) // 页脚状态栏纵坐标
#define RIGHT_X (LCD_W - 40)  // 页脚右侧起始横坐标

#define EBOOK_READ_BUF 2048   // 单页读取缓冲区大小
#define EBOOK_DETECT_BUF 1024 // 编码探测采样字节数
#define EBOOK_MAX_PAGES 4096  // 最多记录的页起始偏移数量

static FATFS s_fs;                      // 文件系统对象
static FIL s_fp;                        // 小说文件对象
static u8 s_buf[EBOOK_READ_BUF];        // 读取缓冲区
static u8 s_opened = 0;                 // 文件是否已打开
static u8 s_utf8 = 0;                   // 1=UTF-8 编码，0=GBK 编码
static FSIZE_t s_page[EBOOK_MAX_PAGES]; // 各页在文件中的起始偏移
static u32 s_page_cnt = 0;              // 已知的页数量
static u32 s_cur = 0;                   // 当前页索引

/******************************************************************************
      函数说明：探测文件是否为 UTF-8 编码（含 UTF-8 BOM 判断）
      返 回 值：1 UTF-8  0 GBK/其它
      备    注：GBK 双字节很难在整段采样中都构成合法 UTF-8 序列，故以此区分
******************************************************************************/
static u8 ebook_detect_utf8(void)
{
    UINT br = 0, i;
    u8 c, need, k;

    f_lseek(&s_fp, 0);
    f_read(&s_fp, s_buf, EBOOK_DETECT_BUF, &br);

    if (br >= 3 && s_buf[0] == 0xEF && s_buf[1] == 0xBB && s_buf[2] == 0xBF)
        return 1; // UTF-8 BOM

    for (i = 0; i < br;)
    {
        c = s_buf[i];
        if (c < 0x80)
        {
            i++;
            continue;
        } // ASCII
        else if ((c & 0xE0) == 0xC0)
            need = 1;
        else if ((c & 0xF0) == 0xE0)
            need = 2;
        else if ((c & 0xF8) == 0xF0)
            need = 3;
        else
            return 0; // 非法序列 → GBK
        if (i + need >= br)
            break; // 采样被截断
        for (k = 1; k <= need; k++)
            if ((s_buf[i + k] & 0xC0) != 0x80)
                return 0;
        i += need + 1;
    }
    return 1;
}

/******************************************************************************
      函数说明：在指定位置显示一个无符号整数（ASCII）
******************************************************************************/
static void ebook_show_u32(u16 x, u16 y, u32 num)
{
    u8 buf[12];
    u8 i = 0, j, t;

    if (num == 0)
        buf[i++] = '0';
    else
    {
        while (num)
        {
            buf[i++] = (u8)('0' + num % 10);
            num /= 10;
        }
    }
    for (j = 0; j < i / 2; j++)
    {
        t = buf[j];
        buf[j] = buf[i - 1 - j];
        buf[i - 1 - j] = t;
    }
    buf[i] = 0;
    LCD_ShowString_Flash(x, y, buf, BLACK, WHITE, EBOOK_FONT_SIZE, 0);
}

/******************************************************************************
      函数说明：绘制页脚状态（页码 / 进度百分比 / END）
      入口数据：next 为当前页之后下一页的起始偏移
******************************************************************************/
static void ebook_draw_status(FSIZE_t next)
{
    u32 start = (u32)s_page[s_cur];
    u32 size = (u32)f_size(&s_fp);

    LCD_ShowString_Flash(TEXT_X0, STATUS_Y, (const u8 *)"P:", BLACK, WHITE, EBOOK_FONT_SIZE, 0);
    ebook_show_u32(TEXT_X0 + 16, STATUS_Y, s_cur + 1);

    if (next >= f_size(&s_fp))
    {
        LCD_ShowString_Flash(RIGHT_X, STATUS_Y, (const u8 *)"END", BLACK, WHITE, EBOOK_FONT_SIZE, 0);
    }
    else if (size > 0)
    {
        ebook_show_u32(RIGHT_X, STATUS_Y, (u32)((float)start * 100.0f / (float)size));
        LCD_ShowString_Flash(RIGHT_X + 24, STATUS_Y, (const u8 *)"%", BLACK, WHITE, EBOOK_FONT_SIZE, 0);
    }
}

/******************************************************************************
      函数说明：将单个 UTF-8 字符转换为 GBK 字节（供 GB2312 字库使用）
      入口数据：utf8 指向一个完整 UTF-8 字符，nbytes 为其字节数
      出口数据：gbk 返回 GBK 编码并按需补 '\0'
      返 回 值：2 表示汉字或全角/中文标点，0 表示无对应字模
      备    注：没有用 lcd.c 内部的 SwitchToGbk，因为它只对 [0x4E00,0x9FA5]
               汉字建表，遇到《》等中文标点会负下标越界读表得出垃圾码。
******************************************************************************/
static u8 ebook_utf8_to_gbk(const u8 *utf8, u8 nbytes, u8 *gbk)
{
    u32 uni;
    WCHAR code;

    if (nbytes == 2)
        uni = ((u32)(utf8[0] & 0x1F) << 6) | (utf8[1] & 0x3F);
    else if (nbytes == 3)
        uni = ((u32)(utf8[0] & 0x0F) << 12) | ((u32)(utf8[1] & 0x3F) << 6) | (utf8[2] & 0x3F);
    else if (nbytes == 4)
        uni = ((u32)(utf8[0] & 0x07) << 18) | ((u32)(utf8[1] & 0x3F) << 12) |
              ((u32)(utf8[2] & 0x3F) << 6) | (utf8[3] & 0x3F);
    else
        return 0;

    code = ff_uni2oem(uni, 936); // Unicode → GBK，使用 FatFs 的 936 码页表
    if (code == 0)
        return 0; // 该字符不在 GBK 字库中，留空

    if (code >= 0x100) // 双字节：汉字或全角/中文标点
    {
        gbk[0] = (u8)(code >> 8);
        gbk[1] = (u8)(code & 0xFF);
        gbk[2] = 0;
        return 2;
    }
    gbk[0] = (u8)code; // 单字节（如部分拉丁字符）
    gbk[1] = 0;
    return 1;
}

/******************************************************************************
      函数说明：从 start 偏移处读取并绘制一页正文
      返 回 值：下一页在文件中的起始偏移（若等于文件大小则表示已到末尾）
      备    注：自动换行；\r 忽略，\n 强制换行；页满或到达文件末尾时结束
******************************************************************************/
static FSIZE_t ebook_render_page(FSIZE_t start)
{
    UINT br = 0, i = 0;
    u16 x = TEXT_X0, y = TEXT_Y0;
    u8 line = 0;
    u8 ch[8];
    u8 c, nbytes, width;

    f_lseek(&s_fp, start);
    f_read(&s_fp, s_buf, EBOOK_READ_BUF, &br);

    /* 跳过页首的连续空行，避免整页空白（如章节分隔处） */
    while (i < br && (s_buf[i] == '\r' || s_buf[i] == '\n'))
        i++;

    while (i < br)
    {
        c = s_buf[i];

        if (c == '\r')
        {
            i++;
            continue;
        } // 忽略回车
        if (c == '\n') // 强制换行
        {
            i++;
            if (line + 1 >= LINES_PER_PAGE)
                break; // 本页已满
            line++;
            x = TEXT_X0;
            y = TEXT_Y0 + line * LINE_H;
            continue;
        }

        /* 判断字符占用的字节数与显示宽度 */
        if (c < 0x80)
        {
            nbytes = 1;
            width = EBOOK_FONT_SIZE / 2;
        }
        else if (s_utf8)
        {
            if ((c & 0xE0) == 0xC0)
                nbytes = 2;
            else if ((c & 0xF0) == 0xE0)
                nbytes = 3;
            else if ((c & 0xF8) == 0xF0)
                nbytes = 4;
            else
            {
                i++;
                continue;
            } // 非法字节，跳过
            width = EBOOK_FONT_SIZE;
        }
        else
        {
            nbytes = 2;
            width = EBOOK_FONT_SIZE;
        } // GBK 双字节

        if (i + nbytes > br)
            break; // 缓冲区尾部残缺字符，留到下一页

        if (x + width > TEXT_X1) // 自动换行
        {
            if (line + 1 >= LINES_PER_PAGE)
                break; // 本页已满
            line++;
            x = TEXT_X0;
            y = TEXT_Y0 + line * LINE_H;
        }

        /* 送字库显示：UTF-8 先转成 GBK，GBK 文件直接送 2 字节 */
        if (nbytes == 1)
        {
            LCD_ShowChar_Flash(x, y, s_buf[i], BLACK, WHITE, EBOOK_FONT_SIZE, 0);
        }
        else if (s_utf8)
        {
            if (ebook_utf8_to_gbk(&s_buf[i], nbytes, ch) == 2)
                LCD_ShowChinese16x16_Flash(x, y, ch, BLACK, WHITE, EBOOK_FONT_SIZE, 0);
            /* 无对应 GBK 字模的字符留空不画 */
        }
        else
        {
            ch[0] = s_buf[i];
            ch[1] = s_buf[i + 1];
            ch[2] = 0;
            LCD_ShowChinese16x16_Flash(x, y, ch, BLACK, WHITE, EBOOK_FONT_SIZE, 0);
        }

        x += width;
        i += nbytes;
    }

    return start + i;
}

void ebook_show_page(void)
{
    FSIZE_t next;

    LCD_Fill(0, 0, LCD_W, LCD_H, WHITE);     // 清屏
    next = ebook_render_page(s_page[s_cur]); // 绘制当前页

    /* 记录下一页起始偏移，便于向前翻页 */
    if (s_cur + 1 < EBOOK_MAX_PAGES &&
        next > s_page[s_cur] && next < f_size(&s_fp))
    {
        s_page[s_cur + 1] = next;
        if (s_page_cnt < s_cur + 2)
            s_page_cnt = s_cur + 2;
    }
    if (s_page_cnt < s_cur + 1)
        s_page_cnt = s_cur + 1;

    ebook_draw_status(next);
}

u8 ebook_next_page(void)
{
    if (s_cur + 1 >= s_page_cnt)
        return 0; // 已是最后一页
    s_cur++;
    ebook_show_page();
    return 1;
}

u8 ebook_prev_page(void)
{
    if (s_cur == 0)
        return 0; // 已是第一页
    s_cur--;
    ebook_show_page();
    return 1;
}

u8 ebook_open(const char *path)
{
    FRESULT res;
    UINT br = 0;

    res = f_mount(&s_fs, "0", 1); // 挂载文件系统
    if (res != FR_OK)
    {
        printf("ebook: 挂载失败 (%d)\r\n", res);
        return 1;
    }

    res = f_open(&s_fp, path, FA_READ); // 只读打开
    if (res != FR_OK)
    {
        printf("ebook: 打开 %s 失败 (%d)\r\n", path, res);
        return 2;
    }

    s_opened = 1;
    s_utf8 = ebook_detect_utf8();

    /* 若存在 UTF-8 BOM，则正文从第 3 字节开始 */
    s_page[0] = 0;
    f_lseek(&s_fp, 0);
    f_read(&s_fp, s_buf, 3, &br);
    if (br == 3 && s_buf[0] == 0xEF && s_buf[1] == 0xBB && s_buf[2] == 0xBF)
        s_page[0] = 3;

    s_page_cnt = 1;
    s_cur = 0;

    printf("ebook: 打开成功，编码=%s，大小=%lu 字节\r\n",
           s_utf8 ? "UTF-8" : "GBK", (unsigned long)f_size(&s_fp));

    ebook_show_page();
    return 0;
}

void ebook_close(void)
{
    if (!s_opened)
        return;
    f_close(&s_fp);
    s_opened = 0;
}

void ebook_run(void)
{
    u8 key;
    s8 set;

    while (1)
    {
        key = key_scan();
        if (key == KEY1_PRE) // 下一页
            set = 1;
        else if (key == KEY2_PRE) // 上一页
            set = -1;
        else if (key == KEY3_PRE)
            return;
        else
            set = cst816s_get_swipe();
        if (set == SLI_LEFT)
        {
            ebook_next_page();
        }
        else if (set == SLI_RIGHT)
        {
            ebook_prev_page();
        }
        else
            continue;
    }
}

void ebook_demo(void)
{
    // W25qxx_Init(); // 初始化字库所在的 SPI Flash
    LCD_Fill(0, 0, LCD_W, LCD_H, WHITE);
    // 文件名如果包含中文（此文件为utf-8编码），需要在ffconf.h中设置FF_LFN_UNICODE为2,即 #define FF_LFN_UNICODE	2
    if (ebook_open(EBOOK_TXT_PATH) != 0)
    {
        LCD_ShowString_Flash(8, STATUS_Y, (const u8 *)"EBOOK OPEN ERROR",
                             RED, WHITE, EBOOK_FONT_SIZE, 0);
        return;
    }

    ebook_run();
}
