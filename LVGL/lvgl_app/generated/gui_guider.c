/*
 * Copyright 2026 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "widgets_init.h"

void ui_init_style(lv_style_t *style)
{
    if (style->prop_cnt > 1)
        lv_style_reset(style);
    else
        lv_style_init(style);
}

void ui_load_scr_animation(lv_ui *ui, lv_obj_t **new_scr, bool new_scr_del, bool *old_scr_del, ui_setup_scr_t setup_scr,
                           lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del)
{
    lv_obj_t *old_scr;

    LV_UNUSED(anim_type);
    LV_UNUSED(time);
    LV_UNUSED(delay);
    LV_UNUSED(is_clean);

    if (new_scr_del || *new_scr == NULL)
    {
        setup_scr(ui);
    }

    old_scr = lv_screen_active();

    /* 直接切换，不使用过渡动画：
       1) 过渡期间旧界面仍然可见，原实现还会先 lv_obj_clean(旧界面)，
          导致离开时钟等界面时出现“图片突然消失”；
       2) 带 delay 的过渡会同时存在两个完整界面，连续快速滑动时内存被撑爆，
          随后分配到 NULL 对象，表现为整屏卡死。
       本工程所有调用点本来就使用 LV_SCR_LOAD_ANIM_NONE，所以没有视觉损失。 */
    lv_screen_load(*new_scr);

    /* 延迟删除旧界面，避免在它自己的事件回调里同步释放自己造成野指针。 */
    if (auto_del && old_scr != NULL && old_scr != *new_scr)
    {
        lv_obj_delete_async(old_scr);
    }

    *old_scr_del = auto_del;
}

void ui_animation(void *var, uint32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                  uint32_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                  lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_completed_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb)
{
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, var);
    lv_anim_set_exec_cb(&anim, exec_cb);
    lv_anim_set_values(&anim, start_value, end_value);
    lv_anim_set_time(&anim, duration);
    lv_anim_set_delay(&anim, delay);
    lv_anim_set_path_cb(&anim, path_cb);
    lv_anim_set_repeat_count(&anim, repeat_cnt);
    lv_anim_set_repeat_delay(&anim, repeat_delay);
    lv_anim_set_playback_time(&anim, playback_time);
    lv_anim_set_playback_delay(&anim, playback_delay);
    if (start_cb)
    {
        lv_anim_set_start_cb(&anim, start_cb);
    }
    if (ready_cb)
    {
        lv_anim_set_completed_cb(&anim, ready_cb);
    }
    if (deleted_cb)
    {
        lv_anim_set_deleted_cb(&anim, deleted_cb);
    }
    lv_anim_start(&anim);
}

void init_scr_del_flag(lv_ui *ui)
{

    ui->screen_main_1_del = true;
    ui->screen_main_2_del = true;
    ui->screen_app_del = true;
    ui->screen_cal_del = true;
    ui->screen_clk_del = true;
    ui->screen_ebook_del = true;
    ui->screen_look_del = true;
    ui->screen_music_del = true;
    ui->screen_pic_del = true;
    ui->screen_note_del = true;
    ui->screen_file_del = true;
    ui->screen_news_del = true;
    ui->screen_wifi_del = true;
}

void setup_bottom_layer(void)
{
    lv_theme_apply(lv_layer_bottom());
}

void setup_ui(lv_ui *ui)
{
    setup_bottom_layer();
    init_scr_del_flag(ui);
    init_keyboard(ui);
    setup_scr_screen_main_1(ui);
    lv_screen_load(ui->screen_main_1);
}

void video_play(lv_ui *ui)
{
}

void init_keyboard(lv_ui *ui)
{
}
