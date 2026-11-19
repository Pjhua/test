# FreeRTOS 移植 + 与 LVGL 共用 CCM 内存（改动记录）

日期：2026-11-14
工程：`E:\XYD\STM32\project\test`（STM32F405RG，Keil AC5，标准外设库 SPL，LVGL 9.2）
参考工程：`E:\XYD\STM32\19.FreeRTOS\4.示例程序\freeRTOS`

---

## 1. 内存布局（改完之后）

```
CCM 0x10000000 ─┬─ ucHeap 60KB (0x10000000 ~ 0x1000EFFF)  ← FreeRTOS 堆，heap_ccm.c 定点定义
                │     ├─ LVGL TLSF 池 40KB（lv_init() 里一次性切走，给 lv_malloc 用）
                │     └─ 剩余 ~20KB：任务栈 / TCB / 队列 / 软件定时器
                └─ 0x1000F000 ~ 0x1000FFFF  4KB 空着（原来放 malloc.c 的 CCM 管理表，现已停用）

SRAM 0x20000000 ── 131072 B，当前 RW_IRAM1 = 122192 B，剩 ~8880 B
                └─ 只放 DMA 要访问的东西：LVGL 局部刷新缓冲、LCD 颜色缓冲、
                   I2S 音频双缓冲、SD 卡缓冲、mem1 SRAM 池、主栈、C 库堆
```

原则：**CCM 只有 CPU 能访问，DMA 访问不到**。凡是要过 DMA 的缓冲一律不许从 `pvPortMalloc` 里拿。

## 2. 改了哪些文件

| 文件 | 改动 |
|---|---|
| `freeRTOS\Source\user\heap_ccm.c` | **新建**。`configAPPLICATION_ALLOCATED_HEAP==1` 时用 `__attribute__((at(0x10000000)))` 定义 `ucHeap[configTOTAL_HEAP_SIZE]` |
| `freeRTOS\Source\include\FreeRTOSConfig.h` | `configTOTAL_HEAP_SIZE` 75KB → **60KB**；新增 `configAPPLICATION_ALLOCATED_HEAP 1` |
| `LVGL\lvgl_src\lv_conf.h` | `LV_MEM_SIZE` 56KB → **40KB**；`LV_MEM_POOL_INCLUDE` 改 `"FreeRTOS.h"`；`LV_MEM_POOL_ALLOC(size)` 改 `pvPortMalloc(size)` |
| `HARDWARE\malloc\malloc.c` | `mem2base`（CCM 60KB 池）与 `mem2mapbase`（CCM 4KB 管理表）AC5/AC6 两分支共 4 行全部注释停用；`mallco_dev` 里两个指针改 `NULL` |
| `user\main.c` | 加 `FreeRTOS.h`/`task.h`；`NVIC_PriorityGroup_2` → **`_4`**；`my_mem_init(SRAMCCM)` 注释停用；新增 `lvgl_task()`；原 `while(1)` 换成 `xTaskCreate` + `vTaskStartScheduler()` |
| `freeRTOS\Source\user\freertos_app.c` | 3 处 `KEY1_PRESS/KEY2_PRESS/KEY3_PRESS` → `KEY1_PRE/KEY2_PRE/KEY3_PRE`（本工程 key.h 只有 `_PRE`，纯编译修复） |
| `智慧生活.uvprojx` | `freeRTOS_app` 组内新增 `heap_ccm.c` 文件节点 |

备份（可整体回滚）：`_backup_freertos_ccm_20261114\`

## 3. 编译验证结果（UV4 命令行，AC5）

```
Program Size: Code=275086  RO-data=574742  RW-data=1120  ZI-data=1230256
0 Error(s), 1 Warning(s)   ← 唯一警告：freertos_app.c 变量 ret 未引用
```

map 实测：

```
ucHeap              0x10000000  Data  61440  heap_ccm.o(.ARM.__AT_0x10000000)
ER$$.ARM.__AT_0x10000000   Size 0x0000F000 (60KB) UNINIT
RW_IRAM1                   Size 0x0001DD50 (122192B) / Max 0x20000 → 剩 8880B
lv_mem_core_builtin.o(i.lv_mem_init) refers to heap_4.o(i.pvPortMalloc)
SysTick_Handler  ← delay.o(i.SysTick_Handler) → xPortSysTickHandler ✓
SVC_Handler / PendSV_Handler ← port.o(.emb_text) ✓
```

原来的 `LR$$.ARM.__AT_0x1000F000`（CCM 管理表）已经不存在了，说明 CCM 整块腾给了 FreeRTOS。

## 4. 上板验证要点

1. 开机应当正常进 LVGL 界面（`lv_init()` → 从堆里切 40KB → `vTaskStartScheduler()`）。
2. 串口若打印 `FreeRTOS 启动失败!` → 堆不够，`xTaskCreate` 或调度器没起来。
3. 串口若打印 `vApplicationMallocFailedHook` / `stack overflow` → 对应调小/调大下面两个数：
   - LVGL 界面控件消失/花屏/切相册就崩 → `lv_conf.h` 的 `LV_MEM_SIZE` 40KB 提到 44KB（堆 60KB 仍够，FreeRTOS 余量降到 16KB）
   - LVGL 任务栈不够（解码大图时崩）→ `main.c` 里 `xTaskCreate(..., 1024, ...)` 提到 1536
4. 音频 `mus_tick()` 现在在 LVGL 任务里每 10ms 跑一次（`vTaskDelay` 走 1ms tick），补数节奏与原来一致。
5. 中断优先级：分组改成 4 之后，EXTI/USART1/DMA 仍是抢占 0，TIM 是 1~3，都**不能**调 `FromISR` 系列 API；如果以后要在这些中断里发信号量/队列，得把抢占优先级改到 ≥5（`configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY`）。

## 5. 还没处理的隐患（与本次改动无关，但多任务后会放大）

- `mymalloc / myfree`（`HARDWARE\malloc\malloc.c`）没有临界区保护，多任务同时分配会撕裂链表 —— 目前只有主任务在初始化阶段用，暂时安全。
- C 库 `malloc`（`lcd.c` 里的 `row_buf/readBuf`）和 `printf` 不可重入。
- `LV_USE_OS = LV_OS_NONE`：LVGL 只能在一个任务里访问，别在别的任务里碰任何 `lv_*`。
- `freertos_app.c` 是教学例程（`lcd_task` 直接 `LCD_ShowString/LCD_Fill`），**main 没有调用 `freertos_init()`**，现在只是被编译的死代码；一旦调用会与 LVGL 抢同一块屏。
