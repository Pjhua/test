/*
 * Copyright 2026 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "events_init.h"
#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "widgets_init.h"
#include "led.h"
#include "beep.h"
#include "relay.h"
#include "pwm.h"
#include "w2812.h"
#include "scanf_dir.h"
#include "ebook_view.h"
#include "music_player.h"
#include "app_tasks.h" /* 测量采集任务的共享数据快照 */
#include "esp8266.h"   /* WiFi 扫描 / 连接 */
#include <string.h>
#include <math.h>

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

// 控制界面
static lv_app ctrl_state = {0, 0, 0, 0, 0, 0, 0, 1, 128, 128, 128};
static unsigned int ctrl_rgb_color = 0;
/* 语音侧 RGB 氛围灯状态（voice.h），进界面时同步显示 */
extern volatile unsigned int g_rgb_color;
extern volatile unsigned char g_rgb_effect;
/* 进界面同步期间置 1：程序化 set_selected/set_value 也会触发控件事件，
   此时不能把语音的七彩流动特效关掉 */
static unsigned char s_rgb_entry_sync = 0;
/* 控制界面(screen_app)入口来源：0=主界面1的"应用"图标
   （主界面2的"设置"图标已改为进入 WiFi 连接界面） */
static uint8_t s_app_from_main2 = 0;

// 查看界面
static lv_timer_t *s_app_timer = NULL;

/* 图片浏览界面（screen_picture）
   用 scanf_dir.c 的链表保存文件路径：mymalloc 内存池是编译期静态数组，
   ZI 占用恒定（80KB 池 + 60KB 池早已链入），运行期从池里切——
   每个节点约 272B（full_path[256] + 链表指针），80KB 池可存约 300 个文件 */
#define ALBUM_DIR "0:/bmp"
static FileNode *s_album_cur = NULL;    /* 当前显示的图片节点 */
static lv_obj_t *s_album_dialog = NULL; /* 删除确认弹窗（挂在 lv_layer_top） */
static lv_obj_t *s_album_empty = NULL;  /* 目录无图片时的提示 label */
static lv_point_t s_album_press_pt;     /* 按下点坐标 */
static int32_t s_album_press_max_d2;    /* 按下后跟踪到的最大位移平方（px²） */

/* 音乐界面顶部下拉框/歌名 label 状态；全局长按控制见文件末尾"音乐后台控制" */
static lv_obj_t *s_song_label = NULL;   /* 下拉框隐藏时，原位置显示当前歌名 */
static bool s_dd_shown = false;         /* 下拉框是否处于长按唤出的可见状态 */

/* 电子书界面顶部下拉框/书名 label：与音乐界面共用同一套长按 + 空闲收起机制 */
static lv_obj_t *s_ebook_name_label = NULL; /* 下拉框隐藏时，原位置显示当前书名 */
static uint32_t s_last_input = 0;           /* 最近一次触摸时刻（下拉框空闲判断） */

static void music_dd_setup(lv_ui *ui);
static void music_dd_toggle(void);
static void music_ctl_longpress(void);

static void screen_main_1_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch (dir)
        {
        case LV_DIR_LEFT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_2, guider_ui.screen_main_2_del, &guider_ui.screen_main_1_del, setup_scr_screen_main_2, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
            break;
        }
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_2, guider_ui.screen_main_2_del, &guider_ui.screen_main_1_del, setup_scr_screen_main_2, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_main_1_icon_app_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
    {
        s_app_from_main2 = 0; /* 从主界面1进入，上滑返回主界面1 */
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_app, guider_ui.screen_app_del, &guider_ui.screen_main_1_del, setup_scr_screen_app, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_main_1_icon_cal_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_cal, guider_ui.screen_cal_del, &guider_ui.screen_main_1_del, setup_scr_screen_cal, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_main_1_icon_clk_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_clk, guider_ui.screen_clk_del, &guider_ui.screen_main_1_del, setup_scr_screen_clk, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_main_1_icon_music_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_music, guider_ui.screen_music_del, &guider_ui.screen_main_1_del, setup_scr_screen_music, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_main_1_icon_ebook_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_ebook, guider_ui.screen_ebook_del, &guider_ui.screen_main_1_del, setup_scr_screen_ebook, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_main_1_icon_look_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_look, guider_ui.screen_look_del, &guider_ui.screen_main_1_del, setup_scr_screen_look, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_main_1(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_main_1, screen_main_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_1_icon_app, screen_main_1_icon_app_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_1_icon_cal, screen_main_1_icon_cal_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_1_icon_clk, screen_main_1_icon_clk_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_1_icon_music, screen_main_1_icon_music_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_1_icon_ebook, screen_main_1_icon_ebook_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_1_icon_look, screen_main_1_icon_look_event_handler, LV_EVENT_ALL, ui);
}

static void screen_main_2_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch (dir)
        {
        case LV_DIR_LEFT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_1, guider_ui.screen_main_1_del, &guider_ui.screen_main_2_del, setup_scr_screen_main_1, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
            break;
        }
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_1, guider_ui.screen_main_1_del, &guider_ui.screen_main_2_del, setup_scr_screen_main_1, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_main_2_icon_pic_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_pic, guider_ui.screen_pic_del, &guider_ui.screen_main_2_del, setup_scr_screen_pic, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

/* 第 6 格：首页图标按钮 → 返回主界面 1 */
static void screen_main_2_btn_home_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_1, guider_ui.screen_main_1_del, &guider_ui.screen_main_2_del, setup_scr_screen_main_1, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

/* 便签图标 → 便签界面（独立功能界面） */
static void screen_main_2_icon_note_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_note, guider_ui.screen_note_del, &guider_ui.screen_main_2_del, setup_scr_screen_note, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
}

/* 文件图标 → 文件界面（独立功能界面） */
static void screen_main_2_icon_file_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_file, guider_ui.screen_file_del, &guider_ui.screen_main_2_del, setup_scr_screen_file, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
}

/* 设置图标 → WiFi 连接界面 */
static void screen_main_2_icon_set_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_wifi, guider_ui.screen_wifi_del, &guider_ui.screen_main_2_del, setup_scr_screen_wifi, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
}

/* 消息图标 → 消息界面（独立功能界面） */
static void screen_main_2_icon_news_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_news, guider_ui.screen_news_del, &guider_ui.screen_main_2_del, setup_scr_screen_news, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
}

void events_init_screen_main_2(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_main_2, screen_main_2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_2_icon_note, screen_main_2_icon_note_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_2_icon_file, screen_main_2_icon_file_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_2_icon_pic, screen_main_2_icon_pic_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_2_icon_set, screen_main_2_icon_set_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_2_icon_news, screen_main_2_icon_news_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_2_btn_home, screen_main_2_btn_home_event_handler, LV_EVENT_ALL, ui);
}

/* 应用界面 500ms 轮询：语音/云端改了硬件后，停留在界面上的控件跟着刷新 */
static lv_timer_t *s_appsync_timer = NULL;
static void screen_app_rgb_update(void); /* 定义在本文件后面 */

static void app_sync_timer_cb(lv_timer_t *t)
{
    lv_ui *ui = (lv_ui *)lv_timer_get_user_data(t);
    u8 hw, ui_v;

    /* ---- 开关类：LED 低电平点亮 ---- */
    hw = (LED1 == 0);
    ui_v = lv_obj_has_state(ui->screen_app_sw_led1, LV_STATE_CHECKED) ? 1 : 0;
    if (hw != ui_v)
    {
        if (hw) lv_obj_add_state(ui->screen_app_sw_led1, LV_STATE_CHECKED);
        else    lv_obj_clear_state(ui->screen_app_sw_led1, LV_STATE_CHECKED);
    }
    hw = (LED2 == 0);
    ui_v = lv_obj_has_state(ui->screen_app_sw_led2, LV_STATE_CHECKED) ? 1 : 0;
    if (hw != ui_v)
    {
        if (hw) lv_obj_add_state(ui->screen_app_sw_led2, LV_STATE_CHECKED);
        else    lv_obj_clear_state(ui->screen_app_sw_led2, LV_STATE_CHECKED);
    }
    hw = (LED3 == 0);
    ui_v = lv_obj_has_state(ui->screen_app_sw_led3, LV_STATE_CHECKED) ? 1 : 0;
    if (hw != ui_v)
    {
        if (hw) lv_obj_add_state(ui->screen_app_sw_led3, LV_STATE_CHECKED);
        else    lv_obj_clear_state(ui->screen_app_sw_led3, LV_STATE_CHECKED);
    }
    hw = (BEEP != 0);
    ui_v = lv_obj_has_state(ui->screen_app_sw_beep, LV_STATE_CHECKED) ? 1 : 0;
    if (hw != ui_v)
    {
        if (hw) lv_obj_add_state(ui->screen_app_sw_beep, LV_STATE_CHECKED);
        else    lv_obj_clear_state(ui->screen_app_sw_beep, LV_STATE_CHECKED);
    }
    hw = (RELAY != 0);
    ui_v = lv_obj_has_state(ui->screen_app_sw_relay, LV_STATE_CHECKED) ? 1 : 0;
    if (hw != ui_v)
    {
        if (hw) lv_obj_add_state(ui->screen_app_sw_relay, LV_STATE_CHECKED);
        else    lv_obj_clear_state(ui->screen_app_sw_relay, LV_STATE_CHECKED);
    }

    /* ---- 风扇 / 舵机滚轮 ---- */
    hw = fan_get_gear();
    if (hw != lv_roller_get_selected(ui->screen_app_roller_fan))
        lv_roller_set_selected(ui->screen_app_roller_fan, hw, LV_ANIM_OFF);

    hw = sg90_get_angle() / 45;
    if (hw != lv_roller_get_selected(ui->screen_app_roller_sg90))
        lv_roller_set_selected(ui->screen_app_roller_sg90, hw, LV_ANIM_OFF);

    /* ---- RGB：七彩流动中颜色一直在变，不覆盖控件；静止时跟语音同步 ---- */
    if (g_rgb_effect == 0)
    {
        int cnt = (g_rgb_color != 0) ? Pixel_NUM : 0;

        if (cnt != (int)lv_roller_get_selected(ui->screen_app_roller_RGB_NUM))
            lv_roller_set_selected(ui->screen_app_roller_RGB_NUM, (unsigned)cnt, LV_ANIM_OFF);

        if (!lv_slider_is_dragged(ui->screen_app_slider_R))
            lv_slider_set_value(ui->screen_app_slider_R, (g_rgb_color >> 16) & 0xFF, LV_ANIM_OFF);
        if (!lv_slider_is_dragged(ui->screen_app_slider_G))
            lv_slider_set_value(ui->screen_app_slider_G, (g_rgb_color >> 8) & 0xFF, LV_ANIM_OFF);
        if (!lv_slider_is_dragged(ui->screen_app_slider_B))
            lv_slider_set_value(ui->screen_app_slider_B, (g_rgb_color & 0xFF), LV_ANIM_OFF);

        ctrl_rgb_color   = g_rgb_color;
        ctrl_state.red   = (unsigned char)((g_rgb_color >> 16) & 0xFF);
        ctrl_state.green = (unsigned char)((g_rgb_color >> 8) & 0xFF);
        ctrl_state.blue  = (unsigned char)(g_rgb_color & 0xFF);
        screen_app_rgb_update(); /* 刷新 R/G/B 数值标签 + 预览色块 */
    }
}

static void screen_app_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_SCREEN_LOADED:
    {
        if (s_appsync_timer == NULL)
            s_appsync_timer = lv_timer_create(app_sync_timer_cb, 500, &guider_ui);
        break;
    }
    case LV_EVENT_SCREEN_UNLOADED:
    {
        if (s_appsync_timer != NULL)
        {
            lv_timer_del(s_appsync_timer);
            s_appsync_timer = NULL;
        }
        break;
    }
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch (dir)
        {
        case LV_DIR_TOP: /* 上滑 → 回到进入时的主页面（图标所在页） */
        {
            lv_indev_wait_release(lv_indev_active());
            if (s_app_from_main2)
                ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_2, guider_ui.screen_main_2_del, &guider_ui.screen_app_del, setup_scr_screen_main_2, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
            else
                ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_1, guider_ui.screen_main_1_del, &guider_ui.screen_app_del, setup_scr_screen_main_1, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }

    default:
        break;
    }
}

static void screen_app_roller_fan_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    u8 i;
    switch (code)
    {
    case LV_EVENT_VALUE_CHANGED:
    {
        i = lv_roller_get_selected(obj);
        ctrl_state.fan = i;
        fan_set_gear(i);
        break;
    }
    default:
        break;
    }
}

