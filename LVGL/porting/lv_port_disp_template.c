/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include <stdbool.h>
#include "lcd_init.h"
#include "lcd.h"

#define MY_DISP_HOR_RES LCD_W
#define MY_DISP_VER_RES LCD_H

/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
#warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 320 is used for now.
#define MY_DISP_HOR_RES 320
#endif

#ifndef MY_DISP_VER_RES
#warning Please define or replace the macro MY_DISP_VER_RES with the actual screen height, default value 240 is used for now.
#define MY_DISP_VER_RES 240
#endif

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /*will be 2 for RGB565 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    lv_display_t *disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_flush_cb(disp, disp_flush);

    /* Example 2
     * Two buffers for partial rendering
     * 每块缓冲 10 行，逐行 DMA 发送，避免单次 NDTR 超过 65535 字节限制 */
    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_2_1[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];

    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_2_2[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];
    lv_display_set_buffers(disp, buf_2_1, buf_2_2, sizeof(buf_2_1), LV_DISPLAY_RENDER_MODE_PARTIAL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
    LCD_Init();                          // LCD初始化
    LCD_Fill(0, 0, LCD_W, LCD_H, WHITE); // 上电清屏
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}


/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/
static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    if(disp_flush_enabled) {
        uint16_t width  = area->x2 - area->x1 + 1;
        uint16_t height = area->y2 - area->y1 + 1;
        uint32_t total_bytes = (uint32_t)width * height * BYTE_PER_PIXEL;

        /* LVGL输出为小端RGB565，LCD需要大端，逐像素交换高低字节 */
        uint32_t i;
        for(i = 0; i < total_bytes; i += 2) {
            uint8_t tmp   = px_map[i];
            px_map[i]     = px_map[i + 1];
            px_map[i + 1] = tmp;
        }

        /* 设置 LCD 显示区域 (内部自动发送 0x2A/0x2B/0x2C 命令) */
        LCD_Address_Set(area->x1, area->y1, area->x2, area->y2);

        /* 整块区域一次DMA发送，避免逐行调用带来的片选/DMA配置开销
         * (DMA单次长度上限65535，超过则分段) */
        uint8_t *p = px_map;
        uint32_t remain = total_bytes;
        while(remain > 0) {
            uint32_t chunk = (remain > 60000U) ? 60000U : remain;
            LCD_Writ_Bus_DMA(p, (uint16_t)chunk);
            p += chunk;
            remain -= chunk;
        }
    }

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_display_flush_ready(disp_drv);
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
