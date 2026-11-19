/**
 * @file    bin_image.c
 * @brief   W25Q64 资源烧录 —— 字库 + 图片 bin
 *
 *  上电时 is_loadfont() 检查烧录标志:
 *     未烧录 (或被擦除) → 自动把字库和图片写进 W25Q64, 再写标志
 *     已烧录           → 直接跳过
 *     启动时按住按键   → 强制重烧
 *
 *  地址规划 (W25Q64, 共 8MB):
 *  ┌──────────────────────────────────────┬─────────────┬─────────────────────┐
 *  │ myFont_simkai16.bin (字库)           │ 扇区 0   ~238│ 0x000000 ~ 0x0ECFFF │
 *  │ mergeBinFile.bin (guider 图片合并)   │ 扇区 239 ~335│ 0x0EF000 ~ 0x14FFFF │
 *  │ 烧录标志 0xB6                        │ 扇区 400     │ 0x190000            │
 *  └──────────────────────────────────────┴─────────────┴─────────────────────┘
 */

#include "bin_image.h"
#include "stm32f4xx.h"
#include "w25qxx.h"
#include "key.h"
#include "ff.h"
#include "stdio.h"

/**
 * @brief  自动烧录入口 —— 检查烧录标志，未烧录则把字库+图片写进 W25Q64
 *
 *  优先烧单文件 "0:/font/resource.bin" (字库+图片合成一个 bin, 一次烧完);
 *  SD 卡上没有 resource.bin 时, 退回两段式: 先字库, 再 mergeBinFile.bin。
 *  两种方式烧完之后 W25Q64 里的布局完全相同。
 */
void is_loadfont(void)
{
    u8 flag = 0;
    u8 magic = RES_FLAG_MAGIC;
    u8 key  = key_scan();

    /* 1. 读取烧录标志 */
    W25qxx_ReadDatas(&flag, RES_FLAG_ADDR, 1);
    printf("\r\n[is_loadfont] 烧录标志 = 0x%02X (期望 0x%02X)%s\r\n",
           flag, magic, key ? " [按键触发重烧录]" : "");

    /* 标志不对 OR 按键按下 → 触发完整烧录 */
    if (flag != magic || key)
    {
        FILINFO fno;

        /* ========== 方式一：单文件 resource.bin，一次烧完 ========== */
        if (f_stat(RES_FILE_PATH, &fno) == FR_OK &&
            fno.fsize >= RES_FILE_SIZE &&
            fno.fsize <= RES_TOTAL_SECTOR_COUNT * 4096)
        {
            printf("[is_loadfont] 烧录合并文件 %s (%lu 字节) 到扇区 0 ~ %d\r\n",
                   RES_FILE_PATH, fno.fsize, RES_TOTAL_SECTOR_COUNT - 1);
            W25qxx_fatfs_DownloadFile(FONT_START_SECTOR, RES_TOTAL_SECTOR_COUNT, RES_FILE_PATH);
        }
        else
        {
            /* ========== 方式二：字库、图片分两个文件烧 ========== */
            printf("[is_loadfont] 未找到可用的 %s, 改为分段烧录\r\n", RES_FILE_PATH);

            /* 先确认两个文件版本都不小于新资源，避免把旧 bin 烧进去导致图片错位 */
            if (f_stat(FONT_FILE_PATH, &fno) != FR_OK ||
                fno.fsize < FONT_FILE_SIZE ||
                fno.fsize > FONT_SECTOR_COUNT * 4096)
            {
                printf("[is_loadfont] ✗ 字库文件缺失/过旧: %s\r\n", FONT_FILE_PATH);
                return;
            }
            if (f_stat(IMG_FILE_PATH, &fno) != FR_OK ||
                fno.fsize < IMG_FILE_SIZE ||
                fno.fsize > IMG_SECTOR_COUNT * 4096)
            {
                printf("[is_loadfont] ✗ 图片 bin 缺失/过旧(需 %d 字节): %s\r\n",
                       IMG_FILE_SIZE, IMG_FILE_PATH);
                return;
            }

            printf("[is_loadfont] 烧录字库 %s (扇区 0 ~ %d, %dKB)\r\n",
                   FONT_FILE_PATH, FONT_SECTOR_COUNT - 1, FONT_SECTOR_COUNT * 4);
            W25qxx_fatfs_DownloadFile(FONT_START_SECTOR, FONT_SECTOR_COUNT, FONT_FILE_PATH);

            printf("[is_loadfont] 烧录图片 %s (扇区 %d ~ %d)\r\n",
                   IMG_FILE_PATH, IMG_START_SECTOR, IMG_START_SECTOR + IMG_SECTOR_COUNT - 1);
            W25qxx_fatfs_DownloadFile(IMG_START_SECTOR, IMG_SECTOR_COUNT, IMG_FILE_PATH);
        }

        /* ========== 写烧录标志 ========== */
        printf("[is_loadfont] 写入烧录标志到扇区 %d ...\r\n", RES_FLAG_SECTOR);
        W25qxx_EraseSector(RES_FLAG_SECTOR);
        W25qxx_WritePage(&magic, RES_FLAG_ADDR, 1);

        /* 读回校验 */
        flag = 0;
        W25qxx_ReadDatas(&flag, RES_FLAG_ADDR, 1);
        if (flag == magic)
            printf("[is_loadfont] ✓ 烧录完成，下次上电直接使用。\r\n");
        else
            printf("[is_loadfont] ✗ 烧录标志写入失败！(读回 0x%02X)\r\n", flag);
    }
    else
    {
        printf("[is_loadfont] ✓ W25Q64 中字库+图片已就绪。\r\n");
    }
}