static void screen_app_sw_led3_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t *status_obj = lv_event_get_target(e);
        int status = lv_obj_has_state(status_obj, LV_STATE_CHECKED) ? true : false;
        ctrl_state.led3 = status;
        LED3 = !status;
        break;
    }
    default:
        break;
    }
}

static void screen_app_sw_led2_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t *status_obj = lv_event_get_target(e);
        int status = lv_obj_has_state(status_obj, LV_STATE_CHECKED) ? true : false;
        ctrl_state.led2 = status;
        LED2 = !status;
        break;
    }
    default:
        break;
    }
}

static void screen_app_sw_led1_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t *status_obj = lv_event_get_target(e);
        int status = lv_obj_has_state(status_obj, LV_STATE_CHECKED) ? true : false;
        ctrl_state.led1 = status;
        LED1 = !status;
        break;
    }
    default:
        break;
    }
}

static void screen_app_sw_beep_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t *status_obj = lv_event_get_target(e);
        int status = lv_obj_has_state(status_obj, LV_STATE_CHECKED) ? true : false;
        ctrl_state.beep = status;
        BEEP = status;
        break;
    }
    default:
        break;
    }
}

static void screen_app_sw_relay_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t *status_obj = lv_event_get_target(e);
        int status = lv_obj_has_state(status_obj, LV_STATE_CHECKED) ? true : false;
        ctrl_state.relay = status;
        RELAY = status;
        break;
    }
    default:
        break;
    }
}

static void screen_app_roller_sg90_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    switch (code)
    {
    case LV_EVENT_VALUE_CHANGED:
    {
        u8 i = lv_roller_get_selected(obj);
        ctrl_state.sg90 = i;
        printf("sg90:%d\r\n", ctrl_state.sg90);
        sg90_set_angle(i * 45);
        break;
    }
    default:
        break;
    }
}

/* 软件模拟WS2812时序发送一帧：前 rgb_count 个灯珠显示 ctrl_rgb_color，其余熄灭 */
static void screen_ctrl_rgb_apply(void)
{
    unsigned char i;

    __disable_irq(); /* 关中断，防止LVGL心跳等中断打断WS2812时序 */
    w2812_reset();
    for (i = 0; i < Pixel_NUM; i++)
    {
        if (i < ctrl_state.rgb_count)
            w2812_send_color(ctrl_rgb_color);
        else
            w2812_send_color(RGB_BLACK);
    }
    __enable_irq();
}

void screen_app_rgb_update(void)
{
    int r = lv_slider_get_value(guider_ui.screen_app_slider_R);
    int g = lv_slider_get_value(guider_ui.screen_app_slider_G);
    int b = lv_slider_get_value(guider_ui.screen_app_slider_B);

    lv_obj_set_style_bg_color(guider_ui.screen_app_label_RGB_color, lv_color_make((u8)r, (u8)g, (u8)b), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_label_set_text_fmt(guider_ui.screen_app_label_RGB_R, "R:%d", r);
    lv_label_set_text_fmt(guider_ui.screen_app_label_RGB_G, "G:%d", g);
    lv_label_set_text_fmt(guider_ui.screen_app_label_RGB_B, "B:%d", b);

    ctrl_rgb_color = ((unsigned int)r << 16) | ((unsigned int)g << 8) | (unsigned int)b;
    ctrl_state.red = (unsigned char)r;
    ctrl_state.green = (unsigned char)g;
    ctrl_state.blue = (unsigned char)b;

    screen_ctrl_rgb_apply();
}

static void screen_app_slider_R_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_RELEASED:
    {
        if (!s_rgb_entry_sync)
            g_rgb_effect = 0; /* 手动调色接管，停止语音七彩流动 */
        screen_app_rgb_update();
        break;
    }
    default:
        break;
    }
}

static void screen_app_slider_G_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_RELEASED:
    {
        if (!s_rgb_entry_sync)
            g_rgb_effect = 0; /* 手动调色接管，停止语音七彩流动 */
        screen_app_rgb_update();
        break;
    }
    default:
        break;
    }
}

static void screen_app_slider_B_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_RELEASED:
    {
        if (!s_rgb_entry_sync)
            g_rgb_effect = 0; /* 手动调色接管，停止语音七彩流动 */
        screen_app_rgb_update();
        break;
    }
    default:
        break;
    }
}

static void screen_app_roller_RGB_NUM_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    switch (code)
    {
    case LV_EVENT_VALUE_CHANGED:
    {
        if (!s_rgb_entry_sync)
            g_rgb_effect = 0; /* 手动改灯珠数量接管，停止语音七彩流动 */
        ctrl_state.rgb_count = lv_roller_get_selected(obj);
        screen_app_rgb_update();
        break;
    }
    default:
        break;
    }
}

void events_init_screen_app(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_app, screen_app_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_app_roller_fan, screen_app_roller_fan_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_app_sw_led3, screen_app_sw_led3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_app_sw_led2, screen_app_sw_led2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_app_sw_led1, screen_app_sw_led1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_app_sw_beep, screen_app_sw_beep_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_app_sw_relay, screen_app_sw_relay_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_app_roller_sg90, screen_app_roller_sg90_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_app_slider_R, screen_app_slider_R_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_app_slider_G, screen_app_slider_G_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_app_slider_B, screen_app_slider_B_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_app_roller_RGB_NUM, screen_app_roller_RGB_NUM_event_handler, LV_EVENT_ALL, ui);

    /* 进入界面：以硬件当前真实状态为准（语音/云端可能刚操作过），
       否则界面会把语音开着的灯/风扇"还原"成关 */
    ctrl_state.fan   = fan_get_gear();
    ctrl_state.led1  = (LED1 == 0);  /* LED 低电平点亮 */
    ctrl_state.led2  = (LED2 == 0);
    ctrl_state.led3  = (LED3 == 0);
    ctrl_state.beep  = (BEEP != 0);
    ctrl_state.relay = (RELAY != 0);
    ctrl_state.sg90  = sg90_get_angle() / 45; /* 角度反算滚轮档位(0~4) */

    /* RGB 氛围灯：以当前实际颜色同步滑块/数量/预览
       （七彩流动时 g_rgb_color 由特效实时更新，显示的就是当前那一格的颜色） */
    ctrl_rgb_color       = g_rgb_color;
    ctrl_state.rgb_count = (g_rgb_color != 0) ? Pixel_NUM : 0;
    ctrl_state.red       = (unsigned char)((g_rgb_color >> 16) & 0xFF);
    ctrl_state.green     = (unsigned char)((g_rgb_color >> 8) & 0xFF);
    ctrl_state.blue      = (unsigned char)(g_rgb_color & 0xFF);
    s_rgb_entry_sync = 1; /* 下面程序化设置控件触发的事件不要停特效 */

    if (ctrl_state.led1)
        lv_obj_add_state(ui->screen_app_sw_led1, LV_STATE_CHECKED);
    if (ctrl_state.led2)
        lv_obj_add_state(ui->screen_app_sw_led2, LV_STATE_CHECKED);
    if (ctrl_state.led3)
        lv_obj_add_state(ui->screen_app_sw_led3, LV_STATE_CHECKED);
    if (ctrl_state.beep)
        lv_obj_add_state(ui->screen_app_sw_beep, LV_STATE_CHECKED);
    if (ctrl_state.relay)
        lv_obj_add_state(ui->screen_app_sw_relay, LV_STATE_CHECKED);
    lv_roller_set_selected(ui->screen_app_roller_fan, ctrl_state.fan, LV_ANIM_OFF);
    lv_roller_set_selected(ui->screen_app_roller_sg90, ctrl_state.sg90, LV_ANIM_OFF);
    lv_roller_set_selected(ui->screen_app_roller_RGB_NUM, ctrl_state.rgb_count, LV_ANIM_OFF);
    lv_slider_set_value(ui->screen_app_slider_R, ctrl_state.red, LV_ANIM_OFF);
    lv_slider_set_value(ui->screen_app_slider_G, ctrl_state.green, LV_ANIM_OFF);
    lv_slider_set_value(ui->screen_app_slider_B, ctrl_state.blue, LV_ANIM_OFF);

    /* 同步硬件（上面设置控件可能已触发事件，这里再执行一次保证一致） */
    LED1 = !ctrl_state.led1;
    LED2 = !ctrl_state.led2;
    LED3 = !ctrl_state.led3;
    BEEP = ctrl_state.beep;
    RELAY = ctrl_state.relay;

    fan_set_gear((ctrl_state.fan));
    sg90_set_angle(ctrl_state.sg90 * 45);
    screen_app_rgb_update(); /* 恢复R/G/B标签、预览颜色并刷新RGB灯 */
    s_rgb_entry_sync = 0;    /* 同步结束，用户之后手动操作才接管特效 */
}

/* ================= 计算器逻辑 ================= */
static char  calc_input[32] = "0";   /* 当前输入的数字字符串 */
static double calc_prev = 0;          /* 上一个操作数 */
static int   calc_op = 0;             /* 运算符: 0无 1加 2减 3乘 4除 */
static int   calc_reset = 0;          /* 下次输入数字时是否清空 */

/* 根据运算符计算结果 */
static double calc_do_op(double a, double b, int op)
{
    switch (op)
    {
    case 1: return a + b;
    case 2: return a - b;
    case 3: return a * b;
    case 4: return (b != 0) ? a / b : 0;
    default: return b;
    }
}

/* 把数字格式化到 input 字符串（去掉多余的 0 和小数点） */
static void calc_format_result(double val)
{
    if (val == (long long)val && val > -1e15 && val < 1e15)
    {
        snprintf(calc_input, sizeof(calc_input), "%lld", (long long)val);
    }
    else
    {
        snprintf(calc_input, sizeof(calc_input), "%.8g", val);
    }
    if (strlen(calc_input) >= sizeof(calc_input))
        calc_input[sizeof(calc_input) - 1] = '\0';
}

static void screen_cal_btnm_handler(lv_event_t *e)
{
    lv_obj_t *btnm = lv_event_get_target(e);
    uint32_t btn_id = lv_buttonmatrix_get_selected_button(btnm);
    const char *txt = lv_buttonmatrix_get_button_text(btnm, btn_id);
    if (!txt) return;

    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);
    char c = txt[0];

    /* 图标键优先判断（LV_SYMBOL_* 为多字节 UTF-8，首字节非 ASCII） */
    if (strcmp(txt, LV_SYMBOL_BACKSPACE) == 0)
    {
        if (!calc_reset)
        {
            size_t len = strlen(calc_input);
            if (len > 1)
                calc_input[len - 1] = '\0';
            else
                strcpy(calc_input, "0");
        }
        lv_label_set_text(ui->screen_cal_label_1, calc_input);
        return;
    }
    if (strcmp(txt, LV_SYMBOL_CLOSE) == 0)
    {
        /* × 乘号 */
        double cur = atof(calc_input);
        if (calc_op != 0)
        {
            double res = calc_do_op(calc_prev, cur, calc_op);
            calc_format_result(res);
            calc_prev = res;
        }
        else
        {
            calc_prev = cur;
        }
        calc_op = 3;
        calc_reset = 1;
        lv_label_set_text(ui->screen_cal_label_1, calc_input);
        return;
    }

    if (c >= '0' && c <= '9')
    {
        if (calc_reset)
        {
            calc_input[0] = c;
            calc_input[1] = '\0';
            calc_reset = 0;
        }
        else
        {
            size_t len = strlen(calc_input);
            if (len < sizeof(calc_input) - 1)
            {
                /* 避免前导 0，如 "0" -> "5" 而非 "05" */
                if (strcmp(calc_input, "0") == 0)
                    calc_input[0] = c;
                else
                {
                    calc_input[len] = c;
                    calc_input[len + 1] = '\0';
                }
            }
        }
    }
    else if (c == '.')
    {
        if (calc_reset)
        {
            strcpy(calc_input, "0.");
            calc_reset = 0;
        }
        else if (strchr(calc_input, '.') == NULL)
        {
            size_t len = strlen(calc_input);
            if (len < sizeof(calc_input) - 1)
            {
                calc_input[len] = '.';
                calc_input[len + 1] = '\0';
            }
        }
    }
    else if (c == 'C')
    {
        strcpy(calc_input, "0");
        calc_prev = 0;
        calc_op = 0;
        calc_reset = 0;
    }
    else if (c == 'N')
    {
        if (strcmp(calc_input, "0") != 0)
        {
            if (calc_input[0] == '-')
                memmove(calc_input, calc_input + 1, strlen(calc_input));
            else
            {
                size_t len = strlen(calc_input);
                if (len < sizeof(calc_input) - 1)
                {
                    memmove(calc_input + 1, calc_input, len + 1);
                    calc_input[0] = '-';
                }
            }
        }
    }
    else if (c == '%')
    {
        double val = atof(calc_input) / 100.0;
        calc_format_result(val);
        calc_reset = 1;
    }
    else if (c == '+' || c == '-' || c == 'x' || c == '/')
    {
        double cur = atof(calc_input);
        if (calc_op != 0)
        {
            double res = calc_do_op(calc_prev, cur, calc_op);
            calc_format_result(res);
            calc_prev = res;
        }
        else
        {
            calc_prev = cur;
        }
        calc_op = (c == '+') ? 1 : (c == '-') ? 2 : (c == 'x') ? 3 : 4;
        calc_reset = 1;
    }
    else if (c == '=')
    {
        if (calc_op != 0)
        {
            double cur = atof(calc_input);
            double res = calc_do_op(calc_prev, cur, calc_op);
            calc_format_result(res);
            calc_prev = 0;
            calc_op = 0;
            calc_reset = 1;
        }
    }

    lv_label_set_text(ui->screen_cal_label_1, calc_input);
}

