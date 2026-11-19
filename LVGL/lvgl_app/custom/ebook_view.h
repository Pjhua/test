#ifndef __EBOOK_VIEW_H
#define __EBOOK_VIEW_H

#include "stm32f4xx.h"

/* ============================ LVGL 电子书 ============================
 * 只负责文本读取与分页，不用任何裸机 LCD 绘图函数。
 * 目录：0:/ebook（只收录 .txt，大小写不敏感）
 * 分页规则：正文区 224x198 像素，16px 字体 + 2px 行距 ->
 *           每行 14 个汉字，每页 11 行
 * ==================================================================== */

#define EBK_DIR         "0:/ebook"  /* 电子书目录 */
#define EBK_MAX_BOOKS   32          /* 最多收录的 txt 数量 */
#define EBK_PATH_MAX    100         /* 完整路径缓冲 "0:/ebook/xxx.txt"（长中文名也放得下） */

/* 扫描目录，返回收录的书本数量（同时把索引清零） */
uint16_t ebk_scan_dir(void);

uint16_t ebk_book_count(void);      /* 共几本 */
uint16_t ebk_book_index(void);      /* 当前是第几本（0 起） */
const char *ebk_book_name(void);    /* 当前书名（仅文件名部分） */
const char *ebk_book_title(uint16_t index); /* 第 index 本书名，越界返回 "" */

/* 打开第 index 本并完成全书分页；返回 0 成功，非 0 失败 */
uint8_t ebk_open_book(uint16_t index);
/* 下一本，最后一本之后回到第一本；始终返回 1（打不开的书也会切过去并给出原因） */
uint8_t ebk_next_book(void);
/* 最近一次 ebk_open_book 的结果：0 成功，2 文件打不开，3 编码解不出正文 */
uint8_t ebk_error(void);
/* 最近一次 f_open 失败的 FatFs 原始返回码（0 表示没失败过），用于排查 */
uint8_t ebk_fresult(void);
/* 关闭文件并清空所有书本/分页状态 */
void ebk_close_view(void);

uint8_t ebk_next_page(void);        /* 下一页，返回 0 表示已是最后一页 */
uint8_t ebk_prev_page(void);        /* 上一页，返回 0 表示已是第一页 */
uint16_t ebk_page_index(void);      /* 当前页（0 起） */
uint16_t ebk_page_count(void);      /* 总页数（至少为 1） */
uint8_t ebk_progress(void);         /* 阅读进度 0~100 */
uint8_t ebk_truncated(void);        /* 1 表示页数超过上限，后面还有内容 */
const char *ebk_page_text(void);    /* 当前页正文（UTF-8，已带排版换行） */

#endif
