#ifndef __BMP_H__
#define __BMP_H__
#include "stm32f4xx.h"

typedef struct
{
    u16 cfType;                             /* 文件类型, 必须为 "BM" (0x4D42)*/
    u32 cfSize;                             /* 文件的大小(字节) */
    u32 cfReserved;                         /* 保留, 必须为 0 */
    u32 cfoffBits;                          /* 位图阵列相对于文件头的偏移量(字节)*/
} __attribute__((packed)) BITMAPFILEHEADER; /* 文件头结构 */
/*__attribute__ packed属性修改了编译器对结构体成员的布局，
尽可能压缩存储空间。
*/

typedef struct
{
    u32 ciSize;                             /* size of BITMAPINFOHEADER */
    u32 ciWidth;                            /* 位图宽度(像素) */
    u32 ciHeight;                           /* 位图高度(像素) */
    u16 ciPlanes;                           /* 目标设备的位平面数, 必须置为1 */
    u16 ciBitCount;                         /* 每个像素的位数, 1,4,8,16,24或者32 */
    u32 ciCompress;                         /* 位图阵列的压缩方法,0=不压缩 */
    u32 ciSizeImage;                        /* 图像大小(字节) */
    u32 ciXPelsPerMeter;                    /* 目标设备水平每米像素个数 */
    u32 ciYPelsPerMeter;                    /* 目标设备垂直每米像素个数 */
    u32 ciClrUsed;                          /* 位图实际使用的颜色表的颜色数 */
    u32 ciClrImportant;                     /* 重要颜色索引的个数 */
} __attribute__((packed)) BITMAPINFOHEADER; /* 位图信息头结构 */

int bmp_decode(const char *file_path);
u8 bmp_decode_mini(u16 xpos, u16 ypos, const char *file_path);
#endif