static void screen_cal_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch (dir)
        {
        case LV_DIR_TOP:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_1, guider_ui.screen_main_1_del, &guider_ui.screen_cal_del, setup_scr_screen_main_1, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_screen_cal(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_cal, screen_cal_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_cal_btnm_1, screen_cal_btnm_handler, LV_EVENT_VALUE_CHANGED, ui);
}

/* ==================== 时钟界面（screen_clk） ==================== */
static lv_timer_t *s_clk_timer_analog = NULL;
static lv_timer_t *s_clk_timer_digital = NULL;

static void screen_clk_btn_back_event_handler(lv_event_t *e)
{
    lv_indev_wait_release(lv_indev_active());
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_1, guider_ui.screen_main_1_del,
                          &guider_ui.screen_clk_del, setup_scr_screen_main_1,
                          LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
}

static void screen_clk_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_SCREEN_LOADED:
    {
        /* 先手动刷一次，避免进来后白等 1 秒才显示正确时间 */
        screen_clk_digital_clock_1_timer(NULL);
        screen_clk_analog_clock_1_timer(NULL);
        /* 创建定时器：只在本屏幕存活期间运行 */
        if (s_clk_timer_analog == NULL)
            s_clk_timer_analog = lv_timer_create(screen_clk_analog_clock_1_timer, 1000, NULL);
        if (s_clk_timer_digital == NULL)
            s_clk_timer_digital = lv_timer_create(screen_clk_digital_clock_1_timer, 1000, NULL);
        break;
    }
    case LV_EVENT_SCREEN_UNLOADED:
    {
        if (s_clk_timer_analog) { lv_timer_delete(s_clk_timer_analog);   s_clk_timer_analog = NULL; }
        if (s_clk_timer_digital){ lv_timer_delete(s_clk_timer_digital);  s_clk_timer_digital = NULL; }
        break;
    }
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        if (dir == LV_DIR_TOP)
            screen_clk_btn_back_event_handler(e); /* 上滑 = 返回 */
        break;
    }
    default:
        break;
    }
}

void events_init_screen_clk(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_clk, screen_clk_event_handler, LV_EVENT_ALL, ui);
    if (ui->screen_clk_btn_back != NULL)
        lv_obj_add_event_cb(ui->screen_clk_btn_back, screen_clk_btn_back_event_handler, LV_EVENT_CLICKED, ui);
}

/* ==================== 电子书界面（screen_ebook） ====================
   手势：左滑下一页 / 右滑上一页 / 上滑退出 / 下滑下一本（最后一本回绕到第一本）
   顶部：平常只显示书名，长按 5s 唤出下拉框选书（再长按或 15s 无操作收起） */

#define EBOOK_DD_NAME_MAX 59
/* 32 本 × 每本 59 字节 + 换行 + NUL；选项由 lv_dropdown_set_options 复制保存 */
static char s_ebook_dd_opts[EBK_MAX_BOOKS * (EBOOK_DD_NAME_MAX + 1) + 1];

/* 拷贝书名：去掉 .txt 后缀，超长时按 UTF-8 字符边界截断，避免把汉字切成半个 */
static size_t ebook_dd_name_copy(char *dst, const char *src, size_t cap)
{
    size_t full = strlen(src);
    size_t n;

    if (full > 4 &&
        src[full - 4] == '.' &&
        (src[full - 3] == 't' || src[full - 3] == 'T') &&
        (src[full - 2] == 'x' || src[full - 2] == 'X') &&
        (src[full - 1] == 't' || src[full - 1] == 'T'))
        full -= 4;

    n = (full < cap) ? full : cap;
    while (n > 0 && (((unsigned char)src[n]) & 0xC0) == 0x80)
        n--;
    memcpy(dst, src, n);
    dst[n] = '\0';
    return n;
}

/* 扫描完目录后把书名拼成 "书名1\n书名2\n..." 交给下拉框 */
static void ebook_dd_build_options(lv_ui *ui)
{
    uint16_t i;
    size_t len = 0;

    if (ebk_book_count() == 0)
    {
        lv_dropdown_set_options(ui->screen_ebook_ddlist_book, "无书籍");
        return;
    }

    s_ebook_dd_opts[0] = '\0';
    for (i = 0; i < ebk_book_count(); i++)
    {
        if (i > 0)
            s_ebook_dd_opts[len++] = '\n';
        len += ebook_dd_name_copy(&s_ebook_dd_opts[len], ebk_book_title(i), EBOOK_DD_NAME_MAX);
    }
    lv_dropdown_set_options(ui->screen_ebook_ddlist_book, s_ebook_dd_opts);
}

/* 顶部书名 label：下拉框隐藏时显示当前书名（不带 .txt） */
static void ebook_name_update(void)
{
    char name[EBOOK_DD_NAME_MAX + 1];

    if (s_ebook_name_label == NULL || !lv_obj_is_valid(s_ebook_name_label))
        return;

    if (ebk_book_count() == 0)
    {
        lv_label_set_text(s_ebook_name_label, "无书籍");
        return;
    }

    ebook_dd_name_copy(name, ebk_book_name(), sizeof(name));
    lv_label_set_text(s_ebook_name_label, name);
}

/* 进界面：下拉框默认隐藏，原位置放一个同尺寸的书名 label */
static void ebook_dd_setup(lv_ui *ui)
{
    s_dd_shown = false;
    lv_obj_add_flag(ui->screen_ebook_ddlist_book, LV_OBJ_FLAG_HIDDEN);

    if (s_ebook_name_label != NULL && lv_obj_is_valid(s_ebook_name_label))
        lv_obj_delete(s_ebook_name_label); /* 防止重复 LOADED 时叠出两个书名 */

    s_ebook_name_label = lv_label_create(ui->screen_ebook); /* 随界面一起销毁 */
    lv_obj_set_pos(s_ebook_name_label, 6, 4);
    lv_obj_set_size(s_ebook_name_label, 178, 24);
    lv_label_set_long_mode(s_ebook_name_label, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_ebook_name_label, "无书籍");
    lv_obj_set_style_text_font(s_ebook_name_label, &myFont_simkai16, 0);
    lv_obj_set_style_text_color(s_ebook_name_label, lv_color_hex(0x0D3055), 0);
    lv_obj_set_style_pad_top(s_ebook_name_label, 4, 0);
    lv_obj_set_style_pad_left(s_ebook_name_label, 8, 0);
}

/* 长按唤出/收起下拉框（和音乐界面一致；空闲 15s 由后台定时器自动收起） */
static void ebook_dd_toggle(void)
{
    if (!lv_obj_is_valid(guider_ui.screen_ebook_ddlist_book))
        return;

    if (s_dd_shown)
    {
        lv_dropdown_close(guider_ui.screen_ebook_ddlist_book);
        lv_obj_add_flag(guider_ui.screen_ebook_ddlist_book, LV_OBJ_FLAG_HIDDEN);
        if (s_ebook_name_label != NULL && lv_obj_is_valid(s_ebook_name_label))
            lv_obj_remove_flag(s_ebook_name_label, LV_OBJ_FLAG_HIDDEN);
        s_dd_shown = false;
    }
    else
    {
        if (s_ebook_name_label != NULL && lv_obj_is_valid(s_ebook_name_label))
            lv_obj_add_flag(s_ebook_name_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(guider_ui.screen_ebook_ddlist_book, LV_OBJ_FLAG_HIDDEN);
        s_dd_shown = true;
        s_last_input = lv_tick_get();
    }
}

/* 下拉框选择某本书：打开并刷新 */
static void ebook_refresh_ui(lv_ui *ui); /* 定义在下面 */

static void screen_ebook_book_cb(lv_event_t *e)
{
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);
    uint32_t idx;

    if (ebk_book_count() == 0)
        return;

    idx = lv_dropdown_get_selected(lv_event_get_target(e));
    if (idx >= ebk_book_count() || (uint16_t)idx == ebk_book_index())
        return;

    ebk_open_book((uint16_t)idx);
    ebook_refresh_ui(ui);
}

/* 首/末页把翻页按钮置灰，按下没有反应时一眼能看出是到头了 */
static void ebook_btn_enable(lv_obj_t *btn, uint8_t on)
{
    if (on)
        lv_obj_remove_state(btn, LV_STATE_DISABLED);
    else
        lv_obj_add_state(btn, LV_STATE_DISABLED);
}

/* 刷新书名、正文、页码、进度条 */
static void ebook_refresh_ui(lv_ui *ui)
{
    uint16_t page = ebk_page_index();
    uint16_t total = ebk_page_count();
    uint8_t percent = ebk_progress();
    char page_text[40];
    const char *body;

    ebook_name_update();

    if (ebk_book_count() == 0)
    {
        lv_label_set_text(ui->screen_ebook_label_title, "0/0");
        lv_label_set_text(ui->screen_ebook_label_text, "未找到电子书\n请把 txt 文件放入\n0:/ebook 目录");
        lv_label_set_text(ui->screen_ebook_label_page, "第0/0页 0%");
        lv_bar_set_value(ui->screen_ebook_bar_progress, 0, LV_ANIM_OFF);
        ebook_btn_enable(ui->screen_ebook_btn_prev, 0);
        ebook_btn_enable(ui->screen_ebook_btn_next, 0);
        return;
    }

    /* 右上角：第几本 / 共几本（当前书名由顶部书名 label 显示） */
    lv_dropdown_set_selected(ui->screen_ebook_ddlist_book, ebk_book_index());
    lv_label_set_text_fmt(ui->screen_ebook_label_title, "%d/%d",
                          (int)(ebk_book_index() + 1), (int)ebk_book_count());

    /* 打开失败：正文区直接给出原因，页码/进度清零 */
    if (ebk_error() != 0)
    {
        char msg[40];

        if (ebk_error() == 2) /* f_open 失败：带出 FatFs 原始错误码，便于排查 */
            snprintf(msg, sizeof(msg), "文件打不开(%d)\n请下滑换下一本",
                     (int)ebk_fresult());
        else
            snprintf(msg, sizeof(msg), "文件编码解不出正文\n请下滑换下一本");

        lv_label_set_text(ui->screen_ebook_label_text, msg);
        lv_label_set_text(ui->screen_ebook_label_page, "第0/0页 0%");
        lv_bar_set_value(ui->screen_ebook_bar_progress, 0, LV_ANIM_OFF);
        ebook_btn_enable(ui->screen_ebook_btn_prev, 0);
        ebook_btn_enable(ui->screen_ebook_btn_next, 0);
        return;
    }

    /* 正文：分页模块已经插好换行，直接贴到 label */
    body = ebk_page_text();
    lv_label_set_text(ui->screen_ebook_label_text, (body[0] != '\0') ? body : "本页无内容");

    /* 页码 + 百分比：第几页 / 共几页 进度%（页数超上限时补一个 '+'） */
    if (ebk_truncated())
        snprintf(page_text, sizeof(page_text), "第%d/%d+页 %d%%",
                 (int)(page + 1), (int)total, (int)percent);
    else
        snprintf(page_text, sizeof(page_text), "第%d/%d页 %d%%",
                 (int)(page + 1), (int)total, (int)percent);
    lv_label_set_text(ui->screen_ebook_label_page, page_text);

    lv_bar_set_value(ui->screen_ebook_bar_progress, percent, LV_ANIM_OFF);

    /* 首页禁用“上一页”，末页禁用“下一页” */
    ebook_btn_enable(ui->screen_ebook_btn_prev, page > 0);
    ebook_btn_enable(ui->screen_ebook_btn_next, (uint16_t)(page + 1) < total);
}

