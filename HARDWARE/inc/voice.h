#ifndef __VOICE_H
#define __VOICE_H

#include "stm32f4xx.h"

/*
 * HLK-V20 语音识别模块（接 USART3，115200）
 *   识别结果帧格式：4F 00 <命令ID> F4        （模组“自定义串口协议输出”）
 *   命令 ID 与《固件命令词与播报词格式.xlsx》一一对应
 *
 * 回复音（播报）
 *   直接打开 SD 卡 0:/prompt/ 下“以播报语命名”的 wav 播放，例如：
 *     0:/prompt/好的，已为您打开风扇.wav
 *     0:/prompt/嗯！我来了.wav        （免唤醒命令词“小盈小盈”的回复）
 *     0:/prompt/有需要再叫我.wav      （主动退出“再见”的回复）
 *   文件不存在则只执行动作、不出声。
 *   唤醒词（小信小信/小盈同学）由模块自己播报回复语，串口无输出。
 */

void voice_init(void);   /* 清空接收状态（在通信任务启动前调用一次） */
void voice_handle(void); /* 在通信(WiFi)任务主循环里不断调用：解析命令、控硬件 */

/* 在 MUSIC 任务主循环里不断调用：执行语音投递过来的播报/音乐命令，
   并负责“播报打断歌曲→播完回原位置续播”的调度 */
void voice_music_service(void);

/* RGB 氛围灯当前状态（语音改过之后供界面同步显示；w2812 PB15 软件时序） */
extern volatile unsigned int g_rgb_color;  /* 0xRRGGBB，0=全灭 */
extern volatile unsigned char g_rgb_effect; /* 0=静止 1=七彩流动(音乐模式) */

#endif
