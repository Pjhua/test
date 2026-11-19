/**
 * @file    app_tasks.c
 * @brief   FreeRTOS 任务总入口
 *
 *  任务结构（全部由“开始任务”统一创建，创建完自删除）：
 *    START   开始任务    创建下面 4 个任务后退出
 *    LVGL    界面任务    lv_task_handler + 界面事件（lv_* 只允许在这里调用）
 *    MUSIC   音乐任务    mus_tick() 喂 DMA 缓冲 + 语音播报/音乐命令执行
 *    WIFI    通信任务    语音模块(USART3)识别结果处理 + ESP8266(USART2) WiFi/MQTT
 *    MEAS    测量任务    SHT30 温湿度 + 光敏 ADC + MPU6050 姿态，结果放共享快照供 UI 读取
 */

#include "app_tasks.h"

#include <stdio.h>
#include <math.h>

#include "FreeRTOS.h"
#include "task.h"

#include "gui_guider.h"
#include "custom.h"
#include "music_player.h"
#include "events_init.h"
#include "voice.h"
#include "timer.h"   /* time_handle: 消费 TIM7 tick_cnt 让 g_datetime 走时 */
#include "esp8266.h"

#include "sht3x.h"
#include "adc.h"
#include "mpu6050.h"
#include "inv_mpu.h"

/* 任务优先级：音乐补数 > 界面 > 通信 > 测量 */
#define PRIO_MUSIC   6
#define PRIO_LVGL    5
#define PRIO_WIFI    4
#define PRIO_START   3
#define PRIO_MEAS    2

/* 任务栈（单位：字，1 字 = 4 字节）
   FreeRTOS 堆在 CCM 64KB，configTOTAL_HEAP_SIZE=60KB，
   LVGL 预切 40KB，剩余 ~20KB 给任务栈+队列。
   开机剩余堆通常 ~17KB，5 个任务总栈必须 ≤ 约 4300 字。 */
#define STK_START    192
#define STK_LVGL     1152  /* 曾为 1024，弹窗 lv_obj_create 深调用链加一点余量 */
#define STK_MUSIC    1536  /* 语音播报 + FatFs f_open + wav 解析 */
#define STK_WIFI     960   /* 语音模块处理 + ESP8266 AT/MQTT，比 LVGL 省 */
#define STK_MEAS     384

volatile u8 g_app_hw_ready = 0;
static volatile u8 s_audio_ok = 0; /* MUSIC 任务是否创建成功（失败时由 LVGL 任务兜底补数） */


static app_measure_t s_meas;

void app_measure_get(app_measure_t *out)
{
    if (out != NULL)
        *out = s_meas;
}

/* ======================= 测量采集 ======================= */

/* DMP 读不到数据时的兜底姿态解算（加速度计算 pitch/roll，陀螺仪 Z 轴积分算 yaw） */
static u8    m_dmp_ok = 0; /* DMP 是否成功输出过姿态 */
static float m_dmp_pitch = 0.0f, m_dmp_roll = 0.0f, m_dmp_yaw = 0.0f;
static u8    m_att_ready = 0;
static float m_pitch = 0.0f, m_roll = 0.0f, m_yaw = 0.0f;

static void mpu_raw_attitude(void)
{
    short ax, ay, az, gx, gy, gz;
    float acc_pitch, acc_roll, gz_dps;

    if (Mpu6050_GetAccelerometer(&ax, &ay, &az) != 0)
        return;
    if (Mpu6050_GetGyroscope(&gx, &gy, &gz) != 0)
        return;

    /* ±2g 量程：16384 LSB/g */
    acc_pitch = (float)(atan2((double)ay, (double)az) * 57.29578);
    acc_roll = (float)(atan2((double)(-ax),
                             sqrt((double)ay * (double)ay + (double)az * (double)az)) *
                      57.29578);

    if (!m_att_ready)
    {
        m_pitch = acc_pitch;
        m_roll = acc_roll;
        m_yaw = 0.0f;
        m_att_ready = 1;
    }
    else
    {
        m_pitch = m_pitch * 0.7f + acc_pitch * 0.3f;
        m_roll = m_roll * 0.7f + acc_roll * 0.3f;

        /* ±2000dps 量程：16.4 LSB/(°/s)，死区防静止漂移 */
        gz_dps = (float)gz / 16.4f;
        if (gz_dps > 1.5f || gz_dps < -1.5f)
            m_yaw += gz_dps * 0.3f;
        if (m_yaw > 180.0f)
            m_yaw -= 360.0f;
        else if (m_yaw < -180.0f)
            m_yaw += 360.0f;
    }
}

