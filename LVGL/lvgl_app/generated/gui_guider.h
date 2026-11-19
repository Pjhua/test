/*
 * Copyright 2026 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

/* ---- 全局 UI 设计规范（240x280 固定屏，所有界面遵循）----------------------
   主色        0x2195F6（进度条、滑块、选中态、主按钮）
   文字色      0x0D3055（浅色背景上的所有文字统一深蓝，替代纯黑保证柔和一致）
   卡片/底     0xFFFFFF
   轨道/分隔   0xE6E2E6
   关闭态      0x9AA0A6（边框 0x7D838B，半透明）
   状态灯      LED1 0x22C55E / LED2 0x2195F6 / LED3 0xF59E0B / 继电器 0xEF4444
   圆角        卡片 8、进度条/滑块 5~10、指示灯为圆形
   间距        页面左右边距 5px；查看页行距 42px；图标栅格间距 10px
   字体        统一 myFont_simkai16（工程内仅一套 XBF 字库，层级靠颜色与
               字距区分，避免缺字方块）
   交互        图片按钮按下 image_opa=170 作为按压反馈；切屏统一
               LV_SCR_LOAD_ANIM_NONE 立即切换（帧缓冲与 RAM 有限，
               淡入/滑动动画会同时保留两屏，得不偿失）
------------------------------------------------------------------------- */

    typedef struct
    {

        lv_obj_t *screen_main_1;
        bool screen_main_1_del;
        lv_obj_t *screen_main_1_icon_app;    /* 应用图标 */
        lv_obj_t *screen_main_1_icon_cal;    /* 计算器图标 */
        lv_obj_t *screen_main_1_icon_clk;    /* 时钟图标 */
        lv_obj_t *screen_main_1_icon_music;  /* 音乐图标 */
        lv_obj_t *screen_main_1_icon_ebook;  /* 电子书图标 */
        lv_obj_t *screen_main_1_icon_look;   /* 查看图标 */
        lv_obj_t *screen_main_1_label_app;
        lv_obj_t *screen_main_1_label_clk;
        lv_obj_t *screen_main_1_label_cal;
        lv_obj_t *screen_main_1_label_music;
        lv_obj_t *screen_main_1_label_look;
        lv_obj_t *screen_main_1_label_ebook;
        lv_obj_t *screen_main_2;
        bool screen_main_2_del;
        lv_obj_t *screen_main_2_icon_note;   /* 便签图标 */
        lv_obj_t *screen_main_2_icon_file;   /* 文件图标 */
        lv_obj_t *screen_main_2_icon_pic;    /* 相册图标 */
        lv_obj_t *screen_main_2_icon_set;    /* 设置图标 */
        lv_obj_t *screen_main_2_icon_news;   /* 消息图标 */
        lv_obj_t *screen_main_2_label_set;
        lv_obj_t *screen_main_2_label_news;
        lv_obj_t *screen_main_2_label_pic;
        lv_obj_t *screen_main_2_label_file;
        lv_obj_t *screen_main_2_label_note;
        lv_obj_t *screen_main_2_btn_home;    /* 第 6 格：首页（LVGL 内置图标） */
        lv_obj_t *screen_main_2_label_home;
        lv_obj_t *screen_app;
        bool screen_app_del;
        lv_obj_t *screen_app_roller_fan;
        lv_obj_t *screen_app_sw_led3;
        lv_obj_t *screen_app_sw_led2;
        lv_obj_t *screen_app_sw_led1;
        lv_obj_t *screen_app_sw_beep;
        lv_obj_t *screen_app_sw_relay;
        lv_obj_t *screen_app_label_fan;
        lv_obj_t *screen_app_label_led3;
        lv_obj_t *screen_app_label_led2;
        lv_obj_t *screen_app_label_led1;
        lv_obj_t *screen_app_label_beep;
        lv_obj_t *screen_app_label_relay;
        lv_obj_t *screen_app_label_sg90;
        lv_obj_t *screen_app_roller_sg90;
        lv_obj_t *screen_app_slider_R;
        lv_obj_t *screen_app_slider_G;
        lv_obj_t *screen_app_slider_B;
        lv_obj_t *screen_app_roller_RGB_NUM;
        lv_obj_t *screen_app_label_RGB_R;
        lv_obj_t *screen_app_label_RGB_G;
        lv_obj_t *screen_app_label_RGB_B;
        lv_obj_t *screen_app_label_RGB_NUM;
        lv_obj_t *screen_app_label_RGB_color;
        lv_obj_t *screen_app_icon_fan;       /* 风扇行图标 */
        lv_obj_t *screen_app_icon_sg90;      /* 舵机行图标 */
        lv_obj_t *screen_app_icon_rgbnum;    /* RGB 数量行图标 */
        lv_obj_t *screen_cal;
        bool screen_cal_del;
        lv_obj_t *screen_cal_btnm_1;
        lv_obj_t *screen_cal_label_1;
        lv_obj_t *screen_clk;
        bool screen_clk_del;
        lv_obj_t *screen_clk_btn_back;
        lv_obj_t *screen_clk_title;
        lv_obj_t *screen_clk_analog_clock_1;
        lv_obj_t *screen_clk_analog_clock_1_hour_needle;
        lv_obj_t *screen_clk_analog_clock_1_min_needle;
        lv_obj_t *screen_clk_analog_clock_1_sec_needle;
        lv_obj_t *screen_clk_digital_clock_1;
        lv_obj_t *screen_clk_datetext_1;
        lv_obj_t *screen_ebook;
        bool screen_ebook_del;
        lv_obj_t *screen_ebook_label_title;
        lv_obj_t *screen_ebook_label_text;
        lv_obj_t *screen_ebook_label_page;
        lv_obj_t *screen_ebook_bar_progress;
        lv_obj_t *screen_ebook_ddlist_book;
        lv_obj_t *screen_ebook_btn_prev;     /* 上一页（LVGL 内置图标） */
        lv_obj_t *screen_ebook_btn_next;     /* 下一页（LVGL 内置图标） */
        lv_obj_t *screen_look;
        bool screen_look_del;
        lv_obj_t *screen_look_bar_temp;
        lv_obj_t *screen_look_bar_hum;
        lv_obj_t *screen_look_bar_lig;
        lv_obj_t *screen_look_bar_gear;
        lv_obj_t *screen_look_bar_angle;
        lv_obj_t *screen_look_label_temp;
        lv_obj_t *screen_look_label_hum;
        lv_obj_t *screen_look_label_lig;
        lv_obj_t *screen_look_label_angle;
        lv_obj_t *screen_look_label_gear;
        lv_obj_t *screen_look_label_angle_vol;
        lv_obj_t *screen_look_label_gear_vol;
        lv_obj_t *screen_look_label_lig_vol;
        lv_obj_t *screen_look_label_hum_vol;
        lv_obj_t *screen_look_label_temp_vol;
        lv_obj_t *screen_look_label_mpu;
        lv_obj_t *screen_look_label_mpu_z;
        lv_obj_t *screen_look_label_mpu_x;
        lv_obj_t *screen_look_label_mpu_y;
        lv_obj_t *screen_look_led1_dot;
        lv_obj_t *screen_look_led2_dot;
        lv_obj_t *screen_look_led3_dot;
        lv_obj_t *screen_look_relay_dot;
        lv_obj_t *screen_music;
        bool screen_music_del;
        lv_obj_t *screen_music_bar_music;
        lv_obj_t *screen_music_label_music_time;
        lv_obj_t *screen_music_icon_prev;   /* 上一曲 */
        lv_obj_t *screen_music_icon_play;   /* 播放/暂停 */
        lv_obj_t *screen_music_icon_play_glyph; /* 播放/暂停字形（切换用） */
        lv_obj_t *screen_music_icon_next;   /* 下一曲 */
        lv_obj_t *screen_music_icon_stop;   /* 停止播放 */
        lv_obj_t *screen_music_ddlist_song;
        lv_obj_t *screen_music_slider_vol;
        lv_obj_t *screen_music_label_vol;
        lv_obj_t *screen_music_label_lyr_5;
        lv_obj_t *screen_music_label_lyr_4;
        lv_obj_t *screen_music_label_lyr_3;
        lv_obj_t *screen_music_label_lyr_2;
        lv_obj_t *screen_music_label_lyr_1;
        lv_obj_t *screen_pic;
        bool screen_pic_del;
        lv_obj_t *screen_pic_img_2;
        /* ---- main_2 图标的独立功能界面 ---- */
        lv_obj_t *screen_note;               /* 便签 */
        bool screen_note_del;
        lv_obj_t *screen_note_btn_back;
        lv_obj_t *screen_note_btn_clear;
        lv_obj_t *screen_note_label_title;
        lv_obj_t *screen_note_ta_text;
        lv_obj_t *screen_note_kb_input;
        lv_obj_t *screen_file;               /* 文件 */
        bool screen_file_del;
        lv_obj_t *screen_file_btn_back;
        lv_obj_t *screen_file_btn_refresh;
        lv_obj_t *screen_file_label_title;
        lv_obj_t *screen_file_list_box;
        lv_obj_t *screen_news;               /* 消息 */
        bool screen_news_del;
        lv_obj_t *screen_news_btn_back;
        lv_obj_t *screen_news_label_title;
        lv_obj_t *screen_news_list_box;
        lv_obj_t *screen_wifi;               /* WiFi 连接 */
        bool screen_wifi_del;
        lv_obj_t *screen_wifi_btn_back;
        lv_obj_t *screen_wifi_btn_scan;
        lv_obj_t *screen_wifi_label_title;
        lv_obj_t *screen_wifi_dd_ap;
        lv_obj_t *screen_wifi_label_status;
        lv_obj_t *screen_wifi_label_pwd;
        lv_obj_t *screen_wifi_roller_num;
        lv_obj_t *screen_wifi_roller_low;
        lv_obj_t *screen_wifi_roller_up;
        lv_obj_t *screen_wifi_btn_del;
        lv_obj_t *screen_wifi_btn_input;
        lv_obj_t *screen_wifi_btn_ok;
        lv_obj_t *screen_wifi_label_toast;   /* 连接中/连接成功 浮动提示 */
    } lv_ui;

    typedef void (*ui_setup_scr_t)(lv_ui *ui);

    void ui_init_style(lv_style_t *style);

    void ui_load_scr_animation(lv_ui *ui, lv_obj_t **new_scr, bool new_scr_del, bool *old_scr_del, ui_setup_scr_t setup_scr,
                               lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

    void ui_animation(void *var, uint32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                      uint32_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                      lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_completed_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);

    void init_scr_del_flag(lv_ui *ui);

    void setup_bottom_layer(void);

    void setup_ui(lv_ui *ui);

    void video_play(lv_ui *ui);

    void init_keyboard(lv_ui *ui);

    extern lv_ui guider_ui;

    void setup_scr_screen_main_1(lv_ui *ui);
    void setup_scr_screen_main_2(lv_ui *ui);
    void setup_scr_screen_app(lv_ui *ui);
    void setup_scr_screen_cal(lv_ui *ui);
    void setup_scr_screen_clk(lv_ui *ui);
    void setup_scr_screen_ebook(lv_ui *ui);
    void setup_scr_screen_look(lv_ui *ui);
    void setup_scr_screen_music(lv_ui *ui);
    void setup_scr_screen_pic(lv_ui *ui);
    void setup_scr_screen_note(lv_ui *ui);
    void setup_scr_screen_file(lv_ui *ui);
    void setup_scr_screen_news(lv_ui *ui);
    void setup_scr_screen_wifi(lv_ui *ui);

/* ================= 图片来源选择 =================
 *   LV_PIC_SRC_C_ARRAY    : 编译进 MCU 的图像数组 (原 guider 默认)
 *   LV_PIC_SRC_SD_BMP     : SD 卡上的 bmp 文件
 *   LV_PIC_SRC_FLASH_BIN  : W25Q64 中 mergeBinFile.bin 里的图片 (经 'M' 驱动读取)
 *   改下面一行即可整体切换，界面代码完全不用动 */
#define LV_PIC_SRC_C_ARRAY    0
#define LV_PIC_SRC_SD_BMP     1
#define LV_PIC_SRC_FLASH_BIN  2

#define LV_PIC_SRC  LV_PIC_SRC_C_ARRAY

/* 图像数组声明 (C 数组模式使用; flash bin 模式下界面图片全部走 W25Q64) */
LV_IMAGE_DECLARE(_back_240280_RGB565A8_240x280);
LV_IMAGE_DECLARE(_app_70_RGB565A8_70x70);
LV_IMAGE_DECLARE(_calc_70_RGB565A8_70x70);
LV_IMAGE_DECLARE(_clock_70_RGB565A8_70x70);
LV_IMAGE_DECLARE(_music_70_RGB565A8_70x70);
LV_IMAGE_DECLARE(_ebook_70_RGB565A8_70x70);
LV_IMAGE_DECLARE(_look_RGB565A8_70x70);
LV_IMAGE_DECLARE(_notepad_70_RGB565A8_70x70);
LV_IMAGE_DECLARE(_paint_70_RGB565A8_70x70);
LV_IMAGE_DECLARE(_picture_70_RGB565A8_70x70);
LV_IMAGE_DECLARE(_set_70_RGB565A8_70x70);
LV_IMAGE_DECLARE(_sms_70_RGB565A8_70x70);
LV_IMAGE_DECLARE(_PrevP_RGB565A8_60x40);
LV_IMAGE_DECLARE(_PauseP_RGB565A8_80x40);
LV_IMAGE_DECLARE(_PlayP_RGB565A8_80x40);
LV_IMAGE_DECLARE(_NextP_RGB565A8_60x40);

#if (LV_PIC_SRC == LV_PIC_SRC_SD_BMP)

#define PIC_BACK_240280 "0:/ICOS/app/back_240280.bmp"
#define PIC_APP_70 "0:/ICOS/app/app_70.bmp"
#define PIC_CALC_70 "0:/ICOS/app/calc_70.bmp"
#define PIC_CLOCK_70 "0:/ICOS/app/clock_70.bmp"
#define PIC_MUSIC_70 "0:/ICOS/app/music_70.bmp"
#define PIC_EBOOK_70 "0:/ICOS/app/ebook_70.bmp"
#define PIC_LOOK_70 "0:/ICOS/app/game_70.bmp"
#define PIC_NOTEPAD_70 "0:/ICOS/app/notepad_70.bmp"
#define PIC_PAINT_70 "0:/ICOS/app/paint_70.bmp"
#define PIC_PICTURE_70 "0:/ICOS/app/picture_70.bmp"
#define PIC_SET_70 "0:/ICOS/app/set_70.bmp"
#define PIC_SMS_70 "0:/ICOS/app/sms_70.bmp"
#define PIC_PHO_IMG1 "0:/ICOS/app/back_240280.bmp"   /* 无图片时的兜底：系统背景图 */
#define PIC_MUSIC_PREV "0:/ICOS/audio/PrevP.bmp"
#define PIC_MUSIC_PAUSE "0:/ICOS/audio/PauseP.bmp"
#define PIC_MUSIC_PLAY "0:/ICOS/audio/PlayP.bmp"
#define PIC_MUSIC_NEXT "0:/ICOS/audio/NextP.bmp"

#elif (LV_PIC_SRC == LV_PIC_SRC_FLASH_BIN)

/* 名字与 guider 生成的 mergeBinFile.c 文件表一一对应, 由 app/lv_merge_decoder.c 提供 'M' 驱动 */
#define PIC_BACK_240280 "M:/back_240280.bin"
#define PIC_APP_70 "M:/app_70.bin"
#define PIC_CALC_70 "M:/calc_70.bin"
#define PIC_CLOCK_70 "M:/clock_70.bin"
#define PIC_MUSIC_70 "M:/music_70.bin"
#define PIC_EBOOK_70 "M:/ebook_70.bin"
#define PIC_LOOK_70 "M:/look.bin"
#define PIC_NOTEPAD_70 "M:/notepad_70.bin"
#define PIC_PAINT_70 "M:/paint_70.bin"
#define PIC_PICTURE_70 "M:/picture_70.bin"
#define PIC_SET_70 "M:/set_70.bin"
#define PIC_SMS_70 "M:/sms_70.bin"
#define PIC_MUSIC_PREV "M:/PrevP.bin"
#define PIC_MUSIC_PAUSE "M:/PauseP.bin"
#define PIC_MUSIC_NEXT "M:/NextP.bin"
#define PIC_PHO_IMG1 PIC_BACK_240280                    /* 无图片时的兜底：系统背景图 */
#define PIC_MUSIC_PLAY "M:/PlayP.bin"

#else

#define PIC_BACK_240280 &_back_240280_RGB565A8_240x280
#define PIC_APP_70 &_app_70_RGB565A8_70x70
#define PIC_CALC_70 &_calc_70_RGB565A8_70x70
#define PIC_CLOCK_70 &_clock_70_RGB565A8_70x70
#define PIC_MUSIC_70 &_music_70_RGB565A8_70x70
#define PIC_EBOOK_70 &_ebook_70_RGB565A8_70x70
#define PIC_LOOK_70 &_look_RGB565A8_70x70
#define PIC_NOTEPAD_70 &_notepad_70_RGB565A8_70x70
#define PIC_PAINT_70 &_paint_70_RGB565A8_70x70
#define PIC_PICTURE_70 &_picture_70_RGB565A8_70x70
#define PIC_SET_70 &_set_70_RGB565A8_70x70
#define PIC_SMS_70 &_sms_70_RGB565A8_70x70
#define PIC_PHO_IMG1 &_back_240280_RGB565A8_240x280     /* 无图片时的兜底：系统背景图 */
#define PIC_MUSIC_PREV &_PrevP_RGB565A8_60x40
#define PIC_MUSIC_PAUSE &_PauseP_RGB565A8_80x40
#define PIC_MUSIC_PLAY &_PlayP_RGB565A8_80x40
#define PIC_MUSIC_NEXT &_NextP_RGB565A8_60x40

#endif

    LV_FONT_DECLARE(lv_font_montserratMedium_12)
    LV_FONT_DECLARE(lv_font_montserratMedium_14)

#ifdef __cplusplus
}
#endif
#endif
