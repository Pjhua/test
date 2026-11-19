#ifndef __LYRIC_H
#define __LYRIC_H

#include "stm32f4xx.h"
#include "malloc.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "delay.h"
#include "ff.h"



/****************************************************************************************
 * LRC 歌词解析模块
 * 使用：Lyric_Load("0:/music/周杰伦 - 东风破.wav") → 内部自动找同名 .lrc
 *       Lyric_GetCurrent(cursec) → 返回当前秒对应的歌词文本指针
 *       Lyric_Unload() → 释放内存
 ****************************************************************************************/

/* 每行歌词最多 63 字节 + '\0'，够装 "一盏离愁孤单伫立在窗口" 这种 11 个汉字了 */
#define LYRIC_TEXT_MAX  64
/* 最多解析 200 行歌词，一般歌曲 80~150 行足够 */
#define LYRIC_LINE_MAX  200

typedef struct {
	u16  sec;               /* 时间戳，单位 秒（由 mm:ss.xx 转来） */
	u8   text[LYRIC_TEXT_MAX];  /* 歌词文本（UTF-8，不含时间戳） */
} LyricLine;

/* 加载与 wav_path 同名的 .lrc 文件；成功返回文件内的歌词行数，失败返回 0 */
u16  Lyric_Load(const char *wav_path);

/* 释放歌词所占内存 */
void Lyric_Unload(void);

/* 查询当前 cursec 应该显示哪一行歌词；返回 NULL 表示没有（未加载或已到尾部） */
const u8 *Lyric_GetCurrent(u32 cursec);

/* 返回"行号"，如果这一秒歌词没变返回 0xFFFF；变了返回新行号
 * LCD 可据此判断要不要重刷（避免每秒画相同内容） */
u16 Lyric_GetCurrentIdx(u32 cursec);

/* 获取总歌词行数（调试/显示用） */
u16  Lyric_GetLineCount(void);

/* 按行号取歌词文本（用于一次显示多行歌词），越界返回 NULL */
const u8 *Lyric_GetLine(u16 idx);

#endif
