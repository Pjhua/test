#ifndef _W2812_H
#define _W2812_H

#include "stm32f4xx.h"
#include "io_bit.h"
#include "delay.h"

#define W2812 PBout(15)

// 24位RGB颜色宏定义 格式：0xRRGGBB RR红色 GG绿色 BB蓝色
#define RGB_RED 0xFF0000   // 红色
#define RGB_GREEN 0x00FF00 // 绿色
#define RGB_BLUE 0x0000FF  // 蓝色
#define RGB_BLACK 0x000000 // 黑色（熄灭）
#define RGB_WHITE 0xFFFFFF // 白色

// 二次混合色
#define RGB_YELLOW 0xFFFF00  // 黄色
#define RGB_CYAN 0x00FFFF    // 青色
#define RGB_MAGENTA 0xFF00FF // 品红/洋红

// 常用柔和色系
#define RGB_ORANGE 0xFFA500 // 橙色
#define RGB_PURPLE 0x800080 // 紫色
#define RGB_PINK 0xFFC0CB   // 粉色
#define RGB_LIME 0x00FF00   // 酸橙绿
#define RGB_TEAL 0x008080   // 水鸭青
#define RGB_OLIVE 0x808000  // 橄榄绿
#define RGB_MAROON 0x800000 // 褐红
#define RGB_NAVY 0x000080   // 藏青

// 灰度系列
#define RGB_GRAY 0x808080      // 灰色
#define RGB_LIGHTGRAY 0xD3D3D3 // 浅灰色
#define RGB_DARKGRAY 0x404040  // 深灰色

// 其他常用
#define RGB_SKYBLUE 0x87CEEB // 天蓝色
#define RGB_GOLD 0xFFD700    // 金色
#define RGB_SILVER 0xC0C0C0  // 银色
#define RGB_BROWN 0xA52A2A   // 棕色

#define RGB_DARK_RED 0x400000   // 暗红
#define RGB_DARK_GREEN 0x004000 // 暗绿
#define RGB_DARK_BLUE 0x000040  // 暗蓝

#define RGB_LOW_RED 0x100000   // 极弱红
#define RGB_LOW_GREEN 0x001000 // 极弱绿
#define RGB_LOW_BLUE 0x000010  // 极弱蓝

void w2812_init(void);
void w2812_send_color(u32 color);
void w2812_reset(void);
void w2812_write_byte(u8 data);

#define Pixel_NUM 4 // 灯珠实际数量
#define CODE0 0xC0  // 0码发送的时间
#define CODE1 0xF8  // 1码发送的时间

void RgbDma_Init(void);
void RGB_SetColor(unsigned char LedId, const unsigned int Color);
void RGB_SetNumColor(unsigned char Pixel_LEN, const unsigned int Color);

#endif
