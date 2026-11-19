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

/* 状态指示灯：12x12 圆点，默认熄灭（灰色半透明），点亮样式由刷新定时器设置 */
static lv_obj_t *look_dot_create(lv_obj_t *parent, int x, int y)
{
    lv_obj_t *dot = lv_obj_create(parent);
    lv_obj_set_pos(dot, x, y);
    lv_obj_set_size(dot, 12, 12);
    lv_obj_remove_flag(dot, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(dot, 120, 0);
    lv_obj_set_style_bg_color(dot, lv_color_hex(0x9AA0A6), 0);
    lv_obj_set_style_border_width(dot, 1, 0);
    lv_obj_set_style_border_color(dot, lv_color_hex(0x7D838B), 0);
    lv_obj_set_style_pad_all(dot, 0, 0);
    lv_obj_set_style_shadow_width(dot, 0, 0);
    lv_obj_remove_flag(dot, LV_OBJ_FLAG_CLICKABLE);
    return dot;
}

/* 指示灯文字标签 */
static void look_status_label_create(lv_obj_t *parent, int x, int y, int w, const char *text)
{
    lv_obj_t *lbl = lv_label_create(parent);
    lv_obj_set_pos(lbl, x, y);
    lv_obj_set_size(lbl, w, 18);
    lv_label_set_text(lbl, text);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_font(lbl, &myFont_simkai16, 0);
    lv_obj_set_style_text_color(lbl, lv_color_hex(0x0D3055), 0);
    lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_pad_all(lbl, 0, 0);
}

void setup_scr_screen_look(lv_ui *ui)
{
    // Write codes screen_look
    ui->screen_look = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_look, 240, 280);
    lv_obj_set_scrollbar_mode(ui->screen_look, LV_SCROLLBAR_MODE_OFF);

    // Write style for screen_look, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_look, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen_look, PIC_BACK_240280, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_look, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_look, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_look_bar_temp
    ui->screen_look_bar_temp = lv_bar_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_bar_temp, 52, 14);
    lv_obj_set_size(ui->screen_look_bar_temp, 100, 12);
    lv_obj_set_style_anim_duration(ui->screen_look_bar_temp, 300, 0);
    lv_bar_set_mode(ui->screen_look_bar_temp, LV_BAR_MODE_NORMAL);
    lv_bar_set_range(ui->screen_look_bar_temp, 0, 100);
    lv_bar_set_value(ui->screen_look_bar_temp, 26, LV_ANIM_OFF);

    // Write style for screen_look_bar_temp, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_look_bar_temp, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_look_bar_temp, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_look_bar_temp, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_bar_temp, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_bar_temp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_look_bar_temp, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_look_bar_temp, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_look_bar_temp, lv_color_hex(0x2195f6), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_look_bar_temp, LV_GRAD_DIR_NONE, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_bar_temp, 10, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // Write codes screen_look_bar_hum
    ui->screen_look_bar_hum = lv_bar_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_bar_hum, 52, 56);
    lv_obj_set_size(ui->screen_look_bar_hum, 100, 12);
    lv_obj_set_style_anim_duration(ui->screen_look_bar_hum, 300, 0);
    lv_bar_set_mode(ui->screen_look_bar_hum, LV_BAR_MODE_NORMAL);
    lv_bar_set_range(ui->screen_look_bar_hum, 0, 100);
    lv_bar_set_value(ui->screen_look_bar_hum, 67, LV_ANIM_OFF);

    // Write style for screen_look_bar_hum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_look_bar_hum, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_look_bar_hum, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_look_bar_hum, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_bar_hum, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_bar_hum, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_look_bar_hum, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_look_bar_hum, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_look_bar_hum, lv_color_hex(0x2195f6), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_look_bar_hum, LV_GRAD_DIR_NONE, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_bar_hum, 10, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // Write codes screen_look_bar_lig
    ui->screen_look_bar_lig = lv_bar_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_bar_lig, 52, 98);
    lv_obj_set_size(ui->screen_look_bar_lig, 100, 12);
    lv_obj_set_style_anim_duration(ui->screen_look_bar_lig, 300, 0);
    lv_bar_set_mode(ui->screen_look_bar_lig, LV_BAR_MODE_NORMAL);
    lv_bar_set_range(ui->screen_look_bar_lig, 0, 100);
    lv_bar_set_value(ui->screen_look_bar_lig, 66, LV_ANIM_OFF);

    // Write style for screen_look_bar_lig, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_look_bar_lig, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_look_bar_lig, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_look_bar_lig, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_bar_lig, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_bar_lig, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_look_bar_lig, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_look_bar_lig, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_look_bar_lig, lv_color_hex(0x2195f6), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_look_bar_lig, LV_GRAD_DIR_NONE, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_bar_lig, 10, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // Write codes screen_look_bar_gear
    ui->screen_look_bar_gear = lv_bar_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_bar_gear, 52, 140);
    lv_obj_set_size(ui->screen_look_bar_gear, 100, 12);
    lv_obj_set_style_anim_duration(ui->screen_look_bar_gear, 300, 0);
    lv_bar_set_mode(ui->screen_look_bar_gear, LV_BAR_MODE_NORMAL);
    lv_bar_set_range(ui->screen_look_bar_gear, 0, 3);
    lv_bar_set_value(ui->screen_look_bar_gear, 0, LV_ANIM_OFF);

    // Write style for screen_look_bar_gear, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_look_bar_gear, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_look_bar_gear, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_look_bar_gear, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_bar_gear, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_bar_gear, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_look_bar_gear, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_look_bar_gear, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_look_bar_gear, lv_color_hex(0x2195f6), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_look_bar_gear, LV_GRAD_DIR_NONE, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_bar_gear, 10, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // Write codes screen_look_bar_angle
    ui->screen_look_bar_angle = lv_bar_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_bar_angle, 52, 182);
    lv_obj_set_size(ui->screen_look_bar_angle, 100, 12);
    lv_obj_set_style_anim_duration(ui->screen_look_bar_angle, 300, 0);
    lv_bar_set_mode(ui->screen_look_bar_angle, LV_BAR_MODE_NORMAL);
    lv_bar_set_range(ui->screen_look_bar_angle, 0, 180);
    lv_bar_set_value(ui->screen_look_bar_angle, 0, LV_ANIM_OFF);

    // Write style for screen_look_bar_angle, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_look_bar_angle, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_look_bar_angle, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_look_bar_angle, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_bar_angle, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_bar_angle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_look_bar_angle, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_look_bar_angle, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_look_bar_angle, lv_color_hex(0x2195f6), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_look_bar_angle, LV_GRAD_DIR_NONE, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_bar_angle, 10, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // Write codes screen_look_label_temp
    ui->screen_look_label_temp = lv_label_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_label_temp, 5, 10);
    lv_obj_set_size(ui->screen_look_label_temp, 40, 20);
    lv_label_set_text(ui->screen_look_label_temp, "温度");
    lv_label_set_long_mode(ui->screen_look_label_temp, LV_LABEL_LONG_WRAP);

    // Write style for screen_look_label_temp, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_look_label_temp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_label_temp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_look_label_temp, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_look_label_temp, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_look_label_temp, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_look_label_temp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_look_label_temp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_look_label_temp, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_look_label_temp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_look_label_temp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_look_label_temp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_look_label_temp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_look_label_temp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_label_temp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_look_label_hum
    ui->screen_look_label_hum = lv_label_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_label_hum, 5, 52);
    lv_obj_set_size(ui->screen_look_label_hum, 40, 20);
    lv_label_set_text(ui->screen_look_label_hum, "湿度");
    lv_label_set_long_mode(ui->screen_look_label_hum, LV_LABEL_LONG_WRAP);

    // Write style for screen_look_label_hum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_look_label_hum, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_label_hum, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_look_label_hum, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_look_label_hum, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_look_label_hum, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_look_label_hum, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_look_label_hum, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_look_label_hum, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_look_label_hum, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_look_label_hum, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_look_label_hum, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_look_label_hum, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_look_label_hum, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_label_hum, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_look_label_lig
    ui->screen_look_label_lig = lv_label_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_label_lig, 5, 94);
    lv_obj_set_size(ui->screen_look_label_lig, 40, 20);
    lv_label_set_text(ui->screen_look_label_lig, "光照");
    lv_label_set_long_mode(ui->screen_look_label_lig, LV_LABEL_LONG_WRAP);

    // Write style for screen_look_label_lig, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_look_label_lig, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_label_lig, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_look_label_lig, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_look_label_lig, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_look_label_lig, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_look_label_lig, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_look_label_lig, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_look_label_lig, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_look_label_lig, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_look_label_lig, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_look_label_lig, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_look_label_lig, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_look_label_lig, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_label_lig, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_look_label_angle
    ui->screen_look_label_angle = lv_label_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_label_angle, 5, 178);
    lv_obj_set_size(ui->screen_look_label_angle, 40, 20);
    lv_label_set_text(ui->screen_look_label_angle, "舵机");
    lv_label_set_long_mode(ui->screen_look_label_angle, LV_LABEL_LONG_WRAP);

    // Write style for screen_look_label_angle, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_look_label_angle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_label_angle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_look_label_angle, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_look_label_angle, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_look_label_angle, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_look_label_angle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_look_label_angle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_look_label_angle, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_look_label_angle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_look_label_angle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_look_label_angle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_look_label_angle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_look_label_angle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_label_angle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_look_label_gear
    ui->screen_look_label_gear = lv_label_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_label_gear, 5, 136);
    lv_obj_set_size(ui->screen_look_label_gear, 40, 20);
    lv_label_set_text(ui->screen_look_label_gear, "风扇");
    lv_label_set_long_mode(ui->screen_look_label_gear, LV_LABEL_LONG_WRAP);

    // Write style for screen_look_label_gear, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_look_label_gear, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_label_gear, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_look_label_gear, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_look_label_gear, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_look_label_gear, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_look_label_gear, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_look_label_gear, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_look_label_gear, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_look_label_gear, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_look_label_gear, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_look_label_gear, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_look_label_gear, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_look_label_gear, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_label_gear, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_look_label_angle_vol
    ui->screen_look_label_angle_vol = lv_label_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_label_angle_vol, 162, 178);
    lv_obj_set_size(ui->screen_look_label_angle_vol, 44, 20);
    lv_label_set_text(ui->screen_look_label_angle_vol, "0度");
    lv_label_set_long_mode(ui->screen_look_label_angle_vol, LV_LABEL_LONG_WRAP);

    // Write style for screen_look_label_angle_vol, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_look_label_angle_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_label_angle_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_look_label_angle_vol, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_look_label_angle_vol, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_look_label_angle_vol, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_look_label_angle_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_look_label_angle_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_look_label_angle_vol, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_look_label_angle_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_look_label_angle_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_look_label_angle_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_look_label_angle_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_look_label_angle_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_label_angle_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_look_label_gear_vol
    ui->screen_look_label_gear_vol = lv_label_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_label_gear_vol, 162, 136);
    lv_obj_set_size(ui->screen_look_label_gear_vol, 44, 20);
    lv_label_set_text(ui->screen_look_label_gear_vol, "0挡");
    lv_label_set_long_mode(ui->screen_look_label_gear_vol, LV_LABEL_LONG_WRAP);

    // Write style for screen_look_label_gear_vol, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_look_label_gear_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_label_gear_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_look_label_gear_vol, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_look_label_gear_vol, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_look_label_gear_vol, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_look_label_gear_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_look_label_gear_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_look_label_gear_vol, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_look_label_gear_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_look_label_gear_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_look_label_gear_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_look_label_gear_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_look_label_gear_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_label_gear_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_look_label_lig_vol
    ui->screen_look_label_lig_vol = lv_label_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_label_lig_vol, 162, 94);
    lv_obj_set_size(ui->screen_look_label_lig_vol, 44, 20);
    lv_label_set_text(ui->screen_look_label_lig_vol, "70%");
    lv_label_set_long_mode(ui->screen_look_label_lig_vol, LV_LABEL_LONG_WRAP);

    // Write style for screen_look_label_lig_vol, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_look_label_lig_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_label_lig_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_look_label_lig_vol, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_look_label_lig_vol, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_look_label_lig_vol, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_look_label_lig_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_look_label_lig_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_look_label_lig_vol, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_look_label_lig_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_look_label_lig_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_look_label_lig_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_look_label_lig_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_look_label_lig_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_label_lig_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_look_label_hum_vol
    ui->screen_look_label_hum_vol = lv_label_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_label_hum_vol, 162, 52);
    lv_obj_set_size(ui->screen_look_label_hum_vol, 44, 20);
    lv_label_set_text(ui->screen_look_label_hum_vol, "66%");
    lv_label_set_long_mode(ui->screen_look_label_hum_vol, LV_LABEL_LONG_WRAP);

    // Write style for screen_look_label_hum_vol, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_look_label_hum_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_label_hum_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_look_label_hum_vol, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_look_label_hum_vol, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_look_label_hum_vol, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_look_label_hum_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_look_label_hum_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_look_label_hum_vol, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_look_label_hum_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_look_label_hum_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_look_label_hum_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_look_label_hum_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_look_label_hum_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_label_hum_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_look_label_temp_vol
    ui->screen_look_label_temp_vol = lv_label_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_label_temp_vol, 162, 10);
    lv_obj_set_size(ui->screen_look_label_temp_vol, 44, 20);
    lv_label_set_text(ui->screen_look_label_temp_vol, "26度");
    lv_label_set_long_mode(ui->screen_look_label_temp_vol, LV_LABEL_LONG_WRAP);

    // Write style for screen_look_label_temp_vol, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_look_label_temp_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_label_temp_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_look_label_temp_vol, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_look_label_temp_vol, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_look_label_temp_vol, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_look_label_temp_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_look_label_temp_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_look_label_temp_vol, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_look_label_temp_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_look_label_temp_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_look_label_temp_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_look_label_temp_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_look_label_temp_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_label_temp_vol, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_look_label_mpu
    ui->screen_look_label_mpu = lv_label_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_label_mpu, 5, 212);
    lv_obj_set_size(ui->screen_look_label_mpu, 48, 20);
    lv_label_set_text(ui->screen_look_label_mpu, "陀螺仪");
    lv_label_set_long_mode(ui->screen_look_label_mpu, LV_LABEL_LONG_WRAP);

    // Write style for screen_look_label_mpu, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_look_label_mpu, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_label_mpu, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_look_label_mpu, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_look_label_mpu, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_look_label_mpu, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_look_label_mpu, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_look_label_mpu, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_look_label_mpu, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_look_label_mpu, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_look_label_mpu, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_look_label_mpu, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_look_label_mpu, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_look_label_mpu, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_label_mpu, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_look_label_mpu_z
    ui->screen_look_label_mpu_z = lv_label_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_label_mpu_z, 178, 212);
    lv_obj_set_size(ui->screen_look_label_mpu_z, 58, 20);
    lv_label_set_text(ui->screen_look_label_mpu_z, "0");
    lv_label_set_long_mode(ui->screen_look_label_mpu_z, LV_LABEL_LONG_WRAP);

    // Write style for screen_look_label_mpu_z, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_look_label_mpu_z, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_label_mpu_z, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_look_label_mpu_z, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_look_label_mpu_z, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_look_label_mpu_z, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_look_label_mpu_z, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_look_label_mpu_z, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_look_label_mpu_z, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_look_label_mpu_z, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_look_label_mpu_z, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_look_label_mpu_z, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_look_label_mpu_z, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_look_label_mpu_z, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_label_mpu_z, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_look_label_mpu_x
    ui->screen_look_label_mpu_x = lv_label_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_label_mpu_x, 58, 212);
    lv_obj_set_size(ui->screen_look_label_mpu_x, 58, 20);
    lv_label_set_text(ui->screen_look_label_mpu_x, "0");
    lv_label_set_long_mode(ui->screen_look_label_mpu_x, LV_LABEL_LONG_WRAP);

    // Write style for screen_look_label_mpu_x, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_look_label_mpu_x, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_label_mpu_x, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_look_label_mpu_x, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_look_label_mpu_x, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_look_label_mpu_x, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_look_label_mpu_x, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_look_label_mpu_x, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_look_label_mpu_x, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_look_label_mpu_x, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_look_label_mpu_x, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_look_label_mpu_x, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_look_label_mpu_x, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_look_label_mpu_x, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_label_mpu_x, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_look_label_mpu_y
    ui->screen_look_label_mpu_y = lv_label_create(ui->screen_look);
    lv_obj_set_pos(ui->screen_look_label_mpu_y, 118, 212);
    lv_obj_set_size(ui->screen_look_label_mpu_y, 58, 20);
    lv_label_set_text(ui->screen_look_label_mpu_y, "0");
    lv_label_set_long_mode(ui->screen_look_label_mpu_y, LV_LABEL_LONG_WRAP);

    // Write style for screen_look_label_mpu_y, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_look_label_mpu_y, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_look_label_mpu_y, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_look_label_mpu_y, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_look_label_mpu_y, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_look_label_mpu_y, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_look_label_mpu_y, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_look_label_mpu_y, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_look_label_mpu_y, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_look_label_mpu_y, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_look_label_mpu_y, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_look_label_mpu_y, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_look_label_mpu_y, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_look_label_mpu_y, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_look_label_mpu_y, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ---- LED1/LED2/LED3/继电器 状态指示灯（底部一行，4 项等距分布，不与陀螺仪行重叠）----
    ui->screen_look_led1_dot = look_dot_create(ui->screen_look, 6, 250);
    look_status_label_create(ui->screen_look, 21, 246, 38, "LED1");
    ui->screen_look_led2_dot = look_dot_create(ui->screen_look, 61, 250);
    look_status_label_create(ui->screen_look, 76, 246, 38, "LED2");
    ui->screen_look_led3_dot = look_dot_create(ui->screen_look, 116, 250);
    look_status_label_create(ui->screen_look, 131, 246, 38, "LED3");
    ui->screen_look_relay_dot = look_dot_create(ui->screen_look, 171, 250);
    look_status_label_create(ui->screen_look, 186, 246, 50, "继电器");

    // The custom code of screen_look.

    // Update current screen layout.
    lv_obj_update_layout(ui->screen_look);

    // Init events for screen.
    events_init_screen_look(ui);
}
