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
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"

/* 返回按钮：复用 main_2 的顶部按钮写法 */
static lv_obj_t *clk_btn_top(lv_obj_t *parent, const char *text)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 32, 24);
    lv_obj_set_pos(btn, 4, 4);
    lv_obj_set_style_bg_opa(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    lv_obj_set_style_text_font(label, &lv_font_montserratMedium_12, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x0D3055), 0);
    return btn;
}

void setup_scr_screen_clk(lv_ui *ui)
{
    /* ================== 屏幕 ================== */
    ui->screen_clk = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_clk, 240, 280);
    lv_obj_set_scrollbar_mode(ui->screen_clk, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(ui->screen_clk, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen_clk, PIC_BACK_240280, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_clk, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_clk, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_clk, 0, LV_PART_MAIN);

    /* 返回按钮（左上角） */
    ui->screen_clk_btn_back = clk_btn_top(ui->screen_clk, LV_SYMBOL_LEFT);
    lv_obj_add_event_cb(ui->screen_clk_btn_back, NULL, LV_EVENT_ALL, ui); /* 真正事件在 events_init_screen_clk 里统一加 */

    /* 标题 */
    ui->screen_clk_title = lv_label_create(ui->screen_clk);
    lv_obj_set_pos(ui->screen_clk_title, 0, 4);
    lv_obj_set_size(ui->screen_clk_title, 240, 24);
    lv_label_set_text(ui->screen_clk_title, "时钟");
    lv_obj_set_style_text_align(ui->screen_clk_title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(ui->screen_clk_title, lv_color_hex(0x0D3055), 0);
    lv_obj_set_style_text_font(ui->screen_clk_title, &myFont_simkai16, 0);

    /* 顶部分割线 */
    lv_obj_t *line = lv_line_create(ui->screen_clk);
    static const lv_point_precise_t pts_top_line[] = {{5, 32}, {235, 32}};
    lv_line_set_points(line, pts_top_line, 2);
    lv_obj_set_style_line_color(line, lv_color_hex(0x0D3055), 0);
    lv_obj_set_style_line_opa(line, 180, 0);
    lv_obj_set_style_line_width(line, 1, 0);

    /* ================== 模拟表盘 ==================
       比原版本小一点（120px），留空间给下面的数字时钟 + 日期。
       指针：时针粗短深灰、分针中等青绿、秒针细长红（高对比秒针好认）。 */
    static const char *hour_ticks[] = {"12","1","2","3","4","5","6","7","8","9","10","11", NULL};
    ui->screen_clk_analog_clock_1 = lv_scale_create(ui->screen_clk);
    lv_obj_set_pos(ui->screen_clk_analog_clock_1, 60, 44);
    lv_obj_set_size(ui->screen_clk_analog_clock_1, 120, 120);
    lv_scale_set_mode(ui->screen_clk_analog_clock_1, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_angle_range(ui->screen_clk_analog_clock_1, 360U);
    lv_scale_set_range(ui->screen_clk_analog_clock_1, 0U, 60U);
    lv_scale_set_rotation(ui->screen_clk_analog_clock_1, 270U);
    lv_obj_set_style_radius(ui->screen_clk_analog_clock_1, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_clip_corner(ui->screen_clk_analog_clock_1, true, LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui->screen_clk_analog_clock_1, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui->screen_clk_analog_clock_1, 0, LV_PART_MAIN); /* 透明，叠在背景图上 */
    lv_obj_set_style_border_width(ui->screen_clk_analog_clock_1, 0, LV_PART_MAIN);
    lv_scale_set_total_tick_count(ui->screen_clk_analog_clock_1, 61);
    lv_scale_set_major_tick_every(ui->screen_clk_analog_clock_1, 5);
    lv_scale_set_text_src(ui->screen_clk_analog_clock_1, hour_ticks);

    /* 表盘外圈细线（代替原来的 bg_color=white，更清爽） */
    lv_obj_set_style_line_width(ui->screen_clk_analog_clock_1, 1, LV_PART_INDICATOR);
    lv_obj_set_style_line_color(ui->screen_clk_analog_clock_1, lv_color_hex(0x0D3055), LV_PART_INDICATOR);
    lv_obj_set_style_line_opa(ui->screen_clk_analog_clock_1, 200, LV_PART_INDICATOR);
    lv_obj_set_style_line_rounded(ui->screen_clk_analog_clock_1, true, LV_PART_INDICATOR);
    lv_obj_set_style_text_color(ui->screen_clk_analog_clock_1, lv_color_hex(0x0D3055), LV_PART_INDICATOR);
    lv_obj_set_style_text_font(ui->screen_clk_analog_clock_1, &lv_font_montserratMedium_12, LV_PART_INDICATOR);

    /* 小时刻度（黑、长度 8） */
    lv_obj_set_style_line_width(ui->screen_clk_analog_clock_1, 2, LV_PART_ITEMS);
    lv_obj_set_style_line_color(ui->screen_clk_analog_clock_1, lv_color_hex(0x0D3055), LV_PART_ITEMS);
    lv_obj_set_style_line_opa(ui->screen_clk_analog_clock_1, 230, LV_PART_ITEMS);
    lv_obj_set_style_line_rounded(ui->screen_clk_analog_clock_1, true, LV_PART_ITEMS);
    lv_obj_set_style_length(ui->screen_clk_analog_clock_1, 8, LV_PART_ITEMS);

    lv_obj_update_layout(ui->screen_clk_analog_clock_1);

    /* 时针：粗 3, 长度 24, 深灰绿 */
    ui->screen_clk_analog_clock_1_hour_needle = lv_line_create(ui->screen_clk_analog_clock_1);
    lv_obj_set_style_line_width(ui->screen_clk_analog_clock_1_hour_needle, 3, LV_PART_MAIN);
    lv_obj_set_style_line_color(ui->screen_clk_analog_clock_1_hour_needle, lv_color_hex(0x2E6B4F), LV_PART_MAIN);
    lv_obj_set_style_line_rounded(ui->screen_clk_analog_clock_1_hour_needle, true, LV_PART_MAIN);
    lv_scale_set_line_needle_value(ui->screen_clk_analog_clock_1, ui->screen_clk_analog_clock_1_hour_needle, 24, 0);

    /* 分针：粗 2, 长度 36, 青绿 */
    ui->screen_clk_analog_clock_1_min_needle = lv_line_create(ui->screen_clk_analog_clock_1);
    lv_obj_set_style_line_width(ui->screen_clk_analog_clock_1_min_needle, 2, LV_PART_MAIN);
    lv_obj_set_style_line_color(ui->screen_clk_analog_clock_1_min_needle, lv_color_hex(0x2EA870), LV_PART_MAIN);
    lv_obj_set_style_line_rounded(ui->screen_clk_analog_clock_1_min_needle, true, LV_PART_MAIN);
    lv_scale_set_line_needle_value(ui->screen_clk_analog_clock_1, ui->screen_clk_analog_clock_1_min_needle, 36, 0);

    /* 秒针：粗 1, 长度 50, 亮红 */
    ui->screen_clk_analog_clock_1_sec_needle = lv_line_create(ui->screen_clk_analog_clock_1);
    lv_obj_set_style_line_width(ui->screen_clk_analog_clock_1_sec_needle, 1, LV_PART_MAIN);
    lv_obj_set_style_line_color(ui->screen_clk_analog_clock_1_sec_needle, lv_color_hex(0xE13D3D), LV_PART_MAIN);
    lv_obj_set_style_line_rounded(ui->screen_clk_analog_clock_1_sec_needle, true, LV_PART_MAIN);
    lv_scale_set_line_needle_value(ui->screen_clk_analog_clock_1, ui->screen_clk_analog_clock_1_sec_needle, 50, 0);

    /* 表盘中心圆点 */
    lv_obj_t *dot = lv_obj_create(ui->screen_clk_analog_clock_1);
    lv_obj_set_size(dot, 8, 8);
    lv_obj_set_style_bg_color(dot, lv_color_hex(0xE13D3D), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(dot, 255, LV_PART_MAIN);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_width(dot, 0, LV_PART_MAIN);
    lv_obj_align(dot, LV_ALIGN_CENTER, 0, 0);

    /* ================== 分隔线 ================== */
    lv_obj_t *sep = lv_line_create(ui->screen_clk);
    static const lv_point_precise_t pts_sep[] = {{10, 175}, {230, 175}};
    lv_line_set_points(sep, pts_sep, 2);
    lv_obj_set_style_line_color(sep, lv_color_hex(0x0D3055), 0);
    lv_obj_set_style_line_opa(sep, 120, 0);
    lv_obj_set_style_line_width(sep, 1, 0);

    /* ================== 数字时钟容器（240×46 深蓝半透明底框，Montserrat 28 白字） */
    {
        lv_obj_t *box = lv_obj_create(ui->screen_clk);
        lv_obj_set_pos(box, 8, 182);
        lv_obj_set_size(box, 224, 46);
        lv_obj_set_style_radius(box, 6, 0);
        lv_obj_set_style_bg_color(box, lv_color_hex(0x0D3055), 0);
        lv_obj_set_style_bg_opa(box, 200, 0);
        lv_obj_set_style_border_width(box, 0, 0);
        lv_obj_set_style_pad_all(box, 0, 0);
        lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE);

        ui->screen_clk_digital_clock_1 = lv_label_create(box);
        lv_label_set_text(ui->screen_clk_digital_clock_1, "--:--:--");
        lv_obj_center(ui->screen_clk_digital_clock_1);
        lv_obj_set_style_text_align(ui->screen_clk_digital_clock_1, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(ui->screen_clk_digital_clock_1, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_text_font(ui->screen_clk_digital_clock_1, &lv_font_montserrat_28, 0);
        lv_obj_set_style_border_width(ui->screen_clk_digital_clock_1, 0, 0);
        lv_obj_set_style_bg_opa(ui->screen_clk_digital_clock_1, 0, 0);
    }

    /* ================== 日期容器（240×28 浅蓝半透明底框，montserratMedium_16 白字） */
    {
        lv_obj_t *box = lv_obj_create(ui->screen_clk);
        lv_obj_set_pos(box, 8, 236);
        lv_obj_set_size(box, 224, 30);
        lv_obj_set_style_radius(box, 6, 0);
        lv_obj_set_style_bg_color(box, lv_color_hex(0x2195F6), 0);
        lv_obj_set_style_bg_opa(box, 210, 0);
        lv_obj_set_style_border_width(box, 0, 0);
        lv_obj_set_style_pad_all(box, 0, 0);
        lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE);

        ui->screen_clk_datetext_1 = lv_label_create(box);
        lv_label_set_text(ui->screen_clk_datetext_1, "----/--/--");
        lv_obj_center(ui->screen_clk_datetext_1);
        lv_obj_set_style_text_align(ui->screen_clk_datetext_1, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(ui->screen_clk_datetext_1, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_text_font(ui->screen_clk_datetext_1, &myFont_simkai16, 0);
        lv_obj_set_style_border_width(ui->screen_clk_datetext_1, 0, 0);
        lv_obj_set_style_bg_opa(ui->screen_clk_datetext_1, 0, 0);
    }

    lv_obj_update_layout(ui->screen_clk);
    events_init_screen_clk(ui);
}
