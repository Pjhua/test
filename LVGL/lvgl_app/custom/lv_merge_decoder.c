/**
 * @file    lv_merge_decoder.c
 * @brief   把 guider 的 mergeBinFile.bin (rawfs 合并格式) 挂成 LVGL 文件系统
 *
 * 背景:
 *   guider 生成 mergeBinFile.bin (397092 字节), 内部按偏移量塞了 16 张图,
 *   并配套生成 mergeBinFile.c 里的文件表 (文件名 + 偏移 + 大小)。
 *   本工程 LVGL 没有 rawfs 驱动, 这里手写一个等价的最小 FS 驱动。
 *
 * 原理:
 *   注册 lv_fs_drv, 字母 'M'。LVGL 打开 "M:/app_70.bin" 时:
 *     open_cb → 查表得到该图在 mergeBinFile.bin 内的偏移和大小, 分配一份文件状态
 *     seek_cb → 位置 = 图起始偏移 + pos (LVGL 的 seek 都是相对这个"文件"的)
 *     read_cb → 从 W25Q64 或 SD 卡读对应字节
 *   之后 LVGL 内置的 lv_bin_decoder 负责解析 header、取像素 (RGB565A8 双平面),
 *   无需自定义 decoder。
 *
 * 用法:
 *   lv_merge_decoder_init();
 *   lv_image_set_src(img, "M:/app_70.bin");
 *   lv_image_set_src(img, "M:/back_240280.bin");
 */

#include "lv_merge_decoder.h"
#include "ff.h"
#include "w25qxx.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if MERGE_SRC == MERGE_SRC_FLASH
  #include "bin_image.h"     /* IMG_FLASH_ADDR */
#endif

/* ============================================================================
 *  文件表 —— 对应 guider 生成的 mergeBinFile.c
 *  字段: 在 mergeBinFile.bin 内的偏移, 文件大小(含12字节头), 文件名
 * ============================================================================ */
typedef struct {
    u32         offset;
    u32         size;
    const char *name;
} merge_file_t;

static const merge_file_t merge_files[] = {
    { 0x00000, 14712,  "/app_70.bin"      },
    { 0x03978, 14712,  "/calc_70.bin"     },
    { 0x072f0, 14712,  "/clock_70.bin"    },
    { 0x0ac68, 14712,  "/music_70.bin"    },
    { 0x0e5e0, 14712,  "/look.bin"        },
    { 0x11f58, 14712,  "/ebook_70.bin"    },
    { 0x158d0, 14712,  "/sms_70.bin"      },
    { 0x19248, 14712,  "/set_70.bin"      },
    { 0x1cbc0, 14712,  "/picture_70.bin"  },
    { 0x20538, 14712,  "/paint_70.bin"    },
    { 0x23eb0, 14712,  "/notepad_70.bin"  },
    { 0x27828, 8412,   "/PrevP.bin"       },
    { 0x29904, 8412,   "/NextP.bin"       },
    { 0x2b9e0, 8412,   "/PauseP.bin"      },
    { 0x2dabc, 8412,   "/PlayP.bin"       },
    { 0x2fb98, 201612, "/back_240280.bin" },
};
#define MERGE_FILE_COUNT  (sizeof(merge_files) / sizeof(merge_files[0]))

/* 每个打开的文件一份独立状态 (同屏多图时不能共用) */
typedef struct {
    u32  start;    /* 该文件在 mergeBinFile.bin 内的偏移 */
    u32  size;     /* 该文件大小 */
    u32  pos;      /* 当前读位置(相对文件起点) */
#if MERGE_SRC == MERGE_SRC_SD
    FIL  file;
    u8   opened;
#endif
} merge_fs_state_t;

/* ============ FS 回调 ============ */