static void mpu_update_attitude(void)
{
    int i, got = 0;
    float p = 0.0f, r = 0.0f, y = 0.0f;

    /* 一次读空 FIFO（最多 8 包），取最新姿态，避免转动后显示滞后 */
    for (i = 0; i < 8; i++)
    {
        if (mpu_dmp_get_data(&p, &r, &y) != 0)
            break;
        got = 1;
    }

    if (got)
    {
        m_dmp_ok = 1;
        m_dmp_pitch = p;
        m_dmp_roll = r;
        m_dmp_yaw = y;
        m_pitch = p;
        m_roll = r;
        m_yaw = y;
    }
    else if (m_dmp_ok)
    {
        /* DMP 已工作，只是本次 FIFO 没新数据：保持上次角度 */
        m_pitch = m_dmp_pitch;
        m_roll = m_dmp_roll;
        m_yaw = m_dmp_yaw;
    }
    else
    {
        mpu_raw_attitude(); /* DMP 从未成功过，用原始数据兜底 */
    }

    s_meas.pitch = m_pitch;
    s_meas.roll = m_roll;
    s_meas.yaw = m_yaw;
}

static void measure_task(void *arg)
{
    (void)arg;

    /* 等 custom_init() 完成（SHT30/ADC/MPU DMP 都在那里初始化） */
    while (!g_app_hw_ready)
        vTaskDelay(pdMS_TO_TICKS(100));

    while (1)
    {
        Sht3x_TypeDef sht3;

        if (Sht3x_Read_TemperatureHumidity(&sht3) == 0)
        {
            s_meas.temperature = sht3.temperature;
            s_meas.humidity = sht3.humidity;
        }

        {
            u16 adc_light = ADC_Filter(10, LIGHT_CH_IDX);
            u32 mv = (u32)adc_light * 3300 / 4096;
            s_meas.light_mv = (u16)mv;
            s_meas.light_pct = Get_LightIntensity(mv);
        }

        mpu_update_attitude();

        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

/* ======================= 界面任务 ======================= */

static void lvgl_task(void *arg)
{
    (void)arg;

    music_ctl_init(); /* 全局长按检测（音乐后台控制弹窗/下拉框） */

    /* 慢速外设初始化放这里：首帧已由 main 的 lv_refr_now() 刷上屏，
       传感器晚 1~2 秒就绪不影响界面显示 */
    custom_init(&guider_ui);
    g_app_hw_ready = 1;

    while (1)
    {
        lv_task_handler();
        time_handle(); /* 消费 TIM7 的 1s tick，让 g_datetime 走时（极轻：ticks 通常 0 或 1） */
        if (!s_audio_ok) /* MUSIC 任务没建起来时，兜底在本任务补数 */
        {
            voice_music_service();
            mus_tick();
            mus_vol_service();
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/* ======================= 音乐任务 ======================= */

static void music_task(void *arg)
{
    (void)arg;
    while (1)
    {
        /* 语音播报、音乐命令、暂停/续播都在这里执行（FatFs/wav 解析也落在本任务栈） */
        voice_music_service();

        mus_tick();
        /* 音量平滑：必须放在常驻任务里，否则离开音乐界面后
           语音“增大/减小音量”只改了目标值，硬件音量不动 */
        mus_vol_service();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

/* ======================= WiFi / 语音任务 ======================= */

static void wifi_task(void *arg)
{
    u8 mqtt_ok = 0;
    TickType_t last_pub;
    TickType_t last_net_sync;
    TickType_t last_reconn;

    (void)arg;
    voice_init();      /* 清空语音模块接收状态 */
    voice_handle();
    vTaskDelay(pdMS_TO_TICKS(300));

    /* WiFi 连接是阻塞式 AT 交互（内部用 delay_ms → vTaskDelay，会让出 CPU，
       不影响界面和音乐）。连不上也继续跑：语音控制不依赖网络 */
    printf("[wifi] 正在连接 WiFi...\r\n");
    if (esp8266_connect_wifi() == 0)
    {
        printf("[wifi] WiFi 已连接\r\n");
        esp8266_sync_net_time(); /* 拉网络时间（失败不阻塞，后面会后台重试） */
        if (esp8266_connect_mqtt() == 0)
            mqtt_ok = 1;
    }
    else
    {
        printf("[wifi] WiFi 未连接，仅使用语音控制\r\n");
    }

    last_pub = xTaskGetTickCount();
    last_net_sync = xTaskGetTickCount();
    last_reconn = xTaskGetTickCount();

    while (1)
    {
        /* 语音模块 HLK-V20：识别到命令词 → 控制硬件 + 把回复音交给 MUSIC 任务 */
        voice_handle();

        /* WiFi 界面下发的扫描/连接请求（AT 交互耗时，不能放在界面任务里） */
        if (esp8266_wifi_service())
        {
            printf("[wifi] 新热点连接成功，重连 MQTT\r\n");
            mqtt_ok = (esp8266_connect_mqtt() == 0);
            last_reconn = xTaskGetTickCount();
            last_net_sync = xTaskGetTickCount(); /* 换热点后重新计时 */
        }

        /* MQTT 掉线后每 30 秒自动重连（AT 交互会让出 CPU，不影响界面和音乐） */
        if (!mqtt_ok &&
            (xTaskGetTickCount() - last_reconn) >= pdMS_TO_TICKS(30000))
        {
            printf("[wifi] 尝试重连 MQTT...\r\n");
            mqtt_ok = (esp8266_connect_mqtt() == 0);
            if (mqtt_ok)
            {
                printf("[wifi] MQTT 重连成功\r\n");
                last_pub = xTaskGetTickCount();
            }
            last_reconn = xTaskGetTickCount();
        }

        /* 网络时间后台重试：还没同步成功时每 60 秒再试一次
           （函数内部有 synced 标志，已成功直接 return 不浪费时间） */
        if ((xTaskGetTickCount() - last_net_sync) >= pdMS_TO_TICKS(60000))
        {
            esp8266_sync_net_time();
            last_net_sync = xTaskGetTickCount();
        }

        if (mqtt_ok)
        {
            /* 云端下发的设备控制 JSON */
            esp8266_mqtt_handle();

            /* 每 30 秒上报一次全部数据（环境 + 姿态 + 设备状态），
               发布失败视为掉线，转去后台重连 */
            if ((xTaskGetTickCount() - last_pub) >= pdMS_TO_TICKS(30000))
            {
                app_measure_t m;
                last_pub = xTaskGetTickCount();
                app_measure_get(&m);
                if (esp826_mqtt_pub(m.temperature, m.humidity,
                                    m.light_mv, m.light_pct,
                                    m.pitch, m.roll, m.yaw) != 0)
                {
                    printf("[wifi] MQTT 发布失败，判定掉线，进入后台重连\r\n");
                    mqtt_ok = 0;
                    last_reconn = xTaskGetTickCount();
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/* ======================= 开始任务 ======================= */

static void start_task(void *arg)
{
    (void)arg;

    printf("[rtos] 开始创建任务，剩余堆=%u B\r\n", (unsigned)xPortGetFreeHeapSize());

    if (xTaskCreate(lvgl_task, "LVGL", STK_LVGL, NULL, PRIO_LVGL, NULL) != pdPASS)
        printf("[rtos] LVGL 任务创建失败(堆不够)\r\n");

    if (xTaskCreate(music_task, "MUSIC", STK_MUSIC, NULL, PRIO_MUSIC, NULL) == pdPASS)
        s_audio_ok = 1;
    else
        printf("[rtos] MUSIC 任务创建失败(堆不够)，音频补数退回 LVGL 任务\r\n");

    if (xTaskCreate(wifi_task, "WIFI", STK_WIFI, NULL, PRIO_WIFI, NULL) != pdPASS)
        printf("[rtos] WIFI 任务创建失败(堆不够)\r\n");

    if (xTaskCreate(measure_task, "MEAS", STK_MEAS, NULL, PRIO_MEAS, NULL) != pdPASS)
        printf("[rtos] MEAS 任务创建失败(堆不够)\r\n");

    printf("[rtos] 任务创建完成，剩余堆=%u B\r\n", (unsigned)xPortGetFreeHeapSize());

    vTaskDelete(NULL); /* 开始任务使命完成，自删除 */
}

void app_tasks_start(void)
{
    if (xTaskCreate(start_task, "START", STK_START, NULL, PRIO_START, NULL) != pdPASS)
        printf("[rtos] START 任务创建失败(堆不够)\r\n");
}
