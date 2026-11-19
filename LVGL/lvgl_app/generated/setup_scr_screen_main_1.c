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

void setup_scr_screen_main_1(lv_ui *ui)
{
   // Write codes screen_main_1
   ui->screen_main_1 = lv_obj_create(NULL);
   lv_obj_set_size(ui->screen_main_1, 240, 280);
   lv_obj_set_scrollbar_mode(ui->screen_main_1, LV_SCROLLBAR_MODE_OFF);

   // // Write style for screen_main_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
   lv_obj_set_style_bg_opa(ui->screen_main_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_bg_image_src(ui->screen_main_1, PIC_BACK_240280 , LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_bg_image_opa(ui->screen_main_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_bg_image_recolor_opa(ui->screen_main_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

   // Write codes screen_main_1_icon_app
ui->screen_main_1_icon_app = lv_image_create(ui->screen_main_1);
   lv_obj_add_flag(ui->screen_main_1_icon_app, LV_OBJ_FLAG_CLICKABLE);
   lv_obj_set_pos(ui->screen_main_1_icon_app, 5, 20);
   lv_obj_set_size(ui->screen_main_1_icon_app, 70, 70);
   lv_image_set_src(ui->screen_main_1_icon_app, PIC_APP_70);
   lv_obj_set_style_pad_all(ui->screen_main_1_icon_app, 0, LV_STATE_DEFAULT);

   // Write style for screen_main_1_icon_app, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.

   // Write style for screen_main_1_icon_app, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
   lv_obj_set_style_image_recolor_opa(ui->screen_main_1_icon_app, 0, LV_PART_MAIN | LV_STATE_PRESSED);
   lv_obj_set_style_image_opa(ui->screen_main_1_icon_app, 170, LV_PART_MAIN | LV_STATE_PRESSED);

   // Write style for screen_main_1_icon_app, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
   lv_obj_set_style_image_recolor_opa(ui->screen_main_1_icon_app, 0, LV_PART_MAIN | LV_STATE_CHECKED);
   lv_obj_set_style_image_opa(ui->screen_main_1_icon_app, 255, LV_PART_MAIN | LV_STATE_CHECKED);


   // Write codes screen_main_1_icon_cal
ui->screen_main_1_icon_cal = lv_image_create(ui->screen_main_1);
   lv_obj_add_flag(ui->screen_main_1_icon_cal, LV_OBJ_FLAG_CLICKABLE);
   lv_obj_set_pos(ui->screen_main_1_icon_cal, 85, 20);
   lv_obj_set_size(ui->screen_main_1_icon_cal, 70, 70);
   lv_image_set_src(ui->screen_main_1_icon_cal, PIC_CALC_70);
   lv_obj_set_style_pad_all(ui->screen_main_1_icon_cal, 0, LV_STATE_DEFAULT);

   // Write style for screen_main_1_icon_cal, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.

   // Write style for screen_main_1_icon_cal, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
   lv_obj_set_style_image_recolor_opa(ui->screen_main_1_icon_cal, 0, LV_PART_MAIN | LV_STATE_PRESSED);
   lv_obj_set_style_image_opa(ui->screen_main_1_icon_cal, 170, LV_PART_MAIN | LV_STATE_PRESSED);

   // Write style for screen_main_1_icon_cal, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
   lv_obj_set_style_image_recolor_opa(ui->screen_main_1_icon_cal, 0, LV_PART_MAIN | LV_STATE_CHECKED);
   lv_obj_set_style_image_opa(ui->screen_main_1_icon_cal, 255, LV_PART_MAIN | LV_STATE_CHECKED);


   // Write codes screen_main_1_icon_clk
ui->screen_main_1_icon_clk = lv_image_create(ui->screen_main_1);
   lv_obj_add_flag(ui->screen_main_1_icon_clk, LV_OBJ_FLAG_CLICKABLE);
   lv_obj_set_pos(ui->screen_main_1_icon_clk, 165, 20);
   lv_obj_set_size(ui->screen_main_1_icon_clk, 70, 70);
   lv_image_set_src(ui->screen_main_1_icon_clk, PIC_CLOCK_70);
   lv_obj_set_style_pad_all(ui->screen_main_1_icon_clk, 0, LV_STATE_DEFAULT);

   // Write style for screen_main_1_icon_clk, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.

   // Write style for screen_main_1_icon_clk, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
   lv_obj_set_style_image_recolor_opa(ui->screen_main_1_icon_clk, 0, LV_PART_MAIN | LV_STATE_PRESSED);
   lv_obj_set_style_image_opa(ui->screen_main_1_icon_clk, 170, LV_PART_MAIN | LV_STATE_PRESSED);

   // Write style for screen_main_1_icon_clk, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
   lv_obj_set_style_image_recolor_opa(ui->screen_main_1_icon_clk, 0, LV_PART_MAIN | LV_STATE_CHECKED);
   lv_obj_set_style_image_opa(ui->screen_main_1_icon_clk, 255, LV_PART_MAIN | LV_STATE_CHECKED);


   // Write codes screen_main_1_icon_music
ui->screen_main_1_icon_music = lv_image_create(ui->screen_main_1);
   lv_obj_add_flag(ui->screen_main_1_icon_music, LV_OBJ_FLAG_CLICKABLE);
   lv_obj_set_pos(ui->screen_main_1_icon_music, 165, 160);
   lv_obj_set_size(ui->screen_main_1_icon_music, 70, 70);
   lv_image_set_src(ui->screen_main_1_icon_music, PIC_MUSIC_70);
   lv_obj_set_style_pad_all(ui->screen_main_1_icon_music, 0, LV_STATE_DEFAULT);

   // Write style for screen_main_1_icon_music, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.

   // Write style for screen_main_1_icon_music, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
   lv_obj_set_style_image_recolor_opa(ui->screen_main_1_icon_music, 0, LV_PART_MAIN | LV_STATE_PRESSED);
   lv_obj_set_style_image_opa(ui->screen_main_1_icon_music, 170, LV_PART_MAIN | LV_STATE_PRESSED);

   // Write style for screen_main_1_icon_music, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
   lv_obj_set_style_image_recolor_opa(ui->screen_main_1_icon_music, 0, LV_PART_MAIN | LV_STATE_CHECKED);
   lv_obj_set_style_image_opa(ui->screen_main_1_icon_music, 255, LV_PART_MAIN | LV_STATE_CHECKED);


   // Write codes screen_main_1_icon_ebook
ui->screen_main_1_icon_ebook = lv_image_create(ui->screen_main_1);
   lv_obj_add_flag(ui->screen_main_1_icon_ebook, LV_OBJ_FLAG_CLICKABLE);
   lv_obj_set_pos(ui->screen_main_1_icon_ebook, 5, 160);
   lv_obj_set_size(ui->screen_main_1_icon_ebook, 70, 70);
   lv_image_set_src(ui->screen_main_1_icon_ebook, PIC_EBOOK_70);
   lv_obj_set_style_pad_all(ui->screen_main_1_icon_ebook, 0, LV_STATE_DEFAULT);

   // Write style for screen_main_1_icon_ebook, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.

   // Write style for screen_main_1_icon_ebook, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
   lv_obj_set_style_image_recolor_opa(ui->screen_main_1_icon_ebook, 0, LV_PART_MAIN | LV_STATE_PRESSED);
   lv_obj_set_style_image_opa(ui->screen_main_1_icon_ebook, 170, LV_PART_MAIN | LV_STATE_PRESSED);

   // Write style for screen_main_1_icon_ebook, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
   lv_obj_set_style_image_recolor_opa(ui->screen_main_1_icon_ebook, 0, LV_PART_MAIN | LV_STATE_CHECKED);
   lv_obj_set_style_image_opa(ui->screen_main_1_icon_ebook, 255, LV_PART_MAIN | LV_STATE_CHECKED);


   // Write codes screen_main_1_icon_look
ui->screen_main_1_icon_look = lv_image_create(ui->screen_main_1);
   lv_obj_add_flag(ui->screen_main_1_icon_look, LV_OBJ_FLAG_CLICKABLE);
   lv_obj_set_pos(ui->screen_main_1_icon_look, 85, 160);
   lv_obj_set_size(ui->screen_main_1_icon_look, 70, 70);
   lv_image_set_src(ui->screen_main_1_icon_look, PIC_LOOK_70);
   lv_obj_set_style_pad_all(ui->screen_main_1_icon_look, 0, LV_STATE_DEFAULT);

   // Write style for screen_main_1_icon_look, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.

   // Write style for screen_main_1_icon_look, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
   lv_obj_set_style_image_recolor_opa(ui->screen_main_1_icon_look, 0, LV_PART_MAIN | LV_STATE_PRESSED);
   lv_obj_set_style_image_opa(ui->screen_main_1_icon_look, 170, LV_PART_MAIN | LV_STATE_PRESSED);

   // Write style for screen_main_1_icon_look, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
   lv_obj_set_style_image_recolor_opa(ui->screen_main_1_icon_look, 0, LV_PART_MAIN | LV_STATE_CHECKED);
   lv_obj_set_style_image_opa(ui->screen_main_1_icon_look, 255, LV_PART_MAIN | LV_STATE_CHECKED);


   // Write codes screen_main_1_label_app
   ui->screen_main_1_label_app = lv_label_create(ui->screen_main_1);
   lv_obj_set_pos(ui->screen_main_1_label_app, 5, 100);
   lv_obj_set_size(ui->screen_main_1_label_app, 70, 20);
   lv_label_set_text(ui->screen_main_1_label_app, "应用");
   lv_label_set_long_mode(ui->screen_main_1_label_app, LV_LABEL_LONG_WRAP);

   // Write style for screen_main_1_label_app, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
   lv_obj_set_style_border_width(ui->screen_main_1_label_app, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_radius(ui->screen_main_1_label_app, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_color(ui->screen_main_1_label_app, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_font(ui->screen_main_1_label_app, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_opa(ui->screen_main_1_label_app, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_letter_space(ui->screen_main_1_label_app, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_line_space(ui->screen_main_1_label_app, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_align(ui->screen_main_1_label_app, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_bg_opa(ui->screen_main_1_label_app, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_top(ui->screen_main_1_label_app, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_right(ui->screen_main_1_label_app, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_bottom(ui->screen_main_1_label_app, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_left(ui->screen_main_1_label_app, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_shadow_width(ui->screen_main_1_label_app, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

   // Write codes screen_main_1_label_clk
   ui->screen_main_1_label_clk = lv_label_create(ui->screen_main_1);
   lv_obj_set_pos(ui->screen_main_1_label_clk, 165, 100);
   lv_obj_set_size(ui->screen_main_1_label_clk, 70, 20);
   lv_label_set_text(ui->screen_main_1_label_clk, "时钟");
   lv_label_set_long_mode(ui->screen_main_1_label_clk, LV_LABEL_LONG_WRAP);

   // Write style for screen_main_1_label_clk, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
   lv_obj_set_style_border_width(ui->screen_main_1_label_clk, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_radius(ui->screen_main_1_label_clk, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_color(ui->screen_main_1_label_clk, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_font(ui->screen_main_1_label_clk, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_opa(ui->screen_main_1_label_clk, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_letter_space(ui->screen_main_1_label_clk, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_line_space(ui->screen_main_1_label_clk, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_align(ui->screen_main_1_label_clk, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_bg_opa(ui->screen_main_1_label_clk, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_top(ui->screen_main_1_label_clk, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_right(ui->screen_main_1_label_clk, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_bottom(ui->screen_main_1_label_clk, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_left(ui->screen_main_1_label_clk, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_shadow_width(ui->screen_main_1_label_clk, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

   // Write codes screen_main_1_label_cal
   ui->screen_main_1_label_cal = lv_label_create(ui->screen_main_1);
   lv_obj_set_pos(ui->screen_main_1_label_cal, 85, 100);
   lv_obj_set_size(ui->screen_main_1_label_cal, 70, 20);
   lv_label_set_text(ui->screen_main_1_label_cal, "计算器");
   lv_label_set_long_mode(ui->screen_main_1_label_cal, LV_LABEL_LONG_WRAP);

   // Write style for screen_main_1_label_cal, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
   lv_obj_set_style_border_width(ui->screen_main_1_label_cal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_radius(ui->screen_main_1_label_cal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_color(ui->screen_main_1_label_cal, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_font(ui->screen_main_1_label_cal, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_opa(ui->screen_main_1_label_cal, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_letter_space(ui->screen_main_1_label_cal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_line_space(ui->screen_main_1_label_cal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_align(ui->screen_main_1_label_cal, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_bg_opa(ui->screen_main_1_label_cal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_top(ui->screen_main_1_label_cal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_right(ui->screen_main_1_label_cal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_bottom(ui->screen_main_1_label_cal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_left(ui->screen_main_1_label_cal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_shadow_width(ui->screen_main_1_label_cal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

   // Write codes screen_main_1_label_music
   ui->screen_main_1_label_music = lv_label_create(ui->screen_main_1);
   lv_obj_set_pos(ui->screen_main_1_label_music, 165, 240);
   lv_obj_set_size(ui->screen_main_1_label_music, 70, 20);
   lv_label_set_text(ui->screen_main_1_label_music, "音乐");
   lv_label_set_long_mode(ui->screen_main_1_label_music, LV_LABEL_LONG_WRAP);

   // Write style for screen_main_1_label_music, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
   lv_obj_set_style_border_width(ui->screen_main_1_label_music, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_radius(ui->screen_main_1_label_music, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_color(ui->screen_main_1_label_music, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_font(ui->screen_main_1_label_music, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_opa(ui->screen_main_1_label_music, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_letter_space(ui->screen_main_1_label_music, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_line_space(ui->screen_main_1_label_music, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_align(ui->screen_main_1_label_music, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_bg_opa(ui->screen_main_1_label_music, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_top(ui->screen_main_1_label_music, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_right(ui->screen_main_1_label_music, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_bottom(ui->screen_main_1_label_music, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_left(ui->screen_main_1_label_music, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_shadow_width(ui->screen_main_1_label_music, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

   // Write codes screen_main_1_label_look
   ui->screen_main_1_label_look = lv_label_create(ui->screen_main_1);
   lv_obj_set_pos(ui->screen_main_1_label_look, 85, 240);
   lv_obj_set_size(ui->screen_main_1_label_look, 70, 20);
   lv_label_set_text(ui->screen_main_1_label_look, "查看");
   lv_label_set_long_mode(ui->screen_main_1_label_look, LV_LABEL_LONG_WRAP);

   // Write style for screen_main_1_label_look, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
   lv_obj_set_style_border_width(ui->screen_main_1_label_look, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_radius(ui->screen_main_1_label_look, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_color(ui->screen_main_1_label_look, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_font(ui->screen_main_1_label_look, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_opa(ui->screen_main_1_label_look, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_letter_space(ui->screen_main_1_label_look, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_line_space(ui->screen_main_1_label_look, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_align(ui->screen_main_1_label_look, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_bg_opa(ui->screen_main_1_label_look, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_top(ui->screen_main_1_label_look, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_right(ui->screen_main_1_label_look, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_bottom(ui->screen_main_1_label_look, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_left(ui->screen_main_1_label_look, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_shadow_width(ui->screen_main_1_label_look, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

   // Write codes screen_main_1_label_ebook
   ui->screen_main_1_label_ebook = lv_label_create(ui->screen_main_1);
   lv_obj_set_pos(ui->screen_main_1_label_ebook, 5, 240);
   lv_obj_set_size(ui->screen_main_1_label_ebook, 70, 20);
   lv_label_set_text(ui->screen_main_1_label_ebook, "电子书");
   lv_label_set_long_mode(ui->screen_main_1_label_ebook, LV_LABEL_LONG_WRAP);

   // Write style for screen_main_1_label_ebook, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
   lv_obj_set_style_border_width(ui->screen_main_1_label_ebook, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_radius(ui->screen_main_1_label_ebook, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_color(ui->screen_main_1_label_ebook, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_font(ui->screen_main_1_label_ebook, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_opa(ui->screen_main_1_label_ebook, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_letter_space(ui->screen_main_1_label_ebook, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_line_space(ui->screen_main_1_label_ebook, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_text_align(ui->screen_main_1_label_ebook, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_bg_opa(ui->screen_main_1_label_ebook, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_top(ui->screen_main_1_label_ebook, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_right(ui->screen_main_1_label_ebook, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_bottom(ui->screen_main_1_label_ebook, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_pad_left(ui->screen_main_1_label_ebook, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
   lv_obj_set_style_shadow_width(ui->screen_main_1_label_ebook, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

   // The custom code of screen_main_1.

   // Update current screen layout.
   lv_obj_update_layout(ui->screen_main_1);

   // Init events for screen.
   events_init_screen_main_1(ui);
}
