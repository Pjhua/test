/*
 * Copyright 2026 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and be bound by such license terms.  If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"

/* 电子书界面（240x280 竖屏）
 *   顶栏   : 书名（平常只显示文字，长按才换出下拉框切书）+ 第几本/共几本
 *   分隔线 : 1px 浅灰，把顶栏 / 正文 / 底栏分成三层
 *   正文   : 14 个汉字宽 x 11 行，224x198，16px 字 + 2px 行距，
 *            给 17~18px 高的字形留出上下空间，不再互相压字或被裁切
 *   底栏   : 上一页 / 页码 + 百分比 / 下一页，进度条贴在最下方
 * 这里刻意不用背景图片：LV_CACHE_DEF_SIZE=0 时每帧都要从 W25Q64 解码 200KB 图片，
 * 翻页会明显卡顿，纯白底最快。 */

#define EBOOK_COLOR_TEXT   0x0D3055  /* 主文字（全局规范） */
#define EBOOK_COLOR_MAIN   0x2195F6  /* 主色 */
#define EBOOK_COLOR_LINE   0xE6E2E6  /* 分隔线 / 轨道 */
#define EBOOK_COLOR_BORDER 0xE1E6EE  /* 控件边框 */

/* 1px 分隔线：不可点、不可滚，避免吃掉手势和点击 */
static lv_obj_t *ebook_line_create(lv_obj_t *parent, int x, int y, int w)
{
  lv_obj_t *line = lv_obj_create(parent);

  lv_obj_set_pos(line, x, y);
  lv_obj_set_size(line, w, 1);
  lv_obj_remove_flag(line, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(line, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_bg_color(line, lv_color_hex(EBOOK_COLOR_LINE), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(line, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(line, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_radius(line, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_all(line, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  return line;
}

/* 翻页按钮：32x22 圆角块，内置左右箭头，按下变深蓝，首/末页置灰 */
static lv_obj_t *ebook_btn_create(lv_obj_t *parent, int x, const char *symbol)
{
  lv_obj_t *btn = lv_button_create(parent);
  lv_obj_t *ico;

  lv_obj_set_pos(btn, x, 237);
  lv_obj_set_size(btn, 32, 22);
  lv_obj_set_style_radius(btn, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(btn, lv_color_hex(EBOOK_COLOR_MAIN), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(btn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(btn, lv_color_hex(0x1670C9), LV_PART_MAIN | LV_STATE_PRESSED);
  lv_obj_set_style_bg_color(btn, lv_color_hex(0xC7CDD6), LV_PART_MAIN | LV_STATE_DISABLED);
  lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_all(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

  ico = lv_label_create(btn);
  lv_label_set_text(ico, symbol);
  lv_obj_set_style_text_font(ico, &lv_font_montserratMedium_14, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ico, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(ico);
  return btn;
}

void setup_scr_screen_ebook(lv_ui *ui)
{
  // Write codes screen_ebook
  ui->screen_ebook = lv_obj_create(NULL);
  lv_obj_set_size(ui->screen_ebook, 240, 280);
  lv_obj_set_scrollbar_mode(ui->screen_ebook, LV_SCROLLBAR_MODE_OFF);

  // Write style for screen_ebook, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
  lv_obj_set_style_bg_color(ui->screen_ebook, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(ui->screen_ebook, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

  /* ===== 顶栏：书名下拉框 + 第几本/共几本 ===== */
  ui->screen_ebook_ddlist_book = lv_dropdown_create(ui->screen_ebook);
  lv_obj_set_pos(ui->screen_ebook_ddlist_book, 6, 4);
  lv_obj_set_size(ui->screen_ebook_ddlist_book, 178, 24);
  lv_dropdown_set_options(ui->screen_ebook_ddlist_book, "无书籍");
  lv_dropdown_set_symbol(ui->screen_ebook_ddlist_book, LV_SYMBOL_DOWN);
  /* 平常隐藏，原位置只显示书名；长按才唤出下拉框（见 events_init.c） */
  lv_obj_add_flag(ui->screen_ebook_ddlist_book, LV_OBJ_FLAG_HIDDEN);

  lv_obj_set_style_text_color(ui->screen_ebook_ddlist_book, lv_color_hex(EBOOK_COLOR_TEXT), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui->screen_ebook_ddlist_book, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui->screen_ebook_ddlist_book, lv_color_hex(EBOOK_COLOR_MAIN), LV_PART_INDICATOR | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui->screen_ebook_ddlist_book, &lv_font_montserratMedium_14, LV_PART_INDICATOR | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(ui->screen_ebook_ddlist_book, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_color(ui->screen_ebook_ddlist_book, lv_color_hex(EBOOK_COLOR_BORDER), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_radius(ui->screen_ebook_ddlist_book, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(ui->screen_ebook_ddlist_book, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui->screen_ebook_ddlist_book, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_top(ui->screen_ebook_ddlist_book, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_left(ui->screen_ebook_ddlist_book, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_right(ui->screen_ebook_ddlist_book, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_width(ui->screen_ebook_ddlist_book, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

  // 下拉列表样式（最多显示 90px 高，超出可滚动；当前书蓝色高亮）
  static lv_style_t style_screen_ebook_ddlist_book_list;
  ui_init_style(&style_screen_ebook_ddlist_book_list);
  lv_style_set_max_height(&style_screen_ebook_ddlist_book_list, 90);
  lv_style_set_text_font(&style_screen_ebook_ddlist_book_list, &myFont_simkai16);
  lv_style_set_text_color(&style_screen_ebook_ddlist_book_list, lv_color_hex(EBOOK_COLOR_TEXT));
  lv_style_set_border_width(&style_screen_ebook_ddlist_book_list, 1);
  lv_style_set_border_color(&style_screen_ebook_ddlist_book_list, lv_color_hex(EBOOK_COLOR_BORDER));
  lv_style_set_radius(&style_screen_ebook_ddlist_book_list, 3);
  lv_style_set_bg_opa(&style_screen_ebook_ddlist_book_list, 255);
  lv_style_set_bg_color(&style_screen_ebook_ddlist_book_list, lv_color_hex(0xffffff));
  lv_obj_add_style(lv_dropdown_get_list(ui->screen_ebook_ddlist_book), &style_screen_ebook_ddlist_book_list, LV_PART_MAIN | LV_STATE_DEFAULT);

  static lv_style_t style_screen_ebook_ddlist_book_selected;
  ui_init_style(&style_screen_ebook_ddlist_book_selected);
  lv_style_set_bg_opa(&style_screen_ebook_ddlist_book_selected, 255);
  lv_style_set_bg_color(&style_screen_ebook_ddlist_book_selected, lv_color_hex(EBOOK_COLOR_MAIN));
  lv_style_set_text_color(&style_screen_ebook_ddlist_book_selected, lv_color_hex(0xffffff));
  lv_style_set_radius(&style_screen_ebook_ddlist_book_selected, 3);
  lv_obj_add_style(lv_dropdown_get_list(ui->screen_ebook_ddlist_book), &style_screen_ebook_ddlist_book_selected, LV_PART_SELECTED | LV_STATE_CHECKED);

  // Write codes screen_ebook_label_title（第几本/共几本）
  ui->screen_ebook_label_title = lv_label_create(ui->screen_ebook);
  lv_obj_set_pos(ui->screen_ebook_label_title, 188, 8);
  lv_obj_set_size(ui->screen_ebook_label_title, 46, 16);
  lv_obj_set_style_pad_all(ui->screen_ebook_label_title, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui->screen_ebook_label_title, lv_color_hex(EBOOK_COLOR_TEXT), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui->screen_ebook_label_title, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui->screen_ebook_label_title, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_label_set_long_mode(ui->screen_ebook_label_title, LV_LABEL_LONG_CLIP);
  lv_label_set_text(ui->screen_ebook_label_title, "0/0");

  /* 顶栏与正文之间的分隔线 */
  ebook_line_create(ui->screen_ebook, 6, 31, 228);

  // Write codes screen_ebook_label_text（正文：14 汉字 x 11 行 + 2px 行距）
  ui->screen_ebook_label_text = lv_label_create(ui->screen_ebook);
  lv_obj_set_pos(ui->screen_ebook_label_text, 6, 33);
  lv_obj_set_size(ui->screen_ebook_label_text, 228, 198);
  lv_obj_set_style_pad_all(ui->screen_ebook_label_text, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui->screen_ebook_label_text, lv_color_hex(EBOOK_COLOR_TEXT), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui->screen_ebook_label_text, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui->screen_ebook_label_text, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
  /* 字库里有 17~18px 高的字形，行距 0 会上下相压、末行还会被 label 底边裁掉；
     顶部再留 1px，避免第一行高出字身 1px 的部分被上边缘裁掉 */
  lv_obj_set_style_text_line_space(ui->screen_ebook_label_text, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_top(ui->screen_ebook_label_text, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_label_set_long_mode(ui->screen_ebook_label_text, LV_LABEL_LONG_CLIP);
  lv_label_set_text(ui->screen_ebook_label_text, "");

  /* 正文与底栏之间的分隔线 */
  ebook_line_create(ui->screen_ebook, 6, 233, 228);

  /* ===== 底栏：上一页 / 页码 / 下一页 / 进度条 ===== */
  ui->screen_ebook_btn_prev = ebook_btn_create(ui->screen_ebook, 6, LV_SYMBOL_LEFT);
  ui->screen_ebook_btn_next = ebook_btn_create(ui->screen_ebook, 202, LV_SYMBOL_RIGHT);

  // Write codes screen_ebook_label_page（页码 + 进度百分比）
  ui->screen_ebook_label_page = lv_label_create(ui->screen_ebook);
  lv_obj_set_pos(ui->screen_ebook_label_page, 44, 239);
  lv_obj_set_size(ui->screen_ebook_label_page, 152, 18);
  lv_obj_set_style_pad_all(ui->screen_ebook_label_page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui->screen_ebook_label_page, lv_color_hex(EBOOK_COLOR_TEXT), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui->screen_ebook_label_page, &myFont_simkai16, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(ui->screen_ebook_label_page, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_label_set_long_mode(ui->screen_ebook_label_page, LV_LABEL_LONG_CLIP);
  lv_label_set_text(ui->screen_ebook_label_page, "第0/0页 0%");

  // Write codes screen_ebook_bar_progress（底栏细进度条）
  ui->screen_ebook_bar_progress = lv_bar_create(ui->screen_ebook);
  lv_obj_set_pos(ui->screen_ebook_bar_progress, 6, 266);
  lv_obj_set_size(ui->screen_ebook_bar_progress, 228, 7);
  lv_bar_set_range(ui->screen_ebook_bar_progress, 0, 100);
  lv_bar_set_value(ui->screen_ebook_bar_progress, 0, LV_ANIM_OFF);
  lv_obj_set_style_bg_color(ui->screen_ebook_bar_progress, lv_color_hex(EBOOK_COLOR_LINE), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(ui->screen_ebook_bar_progress, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_radius(ui->screen_ebook_bar_progress, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui->screen_ebook_bar_progress, lv_color_hex(EBOOK_COLOR_MAIN), LV_PART_INDICATOR | LV_STATE_DEFAULT);
  lv_obj_set_style_radius(ui->screen_ebook_bar_progress, 3, LV_PART_INDICATOR | LV_STATE_DEFAULT);

  // The custom code of screen_ebook.

  // Update current screen layout.
  lv_obj_update_layout(ui->screen_ebook);

  // Init events for screen.
  events_init_screen_ebook(ui);
}
