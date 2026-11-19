#include "lvgl_fs.h"
#include "../../lvgl_src/lvgl.h"
#include "ff.h"
#include <stdlib.h>

static void *fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br);
static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p);

void lv_port_fs_init(void)
{
    static lv_fs_drv_t fs_drv;
    lv_fs_drv_init(&fs_drv);

    fs_drv.letter = '0';              // 关键：盘符必须是 '0'，跟你路径里的 "0:/" 对应
    fs_drv.open_cb  = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb  = fs_read;
    fs_drv.seek_cb  = fs_seek;
    fs_drv.tell_cb  = fs_tell;

    lv_fs_drv_register(&fs_drv);
}

static void *fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);
    FIL *f = (FIL *)malloc(sizeof(FIL));
    if (f == NULL) return NULL;

    FRESULT res;
    if (mode == LV_FS_MODE_RD)
        res = f_open(f, path, FA_READ);
    else if (mode == LV_FS_MODE_WR)
        res = f_open(f, path, FA_WRITE);
    else
        res = f_open(f, path, FA_READ | FA_WRITE);

    if (res != FR_OK) { free(f); return NULL; }
    return f;
}

static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p)
{
    LV_UNUSED(drv);
    f_close(file_p);
    free(file_p);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    LV_UNUSED(drv);
    UINT br2;
    FRESULT res = f_read(file_p, buf, btr, &br2);
    *br = br2;
    return (res == FR_OK) ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    FSIZE_t off = 0;
    if (whence == LV_FS_SEEK_SET) off = pos;
    else if (whence == LV_FS_SEEK_CUR) off = f_tell(file_p) + pos;
    else if (whence == LV_FS_SEEK_END) off = f_size(file_p) + pos;
    return (f_lseek(file_p, off) == FR_OK) ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    LV_UNUSED(drv);
    *pos_p = f_tell(file_p);
    return LV_FS_RES_OK;
}
