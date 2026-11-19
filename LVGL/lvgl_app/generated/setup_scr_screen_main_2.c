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

void setup_scr_screen_main_2(lv_ui *ui)
{
    // Write codes screen_main_2
    ui->screen_main_2 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_main_2, 240, 280);
    lv_obj_set_scrollbar_mode(ui->screen_main_2, LV_SCROLLBAR_MODE_OFF);

    // Write style for screen_main_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen_main_2, PIC_BACK_240280, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_main_2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_main_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_main_2_icon_note
ui->screen_main_2_icon_note = lv_image_create(ui->screen_main_2);
    lv_obj_add_flag(ui->screen_main_2_icon_note, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_pos(ui->screen_main_2_icon_note, 5, 20);
    lv_obj_set_size(ui->screen_main_2_icon_note, 70, 70);
    lv_image_set_src(ui->screen_main_2_icon_note, PIC_NOTEPAD_70);
    lv_obj_set_style_pad_all(ui->screen_main_2_icon_note, 0, LV_STATE_DEFAULT);

    // Write style for screen_main_2_icon_note, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.

    // Write style for screen_main_2_icon_note, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_main_2_icon_note, 0, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_main_2_icon_note, 170, LV_PART_MAIN | LV_STATE_PRESSED);

    // Write style for screen_main_2_icon_note, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_main_2_icon_note, 0, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_main_2_icon_note, 255, LV_PART_MAIN | LV_STATE_CHECKED);


    // Write codes screen_main_2_icon_file
ui->screen_main_2_icon_file = lv_image_create(ui->screen_main_2);
    lv_obj_add_flag(ui->screen_main_2_icon_file, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_pos(ui->screen_main_2_icon_file, 85, 20);
    lv_obj_set_size(ui->screen_main_2_icon_file, 70, 70);
    lv_image_set_src(ui->screen_main_2_icon_file, PIC_PAINT_70);
    lv_obj_set_style_pad_all(ui->screen_main_2_icon_file, 0, LV_STATE_DEFAULT);

    // Write style for screen_main_2_icon_file, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.

    // Write style for screen_main_2_icon_file, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_main_2_icon_file, 0, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_main_2_icon_file, 170, LV_PART_MAIN | LV_STATE_PRESSED);

    // Write style for screen_main_2_icon_file, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_main_2_icon_file, 0, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_main_2_icon_file, 255, LV_PART_MAIN | LV_STATE_CHECKED);


    // Write codes screen_main_2_icon_pic
ui->screen_main_2_icon_pic = lv_image_create(ui->screen_main_2);
    lv_obj_add_flag(ui->screen_main_2_icon_pic, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_pos(ui->screen_main_2_icon_pic, 165, 20);
    lv_obj_set_size(ui->screen_main_2_icon_pic, 70, 70);
    lv_image_set_src(ui->screen_main_2_icon_pic, PIC_PICTURE_70);
    lv_obj_set_style_pad_all(ui->screen_main_2_icon_pic, 0, LV_STATE_DEFAULT);

    // Write style for screen_main_2_icon_pic, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.

    // Write style for screen_main_2_icon_pic, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_main_2_icon_pic, 0, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_main_2_icon_pic, 170, LV_PART_MAIN | LV_STATE_PRESSED);

    // Write style for screen_main_2_icon_pic, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_main_2_icon_pic, 0, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_main_2_icon_pic, 255, LV_PART_MAIN | LV_STATE_CHECKED);


    // Write codes screen_main_2_icon_set
ui->screen_main_2_icon_set = lv_image_create(ui->screen_main_2);
    lv_obj_add_flag(ui->screen_main_2_icon_set, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_pos(ui->screen_main_2_icon_set, 5, 160);
    lv_obj_set_size(ui->screen_main_2_icon_set, 70, 70);
    lv_image_set_src(ui->screen_main_2_icon_set, PIC_SET_70);
    lv_obj_set_style_pad_all(ui->screen_main_2_icon_set, 0, LV_STATE_DEFAULT);

    // Write style for screen_main_2_icon_set, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.

    // Write style for screen_main_2_icon_set, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_main_2_icon_set, 0, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_main_2_icon_set, 170, LV_PART_MAIN | LV_STATE_PRESSED);

    // Write style for screen_main_2_icon_set, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_main_2_icon_set, 0, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_main_2_icon_set, 255, LV_PART_MAIN | LV_STATE_CHECKED);


    // Write codes screen_main_2_icon_news
ui->screen_main_2_icon_news = lv_image_create(ui->screen_main_2);
    lv_obj_add_flag(ui->screen_main_2_icon_news, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_pos(ui->screen_main_2_icon_news, 85, 160);
    lv_obj_set_size(ui->screen_main_2_icon_news, 70, 70);
    lv_image_set_src(ui->screen_main_2_icon_news, PIC_SMS_70);
    lv_obj_set_style_pad_all(ui->screen_main_2_icon_news, 0, LV_STATE_DEFAULT);

    // Write style for screen_main_2_icon_news, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.

    // Write style for screen_main_2_icon_news, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_main_2_icon_news, 0, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_main_2_icon_news, 170, LV_PART_MAIN | LV_STATE_PRESSED);

    // Write style for screen_main_2_icon_news, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_main_2_icon_news, 0, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_main_2_icon_news, 255, LV_PART_MAIN | LV_STATE_CHECKED);


    // Write codes screen_main_2_label_set
    ui->screen_main_2_label_set = lv_label_create(ui->screen_main_2);
    lv_obj_set_pos(ui->screen_main_2_label_set, 5, 240);
    lv_obj_set_size(ui->screen_main_2_label_set, 70, 20);
    lv_label_set_text(ui->screen_main_2_label_set, "设置");
    lv_label_set_long_mode(ui->screen_main_2_label_set, LV_LABEL_LONG_WRAP);

    // Write style for screen_main_2_label_set, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_2_label_set, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_2_label_set, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_2_label_set, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_2_label_set, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_2_label_set, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_main_2_label_set, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_main_2_label_set, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_2_label_set, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_2_label_set, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_2_label_set, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_2_label_set, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_2_label_set, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_2_label_set, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_2_label_set, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_main_2_label_news
    ui->screen_main_2_label_news = lv_label_create(ui->screen_main_2);
    lv_obj_set_pos(ui->screen_main_2_label_news, 85, 240);
    lv_obj_set_size(ui->screen_main_2_label_news, 70, 20);
    lv_label_set_text(ui->screen_main_2_label_news, "消息");
    lv_label_set_long_mode(ui->screen_main_2_label_news, LV_LABEL_LONG_WRAP);

    // Write style for screen_main_2_label_news, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_2_label_news, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_2_label_news, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_2_label_news, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_2_label_news, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_2_label_news, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_main_2_label_news, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_main_2_label_news, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_2_label_news, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_2_label_news, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_2_label_news, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_2_label_news, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_2_label_news, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_2_label_news, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_2_label_news, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_main_2_label_pic
    ui->screen_main_2_label_pic = lv_label_create(ui->screen_main_2);
    lv_obj_set_pos(ui->screen_main_2_label_pic, 165, 100);
    lv_obj_set_size(ui->screen_main_2_label_pic, 70, 20);
    lv_label_set_text(ui->screen_main_2_label_pic, "相册");
    lv_label_set_long_mode(ui->screen_main_2_label_pic, LV_LABEL_LONG_WRAP);

    // Write style for screen_main_2_label_pic, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_2_label_pic, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_2_label_pic, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_2_label_pic, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_2_label_pic, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_2_label_pic, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_main_2_label_pic, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_main_2_label_pic, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_2_label_pic, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_2_label_pic, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_2_label_pic, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_2_label_pic, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_2_label_pic, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_2_label_pic, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_2_label_pic, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_main_2_label_file
    ui->screen_main_2_label_file = lv_label_create(ui->screen_main_2);
    lv_obj_set_pos(ui->screen_main_2_label_file, 85, 100);
    lv_obj_set_size(ui->screen_main_2_label_file, 70, 20);
    lv_label_set_text(ui->screen_main_2_label_file, "文件");
    lv_label_set_long_mode(ui->screen_main_2_label_file, LV_LABEL_LONG_WRAP);

    // Write style for screen_main_2_label_file, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_2_label_file, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_2_label_file, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_2_label_file, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_2_label_file, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_2_label_file, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_main_2_label_file, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_main_2_label_file, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_2_label_file, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_2_label_file, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_2_label_file, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_2_label_file, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_2_label_file, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_2_label_file, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_2_label_file, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_main_2_label_note
    ui->screen_main_2_label_note = lv_label_create(ui->screen_main_2);
    lv_obj_set_pos(ui->screen_main_2_label_note, 5, 100);
    lv_obj_set_size(ui->screen_main_2_label_note, 70, 20);
    lv_label_set_text(ui->screen_main_2_label_note, "便签");
    lv_label_set_long_mode(ui->screen_main_2_label_note, LV_LABEL_LONG_WRAP);

    // Write style for screen_main_2_label_note, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_2_label_note, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_2_label_note, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_2_label_note, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_2_label_note, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_2_label_note, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_main_2_label_note, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_main_2_label_note, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_2_label_note, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_2_label_note, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_2_label_note, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_2_label_note, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_2_label_note, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_2_label_note, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_2_label_note, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ===== 第 6 格：返回主页图标按钮（使用工程已有图标字体） ===== */
    ui->screen_main_2_btn_home = lv_button_create(ui->screen_main_2);
    lv_obj_set_pos(ui->screen_main_2_btn_home, 178, 173);
    lv_obj_set_size(ui->screen_main_2_btn_home, 44, 44);
    lv_obj_set_style_radius(ui->screen_main_2_btn_home, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_main_2_btn_home, lv_color_hex(0xE3F0FD), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_main_2_btn_home, lv_color_hex(0xC9E2FA), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui->screen_main_2_btn_home, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_2_btn_home, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_2_btn_home, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_main_2_btn_home, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *ico = lv_label_create(ui->screen_main_2_btn_home);
        lv_label_set_text(ico, LV_SYMBOL_HOME);
        lv_obj_set_style_text_font(ico, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ico, lv_color_hex(0x2195F6), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_center(ico);
    }

    ui->screen_main_2_label_home = lv_label_create(ui->screen_main_2);
    lv_obj_set_pos(ui->screen_main_2_label_home, 165, 240);
    lv_obj_set_size(ui->screen_main_2_label_home, 70, 20);
    lv_label_set_text(ui->screen_main_2_label_home, "主页");
    lv_obj_set_style_text_color(ui->screen_main_2_label_home, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_2_label_home, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_2_label_home, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_2_label_home, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_main_2_label_home, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_2_label_home, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // The custom code of screen_main_2.

    // Update current screen layout.
    lv_obj_update_layout(ui->screen_main_2);

    // Init events for screen.
    events_init_screen_main_2(ui);
}

/* ==================================================================
 *  main_2 图标的独立功能界面：便签 / 文件 / 消息
 *  放在本文件里定义，避免新增 .c 后还要改 Keil 工程文件列表
 * ================================================================== */

/* 子界面统一背景 */
static void main2_sub_bg(lv_obj_t *scr)
{
    lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE); /* 防止键盘弹起时整屏被滚动 */
    lv_obj_set_scrollbar_mode(scr, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(scr, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(scr, PIC_BACK_240280, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(scr, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(scr, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}

/* 子界面统一顶栏：左侧返回按钮 + 标题 */
static void main2_sub_header(lv_obj_t *scr, lv_obj_t **btn_back, lv_obj_t **label_title, const char *title)
{
    *btn_back = lv_button_create(scr);
    lv_obj_set_pos(*btn_back, 5, 5);
    lv_obj_set_size(*btn_back, 36, 28);
    lv_obj_set_style_radius(*btn_back, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(*btn_back, lv_color_hex(0xE3F0FD), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(*btn_back, lv_color_hex(0xC9E2FA), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(*btn_back, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(*btn_back, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(*btn_back, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(*btn_back, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *ico = lv_label_create(*btn_back);
        lv_label_set_text(ico, LV_SYMBOL_LEFT);
        lv_obj_set_style_text_font(ico, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ico, lv_color_hex(0x2195F6), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_center(ico);
    }

    *label_title = lv_label_create(scr);
    lv_obj_set_pos(*label_title, 48, 8);
    lv_obj_set_size(*label_title, 110, 22);
    lv_label_set_text(*label_title, title);
    lv_obj_set_style_text_color(*label_title, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(*label_title, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(*label_title, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(*label_title, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(*label_title, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}

/* 子界面统一顶栏右侧的小按钮（图标按钮） */
static void main2_sub_top_btn(lv_obj_t *scr, lv_obj_t **btn, const char *symbol)
{
    *btn = lv_button_create(scr);
    lv_obj_set_pos(*btn, 191, 5);
    lv_obj_set_size(*btn, 44, 28);
    lv_obj_set_style_radius(*btn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(*btn, lv_color_hex(0xE3F0FD), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(*btn, lv_color_hex(0xC9E2FA), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(*btn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(*btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(*btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(*btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *ico = lv_label_create(*btn);
        lv_label_set_text(ico, symbol);
        lv_obj_set_style_text_font(ico, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ico, lv_color_hex(0x2195F6), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_center(ico);
    }
}

/* 白色卡片容器 */
static void main2_sub_card(lv_obj_t *obj, int32_t x, int32_t y, int32_t w, int32_t h)
{
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}

/* ---------------------------- 便签 ---------------------------- */
void setup_scr_screen_note(lv_ui *ui)
{
    ui->screen_note = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_note, 240, 280);
    main2_sub_bg(ui->screen_note);
    main2_sub_header(ui->screen_note, &ui->screen_note_btn_back, &ui->screen_note_label_title, "便签");

    /* 清空按钮（用内置图标，避免字库缺字） */
    ui->screen_note_btn_clear = lv_button_create(ui->screen_note);
    lv_obj_set_pos(ui->screen_note_btn_clear, 191, 5);
    lv_obj_set_size(ui->screen_note_btn_clear, 44, 28);
    lv_obj_set_style_radius(ui->screen_note_btn_clear, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_note_btn_clear, lv_color_hex(0xE3F0FD), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_note_btn_clear, lv_color_hex(0xC9E2FA), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui->screen_note_btn_clear, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_note_btn_clear, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_note_btn_clear, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_note_btn_clear, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *t = lv_label_create(ui->screen_note_btn_clear);
        lv_label_set_text(t, LV_SYMBOL_CLOSE);
        lv_obj_set_style_text_font(t, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(t, lv_color_hex(0x2195F6), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_center(t);
    }

    /* 编辑区（ASCII 输入，用工程已有 14px 图标字体，保证有字） */
    ui->screen_note_ta_text = lv_textarea_create(ui->screen_note);
    lv_obj_set_pos(ui->screen_note_ta_text, 5, 38);
    lv_obj_set_size(ui->screen_note_ta_text, 230, 150);
    lv_textarea_set_one_line(ui->screen_note_ta_text, false);
    lv_textarea_set_max_length(ui->screen_note_ta_text, 200);
    lv_textarea_set_placeholder_text(ui->screen_note_ta_text, "Type your note here");
    lv_obj_set_style_radius(ui->screen_note_ta_text, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_note_ta_text, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_note_ta_text, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_note_ta_text, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_note_ta_text, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_note_ta_text, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_note_ta_text, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_note_ta_text, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_note_ta_text, lv_color_hex(0x2195F6), LV_PART_CURSOR | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(ui->screen_note_ta_text, 255, LV_PART_CURSOR | LV_STATE_FOCUSED);

    /* 软键盘 */
    ui->screen_note_kb_input = lv_keyboard_create(ui->screen_note);
    lv_obj_set_pos(ui->screen_note_kb_input, 0, 192);
    lv_obj_set_size(ui->screen_note_kb_input, 240, 88);
    lv_keyboard_set_mode(ui->screen_note_kb_input, LV_KEYBOARD_MODE_TEXT_LOWER);
    lv_keyboard_set_textarea(ui->screen_note_kb_input, ui->screen_note_ta_text);
    lv_obj_set_style_bg_color(ui->screen_note_kb_input, lv_color_hex(0xE3F0FD), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_note_kb_input, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_note_kb_input, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_update_layout(ui->screen_note);
    events_init_screen_note(ui);
}

/* ---------------------------- 文件 ---------------------------- */
void setup_scr_screen_file(lv_ui *ui)
{
    ui->screen_file = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_file, 240, 280);
    main2_sub_bg(ui->screen_file);
    main2_sub_header(ui->screen_file, &ui->screen_file_btn_back, &ui->screen_file_label_title, "文件");
    main2_sub_top_btn(ui->screen_file, &ui->screen_file_btn_refresh, LV_SYMBOL_REFRESH);

    ui->screen_file_list_box = lv_obj_create(ui->screen_file);
    main2_sub_card(ui->screen_file_list_box, 5, 38, 230, 234);
    lv_obj_set_style_pad_all(ui->screen_file_list_box, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->screen_file_list_box, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_flex_flow(ui->screen_file_list_box, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(ui->screen_file_list_box, LV_SCROLLBAR_MODE_AUTO);

    lv_obj_update_layout(ui->screen_file);
    events_init_screen_file(ui);
}

/* ---------------------------- 消息 ---------------------------- */
void setup_scr_screen_news(lv_ui *ui)
{
    static const char *news_title[4] = {"System", "Sensor", "Music", "Album"};
    static const char *news_body[4] = {
        "Welcome to Smart Life, swipe to switch pages",
        "Temp / humi / light data updated",
        "Audio files on SD card are ready",
        "Photos in /ICOS are ready"};
    int i;

    ui->screen_news = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_news, 240, 280);
    main2_sub_bg(ui->screen_news);
    main2_sub_header(ui->screen_news, &ui->screen_news_btn_back, &ui->screen_news_label_title, "消息");

    ui->screen_news_list_box = lv_obj_create(ui->screen_news);
    main2_sub_card(ui->screen_news_list_box, 5, 38, 230, 234);
    lv_obj_set_style_pad_all(ui->screen_news_list_box, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->screen_news_list_box, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_flex_flow(ui->screen_news_list_box, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(ui->screen_news_list_box, LV_SCROLLBAR_MODE_AUTO);

    for (i = 0; i < 4; i++)
    {
        lv_obj_t *card = lv_obj_create(ui->screen_news_list_box);
        lv_obj_set_size(card, 214, 52);
        lv_obj_set_style_radius(card, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(card, lv_color_hex(0xF5F9FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(card, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(card, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(card, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(card, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *ico = lv_label_create(card);
        lv_obj_set_pos(ico, 6, 5);
        lv_label_set_text(ico, LV_SYMBOL_ENVELOPE);
        lv_obj_set_style_text_font(ico, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ico, lv_color_hex(0x2195F6), LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t *t = lv_label_create(card);
        lv_obj_set_pos(t, 26, 3);
        lv_obj_set_size(t, 180, 20);
        lv_label_set_text(t, news_title[i]);
        lv_label_set_long_mode(t, LV_LABEL_LONG_CLIP);
        lv_obj_set_style_text_font(t, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(t, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(t, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(t, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t *b = lv_label_create(card);
        lv_obj_set_pos(b, 6, 26);
        lv_obj_set_size(b, 200, 20);
        lv_label_set_text(b, news_body[i]);
        lv_label_set_long_mode(b, LV_LABEL_LONG_CLIP);
        lv_obj_set_style_text_font(b, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(b, lv_color_hex(0x7D838B), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(b, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(b, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    lv_obj_update_layout(ui->screen_news);
    events_init_screen_news(ui);
}

/* ---------------------------- WiFi 连接 ----------------------------
   下拉框选热点 + 三个三行滚轮选密码（数字特殊字符/小写/大写）。
   点“输入”添加最后操作的滚轮字符；WiFi 页禁用上滑返回，避免滚动误退出。
   扫描与连接由 WIFI 任务执行，界面只投递请求并轮询结果。
------------------------------------------------------------------ */
static const char *wifi_roller_num_opts =
    "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n.\n-\n_\n@\n!\n#\n$\n%\n&\n*\n+\n=\n?\n/";
static const char *wifi_roller_low_opts =
    "a\nb\nc\nd\ne\nf\ng\nh\ni\nj\nk\nl\nm\nn\no\np\nq\nr\ns\nt\nu\nv\nw\nx\ny\nz";
static const char *wifi_roller_up_opts =
    "A\nB\nC\nD\nE\nF\nG\nH\nI\nJ\nK\nL\nM\nN\nO\nP\nQ\nR\nS\nT\nU\nV\nW\nX\nY\nZ";

static void wifi_roller_style(lv_obj_t *r)
{
    lv_obj_set_size(r, 70, 96);
    lv_roller_set_visible_row_count(r, 3);
    lv_obj_set_style_radius(r, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(r, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(r, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(r, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(r, lv_color_hex(0xE6E2E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(r, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(r, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(r, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(r, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(r, 255, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(r, lv_color_hex(0x2195F6), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(r, lv_color_hex(0xFFFFFF), LV_PART_SELECTED | LV_STATE_DEFAULT);
}

void setup_scr_screen_wifi(lv_ui *ui)
{
    lv_obj_t *list;

    ui->screen_wifi = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_wifi, 240, 280);
    main2_sub_bg(ui->screen_wifi);
    main2_sub_header(ui->screen_wifi, &ui->screen_wifi_btn_back, &ui->screen_wifi_label_title, "WiFi");

    /* 热点框与刷新按钮左右分开，展开下拉列表也不会盖住刷新 */
    ui->screen_wifi_dd_ap = lv_dropdown_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_dd_ap, 5, 38);
    lv_obj_set_size(ui->screen_wifi_dd_ap, 174, 36);
    lv_dropdown_set_options_static(ui->screen_wifi_dd_ap, "点右侧刷新扫描");
    lv_dropdown_set_symbol(ui->screen_wifi_dd_ap, LV_SYMBOL_DOWN);
    /* 箭头符号只有 Montserrat 里有，中文字库(XBF)里没有，不单独指定会显示成方框 */
    lv_obj_set_style_text_font(ui->screen_wifi_dd_ap, &lv_font_montserratMedium_14, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_dd_ap, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_dd_ap, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_dd_ap, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wifi_dd_ap, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_wifi_dd_ap, lv_color_hex(0xE6E2E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_dd_ap, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_dd_ap, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wifi_dd_ap, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wifi_dd_ap, 9, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_dd_ap, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    main2_sub_top_btn(ui->screen_wifi, &ui->screen_wifi_btn_scan, LV_SYMBOL_REFRESH);
    lv_obj_set_pos(ui->screen_wifi_btn_scan, 185, 38);
    lv_obj_set_size(ui->screen_wifi_btn_scan, 50, 36);

    /* 展开后的列表：限高可滚动 */
    list = lv_dropdown_get_list(ui->screen_wifi_dd_ap);
    lv_obj_set_style_max_height(list, 120, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(list, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(list, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(list, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(list, lv_color_hex(0xE6E2E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(list, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(list, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(list, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(list, lv_color_hex(0x2195F6), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(list, lv_color_hex(0xFFFFFF), LV_PART_SELECTED | LV_STATE_DEFAULT);

    /* 状态提示：背景是浅蓝壁纸，蓝色字看不清，改用深色 */
    ui->screen_wifi_label_status = lv_label_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_label_status, 5, 77);
    lv_obj_set_size(ui->screen_wifi_label_status, 230, 20);
    lv_label_set_text(ui->screen_wifi_label_status, "滚动选字符 点输入添加");
    lv_label_set_long_mode(ui->screen_wifi_label_status, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_color(ui->screen_wifi_label_status, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_label_status, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_label_status, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_wifi_label_status, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_label_status, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* 密码显示（白卡片） */
    ui->screen_wifi_label_pwd = lv_label_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_label_pwd, 5, 98);
    lv_obj_set_size(ui->screen_wifi_label_pwd, 230, 26);
    lv_label_set_text(ui->screen_wifi_label_pwd, "密码:");
    lv_label_set_long_mode(ui->screen_wifi_label_pwd, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_radius(ui->screen_wifi_label_pwd, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_label_pwd, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_label_pwd, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wifi_label_pwd, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_wifi_label_pwd, lv_color_hex(0xE6E2E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_label_pwd, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_label_pwd, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wifi_label_pwd, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wifi_label_pwd, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_label_pwd, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* 三个三行滚轮：数字+特殊字符 / 小写 / 大写 */
    ui->screen_wifi_roller_num = lv_roller_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_roller_num, 5, 124);
    lv_roller_set_options(ui->screen_wifi_roller_num, wifi_roller_num_opts, LV_ROLLER_MODE_INFINITE);
    wifi_roller_style(ui->screen_wifi_roller_num);

    ui->screen_wifi_roller_low = lv_roller_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_roller_low, 85, 124);
    lv_roller_set_options(ui->screen_wifi_roller_low, wifi_roller_low_opts, LV_ROLLER_MODE_INFINITE);
    wifi_roller_style(ui->screen_wifi_roller_low);

    ui->screen_wifi_roller_up = lv_roller_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_roller_up, 165, 124);
    lv_roller_set_options(ui->screen_wifi_roller_up, wifi_roller_up_opts, LV_ROLLER_MODE_INFINITE);
    wifi_roller_style(ui->screen_wifi_roller_up);

    /* 删除 / 输入 / 连接 */
    ui->screen_wifi_btn_del = lv_button_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_btn_del, 5, 228);
    lv_obj_set_size(ui->screen_wifi_btn_del, 70, 36);
    lv_obj_set_style_radius(ui->screen_wifi_btn_del, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_btn_del, lv_color_hex(0xE3F0FD), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_btn_del, lv_color_hex(0xC9E2FA), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui->screen_wifi_btn_del, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wifi_btn_del, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_btn_del, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_wifi_btn_del, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *t = lv_label_create(ui->screen_wifi_btn_del);
        lv_label_set_text(t, "删除");
        lv_obj_set_style_text_font(t, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(t, lv_color_hex(0x2195F6), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_center(t);
    }

    ui->screen_wifi_btn_input = lv_button_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_btn_input, 85, 228);
    lv_obj_set_size(ui->screen_wifi_btn_input, 70, 36);
    lv_obj_set_style_radius(ui->screen_wifi_btn_input, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_btn_input, lv_color_hex(0xE3F0FD), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_btn_input, lv_color_hex(0xC9E2FA), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui->screen_wifi_btn_input, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wifi_btn_input, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_btn_input, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_wifi_btn_input, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *t = lv_label_create(ui->screen_wifi_btn_input);
        lv_label_set_text(t, "输入");
        lv_obj_set_style_text_font(t, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(t, lv_color_hex(0x2195F6), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_center(t);
    }

    ui->screen_wifi_btn_ok = lv_button_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_btn_ok, 165, 228);
    lv_obj_set_size(ui->screen_wifi_btn_ok, 70, 36);
    lv_obj_set_style_radius(ui->screen_wifi_btn_ok, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_btn_ok, lv_color_hex(0x2195F6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_btn_ok, lv_color_hex(0x1976D2), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui->screen_wifi_btn_ok, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wifi_btn_ok, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_btn_ok, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_wifi_btn_ok, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *t = lv_label_create(ui->screen_wifi_btn_ok);
        lv_label_set_text(t, "连接");
        lv_obj_set_style_text_font(t, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(t, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_center(t);
    }

    /* 连接中/连接成功 浮动提示：深色胶囊+白字，浮在滚轮上方（和视频一致）。
       label 默认不可点击，不会挡住下面滚轮的触摸 */
    ui->screen_wifi_label_toast = lv_label_create(ui->screen_wifi);
    lv_label_set_text(ui->screen_wifi_label_toast, "");
    lv_obj_set_style_bg_color(ui->screen_wifi_label_toast, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_label_toast, 235, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_label_toast, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_label_toast, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_label_toast, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_hor(ui->screen_wifi_label_toast, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_ver(ui->screen_wifi_label_toast, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_label_toast, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(ui->screen_wifi_label_toast, LV_ALIGN_TOP_MID, 0, 190);
    lv_obj_add_flag(ui->screen_wifi_label_toast, LV_OBJ_FLAG_HIDDEN);

    lv_obj_update_layout(ui->screen_wifi);
    events_init_screen_wifi(ui);
}
