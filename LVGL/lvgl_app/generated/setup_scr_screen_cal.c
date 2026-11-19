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

void setup_scr_screen_cal(lv_ui *ui)
{
    // Write codes screen_cal
    ui->screen_cal = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_cal, 240, 280);
    lv_obj_set_scrollbar_mode(ui->screen_cal, LV_SCROLLBAR_MODE_OFF);

    // Write style for screen_cal, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_cal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen_cal, PIC_BACK_240280, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_cal, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_cal, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_cal_btnm_1
    ui->screen_cal_btnm_1 = lv_buttonmatrix_create(ui->screen_cal);
    lv_obj_set_pos(ui->screen_cal_btnm_1, 0, 70);
    lv_obj_set_size(ui->screen_cal_btnm_1, 240, 210);
    static const char *screen_cal_btnm_1_text_map[] = {
        "C", LV_SYMBOL_BACKSPACE, "%", "/", "\n",
        "7", "8", "9", LV_SYMBOL_CLOSE, "\n",
        "4", "5", "6", "-", "\n",
        "1", "2", "3", "+", "\n",
        "0", ".", "=", "N", "",
    };
    lv_buttonmatrix_set_map(ui->screen_cal_btnm_1, screen_cal_btnm_1_text_map);

    // Write style for screen_cal_btnm_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_cal_btnm_1, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_cal_btnm_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_cal_btnm_1, lv_color_hex(0xc9c9c9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_cal_btnm_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_cal_btnm_1, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_cal_btnm_1, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_cal_btnm_1, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_cal_btnm_1, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui->screen_cal_btnm_1, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui->screen_cal_btnm_1, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_cal_btnm_1, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_cal_btnm_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_cal_btnm_1, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_cal_btnm_1, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_cal_btnm_1, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_cal_btnm_1, 1, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_cal_btnm_1, 255, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_cal_btnm_1, lv_color_hex(0xc9c9c9), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_cal_btnm_1, LV_BORDER_SIDE_FULL, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_cal_btnm_1, lv_color_hex(0xffffff), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_cal_btnm_1, &lv_font_montserratMedium_14, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_cal_btnm_1, 255, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_cal_btnm_1, 4, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_cal_btnm_1, 255, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_cal_btnm_1, lv_color_hex(0x2195f6), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_cal_btnm_1, LV_GRAD_DIR_NONE, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_cal_btnm_1, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);

    // Write codes screen_cal_label_1
    ui->screen_cal_label_1 = lv_label_create(ui->screen_cal);
    lv_obj_set_pos(ui->screen_cal_label_1, 0, 0);
    lv_obj_set_size(ui->screen_cal_label_1, 240, 70);
    lv_label_set_text(ui->screen_cal_label_1, "0");
    lv_label_set_long_mode(ui->screen_cal_label_1, LV_LABEL_LONG_WRAP);

    // Write style for screen_cal_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_cal_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_cal_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_cal_label_1, lv_color_hex(0x0D3055), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_cal_label_1, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_cal_label_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_cal_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_cal_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_cal_label_1, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_cal_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_cal_label_1, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_cal_label_1, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_cal_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_cal_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_cal_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // The custom code of screen_cal.

    // Update current screen layout.
    lv_obj_update_layout(ui->screen_cal);

    // Init events for screen.
    events_init_screen_cal(ui);
}
