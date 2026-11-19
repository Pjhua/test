/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include "gui_guider.h"
#include "widgets_init.h"
#include "timer.h"
#include <stdlib.h>
#include <string.h>


__attribute__((unused)) void kb_event_cb (lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *kb = lv_event_get_target(e);
    if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

__attribute__((unused)) void ta_event_cb (lv_event_t *e) {
#if LV_USE_KEYBOARD
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);

    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_active()) != LV_INDEV_TYPE_KEYPAD) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
        }
    } else if(code == LV_EVENT_READY) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_state(ta, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, ta);
    } else if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
#endif
}

/* ============================ 时钟界面定时器 ============================
   两个 timer 都直接读 g_datetime（由 RTC 硬件 + 1s tick 维护），
   避免 GUI Guider 那套"起始值 + 自增"的假时钟逻辑。
   timer 由 events_init_screen_clk() 在 SCREEN_LOADED 创建、SCREEN_UNLOADED 删除。 */

void screen_clk_analog_clock_1_timer(lv_timer_t *timer)
{
    if (!lv_obj_is_valid(guider_ui.screen_clk_analog_clock_1)) return;

    /* 时针带小数：比如 3:30 时指针指向 3 又 1/2 的位置，更自然 */
    int sec_val = (int)g_datetime.sec;
    int min_val = (int)g_datetime.min;
    int hr_val  = (int)g_datetime.hour % 12; /* 0~11 */
    int hr_hand = hr_val * 5 + min_val / 12;  /* 0~59 */

    lv_scale_set_line_needle_value(guider_ui.screen_clk_analog_clock_1,
                                   guider_ui.screen_clk_analog_clock_1_hour_needle, 24, hr_hand);
    lv_scale_set_line_needle_value(guider_ui.screen_clk_analog_clock_1,
                                   guider_ui.screen_clk_analog_clock_1_min_needle, 36, min_val);
    lv_scale_set_line_needle_value(guider_ui.screen_clk_analog_clock_1,
                                   guider_ui.screen_clk_analog_clock_1_sec_needle, 50, sec_val);
}

void screen_clk_digital_clock_1_timer(lv_timer_t *timer)
{
    if (!lv_obj_is_valid(guider_ui.screen_clk_digital_clock_1)) return;
    if (!lv_obj_is_valid(guider_ui.screen_clk_datetext_1))     return;

    lv_label_set_text_fmt(guider_ui.screen_clk_digital_clock_1,
                          "%02d:%02d:%02d", g_datetime.hour, g_datetime.min, g_datetime.sec);
    lv_label_set_text_fmt(guider_ui.screen_clk_datetext_1,
                          "%04d-%02d-%02d", g_datetime.year, g_datetime.month, g_datetime.day);
}

/* 下面三个函数是 GUI Guider 生成的日历默认处理，嵌入式上没用，保留 stub 避免链接错误 */

void screen_clk_datetext_1_event_handler(lv_event_t *e)
{
    (void)e; /* 不跳日历：嵌入式屏幕太小、也没有输入法 */
}

void screen_clk_datetext_1_init_calendar(lv_obj_t *obj, char * s) { (void)obj; (void)s; }
void screen_clk_datetext_1_calendar_event_handler(lv_event_t *e)  { (void)e; }