static void screen_ebook_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);
    switch (code)
    {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch (dir)
        {
        case LV_DIR_LEFT: /* 向左滑 → 下一页 */
        {
            lv_indev_wait_release(lv_indev_active());
            if (ebk_next_page())
                ebook_refresh_ui(ui);
            break;
        }
        case LV_DIR_RIGHT: /* 向右滑 → 上一页 */
        {
            lv_indev_wait_release(lv_indev_active());
            if (ebk_prev_page())
                ebook_refresh_ui(ui);
            break;
        }
        case LV_DIR_BOTTOM: /* 向下滑 → 下一本（最后一本回到第一本） */
        {
            lv_indev_wait_release(lv_indev_active());
            if (ebk_next_book())
                ebook_refresh_ui(ui);
            break;
        }
        case LV_DIR_TOP: /* 向上滑 → 退出到主界面 */
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_1, guider_ui.screen_main_1_del, &guider_ui.screen_ebook_del, setup_scr_screen_main_1, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    case LV_EVENT_SCREEN_LOADED: /* 进界面：扫描 0:/ebook 并打开第一本 */
    {
        ebk_scan_dir();
        ebook_dd_build_options(ui); /* 书名列表 → 下拉框 */
        ebook_dd_setup(ui);         /* 默认只显示书名 label，长按才唤出下拉框 */
        if (ebk_book_count() > 0)
            ebk_open_book(0); /* 打不开也停在这本，界面会显示原因，绝不悄悄跳过 */
        ebook_refresh_ui(ui);
        break;
    }
    case LV_EVENT_SCREEN_UNLOADED: /* 离开界面：关文件、清状态 */
    {
        if (s_dd_shown)
        {
            lv_dropdown_close(ui->screen_ebook_ddlist_book);
            s_dd_shown = false;
        }
        s_ebook_name_label = NULL; /* 随界面一起被释放 */
        ebk_close_view();
        break;
    }
    default:
        break;
    }
}

/* 上一页 / 下一页按钮（LVGL 内置图标），与滑动手势共用同一套分页逻辑 */
static void screen_ebook_btn_prev_cb(lv_event_t *e)
{
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);
    if (ebk_prev_page())
        ebook_refresh_ui(ui);
}

static void screen_ebook_btn_next_cb(lv_event_t *e)
{
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);
    if (ebk_next_page())
        ebook_refresh_ui(ui);
}

void events_init_screen_ebook(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_ebook, screen_ebook_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_ebook_ddlist_book, screen_ebook_book_cb, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_ebook_btn_prev, screen_ebook_btn_prev_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_ebook_btn_next, screen_ebook_btn_next_cb, LV_EVENT_CLICKED, ui);
}

/* 状态指示灯：开启时实心高亮 + 光晕，关闭时灰色半透明 */
static void look_status_dot_update(lv_obj_t *dot, int on, uint32_t on_color)
{
    lv_color_t c = lv_color_hex(on_color);

    if (dot == NULL)
        return;
    lv_obj_set_style_bg_color(dot, on ? c : lv_color_hex(0x9AA0A6), 0);
    lv_obj_set_style_bg_opa(dot, on ? 255 : 120, 0);
    lv_obj_set_style_border_color(dot, on ? c : lv_color_hex(0x7D838B), 0);
    lv_obj_set_style_shadow_width(dot, on ? 6 : 0, 0);
    lv_obj_set_style_shadow_color(dot, c, 0);
    lv_obj_set_style_shadow_opa(dot, on ? 110 : 0, 0);
}

/* screen_sms 数据刷新定时器回调
   每 300ms 刷新一次显示：传感器数据来自测量采集任务(MEAS)的共享快照，
   设备状态直接读输出引脚/TIM 比较值，避免界面线程与采集任务并发访问 I2C */
static void screen_look_refresh_timer_cb(lv_timer_t *timer)
{
    lv_ui *ui = (lv_ui *)lv_timer_get_user_data(timer);
    app_measure_t m;
    float pitch_f, roll_f, yaw_f;
    int servo_deg;
    int fan_pct;
    int t, h;

    app_measure_get(&m);

    /* ---- 温湿度（只显示整数，绕开 lv_sprintf_builtin.c 的 %f 兼容性问题） ---- */
    t = (int)(m.temperature + (m.temperature >= 0 ? 0.5f : -0.5f));
    if (t < 0)
        t = 0;
    if (t > 50)
        t = 50;
    lv_bar_set_value(ui->screen_look_bar_temp, t * 100 / 50, LV_ANIM_ON);
    lv_label_set_text_fmt(ui->screen_look_label_temp_vol, "%d度", t);

    h = (int)((double)m.humidity + (m.humidity >= 0 ? 0.5 : -0.5));
    if (h < 0)
        h = 0;
    if (h > 100)
        h = 100;
    lv_bar_set_value(ui->screen_look_bar_hum, h, LV_ANIM_ON);
    lv_label_set_text_fmt(ui->screen_look_label_hum_vol, "%d%%", h);

    /* ---- 光照 ---- */
    lv_bar_set_value(ui->screen_look_bar_lig, m.light_pct, LV_ANIM_ON);
    lv_label_set_text_fmt(ui->screen_look_label_lig_vol, "%d%%", m.light_pct);

    /* ---- 风扇：直接按 PWM 档位显示（语音/界面操作都能同步） ---- */
    fan_pct = fan_get_gear();
    lv_bar_set_value(ui->screen_look_bar_gear, fan_pct * 100 / 3, LV_ANIM_ON);
    lv_label_set_text_fmt(ui->screen_look_label_gear_vol, "%d档", fan_pct);

    /* ---- 舵机：按 CCR 反算角度显示 ---- */
    servo_deg = sg90_get_angle();
    lv_bar_set_value(ui->screen_look_bar_angle, servo_deg * 100 / 180, LV_ANIM_ON);
    lv_label_set_text_fmt(ui->screen_look_label_angle_vol, "%d度", servo_deg);

    /* ---- 姿态角度（测量任务已做 DMP/原始数据兜底） ---- */
    pitch_f = m.pitch;
    roll_f = m.roll;
    yaw_f = m.yaw;

    {
        int p10, r10, y10;
        int pi, pd, ri, rd, yi, yd;
        p10 = (int)(pitch_f * 10.0f + (pitch_f >= 0 ? 0.5f : -0.5f));
        r10 = (int)(roll_f * 10.0f + (roll_f >= 0 ? 0.5f : -0.5f));
        y10 = (int)(yaw_f * 10.0f + (yaw_f >= 0 ? 0.5f : -0.5f));
        pi = p10 / 10;
        pd = (p10 < 0 ? -p10 : p10) % 10;
        ri = r10 / 10;
        rd = (r10 < 0 ? -r10 : r10) % 10;
        yi = y10 / 10;
        yd = (y10 < 0 ? -y10 : y10) % 10;
        if (p10 < 0)
        {
            lv_label_set_text_fmt(ui->screen_look_label_mpu_x, "X:-%d.%d", -pi, pd);
        }
        else
        {
            lv_label_set_text_fmt(ui->screen_look_label_mpu_x, "X:%d.%d", pi, pd);
        }
        if (r10 < 0)
        {
            lv_label_set_text_fmt(ui->screen_look_label_mpu_y, "Y:-%d.%d", -ri, rd);
        }
        else
        {
            lv_label_set_text_fmt(ui->screen_look_label_mpu_y, "Y:%d.%d", ri, rd);
        }
        if (y10 < 0)
        {
            lv_label_set_text_fmt(ui->screen_look_label_mpu_z, "Z:-%d.%d", -yi, yd);
        }
        else
        {
            lv_label_set_text_fmt(ui->screen_look_label_mpu_z, "Z:%d.%d", yi, yd);
        }
    }

    /* ---- LED1/LED2/LED3/继电器 指示：直接读输出引脚，不做状态缓存，避免错位 ----
       LED 低电平点亮，继电器高电平吸合 */
    look_status_dot_update(ui->screen_look_led1_dot, (LED1 == 0), 0x22C55E);
    look_status_dot_update(ui->screen_look_led2_dot, (LED2 == 0), 0x2195F6);
    look_status_dot_update(ui->screen_look_led3_dot, (LED3 == 0), 0xF59E0B);
    look_status_dot_update(ui->screen_look_relay_dot, (RELAY != 0), 0xEF4444);
}

