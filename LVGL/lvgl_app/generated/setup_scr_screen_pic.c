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

void setup_scr_screen_pic(lv_ui *ui)
{
    // Write codes screen_pic
    ui->screen_pic = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_pic, 240, 280);
    lv_obj_set_scrollbar_mode(ui->screen_pic, LV_SCROLLBAR_MODE_OFF);

    // Write style for screen_pic, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_pic, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    // 系统背景图：相册没有可显示的图片时，用它作为兜底占位
    lv_obj_set_style_bg_image_src(ui->screen_pic, PIC_BACK_240280, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_pic, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_pic, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_pic_img_2
    ui->screen_pic_img_2 = lv_image_create(ui->screen_pic);
    lv_obj_set_pos(ui->screen_pic_img_2, 0, 20);
    lv_obj_set_size(ui->screen_pic_img_2, 240, 240);
    lv_obj_add_flag(ui->screen_pic_img_2, LV_OBJ_FLAG_CLICKABLE);
    /* 不再使用编译进 MCU 的固定图片；图片全部由 SD 卡扫描后动态设置 */
    lv_image_set_src(ui->screen_pic_img_2, NULL);
    lv_image_set_pivot(ui->screen_pic_img_2, 50, 50);
    lv_image_set_rotation(ui->screen_pic_img_2, 0);

    // Write style for screen_pic_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_pic_img_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_pic_img_2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // The custom code of screen_pic.

    // Update current screen layout.
    lv_obj_update_layout(ui->screen_pic);

    // Init events for screen.
    events_init_screen_pic(ui);
}
