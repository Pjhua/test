/**
 * @file    dbg_log.h
 * @brief   统一调试日志开关
 *
 * 把驱动层的调试 printf 收编为 DBG_LOG()，由编译期总开关控制：
 *   - 默认 APP_DEBUG_LOG=1：所有调试日志正常输出（调试期行为与原来完全一致）
 *   - 发布版在 Keil Options -> C/C++ -> Define 里追加 APP_DEBUG_LOG=0，
 *     即可把驱动调试日志整体从固件中剔除（省 Flash、避免串口刷屏）
 *
 * 应用层的诊断日志（main.c / app_tasks.c / FreeRTOS 钩子）不经过本开关，保持常开。
 */
#ifndef DBG_LOG_H
#define DBG_LOG_H

#include <stdio.h>

#ifndef APP_DEBUG_LOG
#define APP_DEBUG_LOG 1
#endif

#if APP_DEBUG_LOG
#define DBG_LOG(...) printf(__VA_ARGS__)
#else
#define DBG_LOG(...) do {} while (0)
#endif

#endif /* DBG_LOG_H */