static void screen_look_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);
    switch (code)
    {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch (dir)
        {
        case LV_DIR_TOP:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_1, guider_ui.screen_main_1_del, &guider_ui.screen_look_del, setup_scr_screen_main_1, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
            if (s_app_timer)
            {
                lv_timer_del(s_app_timer);
                s_app_timer = NULL;
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case LV_EVENT_SCREEN_LOADED:
    {
        if (!s_app_timer)
        {
            s_app_timer = lv_timer_create(screen_look_refresh_timer_cb, 300, ui);
            /* 进入界面立即刷新一次，避免首帧显示占位值 */
            lv_timer_ready(s_app_timer);
        }
        break;
    }
    case LV_EVENT_SCREEN_UNLOADED:
    {
        if (s_app_timer)
        {
            lv_timer_del(s_app_timer);
            s_app_timer = NULL;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_screen_look(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_look, screen_look_event_handler, LV_EVENT_ALL, ui);
}

/* ====================== 音乐播放界面 ====================== */

/* 歌曲下拉列表文本缓冲：32 首 × 64 字节，留出 '\n' 与结束符余量 */
#define MUS_OPTS_MAX (32 * 64 + 64)

static lv_timer_t *s_music_timer = NULL;
static int s_music_ui_state = -1;       /* 上次刷新的播放状态 */
static int s_music_ui_song = -1;        /* 上次刷新的歌曲索引 */
static int s_music_ui_lyric = -2;       /* 上次刷新的歌词行号 */
static int s_music_ui_vol = -1;         /* 上次刷新的音量（语音改音量时同步滑块） */
static unsigned int s_music_ui_sec = 0xFFFFFFFF; /* 上次刷新的秒数 */

/* lyr_2 ~ lyr_5 依次排布在标题下方 */
static lv_obj_t *music_lyric_label(lv_ui *ui, int pos)
{
    switch (pos)
    {
    case 0:  return ui->screen_music_label_lyr_2;
    case 1:  return ui->screen_music_label_lyr_3;
    case 2:  return ui->screen_music_label_lyr_4;
    default: return ui->screen_music_label_lyr_5;
    }
}

/* 播放中显示“暂停”字形，暂停/停止显示“播放”字形（都用 LVGL 内置图标） */
static void music_update_icon(lv_ui *ui)
{
    s_music_ui_state = mus_state();
    lv_label_set_text(ui->screen_music_icon_play_glyph,
                      (s_music_ui_state == MUS_PLAYING) ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
}

static void music_update_song(lv_ui *ui)
{
    int n = mus_current();

    s_music_ui_song = n;
    lv_dropdown_set_selected(ui->screen_music_ddlist_song, (n >= 0) ? (uint32_t)n : 0);
    /* 歌名只在顶部第一行显示（label_lyr_1 已隐藏，避免重复） */
    if (s_song_label != NULL && lv_obj_is_valid(s_song_label))
        lv_label_set_text(s_song_label, (n >= 0) ? mus_title(n) : "未播放");
    s_music_ui_lyric = -2; /* 换歌后强制重刷歌词 */
}

/* 扫描 0:/music，把歌曲名填进顶部下拉列表 */
static void music_reload_list(lv_ui *ui)
{
    static char opts[MUS_OPTS_MAX];
    int n, i, len = 0;

    n = mus_scan();
    opts[0] = '\0';
    if (n <= 0)
    {
        strcpy(opts, "无歌曲");
    }
    else
    {
        for (i = 0; i < n; i++)
        {
            const char *t = mus_title(i);
            size_t tl = strlen(t);
            if (len + (int)tl + 2 > (int)sizeof(opts))
                break;
            if (i > 0)
                opts[len++] = '\n';
            memcpy(opts + len, t, tl);
            len += (int)tl;
            opts[len] = '\0';
        }
    }
    lv_dropdown_set_options(ui->screen_music_ddlist_song, opts);
    music_update_song(ui);
}

static void music_update_time(lv_ui *ui)
{
    unsigned int cur = mus_cursec();
    unsigned int tot = mus_totsec();

    if (cur != s_music_ui_sec)
    {
        char buf[24];
        s_music_ui_sec = cur;
        snprintf(buf, sizeof(buf), "%u:%02u/%u:%02u",
                 cur / 60, cur % 60, tot / 60, tot % 60);
        lv_label_set_text(ui->screen_music_label_music_time, buf);
        lv_bar_set_value(ui->screen_music_bar_music,
                         (tot > 0) ? (int)(cur * 100 / tot) : 0, LV_ANIM_OFF);
    }
}

/* 显示歌词：当前行放 lyr_3（高亮），上下各带一行 */
static void music_update_lyrics(lv_ui *ui)
{
    int idx = mus_lyric_index();
    int k;

    if (idx == s_music_ui_lyric)
        return;
    s_music_ui_lyric = idx;

    for (k = 0; k < 4; k++)
    {
        const char *txt = "";
        lv_obj_t *lb = music_lyric_label(ui, k);

        if (idx >= 0)
        {
            const char *line = mus_lyric_line(idx - 1 + k);
            if (line != NULL)
                txt = line;
        }
        lv_label_set_text(lb, txt);
        lv_obj_set_style_text_color(lb,
                                    (k == 1) ? lv_color_hex(0x3700ff) : lv_color_hex(0x000000), 0);
    }
}

/* 音量百分比标签同步 */
static void music_update_vol(lv_ui *ui, int pct)
{
    char buf[24];

    /* 带音量图标（LVGL 内置图标），静音时切换为 MUTE 图标 */
    if (pct <= 0)
        snprintf(buf, sizeof(buf), LV_SYMBOL_MUTE " %d%%", pct);
    else if (pct < 60)
        snprintf(buf, sizeof(buf), LV_SYMBOL_VOLUME_MID " %d%%", pct);
    else
        snprintf(buf, sizeof(buf), LV_SYMBOL_VOLUME_MAX " %d%%", pct);
    lv_label_set_text(ui->screen_music_label_vol, buf);
}

/* 拖动音量滑块：只记录目标值，硬件音量由定时器里的 mus_vol_service() 逐级逼近 */
static void music_vol_cb(lv_event_t *e)
{
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);
    int pct = (int)lv_slider_get_value((lv_obj_t *)lv_event_get_target(e));

    mus_set_volume(pct);
    music_update_vol(ui, pct);
}

/* 10ms 一次：刷新状态/进度/时间/歌词
   （DMA 补数在 MUSIC 任务 mus_tick()，音量平滑在 MUSIC 任务 mus_vol_service()，
    它们必须常驻运行，本定时器只在音乐界面存在时才跑） */
static void music_timer_cb(lv_timer_t *t)
{
    lv_ui *ui = (lv_ui *)lv_timer_get_user_data(t);

    /* 语音（或别处）改了音量：把滑块和百分比同步过来，否则界面显示不变 */
    if (s_music_ui_vol != mus_volume())
    {
        s_music_ui_vol = mus_volume();
        lv_slider_set_value(ui->screen_music_slider_vol, s_music_ui_vol, LV_ANIM_OFF);
        music_update_vol(ui, s_music_ui_vol);
    }

    if (mus_state() != s_music_ui_state)
        music_update_icon(ui);
    if (mus_current() != s_music_ui_song)
        music_update_song(ui);

    music_update_time(ui);
    music_update_lyrics(ui);
}

static void music_play_pause_cb(lv_event_t *e)
{
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);

    mus_toggle();
    music_update_icon(ui);
    if (mus_current() != s_music_ui_song)
        music_update_song(ui);
    s_music_ui_sec = 0xFFFFFFFF;
}

static void music_prev_cb(lv_event_t *e)
{
    (void)e;
    mus_prev();
}

static void music_next_cb(lv_event_t *e)
{
    (void)e;
    mus_next();
}

/* 停止播放：释放音频资源并回到停止状态（歌名保留，图标切回“播放”） */
static void music_stop_cb(lv_event_t *e)
{
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);

    mus_stop();
    music_update_icon(ui);
    s_music_ui_sec = 0xFFFFFFFF; /* 下一轮定时器把时间/进度条刷成 0 */
}

/* 点击顶部下拉列表里的歌曲名 → 切歌 */
static void music_song_cb(lv_event_t *e)
{
    lv_obj_t *dd = (lv_obj_t *)lv_event_get_target(e);
    uint32_t sel = lv_dropdown_get_selected(dd);

    if ((int)sel != mus_current())
        mus_play((int)sel);
}

static void screen_music_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);

    switch (code)
    {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch (dir)
        {
        case LV_DIR_TOP:
        {
            lv_indev_wait_release(lv_indev_active());
            /* 切屏前先停掉音乐界面的定时器：旧界面随后会被释放，
               若定时器还活着就可能访问已销毁的 label/image 指针，
               造成偶发的图标不显示、只剩背景图等花屏/内存破坏。 */
            if (s_music_timer != NULL)
            {
                lv_timer_del(s_music_timer);
                s_music_timer = NULL;
            }
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_1, guider_ui.screen_main_1_del, &guider_ui.screen_music_del, setup_scr_screen_main_1, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    case LV_EVENT_SCREEN_LOADED:
    {
        mus_init(); /* 提前初始化音频，进界面就能调音量 */
        music_dd_setup(ui); /* 下拉框默认隐藏，原位置显示歌名 */
        music_reload_list(ui);
        music_update_icon(ui);
        lv_slider_set_value(ui->screen_music_slider_vol, mus_volume(), LV_ANIM_OFF);
        music_update_vol(ui, mus_volume());
        s_music_ui_sec = 0xFFFFFFFF;
        if (s_music_timer == NULL)
            s_music_timer = lv_timer_create(music_timer_cb, 10, ui);
        break;
    }
    case LV_EVENT_SCREEN_UNLOADED:
    {
        /* 后台播放：离开界面不停止音乐，音频补数由 AUDIO 任务继续；
           只删掉本界面的刷新定时器，防止访问已销毁的控件 */
        if (s_music_timer != NULL)
        {
            lv_timer_del(s_music_timer);
            s_music_timer = NULL;
        }
        s_song_label = NULL; /* label 是音乐界面的子控件，随界面一起被释放 */
        s_dd_shown = false;
        s_music_ui_state = -1;
        s_music_ui_song = -1;
        s_music_ui_lyric = -2;
        s_music_ui_sec = 0xFFFFFFFF;
        break;
    }
    default:
        break;
    }
}

void events_init_screen_music(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_music, screen_music_event_handler, LV_EVENT_ALL, ui);

    lv_obj_add_event_cb(ui->screen_music_icon_prev, music_prev_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_music_icon_play, music_play_pause_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_music_icon_next, music_next_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_music_icon_stop, music_stop_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_music_ddlist_song, music_song_cb, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_music_slider_vol, music_vol_cb, LV_EVENT_VALUE_CHANGED, ui);
}

/* ============== 图片浏览界面（screen_picture）辅助函数 ============== */

/* 判断文件名是否 .bmp/.BMP 结尾 */
static int album_is_bmp(const char *name)
{
    const char *dot = strrchr(name, '.');
    if (dot == NULL)
        return 0;
    if (dot[1] != 'b' && dot[1] != 'B')
        return 0;
    if (dot[2] != 'm' && dot[2] != 'M')
        return 0;
    if (dot[3] != 'p' && dot[3] != 'P')
        return 0;
    return dot[4] == '\0';
}

/* 显示当前节点指向的图片；目录里没有 bmp 时显示提示 */
static void album_show_current(lv_ui *ui)
{
    if (s_album_cur != NULL)
    {
        lv_image_set_src(ui->screen_pic_img_2, s_album_cur->full_path);
        lv_obj_add_flag(s_album_empty, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        /* 没有可显示的图片：清掉上一张，露出屏幕背景图作为兜底 */
        lv_image_set_src(ui->screen_pic_img_2, NULL);
        lv_obj_remove_flag(s_album_empty, LV_OBJ_FLAG_HIDDEN);
    }
}

/* 从 start 出发沿 dir 找下一个 .bmp 文件节点（跳过目录和非 bmp 文件）；
   找不到返回 NULL。start 本身不参与匹配。 */
static FileNode *album_find_bmp_from(FileNode *start, int dir)
{
    FileNode *n = start;
    int guard = list_file_num + 1;

    if (start == NULL)
        return NULL;
    while (guard-- > 0)
    {
        n = list_next_file(n, dir);
        if (n == NULL || n == start)
            return NULL; /* 绕回起点：没有其他 bmp */
        if (!n->is_dir && album_is_bmp(n->full_path))
            return n;
    }
    return NULL;
}

/* 进界面：递归扫描 ALBUM_DIR 建链表（含子目录），显示第一张 bmp */
static void album_scan(lv_ui *ui)
{
    FileNode *first;

    free_list(&list_head);
    if (recursive_read_dir(ALBUM_DIR, &list_head) == FR_OK)
        list_count(list_head);
    else
        list_count(NULL);

    s_album_cur = NULL;
    first = list_first_file(list_head);
    if (first != NULL)
    {
        if (!first->is_dir && album_is_bmp(first->full_path))
            s_album_cur = first; /* 第一个文件节点恰好是 bmp */
        else
            s_album_cur = album_find_bmp_from(first, 1);
    }
    album_show_current(ui);
}

/* 翻页：dir=1 下一张，dir=-1 上一张（只在 bmp 之间跳，循环绕回） */
static void album_step(int dir, lv_ui *ui)
{
    FileNode *n;

    if (s_album_cur == NULL)
        return;

    n = album_find_bmp_from(s_album_cur, dir);
    if (n != NULL)
    {
        s_album_cur = n;
        album_show_current(ui);
    }
}

static void album_dialog_close(void)
{
    if (s_album_dialog != NULL)
    {
        lv_obj_delete(s_album_dialog);
        s_album_dialog = NULL;
    }
}

/* 删除当前图片：f_unlink + 链表摘除，自动跳到下一张（删光显示提示） */
static void album_delete_current(lv_ui *ui)
{
    FileNode *next;

    if (s_album_cur == NULL)
        return;

    next = album_find_bmp_from(s_album_cur, 1); /* 先找下一个 bmp（没有则返回NULL） */
    if (f_unlink(s_album_cur->full_path) != FR_OK)
        return; /* 删除失败（只读等），保持原状 */

    list_remove_node(s_album_cur); /* 从链表摘除并 myfree 释放 */
    s_album_cur = next;            /* NULL = 删光了，显示提示 */
    album_show_current(ui);
}

static void album_yes_cb(lv_event_t *e) /* “是” → 删除 */
{
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);
    lv_indev_t *indev = lv_indev_active();
    if (indev)
        lv_indev_wait_release(indev);
    album_dialog_close();
    album_delete_current(ui);
}

static void album_no_cb(lv_event_t *e) /* “否” → 仅关闭弹窗 */
{
    lv_indev_t *indev = lv_indev_active();
    if (indev)
        lv_indev_wait_release(indev);
    album_dialog_close();
}

/* 长按图片 → 弹出删除确认框（挂在 lv_layer_top） */
static void album_img_event_cb(lv_event_t *e)
{
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);
    lv_obj_t *label, *btn;
    lv_event_code_t code = lv_event_get_code(e);
    lv_point_t now;
    int32_t dx, dy, d2;

    /* 按下：记录起点，位移清零 */
    if (code == LV_EVENT_PRESSED)
    {
        if (lv_indev_active() != NULL)
            lv_indev_get_point(lv_indev_active(), &s_album_press_pt);
        s_album_press_max_d2 = 0;
        return;
    }

    /* 按住移动：持续跟踪最大位移（不只看长按那一刻的坐标） */
    if (code == LV_EVENT_PRESSING)
    {
        if (lv_indev_active() == NULL)
            return;
        lv_indev_get_point(lv_indev_active(), &now);
        dx = now.x - s_album_press_pt.x;
        dy = now.y - s_album_press_pt.y;
        d2 = dx * dx + dy * dy;
        if (d2 > s_album_press_max_d2)
            s_album_press_max_d2 = d2;
        return;
    }

    if (code != LV_EVENT_LONG_PRESSED)
        return;
    if (s_album_cur == NULL || s_album_dialog != NULL)
        return;

    /* LVGL 长按只看"超时未抬起"，不看手指是否移动。
       整个按压过程漂移超过 15px 就视为滑动，不弹删除框 */
    if (s_album_press_max_d2 > 15 * 15)
        return;

    album_dialog_close();

    /* 只保留中央白色面板，不做全屏遮罩：
       全屏半透明层会让整张图片失效重绘，而 lv_conf.h 里 LV_CACHE_DEF_SIZE=0，
       每次重绘都要从 SD 卡重新解码 BMP，既闪屏又拖慢触摸采样 */
    s_album_dialog = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(s_album_dialog);
    lv_obj_set_size(s_album_dialog, 200, 110);
    lv_obj_align(s_album_dialog, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(s_album_dialog, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(s_album_dialog, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(s_album_dialog, 8, 0);
    lv_obj_clear_flag(s_album_dialog, LV_OBJ_FLAG_SCROLLABLE);

    label = lv_label_create(s_album_dialog);
    lv_label_set_text(label, "删除当前图片?");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    btn = lv_button_create(s_album_dialog); /* 是（红） */
    lv_obj_set_size(btn, 64, 36);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, -40, -14);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xE05050), 0);
    lv_obj_add_event_cb(btn, album_yes_cb, LV_EVENT_CLICKED, ui);
    label = lv_label_create(btn);
    lv_label_set_text(label, "是");
    lv_obj_center(label);

    btn = lv_button_create(s_album_dialog); /* 否（灰） */
    lv_obj_set_size(btn, 64, 36);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 40, -14);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x909090), 0);
    lv_obj_add_event_cb(btn, album_no_cb, LV_EVENT_CLICKED, ui);
    label = lv_label_create(btn);
    lv_label_set_text(label, "否");
    lv_obj_center(label);
}

