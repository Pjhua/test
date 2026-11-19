#include "stm32f4xx.h"
#include "lcd.h"
#include "lcd_init.h"
#include "cst816s.h"
#include "delay.h"
#include <string.h>
#include "app_gui.h"
#include "app.h"
#include "timer.h"
#include "bmp.h"

/************************************* 资源说明 *************************************
 * 字库 allFont.bin 仍烧录在 W25Q64 中（各字库偏移见 lcd.h），中文显示沿用Flash字模。
 * 图片改为存放在SD卡 "0:icos/app" 目录下，显示时由FatFs读取BMP解码：
 *   back_240280.bmp  整屏背景（240x280）
 *   app/calc/clock/ebook/game/music/picture/set/wireless_70.bmp  9个图标（70x70）
 ***********************************************************************************************/
#define APP_PIC_DIR  "0:icos/app"

// 9个图标在SD卡中的文件路径
u8 *pic_path[9] = {
    APP_PIC_DIR "/app_70.bmp",     APP_PIC_DIR "/calc_70.bmp",  APP_PIC_DIR "/clock_70.bmp",
    APP_PIC_DIR "/ebook_70.bmp",   APP_PIC_DIR "/notepad_70.bmp",  APP_PIC_DIR "/music_70.bmp",
    APP_PIC_DIR "/picture_70.bmp", APP_PIC_DIR "/set_70.bmp",   APP_PIC_DIR "/ctrl_70.bmp"
};

// 9个图标的中文说明（与文件路径表一一对应）
u8 *pic_name[9] = {
    "应用", "计算器", "时钟",
    "电子书", "查看", "音乐",
    "相册", "设置", "无线"
};

// 3列图标的x坐标，2排图标的y坐标
u16 icon_x[3] = {10, 85, 160};
u16 icon_y[2] = {25, 150};

u8 gui_page = 0; // 当前页面：0第1页，1第2页

// 显示一个页面（每页6个图标位置，3列2排）
void gui_show_page(u8 page)
{
    u8 i;
    u8 index; // 图标编号（0~8）
    u8 col, row;
    u16 x, y;
    u16 tx;
    u8 words; // 中文说明的汉字个数

    LCD_Fill(0, 0, LCD_W - 1, LCD_H - 1, WHITE); // 先整屏填充白色，把应用界面的旧数据清掉
    bmp_decode(APP_PIC_DIR "/back_240280.bmp");  // 从SD卡读取BMP背景图

    for (i = 0; i < 6; i++)
    {
        index = page * 6 + i;
        if (index >= 9)
            break; // 图标不够6个时，剩余位置留空

        col = i % 3;
        row = i / 3;
        x = icon_x[col];
        y = icon_y[row];

        bmp_decode_mini(x, y, (char *)pic_path[index]); // 从SD卡读取BMP，显示图标

        words = strlen((char *)pic_name[index]) / 3; // UTF-8每个汉字3字节
        tx = x + (70 - words * 24) / 2;				 // 文字在图标正下方居中
        LCD_ShowChinese_Flash(tx, y + 72, pic_name[index], BLACK, WHITE, 24, 1);
    }
}

// 判断触摸点落在哪个图标上（图标+下面的中文都算），返回图标编号0~8，没点中返回0xFF
static u8 gui_icon_hit(u16 px, u16 py)
{
    u8 i, index, col, row;
    u16 x, y;

    for (i = 0; i < 6; i++)
    {
        index = gui_page * 6 + i;
        if (index >= 9)
            break;

        col = i % 3;
        row = i / 3;
        x = icon_x[col];
        y = icon_y[row];

        if (px >= x && px <= x + 70 && py >= y && py <= y + 72 + 24)
        {
            return index;
        }
    }
    return 0xFF;
}

// 根据图标编号进入对应的子界面，子界面返回后重画当前主界面
static void gui_enter_app(u8 index)
{
    switch (index)
    {
    case 0: // 控制
        app_control_new();
        break;
    case 4: // 查看
        app_look();
        break;
    case 2: // 时钟
        app_clock();
        break;
    case 6:
        app_album();
        break;
    case 3:
        app_ebook();
        break;
    case 5:
        app_music();
        break;
    default:
        return; // 其他图标暂未实现
    }
    gui_show_page(gui_page); // 从子界面返回，恢复主界面
}

// 主界面：显示第1页，并循环检测左右滑动切换页面、点击图标进入子界面
void gui_init(void)
{
    u16 x, y;
    u16 start_x = 0, start_y = 0;
    u8 pressed = 0; // 当前是否有一次未结算的触摸按下
    u8 swiped = 0;  // 本次触摸是否已被判定为滑动
    s8 dir;
    u8 hit;

    gui_show_page(0);
    while (1)
    {
        time_handle(); // 软件时钟走时

        // 滑动判断：cst816s_get_swipe内部记录按下起点，抬手瞬间结算水平位移
        // 返回 1=左滑(下一页) -1=右滑(上一页) 0=无滑动
        dir = cst816s_get_swipe();
        if (dir != 0)
        {
            swiped = 1; // 本次触摸以滑动结束，抬手后不作为点击处理
            if (dir == 1 && gui_page < 1) // 左滑，显示下一页
            {
                gui_page++;
                gui_show_page(gui_page);
            }
            else if (dir == -1 && gui_page > 0) // 右滑，显示上一页
            {
                gui_page--;
                gui_show_page(gui_page);
            }
        }

        // 点击判断：INT引脚只在手指按下/抬起瞬间拉低，此时CST816S_Read能读到坐标
        if (CST816S_Read(&x, &y) == 0)
        {
            if (!pressed) // 只记第一次按下的坐标，作为点击位置的判定
            {
                pressed = 1;
                swiped = 0;
                start_x = x;
                start_y = y;
            }
        }
        else if (pressed && swiped) // 滑动结束：清触摸标志，不进图标
        {
            pressed = 0;
        }
        else if (pressed) // 抬手且没有滑动 → 按按下时的坐标判断点了哪个图标
        {
            pressed = 0;
            hit = gui_icon_hit(start_x, start_y);
            if (hit != 0xFF)
            {
                gui_enter_app(hit);
            }
        }
        delay_ms(10);
    }
}
