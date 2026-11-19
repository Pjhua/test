/**
 * @file    lv_merge_decoder.h
 * @brief   把 guider 的 mergeBinFile.bin (rawfs 合并格式) 挂成 LVGL 文件系统
 *
 * 用法:
 *   lv_init();
 *   lv_port_disp_init();
 *   lv_merge_decoder_init();                        // 挂载驱动 'M'
 *   lv_image_set_src(img, "M:/app_70.bin");         // 按名字取图
 *   lv_image_set_src(img, "M:/back_240280.bin");
 *
 * 文件名与偏移量来自 guider 生成的 mergeBinFile.c:
 *   /app_70.bin        /calc_70.bin     /clock_70.bin    /music_70.bin
 *   /look.bin          /ebook_70.bin    /sms_70.bin      /set_70.bin
 *   /picture_70.bin    /paint_70.bin    /notepad_70.bin
 *   /PrevP.bin         /NextP.bin       /PauseP.bin      /back_240280.bin
 */

#ifndef __LV_MERGE_DECODER_H
#define __LV_MERGE_DECODER_H

#include "stm32f4xx.h"
#include "lvgl.h"

/* ------- 数据源 ------- */
#define MERGE_SRC_SD     0    /* 从 SD 卡 0:/font/mergeBinFile.bin 读 */
#define MERGE_SRC_FLASH  1    /* 从 W25Q64 IMG_FLASH_ADDR 读 */

#define MERGE_SRC   MERGE_SRC_FLASH   /* ← 改这里切换 */

/* ------- 接口 ------- */
void lv_merge_decoder_init(void);

#endif /* __LV_MERGE_DECODER_H */