static void screen_pic_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);
    switch (code)
    {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

        if (s_album_dialog != NULL)
            break; /* 删除确认框打开时不翻页/不返回 */

        switch (dir)
        {
        case LV_DIR_LEFT: /* 向左滑 → 下一张 */
        {
            lv_indev_wait_release(lv_indev_active());
            album_step(1, ui);
            break;
        }
        case LV_DIR_RIGHT: /* 向右滑 → 上一张 */
        {
            lv_indev_wait_release(lv_indev_active());
            album_step(-1, ui);
            break;
        }
        case LV_DIR_TOP: /* 向上滑 → 回主界面 */
        {
            lv_indev_wait_release(lv_indev_active());
            /* 必须用 NONE：OVER_TOP 会让新屏从 y=+280 往上滑，本屏缓冲仅 10 行且
               无图片缓存，滑动帧刷新很慢，动画常停在中间(y≈+140)，
               导致 LCD 下半屏残留新屏上半部分。改为与其他界面一致的无动画切换 */
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_2, guider_ui.screen_main_2_del, &guider_ui.screen_pic_del, setup_scr_screen_main_2, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    case LV_EVENT_SCREEN_LOADED:
    {
        album_scan(ui); /* 进入界面：扫描 ALBUM_DIR 显示第一张 */
        break;
    }
    case LV_EVENT_SCREEN_UNLOADED:
    {
        album_dialog_close();    /* 离开界面：关弹窗 */
        free_list(&list_head); /* 释放链表节点（还给 mymalloc 池） */
        s_album_cur = NULL;
        break;
    }
    default:
        break;
    }
}

void events_init_screen_pic(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_pic, screen_pic_event_handler, LV_EVENT_ALL, ui);

    /* 长按图片 → 删除确认弹窗（PRESSED/PRESSING 跟踪位移，区分滑动） */
    lv_obj_add_event_cb(ui->screen_pic_img_2, album_img_event_cb, LV_EVENT_PRESSED, ui);
    lv_obj_add_event_cb(ui->screen_pic_img_2, album_img_event_cb, LV_EVENT_PRESSING, ui);
    lv_obj_add_event_cb(ui->screen_pic_img_2, album_img_event_cb, LV_EVENT_LONG_PRESSED, ui);

    /* 目录无图片提示（默认隐藏） */
    s_album_empty = lv_label_create(ui->screen_pic);
    lv_label_set_text(s_album_empty, ALBUM_DIR " 无图片");
    lv_obj_center(s_album_empty);
    lv_obj_add_flag(s_album_empty, LV_OBJ_FLAG_HIDDEN);
}

void events_init(lv_ui *ui)
{
}

/* ================================================================
 *  便签 / 文件 / 消息 三个独立功能界面的事件
 * ================================================================ */

/* ---------------- 便签 ---------------- */
static char s_note_text[201]; /* 便签内容放 RAM，切界面再回来还在 */

static void note_back_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_2, guider_ui.screen_main_2_del, &guider_ui.screen_note_del, setup_scr_screen_main_2, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
}

static void note_clear_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;
    lv_textarea_set_text(guider_ui.screen_note_ta_text, "");
}

static void note_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_SCREEN_LOADED)
    {
        if (s_note_text[0] != '\0')
            lv_textarea_set_text(guider_ui.screen_note_ta_text, s_note_text);
    }
    else if (code == LV_EVENT_SCREEN_UNLOADED)
    {
        const char *txt = lv_textarea_get_text(guider_ui.screen_note_ta_text);
        if (txt != NULL)
            lv_strlcpy(s_note_text, txt, sizeof(s_note_text));
    }
    else if (code == LV_EVENT_GESTURE) /* 上滑 → 返回主界面2 */
    {
        if (lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_TOP)
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_2, guider_ui.screen_main_2_del, &guider_ui.screen_note_del, setup_scr_screen_main_2, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        }
    }
}

void events_init_screen_note(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_note, note_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_note_btn_back, note_back_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_note_btn_clear, note_clear_event_handler, LV_EVENT_ALL, ui);
}

/* ---------------- 文件 ---------------- */
#define FILE_LIST_MAX 20

/* 扩展名 -> 文件类型图标 (复用主页面2的应用图标, 空格分隔, 大小写不敏感) */
typedef struct {
    const char *exts;
    const char *icon;
} file_type_t;

static const file_type_t file_types[] = {
    { "bmp jpg jpeg png gif ico",     PIC_PICTURE_70 },
    { "mp3 wav flac aac wma mid ogg", PIC_MUSIC_70   },
    { "txt log ini cfg c h cpp md",   PIC_NOTEPAD_70 },
    { "pdf doc docx xls csv",         PIC_EBOOK_70   },
};

static char file_lower(char c)
{
    return (c >= 'A' && c <= 'Z') ? (char)(c + ('a' - 'A')) : c;
}

static bool file_ext_in(const char *ext, const char *list)
{
    size_t n = strlen(ext);
    const char *p = list;

    while (*p)
    {
        const char *q = strchr(p, ' ');
        size_t len = q ? (size_t)(q - p) : strlen(p);
        if (len == n)
        {
            size_t i;
            for (i = 0; i < n; i++)
                if (file_lower(ext[i]) != file_lower(p[i])) break;
            if (i == n) return true;
        }
        if (!q) break;
        p = q + 1;
    }
    return false;
}

/* 目录 → 设置图标; 文件 → 按扩展名取主页面2的应用图标; 其余 → 主图标 */
static const char *file_icon_get(const char *name, bool is_dir)
{
    const char *ext;
    unsigned int i;

    if (is_dir) return PIC_SET_70;
    ext = strrchr(name, '.');
    if (ext == NULL) return PIC_APP_70;
    ext++;
    for (i = 0; i < sizeof(file_types) / sizeof(file_types[0]); i++)
        if (file_ext_in(ext, file_types[i].exts)) return file_types[i].icon;
    return PIC_APP_70;
}

static void file_add_row(lv_obj_t *parent, const char *name, bool is_dir)
{
    /* 一行 = 图标 + 文件名 */
    lv_obj_t *row = lv_obj_create(parent);
    lv_obj_set_size(row, 218, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_all(row, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(row, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(row, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(row, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *icon = lv_image_create(row);
    lv_image_set_src(icon, file_icon_get(name, is_dir));
    lv_image_set_scale(icon, 66); /* 70px 源图缩到约 18px */
    lv_obj_set_size(icon, 18, 18);

    lv_obj_t *lbl = lv_label_create(row);
    lv_label_set_text(lbl, name);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_CLIP);
    lv_obj_set_flex_grow(lbl, 1);
    lv_obj_set_style_text_font(lbl, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lbl, lv_color_hex(is_dir ? 0x2195F6 : 0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(lbl, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(lbl, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void file_rebuild_list(lv_ui *ui)
{
    DIR dir;
    FILINFO info;
    int cnt = 0;

    if (!lv_obj_is_valid(ui->screen_file_list_box))
        return;

    lv_obj_clean(ui->screen_file_list_box); /* 只清空这个容器，不动界面本体 */

    if (f_opendir(&dir, "0:/") != FR_OK)
    {
        file_add_row(ui->screen_file_list_box, "No SD card", false);
        return;
    }

    while (cnt < FILE_LIST_MAX)
    {
        if (f_readdir(&dir, &info) != FR_OK || info.fname[0] == '\0')
            break;
        file_add_row(ui->screen_file_list_box, info.fname, (info.fattrib & AM_DIR) != 0);
        cnt++;
    }
    f_closedir(&dir);

    if (cnt == 0)
        file_add_row(ui->screen_file_list_box, "Empty", false);
}

static void file_back_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_2, guider_ui.screen_main_2_del, &guider_ui.screen_file_del, setup_scr_screen_main_2, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
}

static void file_refresh_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;
    file_rebuild_list(&guider_ui);
}

static void file_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_SCREEN_LOADED)
        file_rebuild_list(&guider_ui);
    else if (code == LV_EVENT_GESTURE) /* 上滑 → 返回主界面2 */
    {
        if (lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_TOP)
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_2, guider_ui.screen_main_2_del, &guider_ui.screen_file_del, setup_scr_screen_main_2, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        }
    }
}

void events_init_screen_file(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_file, file_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_file_btn_back, file_back_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_file_btn_refresh, file_refresh_event_handler, LV_EVENT_ALL, ui);
}

/* ---------------- 消息 ---------------- */
static void news_back_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_2, guider_ui.screen_main_2_del, &guider_ui.screen_news_del, setup_scr_screen_main_2, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
}

static void news_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_GESTURE)
        return;
    if (lv_indev_get_gesture_dir(lv_indev_active()) != LV_DIR_TOP) /* 上滑 → 返回主界面2 */
        return;
    lv_indev_wait_release(lv_indev_active());
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_2, guider_ui.screen_main_2_del, &guider_ui.screen_news_del, setup_scr_screen_main_2, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
}

void events_init_screen_news(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_news, news_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_news_btn_back, news_back_event_handler, LV_EVENT_ALL, ui);
}

/* ================================================================
 *  WiFi 连接界面
 *    - 进入界面自动扫描附近热点，结果显示在顶部下拉框
 *    - 三个三行滚轮分别选择 数字+特殊字符 / 小写 / 大写，
 *      点“输入”把最后操作的滚轮当前字符追加到密码
 *    - 删除：删掉密码最后一个字符；连接：用所选 SSID + 密码连接
 *    - 禁用上滑返回，避免滚动滚轮时误退出；左上角按钮返回
 *    扫描/连接是阻塞 AT 交互，只投递请求给 WIFI 任务，这里轮询结果
 * ================================================================ */
#define WIFI_PWD_MAX WIFI_PWD_LEN
#define WIFI_OPT_MAX (WIFI_AP_MAX * (WIFI_SSID_LEN + 1))

static char s_wifi_pwd[WIFI_PWD_MAX + 1];
static char s_wifi_opts[WIFI_OPT_MAX];
static u8 s_wifi_last_join = 0;
static uint32_t s_wifi_join_tick = 0; /* 按下“确认”的时刻，用于连接超时 */
static uint32_t s_wifi_scan_tick = 0; /* 请求扫描的时刻，用于扫描超时 */
static uint32_t s_wifi_toast_tick = 0; /* 浮动提示开始显示的时刻 */
static uint32_t s_wifi_toast_ms = 0;   /* 浮动提示停留时长，0 表示一直显示 */
static uint32_t s_wifi_back_tick = 0;  /* 连接成功后的自动返回时刻 */
static lv_obj_t *s_wifi_active_roller = NULL; /* 最后触摸或滚动的滚轮 */
static lv_timer_t *s_wifi_timer = NULL;

