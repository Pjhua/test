#ifndef _ESP8266_H
#define _ESP8266_H

#include "stm32f4xx.h"
#include "delay.h"
#include "usart2.h"
#include <stdlib.h>
#include <stdio.h>
#include "cJSON.h"
#include "relay.h"
#include "fan.h"
#include "led.h"
#include "beep.h"

#define WIFI_SSID "113"
#define WIFI_PWD "244466666"

#define MQTT_SERVER "gz-5-mqtt.iot-api.com"
#define MQTT_PORT 1883

#define MQTT_USERNAME "qdxm4bzvfyxzy116"
#define MQTT_PWD "GHHPQBx3hl"
#define MQTT_CLIENT_ID "ghhsjdjfHUKd58hjfudujfivj742HJdr"

u8 esp8266_send_cmd(u8 *str, u8 *ack, u16 timeout);
u8 esp8266_connect_wifi(void);

/* ================= WiFi 扫描 / 连接（WiFi 界面用） =================
   界面(LVGL 任务)只投递请求，真正的 AT 交互在 WIFI 任务里做：
     esp8266_wifi_request_scan()        请求扫描
     esp8266_wifi_request_join(ssid,pwd) 请求连接
     esp8266_wifi_service()             WIFI 任务每轮调用，返回 1 表示刚连上新热点 */
#define WIFI_AP_MAX   20
#define WIFI_SSID_LEN 32
#define WIFI_PWD_LEN  64

typedef struct
{
    char ssid[WIFI_AP_MAX][WIFI_SSID_LEN + 1];
    u8 count;
    volatile u8 state; /* 0=空闲 1=扫描中 2=扫描完成 3=失败 */
} wifi_scan_list;

#define WIFI_JOIN_IDLE 0
#define WIFI_JOIN_BUSY 1 /* 连接中（界面置位） */
#define WIFI_JOIN_OK   2
#define WIFI_JOIN_FAIL 3

extern wifi_scan_list g_wifi_scan;
extern volatile u8 g_wifi_join_state;

void esp8266_wifi_request_scan(void);
u8 esp8266_wifi_request_join(const char *ssid, const char *pwd);
u8 esp8266_wifi_service(void);

u8 esp8266_sync_net_time(void); /* TCP 直连 k780 HTTP 取时间 → 更新 g_datetime + 硬件 RTC */

u8 esp8266_connect_mqtt(void);
int esp8266_mqtt_extract_json(const char *buf, char *out, int out_size);
u8 esp8266_mqtt_parse_json(const char *json);
void esp8266_mqtt_handle(void);
/* 返回 0 发布成功 / 1 发布失败（用于 MQTT 掉线检测与重连） */
u8 esp826_mqtt_pub(float temperature, float humidity,
                     u16 light_mv, u8 light_pct,
                     float pitch, float roll, float yaw);

#endif
