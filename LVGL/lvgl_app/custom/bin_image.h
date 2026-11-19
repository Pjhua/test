#ifndef __BIN_IMAGE_H
#define __BIN_IMAGE_H

#include "stm32f4xx.h"

/* ============================================================
 *  W25Q64 分区地址规划
 *  ------------------------------------------------------------
 *  字库区:   扇区 0 ~ 238    (239个扇区, 约 956KB)
 *            地址 0x000000 ~ 0x0ECFFF
 *            内容: myFont_simkai16.bin (字库)
 *
 *  图片bin区: 扇区 239 ~ 335  (97个扇区, 约 388KB)
 *            地址 0x0EF000 ~ 0x14FFFF
 *            内容: mergeBinFile.bin (guider 生成的图片合并bin, 397092B)
 *
 *  烧录标志:  扇区 400        (留有余量)
 *            地址 0x190000
 *            内容: 字库+图片烧录完成标志 (0xB6)
 * ============================================================ */

/* --- 字库参数 --- */
#define FONT_START_SECTOR   0
#define FONT_SECTOR_COUNT   239                            /* 239*4096 = 978944 ≥ 976581 */
#define FONT_FLASH_ADDR     (FONT_START_SECTOR * 4096)     /* 0x000000 */
#define FONT_FILE_PATH      "0:/font/myFont_simkai16.bin"
#define FONT_FILE_SIZE      976581                         /* 字库实际字节数 */

/* --- 图片 bin 参数 (紧跟字库之后, 字节连续) --- */
#define IMG_START_SECTOR    (FONT_START_SECTOR + FONT_SECTOR_COUNT)   /* 239 */
#define IMG_SECTOR_COUNT    97                             /* 97*4096 = 397312 ≥ 397092 */
#define IMG_FLASH_ADDR      (IMG_START_SECTOR * 4096)      /* 0x0EF000 */
#define IMG_FILE_PATH       "0:/font/mergeBinFile.bin"
#define IMG_FILE_SIZE       397092                         /* 图片 bin 实际字节数(含 PlayP.bin) */

/* --- 单文件合并烧录 (字库 + 图片 = 一个 bin, 一次烧完) ---
 *   resource.bin = myFont_simkai16.bin + (补齐到 239 扇区) + mergeBinFile.bin
 *   烧到扇区 0 起的 336 个扇区后, 布局和上面两段式完全一致 */
#define RES_FILE_PATH       "0:/font/resource.bin"
#define RES_TOTAL_SECTOR_COUNT  336
#define RES_FILE_SIZE      1376036                         /* 978944(字库补齐) + 397092(图片) */

/* --- 烧录标志 ---
 *   magic 变了 → 上电检测失配 → 自动重烧，保证新图片资源完整写入
 *   (0xB6 是资源版本号；旧版烧录值是 0xA5) */
#define RES_FLAG_SECTOR     400
#define RES_FLAG_ADDR       (RES_FLAG_SECTOR * 4096)       /* 0x190000 */
#define RES_FLAG_MAGIC      0xB6

/* --- 功能函数 --- */

/**
 * @brief  自动检查 W25Q64 字库+图片是否已烧录，未烧录则自动烧录
 *         上电时按住按键可强制重烧
 */
void is_loadfont(void);

#endif /* __BIN_IMAGE_H */