static void wifi_set_status(const char *txt)
{
    if (lv_obj_is_valid(guider_ui.screen_wifi_label_status))
        lv_label_set_text(guider_ui.screen_wifi_label_status, txt);
}

/* 连接状态用视频里的浮动胶囊提示，不再挤在顶部状态栏 */
static void wifi_toast_hide(void)
{
    s_wifi_toast_tick = 0;
    s_wifi_toast_ms = 0;
    if (lv_obj_is_valid(guider_ui.screen_wifi_label_toast))
        lv_obj_add_flag(guider_ui.screen_wifi_label_toast, LV_OBJ_FLAG_HIDDEN);
}

/* duration_ms 为 0 表示一直显示，直到收到连接结果 */
static void wifi_toast_show(const char *txt, uint32_t duration_ms)
{
    lv_obj_t *toast = guider_ui.screen_wifi_label_toast;

    if (!lv_obj_is_valid(toast))
        return;
    lv_label_set_text(toast, txt);
    lv_obj_align(toast, LV_ALIGN_TOP_MID, 0, 190);
    lv_obj_remove_flag(toast, LV_OBJ_FLAG_HIDDEN);
    s_wifi_toast_tick = lv_tick_get();
    s_wifi_toast_ms = duration_ms;
}

static void wifi_back_to_main(void)
{
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_2, guider_ui.screen_main_2_del, &guider_ui.screen_wifi_del, setup_scr_screen_main_2, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
}

static void wifi_pwd_refresh(void)
{
    char buf[WIFI_PWD_MAX + 8];
    if (!lv_obj_is_valid(guider_ui.screen_wifi_label_pwd))
        return;
    lv_snprintf(buf, sizeof(buf), "密码:%s", s_wifi_pwd);
    lv_label_set_text(guider_ui.screen_wifi_label_pwd, buf);
}

static void wifi_rebuild_options(void)
{
    u8 i;
    size_t pos = 0;

    for (i = 0; i < g_wifi_scan.count && i < WIFI_AP_MAX; i++)
    {
        size_t n = strlen(g_wifi_scan.ssid[i]);
        if (n == 0 || pos + n + 2 >= sizeof(s_wifi_opts))
            break;
        if (pos > 0)
            s_wifi_opts[pos++] = '\n';
        memcpy(s_wifi_opts + pos, g_wifi_scan.ssid[i], n);
        pos += n;
    }
    s_wifi_opts[pos] = '\0';

    lv_dropdown_set_options_static(guider_ui.screen_wifi_dd_ap, s_wifi_opts);
    lv_dropdown_set_selected(guider_ui.screen_wifi_dd_ap, 0);
}

/* 200ms 轮询：把 WIFI 任务里的扫描/连接结果搬到界面上 */
static void wifi_refresh_timer_cb(lv_timer_t *t)
{
    (void)t;
    if (!lv_obj_is_valid(guider_ui.screen_wifi))
        return;

    if (s_wifi_toast_ms != 0 && s_wifi_toast_tick != 0 &&
        lv_tick_elaps(s_wifi_toast_tick) >= s_wifi_toast_ms)
        wifi_toast_hide();

    /* 连接成功：提示停留一会儿后自动回到主界面（和视频一致） */
    if (s_wifi_back_tick != 0 && lv_tick_elaps(s_wifi_back_tick) >= 1200)
    {
        s_wifi_back_tick = 0;
        wifi_back_to_main();
        return;
    }

    if (g_wifi_scan.state == 2) /* 扫描完成 */
    {
        g_wifi_scan.state = 0;
        s_wifi_scan_tick = 0;
        if (g_wifi_scan.count > 0)
        {
            wifi_rebuild_options();
            wifi_set_status("请输入密码");
        }
        else
        {
            lv_dropdown_set_options_static(guider_ui.screen_wifi_dd_ap, "未发现热点");
            wifi_set_status("未发现热点 请点右侧刷新");
        }
    }
    else if (g_wifi_scan.state == 3) /* 扫描失败 */
    {
        g_wifi_scan.state = 0;
        s_wifi_scan_tick = 0;
        lv_dropdown_set_options_static(guider_ui.screen_wifi_dd_ap, "扫描失败");
        wifi_set_status("扫描失败 请点右侧刷新");
    }
    else if (s_wifi_scan_tick != 0 && lv_tick_elaps(s_wifi_scan_tick) >= 15000)
    {
        /* WIFI 任务被别的 AT 交互占住时，扫描请求会一直排不上 */
        s_wifi_scan_tick = 0;
        wifi_set_status("扫描超时 请点右侧刷新");
    }

    /* 连接超时：WIFI 任务被别的 AT 交互占住时状态会一直卡在“连接中” */
    if (g_wifi_join_state == WIFI_JOIN_BUSY && s_wifi_join_tick != 0 &&
        lv_tick_elaps(s_wifi_join_tick) >= 15000)
    {
        g_wifi_join_state = WIFI_JOIN_FAIL;
        s_wifi_last_join = WIFI_JOIN_FAIL;
        s_wifi_join_tick = 0;
        wifi_set_status("连接超时 请重试");
        wifi_toast_show("连接超时", 2000);
    }

    if (g_wifi_join_state != s_wifi_last_join)
    {
        s_wifi_last_join = g_wifi_join_state;
        s_wifi_join_tick = 0;
        if (g_wifi_join_state == WIFI_JOIN_OK)
        {
            wifi_toast_show("连接成功", 0);
            s_wifi_back_tick = lv_tick_get();
        }
        else if (g_wifi_join_state == WIFI_JOIN_FAIL)
        {
            wifi_set_status("连接失败 密码错误或信号弱");
            wifi_toast_show("连接失败", 2000);
        }
    }
}

static void wifi_back_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;
    wifi_back_to_main();
}

/* 选中热点后提示输入密码（扫描完默认选中第一项也会触发） */
static void wifi_ap_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED)
        return;
    wifi_set_status("请输入密码");
}

static void wifi_scan_event_handler(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;
    wifi_set_status("正在扫描附近 WiFi ...");
    esp8266_wifi_request_scan();
    s_wifi_scan_tick = lv_tick_get();
}

/* 记录最后操作的滚轮；滚动本身不输入，避免划过时误加字符 */
static void wifi_roller_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED || code == LV_EVENT_VALUE_CHANGED)
        s_wifi_active_roller = lv_event_get_target(e);
}

/* 点“输入”：把最后操作的滚轮当前字符加进密码 */
static void wifi_input_event_handler(lv_event_t *e)
{
    char ch[8];
    size_t len;

    if (lv_event_get_code(e) != LV_EVENT_CLICKED || s_wifi_active_roller == NULL)
        return;

    lv_roller_get_selected_str(s_wifi_active_roller, ch, sizeof(ch));
    len = strlen(s_wifi_pwd);
    if (len < WIFI_PWD_MAX)
    {
        s_wifi_pwd[len] = ch[0];
        s_wifi_pwd[len + 1] = '\0';
        wifi_pwd_refresh();
    }
}

static void wifi_del_event_handler(lv_event_t *e)
{
    size_t len;

    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;

    len = strlen(s_wifi_pwd);
    if (len > 0)
    {
        s_wifi_pwd[len - 1] = '\0';
        wifi_pwd_refresh();
    }
}

static void wifi_ok_event_handler(lv_event_t *e)
{
    char ssid[WIFI_SSID_LEN + 1];

    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;

    if (g_wifi_scan.count == 0 || !lv_obj_is_valid(guider_ui.screen_wifi_dd_ap))
    {
        wifi_set_status("请先点右侧扫描 WiFi");
        return;
    }

    ssid[0] = '\0';
    lv_dropdown_get_selected_str(guider_ui.screen_wifi_dd_ap, ssid, sizeof(ssid));
    if (ssid[0] == '\0')
    {
        wifi_set_status("请先选择 WiFi");
        return;
    }

    if (esp8266_wifi_request_join(ssid, s_wifi_pwd) == 0)
    {
        wifi_toast_show("连接中", 0);
        s_wifi_last_join = WIFI_JOIN_BUSY;
        s_wifi_join_tick = lv_tick_get();
    }
    else
    {
        wifi_set_status("上一次连接还在进行中");
        wifi_toast_show("正在连接中", 1500);
    }
}

static void wifi_screen_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_SCREEN_LOADED)
    {
        s_wifi_pwd[0] = '\0';
        s_wifi_active_roller = guider_ui.screen_wifi_roller_num;
        s_wifi_last_join = g_wifi_join_state;
        s_wifi_join_tick = 0;
        s_wifi_back_tick = 0;
        wifi_toast_hide();
        wifi_pwd_refresh();

        /* 进界面自动扫描一次 */
        wifi_set_status("正在扫描附近 WiFi ...");
        esp8266_wifi_request_scan();
        s_wifi_scan_tick = lv_tick_get();

        if (s_wifi_timer == NULL)
            s_wifi_timer = lv_timer_create(wifi_refresh_timer_cb, 200, NULL);
    }
    else if (code == LV_EVENT_SCREEN_UNLOADED)
    {
        s_wifi_active_roller = NULL;
        s_wifi_back_tick = 0;
        s_wifi_toast_tick = 0;
        s_wifi_toast_ms = 0;
        if (s_wifi_timer)
        {
            lv_timer_del(s_wifi_timer);
            s_wifi_timer = NULL;
        }
    }
    /* 不处理 LV_EVENT_GESTURE：滚轮上滑只选字符，返回用左上角按钮 */
}

void events_init_screen_wifi(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_wifi, wifi_screen_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_btn_back, wifi_back_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_btn_scan, wifi_scan_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_dd_ap, wifi_ap_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_roller_num, wifi_roller_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_roller_low, wifi_roller_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_roller_up, wifi_roller_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_btn_del, wifi_del_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_btn_input, wifi_input_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_btn_ok, wifi_ok_event_handler, LV_EVENT_ALL, ui);
}

/* ================================================================
 *  音乐后台播放控制
 *    - 退出音乐界面后音乐继续播放（LV_EVENT_SCREEN_UNLOADED 不再 mus_stop）
 *    - 任意界面长按 5 秒呼出控制弹窗：暂停/继续、上一首、下一首、
 *      退出播放（停止并直接释放音乐界面）、关闭弹窗
 *    - 音乐界面长按 5 秒唤出顶部选歌下拉框，15 秒无触摸自动收起；
 *      平常下拉框隐藏，原位置以 label 显示当前歌名
 *  长按检测：100ms 轮询触摸 indev 按下状态（lv_indev_get_state）
 * ================================================================ */
#define MUSIC_LP_MS       5000  /* 长按触发时长 */
#define MUSIC_DD_IDLE_MS  15000 /* 下拉框无操作自动收起时长 */

static lv_timer_t *s_ctl_timer = NULL;   /* 100ms 轮询定时器 */
static uint32_t    s_press_start = 0;    /* 本次按下起始 tick，0=无按下 */
static bool        s_lp_fired = false;   /* 本次按住是否已触发过长按 */
static uint8_t     s_rel_cnt = 0;        /* 连续检测到松手的次数（抗抖） */
static lv_point_t  s_lp_pt;              /* 本次按下的起点（滑动不算长按） */
static bool        s_lp_moved = false;   /* 本次按住期间移动过 */
static u8          s_ignore_next_click = 0; /* 弹窗/下拉框刚打开，忽略下一次手指点击 */

static lv_obj_t *s_ctl_bg = NULL;        /* 弹窗半透明背板（点它关闭弹窗） */
static lv_obj_t *s_ctl_panel = NULL;     /* 控制弹窗面板 */
static lv_obj_t *s_ctl_btn_pause = NULL; /* 暂停/继续按钮（文字随状态切换） */

/* ---------------- 音乐界面下拉框 ---------------- */

/* 进音乐界面：下拉框默认隐藏，原位置放一个无边框的歌名 label */
static void music_dd_setup(lv_ui *ui)
{
    s_dd_shown = false;
    lv_obj_add_flag(ui->screen_music_ddlist_song, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_music_label_lyr_1, LV_OBJ_FLAG_HIDDEN); /* 第二行不再显示歌名 */

    s_song_label = lv_label_create(ui->screen_music); /* 随界面销毁，UNLOADED 置 NULL */
    lv_obj_set_pos(s_song_label, 5, 0);
    lv_obj_set_size(s_song_label, 230, 34);
    lv_label_set_long_mode(s_song_label, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_song_label, "未播放");
    lv_obj_set_style_text_font(s_song_label, &myFont_simkai16, 0);
    lv_obj_set_style_text_color(s_song_label, lv_color_hex(0x0D3055), 0);
    lv_obj_set_style_text_align(s_song_label, LV_TEXT_ALIGN_CENTER, 0); /* 歌名居中 */
    lv_obj_set_style_pad_top(s_song_label, 8, 0);
    lv_obj_set_style_pad_left(s_song_label, 0, 0);
}

