/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"

#if LV_USE_FS_RAWFS

const rawfs_size_t rawfs_file_count = 16;
rawfs_file_t rawfs_files[16] = {
	0x0, 0, 14712, "/app_70.bin",
	0x3978, 0, 14712, "/calc_70.bin",
	0x72f0, 0, 14712, "/clock_70.bin",
	0xac68, 0, 14712, "/music_70.bin",
	0xe5e0, 0, 14712, "/look.bin",
	0x11f58, 0, 14712, "/ebook_70.bin",
	0x158d0, 0, 14712, "/sms_70.bin",
	0x19248, 0, 14712, "/set_70.bin",
	0x1cbc0, 0, 14712, "/picture_70.bin",
	0x20538, 0, 14712, "/paint_70.bin",
	0x23eb0, 0, 14712, "/notepad_70.bin",
	0x27828, 0, 8412, "/PrevP.bin",
	0x29904, 0, 8412, "/NextP.bin",
	0x2b9e0, 0, 8412, "/PauseP.bin",
	0x2dabc, 0, 8412, "/PlayP.bin",
	0x2fb98, 0, 201612, "/back_240280.bin",

};

#endif  /*LV_USE_FS_RAWFS*/ 
