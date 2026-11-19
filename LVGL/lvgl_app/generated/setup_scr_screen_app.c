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

void setup_scr_screen_app(lv_ui *ui)
{
    // Write codes screen_app
    ui->screen_app = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_app, 240, 280);
    lv_obj_set_scrollbar_mode(ui->screen_app, LV_SCROLLBAR_MODE_OFF);

    // Write style for screen_app, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen_app, PIC_BACK_240280, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_app, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_app, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_app_roller_fan
    ui->screen_app_roller_fan = lv_roller_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_roller_fan, 175, 78);
    lv_obj_set_size(ui->screen_app_roller_fan, 60, 32);
    lv_roller_set_visible_row_count(ui->screen_app_roller_fan, 1);
    lv_roller_set_options(ui->screen_app_roller_fan, "0\n1\n2\n3", LV_ROLLER_MODE_INFINITE);

    // Write style for screen_app_roller_fan, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_app_roller_fan, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_roller_fan, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_roller_fan, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_roller_fan, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_roller_fan, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_roller_fan, &lv_font_montserratMedium_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_roller_fan, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_roller_fan, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_app_roller_fan, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_app_roller_fan, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_app_roller_fan, lv_color_hex(0xe6e6e6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_app_roller_fan, LV_BORDER_SIDE_FULL, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_app_roller_fan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_app_roller_fan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_roller_fan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_app_roller_fan, Part: LV_PART_SELECTED, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_roller_fan, 255, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_roller_fan, lv_color_hex(0x2195f6), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_roller_fan, LV_GRAD_DIR_NONE, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_roller_fan, lv_color_hex(0xFFFFFF), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_roller_fan, &lv_font_montserratMedium_12, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_roller_fan, 255, LV_PART_SELECTED | LV_STATE_DEFAULT);

    lv_roller_set_visible_row_count(ui->screen_app_roller_fan, 1);
    // Write codes screen_app_sw_led3
    ui->screen_app_sw_led3 = lv_switch_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_sw_led3, 165, 15);
    lv_obj_set_size(ui->screen_app_sw_led3, 50, 32);

    // Write style for screen_app_sw_led3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_sw_led3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_sw_led3, lv_color_hex(0xe6e2e6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_sw_led3, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_app_sw_led3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_sw_led3, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_sw_led3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_app_sw_led3, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_app_sw_led3, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->screen_app_sw_led3, lv_color_hex(0x2195f6), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_sw_led3, LV_GRAD_DIR_NONE, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_app_sw_led3, 0, LV_PART_INDICATOR | LV_STATE_CHECKED);

    // Write style for screen_app_sw_led3, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_sw_led3, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_sw_led3, lv_color_hex(0xffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_sw_led3, LV_GRAD_DIR_NONE, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_app_sw_led3, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_sw_led3, 10, LV_PART_KNOB | LV_STATE_DEFAULT);

    // Write codes screen_app_sw_led2
    ui->screen_app_sw_led2 = lv_switch_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_sw_led2, 95, 15);
    lv_obj_set_size(ui->screen_app_sw_led2, 50, 32);

    // Write style for screen_app_sw_led2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_sw_led2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_sw_led2, lv_color_hex(0xe6e2e6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_sw_led2, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_app_sw_led2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_sw_led2, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_sw_led2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_app_sw_led2, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_app_sw_led2, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->screen_app_sw_led2, lv_color_hex(0x2195f6), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_sw_led2, LV_GRAD_DIR_NONE, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_app_sw_led2, 0, LV_PART_INDICATOR | LV_STATE_CHECKED);

    // Write style for screen_app_sw_led2, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_sw_led2, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_sw_led2, lv_color_hex(0xffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_sw_led2, LV_GRAD_DIR_NONE, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_app_sw_led2, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_sw_led2, 10, LV_PART_KNOB | LV_STATE_DEFAULT);

    // Write codes screen_app_sw_led1
    ui->screen_app_sw_led1 = lv_switch_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_sw_led1, 5, 15);
    lv_obj_set_size(ui->screen_app_sw_led1, 50, 32);

    // Write style for screen_app_sw_led1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_sw_led1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_sw_led1, lv_color_hex(0xe6e2e6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_sw_led1, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_app_sw_led1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_sw_led1, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_sw_led1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_app_sw_led1, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_app_sw_led1, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->screen_app_sw_led1, lv_color_hex(0x2195f6), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_sw_led1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_app_sw_led1, 0, LV_PART_INDICATOR | LV_STATE_CHECKED);

    // Write style for screen_app_sw_led1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_sw_led1, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_sw_led1, lv_color_hex(0xffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_sw_led1, LV_GRAD_DIR_NONE, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_app_sw_led1, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_sw_led1, 10, LV_PART_KNOB | LV_STATE_DEFAULT);

    // Write codes screen_app_sw_beep
    ui->screen_app_sw_beep = lv_switch_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_sw_beep, 5, 78);
    lv_obj_set_size(ui->screen_app_sw_beep, 50, 32);

    // Write style for screen_app_sw_beep, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_sw_beep, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_sw_beep, lv_color_hex(0xe6e2e6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_sw_beep, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_app_sw_beep, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_sw_beep, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_sw_beep, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_app_sw_beep, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_app_sw_beep, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->screen_app_sw_beep, lv_color_hex(0x2195f6), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_sw_beep, LV_GRAD_DIR_NONE, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_app_sw_beep, 0, LV_PART_INDICATOR | LV_STATE_CHECKED);

    // Write style for screen_app_sw_beep, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_sw_beep, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_sw_beep, lv_color_hex(0xffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_sw_beep, LV_GRAD_DIR_NONE, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_app_sw_beep, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_sw_beep, 10, LV_PART_KNOB | LV_STATE_DEFAULT);

    // Write codes screen_app_sw_relay
    ui->screen_app_sw_relay = lv_switch_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_sw_relay, 95, 78);
    lv_obj_set_size(ui->screen_app_sw_relay, 50, 32);

    // Write style for screen_app_sw_relay, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_sw_relay, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_sw_relay, lv_color_hex(0xe6e2e6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_sw_relay, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_app_sw_relay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_sw_relay, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_sw_relay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_app_sw_relay, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_app_sw_relay, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->screen_app_sw_relay, lv_color_hex(0x2195f6), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_sw_relay, LV_GRAD_DIR_NONE, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_app_sw_relay, 0, LV_PART_INDICATOR | LV_STATE_CHECKED);

    // Write style for screen_app_sw_relay, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_sw_relay, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_sw_relay, lv_color_hex(0xffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_sw_relay, LV_GRAD_DIR_NONE, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_app_sw_relay, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_sw_relay, 10, LV_PART_KNOB | LV_STATE_DEFAULT);

    // Write codes screen_app_label_fan
    ui->screen_app_label_fan = lv_label_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_label_fan, 194, 115);
    lv_obj_set_size(ui->screen_app_label_fan, 42, 20);
    lv_label_set_text(ui->screen_app_label_fan, "风扇");
    lv_label_set_long_mode(ui->screen_app_label_fan, LV_LABEL_LONG_CLIP);

    // Write style for screen_app_label_fan, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_app_label_fan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_label_fan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_label_fan, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_label_fan, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_label_fan, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_app_label_fan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_app_label_fan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_label_fan, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_label_fan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_app_label_fan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_app_label_fan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_app_label_fan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_app_label_fan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_label_fan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_app_label_led3
    ui->screen_app_label_led3 = lv_label_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_label_led3, 160, 52);
    lv_obj_set_size(ui->screen_app_label_led3, 60, 20);
    lv_label_set_text(ui->screen_app_label_led3, LV_SYMBOL_TINT " LED3");
    lv_label_set_long_mode(ui->screen_app_label_led3, LV_LABEL_LONG_CLIP);

    // Write style for screen_app_label_led3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_app_label_led3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_label_led3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_label_led3, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_label_led3, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_label_led3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_app_label_led3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_app_label_led3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_label_led3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_label_led3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_app_label_led3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_app_label_led3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_app_label_led3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_app_label_led3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_label_led3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_app_label_led2
    ui->screen_app_label_led2 = lv_label_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_label_led2, 90, 52);
    lv_obj_set_size(ui->screen_app_label_led2, 60, 20);
    lv_label_set_text(ui->screen_app_label_led2, LV_SYMBOL_TINT " LED2");
    lv_label_set_long_mode(ui->screen_app_label_led2, LV_LABEL_LONG_CLIP);

    // Write style for screen_app_label_led2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_app_label_led2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_label_led2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_label_led2, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_label_led2, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_label_led2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_app_label_led2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_app_label_led2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_label_led2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_label_led2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_app_label_led2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_app_label_led2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_app_label_led2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_app_label_led2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_label_led2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_app_label_led1
    ui->screen_app_label_led1 = lv_label_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_label_led1, 0, 52);
    lv_obj_set_size(ui->screen_app_label_led1, 60, 20);
    lv_label_set_text(ui->screen_app_label_led1, LV_SYMBOL_TINT " LED1");
    lv_label_set_long_mode(ui->screen_app_label_led1, LV_LABEL_LONG_CLIP);

    // Write style for screen_app_label_led1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_app_label_led1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_label_led1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_label_led1, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_label_led1, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_label_led1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_app_label_led1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_app_label_led1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_label_led1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_label_led1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_app_label_led1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_app_label_led1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_app_label_led1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_app_label_led1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_label_led1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_app_label_beep
    ui->screen_app_label_beep = lv_label_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_label_beep, 0, 115);
    lv_obj_set_size(ui->screen_app_label_beep, 60, 20);
    lv_label_set_text(ui->screen_app_label_beep, LV_SYMBOL_BELL " BEEP");
    lv_label_set_long_mode(ui->screen_app_label_beep, LV_LABEL_LONG_CLIP);

    // Write style for screen_app_label_beep, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_app_label_beep, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_label_beep, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_label_beep, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_label_beep, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_label_beep, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_app_label_beep, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_app_label_beep, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_label_beep, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_label_beep, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_app_label_beep, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_app_label_beep, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_app_label_beep, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_app_label_beep, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_label_beep, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_app_label_relay
    ui->screen_app_label_relay = lv_label_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_label_relay, 85, 115);
    lv_obj_set_size(ui->screen_app_label_relay, 70, 20);
    lv_label_set_text(ui->screen_app_label_relay, LV_SYMBOL_POWER " RELAY");
    lv_label_set_long_mode(ui->screen_app_label_relay, LV_LABEL_LONG_CLIP);

    // Write style for screen_app_label_relay, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_app_label_relay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_label_relay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_label_relay, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_label_relay, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_label_relay, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_app_label_relay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_app_label_relay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_label_relay, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_label_relay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_app_label_relay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_app_label_relay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_app_label_relay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_app_label_relay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_label_relay, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_app_label_sg90
    ui->screen_app_label_sg90 = lv_label_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_label_sg90, 194, 145);
    lv_obj_set_size(ui->screen_app_label_sg90, 42, 20);
    lv_label_set_text(ui->screen_app_label_sg90, "舵机");
    lv_label_set_long_mode(ui->screen_app_label_sg90, LV_LABEL_LONG_CLIP);

    // Write style for screen_app_label_sg90, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_app_label_sg90, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_label_sg90, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_label_sg90, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_label_sg90, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_label_sg90, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_app_label_sg90, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_app_label_sg90, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_label_sg90, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_label_sg90, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_app_label_sg90, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_app_label_sg90, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_app_label_sg90, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_app_label_sg90, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_label_sg90, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_app_roller_sg90
    ui->screen_app_roller_sg90 = lv_roller_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_roller_sg90, 175, 165);
    lv_obj_set_size(ui->screen_app_roller_sg90, 60, 32);
    lv_roller_set_options(ui->screen_app_roller_sg90, "0\n45\n90\n135\n180", LV_ROLLER_MODE_INFINITE);

    // Write style for screen_app_roller_sg90, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_app_roller_sg90, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_roller_sg90, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_roller_sg90, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_roller_sg90, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_roller_sg90, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_roller_sg90, &lv_font_montserratMedium_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_roller_sg90, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_roller_sg90, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_app_roller_sg90, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_app_roller_sg90, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_app_roller_sg90, lv_color_hex(0xe6e6e6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_app_roller_sg90, LV_BORDER_SIDE_FULL, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_app_roller_sg90, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_app_roller_sg90, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_roller_sg90, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_app_roller_sg90, Part: LV_PART_SELECTED, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_roller_sg90, 255, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_roller_sg90, lv_color_hex(0x2195f6), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_roller_sg90, LV_GRAD_DIR_NONE, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_roller_sg90, lv_color_hex(0xFFFFFF), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_roller_sg90, &lv_font_montserratMedium_12, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_roller_sg90, 255, LV_PART_SELECTED | LV_STATE_DEFAULT);

    lv_roller_set_visible_row_count(ui->screen_app_roller_sg90, 1);
    // Write codes screen_app_slider_R
    ui->screen_app_slider_R = lv_slider_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_slider_R, 10, 145);
    lv_obj_set_size(ui->screen_app_slider_R, 110, 12);
    lv_slider_set_range(ui->screen_app_slider_R, 0, 255);
    lv_slider_set_mode(ui->screen_app_slider_R, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_app_slider_R, 50, LV_ANIM_OFF);

    // Write style for screen_app_slider_R, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_slider_R, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_slider_R, lv_color_hex(0xE53935), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_slider_R, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_slider_R, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_app_slider_R, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_slider_R, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_app_slider_R, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_slider_R, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_slider_R, lv_color_hex(0xE53935), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_slider_R, LV_GRAD_DIR_NONE, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_slider_R, 8, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // Write style for screen_app_slider_R, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_slider_R, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_slider_R, lv_color_hex(0xE53935), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_slider_R, LV_GRAD_DIR_NONE, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_slider_R, 8, LV_PART_KNOB | LV_STATE_DEFAULT);

    // Write codes screen_app_slider_G
    ui->screen_app_slider_G = lv_slider_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_slider_G, 10, 180);
    lv_obj_set_size(ui->screen_app_slider_G, 110, 12);
    lv_slider_set_range(ui->screen_app_slider_G, 0, 255);
    lv_slider_set_mode(ui->screen_app_slider_G, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_app_slider_G, 50, LV_ANIM_OFF);

    // Write style for screen_app_slider_G, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_slider_G, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_slider_G, lv_color_hex(0x43A047), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_slider_G, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_slider_G, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_app_slider_G, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_slider_G, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_app_slider_G, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_slider_G, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_slider_G, lv_color_hex(0x43A047), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_slider_G, LV_GRAD_DIR_NONE, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_slider_G, 8, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // Write style for screen_app_slider_G, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_slider_G, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_slider_G, lv_color_hex(0x43A047), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_slider_G, LV_GRAD_DIR_NONE, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_slider_G, 8, LV_PART_KNOB | LV_STATE_DEFAULT);

    // Write codes screen_app_slider_B
    ui->screen_app_slider_B = lv_slider_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_slider_B, 10, 215);
    lv_obj_set_size(ui->screen_app_slider_B, 110, 12);
    lv_slider_set_range(ui->screen_app_slider_B, 0, 255);
    lv_slider_set_mode(ui->screen_app_slider_B, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_app_slider_B, 50, LV_ANIM_OFF);

    // Write style for screen_app_slider_B, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_slider_B, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_slider_B, lv_color_hex(0x1E88E5), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_slider_B, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_slider_B, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_app_slider_B, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_slider_B, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_app_slider_B, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_slider_B, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_slider_B, lv_color_hex(0x1E88E5), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_slider_B, LV_GRAD_DIR_NONE, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_slider_B, 8, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // Write style for screen_app_slider_B, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_slider_B, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_slider_B, lv_color_hex(0x1E88E5), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_slider_B, LV_GRAD_DIR_NONE, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_slider_B, 8, LV_PART_KNOB | LV_STATE_DEFAULT);

    // Write codes screen_app_roller_RGB_NUM
    ui->screen_app_roller_RGB_NUM = lv_roller_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_roller_RGB_NUM, 175, 225);
    lv_obj_set_size(ui->screen_app_roller_RGB_NUM, 60, 32);
    lv_roller_set_options(ui->screen_app_roller_RGB_NUM, "0\n1\n2\n3\n4", LV_ROLLER_MODE_INFINITE);

    // Write style for screen_app_roller_RGB_NUM, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_app_roller_RGB_NUM, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_roller_RGB_NUM, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_roller_RGB_NUM, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_roller_RGB_NUM, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_roller_RGB_NUM, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_roller_RGB_NUM, &lv_font_montserratMedium_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_roller_RGB_NUM, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_roller_RGB_NUM, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_app_roller_RGB_NUM, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_app_roller_RGB_NUM, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_app_roller_RGB_NUM, lv_color_hex(0xe6e6e6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_app_roller_RGB_NUM, LV_BORDER_SIDE_FULL, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_app_roller_RGB_NUM, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_app_roller_RGB_NUM, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_roller_RGB_NUM, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_app_roller_RGB_NUM, Part: LV_PART_SELECTED, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_app_roller_RGB_NUM, 255, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_app_roller_RGB_NUM, lv_color_hex(0x2195f6), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_app_roller_RGB_NUM, LV_GRAD_DIR_NONE, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_roller_RGB_NUM, lv_color_hex(0xFFFFFF), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_roller_RGB_NUM, &lv_font_montserratMedium_12, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_roller_RGB_NUM, 255, LV_PART_SELECTED | LV_STATE_DEFAULT);

    lv_roller_set_visible_row_count(ui->screen_app_roller_RGB_NUM, 1);
    // Write codes screen_app_label_RGB_R
    ui->screen_app_label_RGB_R = lv_label_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_label_RGB_R, 125, 143);
    lv_obj_set_size(ui->screen_app_label_RGB_R, 45, 16);
    lv_label_set_text(ui->screen_app_label_RGB_R, "R:255");
    lv_label_set_long_mode(ui->screen_app_label_RGB_R, LV_LABEL_LONG_WRAP);

    // Write style for screen_app_label_RGB_R, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_app_label_RGB_R, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_label_RGB_R, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_label_RGB_R, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_label_RGB_R, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_label_RGB_R, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_app_label_RGB_R, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_app_label_RGB_R, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_label_RGB_R, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_label_RGB_R, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_app_label_RGB_R, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_app_label_RGB_R, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_app_label_RGB_R, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_app_label_RGB_R, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_label_RGB_R, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_app_label_RGB_G
    ui->screen_app_label_RGB_G = lv_label_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_label_RGB_G, 125, 178);
    lv_obj_set_size(ui->screen_app_label_RGB_G, 45, 16);
    lv_label_set_text(ui->screen_app_label_RGB_G, "G:255");
    lv_label_set_long_mode(ui->screen_app_label_RGB_G, LV_LABEL_LONG_WRAP);

    // Write style for screen_app_label_RGB_G, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_app_label_RGB_G, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_label_RGB_G, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_label_RGB_G, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_label_RGB_G, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_label_RGB_G, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_app_label_RGB_G, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_app_label_RGB_G, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_label_RGB_G, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_label_RGB_G, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_app_label_RGB_G, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_app_label_RGB_G, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_app_label_RGB_G, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_app_label_RGB_G, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_label_RGB_G, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_app_label_RGB_B
    ui->screen_app_label_RGB_B = lv_label_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_label_RGB_B, 125, 213);
    lv_obj_set_size(ui->screen_app_label_RGB_B, 45, 16);
    lv_label_set_text(ui->screen_app_label_RGB_B, "B:255");
    lv_label_set_long_mode(ui->screen_app_label_RGB_B, LV_LABEL_LONG_WRAP);

    // Write style for screen_app_label_RGB_B, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_app_label_RGB_B, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_label_RGB_B, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_label_RGB_B, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_label_RGB_B, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_label_RGB_B, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_app_label_RGB_B, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_app_label_RGB_B, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_label_RGB_B, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_label_RGB_B, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_app_label_RGB_B, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_app_label_RGB_B, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_app_label_RGB_B, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_app_label_RGB_B, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_label_RGB_B, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_app_label_RGB_NUM
    ui->screen_app_label_RGB_NUM = lv_label_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_label_RGB_NUM, 194, 205);
    lv_obj_set_size(ui->screen_app_label_RGB_NUM, 42, 16);
    lv_label_set_text(ui->screen_app_label_RGB_NUM, "数量");
    lv_label_set_long_mode(ui->screen_app_label_RGB_NUM, LV_LABEL_LONG_CLIP);

    // Write style for screen_app_label_RGB_NUM, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_app_label_RGB_NUM, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_label_RGB_NUM, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_label_RGB_NUM, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_label_RGB_NUM, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_label_RGB_NUM, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_app_label_RGB_NUM, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_app_label_RGB_NUM, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_label_RGB_NUM, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_label_RGB_NUM, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_app_label_RGB_NUM, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_app_label_RGB_NUM, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_app_label_RGB_NUM, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_app_label_RGB_NUM, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_label_RGB_NUM, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_app_label_RGB_color
    ui->screen_app_label_RGB_color = lv_label_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_label_RGB_color, 10, 250);
    lv_obj_set_size(ui->screen_app_label_RGB_color, 110, 24);
    lv_label_set_text(ui->screen_app_label_RGB_color, "");
    lv_label_set_long_mode(ui->screen_app_label_RGB_color, LV_LABEL_LONG_WRAP);

    // Write style for screen_app_label_RGB_color, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_app_label_RGB_color, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_app_label_RGB_color, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_label_RGB_color, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_app_label_RGB_color, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_app_label_RGB_color, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_app_label_RGB_color, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_app_label_RGB_color, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_label_RGB_color, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_label_RGB_color, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_app_label_RGB_color, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_app_label_RGB_color, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_app_label_RGB_color, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_app_label_RGB_color, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_app_label_RGB_color, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ===== 内置图标（LV_SYMBOL_*）：风扇 / 舵机 / RGB 数量 ===== */
    ui->screen_app_icon_fan = lv_label_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_icon_fan, 175, 115);
    lv_obj_set_size(ui->screen_app_icon_fan, 18, 20);
    lv_label_set_text(ui->screen_app_icon_fan, LV_SYMBOL_REFRESH);
    lv_obj_set_style_text_font(ui->screen_app_icon_fan, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_icon_fan, lv_color_hex(0x2195F6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_icon_fan, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_icon_fan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_app_icon_fan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui->screen_app_icon_sg90 = lv_label_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_icon_sg90, 175, 145);
    lv_obj_set_size(ui->screen_app_icon_sg90, 18, 20);
    lv_label_set_text(ui->screen_app_icon_sg90, LV_SYMBOL_LOOP);
    lv_obj_set_style_text_font(ui->screen_app_icon_sg90, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_icon_sg90, lv_color_hex(0x2195F6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_icon_sg90, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_icon_sg90, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_app_icon_sg90, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui->screen_app_icon_rgbnum = lv_label_create(ui->screen_app);
    lv_obj_set_pos(ui->screen_app_icon_rgbnum, 175, 205);
    lv_obj_set_size(ui->screen_app_icon_rgbnum, 18, 16);
    lv_label_set_text(ui->screen_app_icon_rgbnum, LV_SYMBOL_LIST);
    lv_obj_set_style_text_font(ui->screen_app_icon_rgbnum, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_app_icon_rgbnum, lv_color_hex(0x2195F6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_app_icon_rgbnum, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_app_icon_rgbnum, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_app_icon_rgbnum, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // The custom code of screen_app.

    // Update current screen layout.
    lv_obj_update_layout(ui->screen_app);

    // Init events for screen.
    events_init_screen_app(ui);
}