static void *fs_open_cb(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);
    LV_UNUSED(mode);

    /* path 已去掉驱动字母, 形如 "/app_70.bin" */
    const merge_file_t *mf = NULL;
    for (u32 i = 0; i < MERGE_FILE_COUNT; i++) {
        if (strcmp(path, merge_files[i].name) == 0) { mf = &merge_files[i]; break; }
    }
    if (mf == NULL) {
        printf("[mergefs] 未知文件: %s\r\n", path);
        return NULL;
    }

    merge_fs_state_t *st = (merge_fs_state_t *)lv_malloc(sizeof(merge_fs_state_t));
    if (st == NULL) {
        printf("[mergefs] 内存不足\r\n");
        return NULL;
    }
    lv_memzero(st, sizeof(merge_fs_state_t));
    st->start = mf->offset;
    st->size  = mf->size;

#if MERGE_SRC == MERGE_SRC_SD
    if (f_open(&st->file, "0:/font/mergeBinFile.bin", FA_READ) != FR_OK) {
        printf("[mergefs] SD 卡打不开 mergeBinFile.bin\r\n");
        lv_free(st);
        return NULL;
    }
    st->opened = 1;
#endif

    printf("[mergefs] open %s (offset 0x%05X, %u bytes)\r\n", path, st->start, st->size);
    return (void *)st;
}

static lv_fs_res_t fs_close_cb(lv_fs_drv_t *drv, void *file_p)
{
    LV_UNUSED(drv);
    merge_fs_state_t *st = (merge_fs_state_t *)file_p;
    if (st == NULL) return LV_FS_RES_INV_PARAM;
#if MERGE_SRC == MERGE_SRC_SD
    if (st->opened) f_close(&st->file);
#endif
    lv_free(st);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_read_cb(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    LV_UNUSED(drv);
    merge_fs_state_t *st = (merge_fs_state_t *)file_p;
    if (st == NULL || br == NULL) return LV_FS_RES_INV_PARAM;

    /* 越界裁剪, 防止读到下一张图 */
    if (st->pos >= st->size) { *br = 0; return LV_FS_RES_OK; }
    if (btr > st->size - st->pos) btr = st->size - st->pos;

#if MERGE_SRC == MERGE_SRC_FLASH
    {
        u32 addr = IMG_FLASH_ADDR + st->start + st->pos;
        u8 *p = (u8 *)buf;
        u32 done = 0;
        while (done < btr) {
            u16 chunk = (u16)((btr - done > 65530) ? 65530 : (btr - done));
            W25qxx_ReadDatas(p + done, addr + done, chunk);
            done += chunk;
        }
        *br = btr;
    }
#else
    {
        UINT n = 0;
        f_lseek(&st->file, st->start + st->pos);
        f_read(&st->file, buf, btr, &n);
        *br = n;
    }
#endif

    st->pos += *br;
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_seek_cb(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    merge_fs_state_t *st = (merge_fs_state_t *)file_p;
    if (st == NULL) return LV_FS_RES_INV_PARAM;

    int32_t new_pos;
    switch (whence) {
        case LV_FS_SEEK_SET: new_pos = (int32_t)pos; break;
        case LV_FS_SEEK_CUR: new_pos = (int32_t)st->pos + (int32_t)pos; break;
        case LV_FS_SEEK_END: new_pos = (int32_t)st->size + (int32_t)pos; break;
        default: return LV_FS_RES_INV_PARAM;
    }
    if (new_pos < 0) new_pos = 0;
    if ((u32)new_pos > st->size) new_pos = (int32_t)st->size;
    st->pos = (u32)new_pos;
    return LV_FS_RES_OK;
}

/* ============ 初始化 ============ */

void lv_merge_decoder_init(void)
{
    static lv_fs_drv_t drv;
    lv_fs_drv_init(&drv);
    drv.letter   = 'M';
    drv.open_cb  = fs_open_cb;
    drv.close_cb = fs_close_cb;
    drv.read_cb  = fs_read_cb;
    drv.seek_cb  = fs_seek_cb;
    drv.ready_cb = NULL;
    lv_fs_drv_register(&drv);

    printf("[mergefs] 驱动 'M' 已挂载, 数据源=%s, 共 %d 个文件 (例: \"M:/app_70.bin\")\r\n",
           (MERGE_SRC == MERGE_SRC_FLASH) ? "W25Q64" : "SD 卡",
           (int)MERGE_FILE_COUNT);
}
