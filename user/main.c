#include "stm32f4xx.h" // Device header
#include "led.h"
#include "key.h"
#include "beep.h"
#include "relay.h"
#include "fan.h"
#include "delay.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "exti.h"
#include "timer.h"
#include "w25qxx.h"
#include "capture.h"
#include "rtc.h"
#include "sd_driver.h"
#include "ff.h"
#include "malloc.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "gui_guider.h"
#include "app_tasks.h"
#include "FreeRTOS.h"
#include "task.h"

lv_ui guider_ui;

int main(void)
{
    u8 ret;
    FATFS fs;
    FRESULT res;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // NVIC分组：FreeRTOS要求4位全为抢占优先级
    led_init();
    key_init();
    beep_init();
    relay_init();
    fan_init();
    delay_init(168);
    usart1_init(115200);
    usart2_init(115200);
    usart3_init(115200); /* 语音模块 HLK-V20 */
    EXTI_Config();
    timer6_init(2, 42);
    timer7_init(10000, 8400);
    timer2_init(2000, 8400);
    W25qxx_Init();
    rtc_init();
    rtc_read_datetime(); /* 把硬件 RTC 的时间同步到 g_datetime，界面时钟从此刻开始走 */
    sr04_init();
    my_mem_init(SRAMIN); // 内存管理

    /* ============== SD 卡 + 文件系统 ============== */
    ret = SD_Initialize();
    printf(ret == 0 ? "SD卡初始化成功\r\n" : "SD卡初始化失败\r\n");

    res = f_mount(&fs, "0", 0);
    printf(res == FR_OK ? "挂载成功\r\n" : "挂载失败\r\n");

    /* ============== 自动烧码字库 + 图片 bin (如果尚未烧录) ============== */
    is_loadfont();

    /* ============== 初始化 LVGL + 挂载 flash 图片文件系统 ============== */
    lv_init();
    timer1_lvgl_init();          // lvgl 心跳
    lv_port_disp_init();         // lvgl 显示
    lv_port_indev_init();        // lvgl 输入
    lv_fs_fatfs_init();          // '0' 盘符: SD 卡
    lv_merge_decoder_init();     // 'M' 盘符: W25Q64 里的 mergeBinFile.bin

    /* ============== 界面 (图片源由 gui_guider.h 的 LV_PIC_SRC 决定) ============== */
    setup_ui(&guider_ui);
    lv_refr_now(NULL); /* 调度器启动前先刷一帧，上电立即可见界面，避免白屏等待 */

    /* ============== 启动 FreeRTOS：由开始任务创建其它任务 ============== */
    app_tasks_start();
    vTaskStartScheduler(); /* 调度器启动后不再返回 */

    /* 只有堆不足、调度器起不来才会执行到这里 */
    printf("FreeRTOS 启动失败!\r\n");
    while (1)
    {
    }
}
