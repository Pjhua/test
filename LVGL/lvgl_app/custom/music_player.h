
#ifndef __MUSIC_PLAYER_H
#define __MUSIC_PLAYER_H

#include "stm32f4xx.h"

/* 播放状态 */
#define MUS_STOPPED  0   /* 停止（未播放） */
#define MUS_PLAYING  1   /* 正在播放 */
#define MUS_PAUSED   2   /* 暂停 */

/* 首次播放前调用（内部只初始化一次 WM8978 + I2S） */
void mus_init(void);
/* 音量调节：pct = 0~100（线性）；mus_vol_service() 需在 10ms 定时器里调用，
   内部按 0.5dB/级逐级逼近目标值，避免音量跳变产生爆音 */
void mus_set_volume(int pct);
int  mus_volume(void);
void mus_vol_service(void);

/* 扫描 0:/music，返回可播放（wav）歌曲数量 */
int  mus_scan(void);
int  mus_current(void);          /* 当前歌曲索引，-1 表示没有 */
int  mus_state(void);

/* 播放第 idx 首（0 开始）。返回 0 成功，<0 失败 */
int  mus_play(int idx);

/* 播放语音回复音（如 "0:/prompt/fanSwitchO.wav"）：
   正在放歌时会记住位置打断，提示音播完自动回到原位置继续播放。
   提示音播放期间硬件音量会自动提升（见 MUS_PROMPT_BOOST），播完恢复原音量。
   返回 0 成功，<0 失败（文件不存在/格式不支持等） */
int  mus_play_prompt(const char *path);
u8   mus_is_prompt(void);        /* 当前播的是提示音（而不是歌曲） */
void mus_toggle(void);           /* 播放/暂停切换（停止时开始播当前曲） */
void mus_pause(void);            /* 只暂停（没在播放则无动作） */
void mus_resume(void);           /* 只续播（暂停中才有效） */
void mus_prev(void);
void mus_next(void);
void mus_stop(void);

/* 从指定字节位置播放第 idx 首（语音交互"暂停→播报→继续"用）：
   byte_pos 传 mus_byte_pos() 的返回值即可回到原位置；0 或越界则从头播 */
int  mus_play_at(int idx, u32 byte_pos);
u32  mus_byte_pos(void);         /* 当前播放位置的字节偏移，0 表示未播放 */

/* 由 main 主循环不断调用：喂 DMA 缓冲区、刷新时间/歌词索引、自动切下一首
   （不能放在 LVGL 定时器里，界面重绘会拖慢补数导致 DMA 断流产生杂音） */
void mus_tick(void);

unsigned int mus_cursec(void);
unsigned int mus_totsec(void);

const char *mus_title(int idx);  /* 歌名（去掉扩展名） */
int  mus_lyric_index(void);      /* 当前歌词行号，-1 表示没有 */
const char *mus_lyric_line(int idx); /* 指定行歌词文本，越界返回 NULL */

#endif