/* 长按唤出/收起下拉框（再长按一次也可收起） */
static void music_dd_toggle(void)
{
    if (!lv_obj_is_valid(guider_ui.screen_music_ddlist_song))
        return;

    if (s_dd_shown)
    {
        lv_dropdown_close(guider_ui.screen_music_ddlist_song);
        lv_obj_add_flag(guider_ui.screen_music_ddlist_song, LV_OBJ_FLAG_HIDDEN);
        if (s_song_label != NULL && lv_obj_is_valid(s_song_label))
            lv_obj_remove_flag(s_song_label, LV_OBJ_FLAG_HIDDEN);
        s_dd_shown = false;
    }
    else
    {
        if (s_song_label != NULL && lv_obj_is_valid(s_song_label))
            lv_obj_add_flag(s_song_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(guider_ui.screen_music_ddlist_song, LV_OBJ_FLAG_HIDDEN);
        s_dd_shown = true;
        s_last_input = lv_tick_get();
    }
}

/* ---------------- 控制弹窗 ---------------- */

static void music_ctl_popup_show(void);
static void music_ctl_popup_hide(void);

/* 弹窗按钮操作后，同步弹窗按钮文字与音乐界面（若还在）的图标/歌名 */
static void music_ctl_sync_ui(void)
{
    if (s_ctl_btn_pause != NULL && lv_obj_is_valid(s_ctl_btn_pause))
        lv_label_set_text(lv_obj_get_child(s_ctl_btn_pause, 0),
                          (mus_state() == MUS_PLAYING) ? "暂停" : "继续");

    if (lv_obj_is_valid(guider_ui.screen_music))
    {
        music_update_icon(&guider_ui);
        music_update_song(&guider_ui);
    }
}

/* 停止播放并直接释放音乐界面（下次进音乐界面会重新创建） */
static void music_screen_release(void)
{
    if (s_music_timer != NULL)
    {
        lv_timer_del(s_music_timer);
        s_music_timer = NULL;
    }

    if (lv_obj_is_valid(guider_ui.screen_music))
    {
        if (lv_screen_active() == guider_ui.screen_music)
        {
            /* 正停在音乐界面（理论上不会发生：音乐界面长按是下拉框）：
               先按正常流程切走，UNLOADED + auto_del 会完成释放 */
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_main_1,
                                  guider_ui.screen_main_1_del, &guider_ui.screen_music_del,
                                  setup_scr_screen_main_1, LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
        }
        else
        {
            lv_obj_delete_async(guider_ui.screen_music);
        }
    }

    s_song_label = NULL;
    s_dd_shown = false;
    s_music_ui_state = -1;
    s_music_ui_song = -1;
    s_music_ui_lyric = -2;
    s_music_ui_sec = 0xFFFFFFFF;
}

static void ctl_btn_pause_cb(lv_event_t *e)
{
    (void)e;
    mus_toggle();
    music_ctl_sync_ui();
}

static void ctl_btn_prev_cb(lv_event_t *e)
{
    (void)e;
    mus_prev();
    music_ctl_sync_ui();
}

static void ctl_btn_next_cb(lv_event_t *e)
{
    (void)e;
    mus_next();
    music_ctl_sync_ui();
}

static void ctl_btn_exit_cb(lv_event_t *e)
{
    (void)e;
    mus_stop();
    music_ctl_popup_hide();
    music_screen_release();
}

static void ctl_btn_close_cb(lv_event_t *e)
{
    (void)e;
    if (s_ignore_next_click) { s_ignore_next_click = 0; return; }
    music_ctl_popup_hide();
}

static void ctl_bg_cb(lv_event_t *e)
{
    (void)e;
    if (s_ignore_next_click) { s_ignore_next_click = 0; return; }
    music_ctl_popup_hide();
}

static lv_obj_t *ctl_btn_create(lv_obj_t *parent, const char *txt, int32_t y,
                                uint32_t bg, lv_event_cb_t cb)
{
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_t *lb;

    lv_obj_set_pos(btn, 10, y);
    lv_obj_set_size(btn, 180, 36);
    lv_obj_set_style_bg_color(btn, lv_color_hex(bg), 0);
    lv_obj_set_style_radius(btn, 6, 0);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);

    lb = lv_label_create(btn);
    lv_label_set_text(lb, txt);
    lv_obj_set_style_text_font(lb, &myFont_simkai16, 0);
    lv_obj_set_style_text_color(lb, lv_color_hex(0xFFFFFF), 0);
    lv_obj_center(lb);
    return btn;
}

static void music_ctl_popup_create(void)
{
    lv_obj_t *title, *close, *x;

    /* 半透明背板：点击空白处关闭弹窗 */
    s_ctl_bg = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(s_ctl_bg);
    lv_obj_set_size(s_ctl_bg, 240, 280);
    lv_obj_set_style_bg_color(s_ctl_bg, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(s_ctl_bg, LV_OPA_50, 0);
    lv_obj_add_flag(s_ctl_bg, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(s_ctl_bg, ctl_bg_cb, LV_EVENT_CLICKED, NULL);

    s_ctl_panel = lv_obj_create(lv_layer_top());
    lv_obj_set_size(s_ctl_panel, 200, 232);
    lv_obj_set_pos(s_ctl_panel, 20, 24);
    lv_obj_set_style_bg_color(s_ctl_panel, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_radius(s_ctl_panel, 8, 0);
    lv_obj_set_style_border_width(s_ctl_panel, 1, 0);
    lv_obj_set_style_border_color(s_ctl_panel, lv_color_hex(0x2195F6), 0);
    lv_obj_set_style_pad_all(s_ctl_panel, 0, 0);

    title = lv_label_create(s_ctl_panel);
    lv_label_set_text(title, "音乐控制");
    lv_obj_set_style_text_font(title, &myFont_simkai16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x0D3055), 0);
    lv_obj_set_pos(title, 12, 10);

    close = lv_button_create(s_ctl_panel);
    lv_obj_set_pos(close, 164, 6);
    lv_obj_set_size(close, 28, 26);
    lv_obj_add_event_cb(close, ctl_btn_close_cb, LV_EVENT_CLICKED, NULL);
    x = lv_label_create(close);
    lv_label_set_text(x, LV_SYMBOL_CLOSE);
    lv_obj_set_style_text_font(x, &lv_font_montserratMedium_14, 0); /* 符号字体，simkai 里没有图标字形 */
    lv_obj_center(x);

    s_ctl_btn_pause = ctl_btn_create(s_ctl_panel, "暂停", 46, 0x2195F6, ctl_btn_pause_cb);
    ctl_btn_create(s_ctl_panel, "上一首", 88, 0x4A6A8A, ctl_btn_prev_cb);
    ctl_btn_create(s_ctl_panel, "下一首", 130, 0x4A6A8A, ctl_btn_next_cb);
    ctl_btn_create(s_ctl_panel, "退出播放", 172, 0xD05050, ctl_btn_exit_cb);
}

static void music_ctl_popup_show(void)
{
    if (s_ctl_panel == NULL)
        music_ctl_popup_create();
    lv_obj_remove_flag(s_ctl_bg, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(s_ctl_panel, LV_OBJ_FLAG_HIDDEN);
    music_ctl_sync_ui();
}

static void music_ctl_popup_hide(void)
{
    if (s_ctl_panel != NULL)
    {
        lv_obj_add_flag(s_ctl_panel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s_ctl_bg, LV_OBJ_FLAG_HIDDEN);
    }
}

/* ---------------- 长按检测与分发 ---------------- */

static void music_ctl_longpress(void)
{
    /* 注意：screen_music_del 在界面创建后不会被置回 false（GUI Guider 只在
       离开时置 true），不能用判断界面是否存在；用 lv_obj_is_valid 才可靠 */
    bool on_music = (lv_obj_is_valid(guider_ui.screen_music) &&
                     lv_screen_active() == guider_ui.screen_music);
    bool on_ebook = (lv_obj_is_valid(guider_ui.screen_ebook) &&
                     lv_screen_active() == guider_ui.screen_ebook);

    if (on_music)
        music_dd_toggle(); /* 音乐界面：长按唤出/收起下拉框 */
    else if (on_ebook)
        ebook_dd_toggle(); /* 电子书界面：长按唤出/收起书名下拉框 */
    else if (s_ctl_panel != NULL && !lv_obj_has_flag(s_ctl_panel, LV_OBJ_FLAG_HIDDEN))
        music_ctl_popup_hide(); /* 弹窗已开着：再长按收起 */
    else
        music_ctl_popup_show(); /* 其他界面：长按呼出控制弹窗 */

    /* 弹窗/下拉框正好生成在按住的手指下面，松手时 LVGL 会把 CLICKED
       派发给新出现的关闭按钮/背景。用 s_ignore_next_click 让关闭回调忽略
       这一次点击，等用户松手后再按一次才真正关闭。
       同时清掉长按状态：下一次手指按是全新长按计时 */
    s_ignore_next_click = 1;
    s_press_start = 0;
    s_lp_fired = false;
    s_lp_moved = false;
}

static void music_ctl_timer_cb(lv_timer_t *t)
{
    (void)t;
    lv_indev_t *in;
    lv_indev_t *pressed_in = NULL;
    uint32_t now = lv_tick_get();
    bool pressed = false;

    /* 遍历所有输入设备查按下状态。
       注意：不能依赖 lv_indev_active()——它只在 indev 处理期间可靠，
       定时器回调时可能为 NULL，导致长按永不触发 */
    for (in = lv_indev_get_next(NULL); in != NULL; in = lv_indev_get_next(in))
    {
        if (lv_indev_get_state(in) == LV_INDEV_STATE_PRESSED)
        {
            pressed = true;
            pressed_in = in;
            break;
        }
    }

    if (pressed)
    {
        s_rel_cnt = 0;
        s_last_input = now;
        if (s_press_start == 0)
        {
            s_press_start = now;
            s_lp_fired = false;
            s_lp_moved = false;
            if (pressed_in != NULL)
                lv_indev_get_point(pressed_in, &s_lp_pt);
        }
        else
        {
            /* 手指移动超过一定距离说明是在滑动翻页，不作长按处理，
               否则主页面来回快滑时也会弹出音乐控制窗口 */
            lv_point_t pt;
            if (pressed_in != NULL)
            {
                lv_indev_get_point(pressed_in, &pt);
                if (abs((int)pt.x - (int)s_lp_pt.x) > 20 ||
                    abs((int)pt.y - (int)s_lp_pt.y) > 20)
                    s_lp_moved = true;
            }

            if (!s_lp_fired && !s_lp_moved && now - s_press_start >= MUSIC_LP_MS)
            {
                s_lp_fired = true; /* 一次按住只触发一次 */
                printf("[music] long press\r\n");
                /* 不再调用 lv_indev_wait_release：它在 timer 回调里容易把
                   LVGL indev 状态机搞乱，导致手势/返回键/屏幕切换同时卡死。
                   改成用 s_ignore_next_click + 关闭回调的标志位过滤第一次点击 */
                music_ctl_longpress();
            }
        }
    }
    else if (s_press_start != 0)
    {
        /* 抗抖：触摸 I2C 偶发读取失败会造成短暂的 RELEASED，
           连续 3 次(300ms)才算真正松手，避免长按计时被重置 */
        if (++s_rel_cnt >= 3)
        {
            s_press_start = 0;
            s_rel_cnt = 0;
            s_ignore_next_click = 0; /* 真正松手后，下一次按压不再过滤 */
        }
    }

    /* 音乐/电子书界面的下拉框：15 秒无触摸自动收起 */
    if (s_dd_shown && lv_tick_elaps(s_last_input) >= MUSIC_DD_IDLE_MS)
    {
        if (lv_obj_is_valid(guider_ui.screen_ebook) &&
            lv_screen_active() == guider_ui.screen_ebook)
            ebook_dd_toggle();
        else
            music_dd_toggle();
    }
}

/* 由 LVGL 任务启动时调用一次（幂等） */
void music_ctl_init(void)
{
    if (s_ctl_timer == NULL)
        s_ctl_timer = lv_timer_create(music_ctl_timer_cb, 100, NULL);
}
