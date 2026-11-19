#ifndef __EBOOK_H
#define __EBOOK_H

#include "stm32f4xx.h"

/* 电子书默认打开的 txt 文件，放在 SD 卡ebook目录，可自行修改 */

#define EBOOK_TXT_PATH "0:/ebook/完美世界.txt" //"0:/novel/斗罗大陆.txt"

/* 正文点阵字号，需与 W25Q64 中已烧录的字库对应（当前使用 16x16） */
#define EBOOK_FONT_SIZE 16

/* 打开电子书：挂载文件系统并打开 EBOOK_TXT_PATH 指向的文件
 * 返回值：0 成功，非 0 失败 */
u8 ebook_open(const char *path);

/* 关闭电子书 */
void ebook_close(void);

/* 显示当前页 */
void ebook_show_page(void);

/* 翻到下一页，返回 1 成功，0 表示已是最后一页 */
u8 ebook_next_page(void);

/* 翻到上一页，返回 1 成功，0 表示已是第一页 */
u8 ebook_prev_page(void);

/* 按键翻页主循环：KEY1 上一页，KEY3 下一页 */
void ebook_run(void);

/* 电子书一键演示：初始化字库 Flash、打开默认小说并进入翻页循环 */
void ebook_demo(void);

#endif
