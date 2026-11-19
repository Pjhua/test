/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


#ifndef EVENTS_INIT_H_
#define EVENTS_INIT_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"


typedef struct 
{
	unsigned char led1:1;
	unsigned char led2:1;
	unsigned char led3:1;
	unsigned char beep:1;
	unsigned char relay:1;
	unsigned char fan:2;
	unsigned char sg90:3;
	unsigned char rgb_count:3;
	unsigned char red;
	unsigned char green;
	unsigned char blue;
}lv_app;


void events_init(lv_ui *ui);

/* 音乐后台播放控制：创建长按检测定时器（LVGL 任务里调用一次，幂等） */
void music_ctl_init(void);

void events_init_screen_main_1(lv_ui *ui);
void events_init_screen_main_2(lv_ui *ui);
void events_init_screen_app(lv_ui *ui);
void events_init_screen_cal(lv_ui *ui);
void events_init_screen_clk(lv_ui *ui);
void events_init_screen_ebook(lv_ui *ui);
void events_init_screen_look(lv_ui *ui);
void events_init_screen_music(lv_ui *ui);
void events_init_screen_pic(lv_ui *ui);
void events_init_screen_note(lv_ui *ui);
void events_init_screen_file(lv_ui *ui);
void events_init_screen_news(lv_ui *ui);
void events_init_screen_wifi(lv_ui *ui);

#ifdef __cplusplus
}
#endif
#endif /* EVENT_CB_H_ */
