/*=============================================================================
 * heap_ccm.c  ——  FreeRTOS 堆的存放位置（放在 CCM）
 *
 * CCM 地址 0x10000000 ~ 0x10010000，共 64KB，只有 CPU 能访问，DMA 访问不到。
 * 这里放的都是纯 CPU 访问的数据：
 *   任务栈、TCB、队列/信号量、软件定时器，以及 LVGL 的 TLSF 内存池
 *   （LVGL 的 LV_MEM_POOL_ALLOC 已改成 pvPortMalloc，从这块钱里切）
 *
 * !! 绝对不要用 pvPortMalloc 出来的内存做 DMA 缓冲：
 *    LCD_Writ_Bus_DMA(像素缓冲)、I2S 音频双缓冲、SDIO、ADC 等都必须留在 SRAM
 *    （static 数组 / 堆栈 / mymalloc(SRAMIN) 池）
 *
 * 对应配置：FreeRTOSConfig.h 中 configAPPLICATION_ALLOCATED_HEAP = 1
 *===========================================================================*/
#include "FreeRTOS.h"

#if ( configAPPLICATION_ALLOCATED_HEAP == 1 )

#if defined ( __CC_ARM )        /* Keil AC5 */
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__((at(0x10000000)));
#else                           /* AC6 / GCC：需要分散加载里放行 .bss.ARM.__at_0x10000000 */
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__((section(".bss.ARM.__at_0x10000000")));
#endif

#endif /* configAPPLICATION_ALLOCATED_HEAP */
