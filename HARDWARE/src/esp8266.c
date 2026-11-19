#include "esp8266.h"
#include "dbg_log.h"
#include "voice.h"
#include "rtc.h"
#include "timer.h"
#include "pwm.h"
#include <stdio.h>
#include <string.h>

// 通过串口2向wifi模块发送数据
u8 esp8266_send_cmd(u8 *cmd, u8 *ack, u16 timeout)
{
    usart2_send_string(cmd);
    u16 cnt = timeout / 10;
    if (ack == NULL)
    {
        /* 等待期间顺便处理语音：WiFi 连接再慢也不会让语音控制“失联” */
        while (cnt--)
        {
            voice_handle();
            delay_ms(10);
        }
        return 0;
    }
    while (cnt)
    {
        if (usart2.recv_flag)
        {
            if (strstr((char *)usart2.recv_buff, (char *)ack))
            {
                memset(&usart2, 0, sizeof(usart2));
                return 0;
            }
            memset(&usart2, 0, sizeof(usart2));
        }
        voice_handle(); /* AT 等包间隙处理语音命令 */
        cnt--;
        delay_ms(10);
        if (cnt == 0)
            return 1;
    }
    return 1;
}

/* 当前使用的热点：初始为编译期配置，WiFi 界面连接成功后更新，
   之后 esp8266_connect_wifi()/MQTT 重连都沿用这里的热点 */
static char s_sta_ssid[WIFI_SSID_LEN + 1] = WIFI_SSID;
static char s_sta_pwd[WIFI_PWD_LEN + 1] = WIFI_PWD;

static u8 wifi_at_init(void);          /* 定义在下面，这里先声明 */
static u8 wifi_wait_join(u16 timeout_ms);

/* 开机后只同步一次网络时间（第一次 esp8266_connect_wifi 真正连上时），
   之后 MQTT 掉线重连会频繁进这里，每次都发 HTTP 太浪费 */
static u8 s_net_time_synced = 0;

/* 连接当前热点（s_sta_ssid / s_sta_pwd） */
u8 esp8266_connect_wifi(void)
{
    static char cmd[128]; /* static 降栈消耗：STK_WIFI 只有 3.8KB */

    if (wifi_at_init() != 0)
    {
        DBG_LOG("[wifi] 模块无应答\r\n");
        return 1;
    }
    esp8266_send_cmd((u8 *)"AT+CWMODE=1\r\n", (u8 *)"OK", 1000);

    /* 已经拿到 IP 就不用重连（MQTT 掉线重连时省掉一次 CWJAP） */
    if (esp8266_send_cmd((u8 *)"AT+CIFSR\r\n", (u8 *)"0.0.0.0", 1000) != 0)
    {
        /* WiFi 已连：只在开机首次/换热点后同步时间，
           后续 MQTT 重连走这里也不应该再触发 HTTP GET */
        return 0;
    }

    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", s_sta_ssid, s_sta_pwd);
    memset(&usart2, 0, sizeof(usart2));
    usart2_send_string((u8 *)cmd);
    if (wifi_wait_join(15000) != 0)
    {
        memset(&usart2, 0, sizeof(usart2));
        DBG_LOG("[wifi] wifi连接失败\r\n");
        return 1;
    }
    memset(&usart2, 0, sizeof(usart2));
    DBG_LOG("[wifi] wifi连接成功\r\n");

    /* 只有真正"新连上"才同步时间。
       注意：不能在这里调 esp8266_sync_net_time() ——
       sync_net_time 之前开头又调 connect_wifi，形成无限递归瞬间炸栈。
       改成由 wifi_task / esp8266_join_ap 成功路径主动调用。 */
    s_net_time_synced = 0; /* 强制下一次 sync_net_time 真跑 HTTP */
    return 0;
}

wifi_scan_list g_wifi_scan;
volatile u8 g_wifi_join_state = WIFI_JOIN_IDLE;

static volatile u8 s_scan_req = 0;
static volatile u8 s_join_req = 0;
static char s_join_ssid[WIFI_SSID_LEN + 1];
static char s_join_pwd[WIFI_PWD_LEN + 1];

void esp8266_wifi_request_scan(void)
{
    s_scan_req = 1;
}

u8 esp8266_wifi_request_join(const char *ssid, const char *pwd)
{
    if (ssid == NULL || ssid[0] == '\0')
        return 1;
    if (g_wifi_join_state == WIFI_JOIN_BUSY)
        return 2;

    strncpy(s_join_ssid, ssid, sizeof(s_join_ssid) - 1);
    s_join_ssid[sizeof(s_join_ssid) - 1] = '\0';
    strncpy(s_join_pwd, (pwd != NULL) ? pwd : "", sizeof(s_join_pwd) - 1);
    s_join_pwd[sizeof(s_join_pwd) - 1] = '\0';

    g_wifi_join_state = WIFI_JOIN_BUSY;
    s_join_req = 1;
    return 0;
}

/* 等 AT 模块就绪（无应答就复位一次） */
static u8 wifi_at_init(void)
{
    if (esp8266_send_cmd((u8 *)"AT\r\n", (u8 *)"OK", 1000) != 0)
    {
        usart2_send_string((u8 *)"+++");
        delay_ms(100);
        esp8266_send_cmd((u8 *)"AT+RST\r\n", NULL, 1000);
        if (esp8266_send_cmd((u8 *)"AT\r\n", (u8 *)"OK", 1000) != 0)
            return 1;
    }
    esp8266_send_cmd((u8 *)"ATE0\r\n", NULL, 300);
    return 0;
}

/* 从 AT+CWLAP 应答里提取 SSID（每行形如 +CWLAP:(ecn,"ssid",rssi,"mac",ch)）
   只认 "+CWLAP:" 开头的行、只取第一个引号里的内容，
   这样即使状态位/MAC/其它串口数据混在缓冲里也不会解析出乱码。
   可多次调用累加；count 由 esp8266_scan_ap() 清空 */
static void wifi_parse_ssids(char *buf)
{
    char *p = buf;

    while ((p = strstr(p, "+CWLAP:")) != NULL)
    {
        char *q;
        char *end;
        int len;
        u8 i;
        u8 dup = 0;

        p += 7;
        q = strchr(p, '"');
        if (q == NULL)
            break;
        end = strchr(q + 1, '"');
        if (end == NULL)
            break; /* 半行（还没收全），剩下的下次再解析 */

        len = (int)(end - q) - 1;
        p = end + 1;

        if (len <= 0 || len > WIFI_SSID_LEN || g_wifi_scan.count >= WIFI_AP_MAX)
            continue;

        for (i = 0; i < g_wifi_scan.count; i++)
        {
            if (strncmp(g_wifi_scan.ssid[i], q + 1, len) == 0 && g_wifi_scan.ssid[i][len] == '\0')
            {
                dup = 1;
                break;
            }
        }
        if (dup)
            continue;

        memcpy(g_wifi_scan.ssid[g_wifi_scan.count], q + 1, len);
        g_wifi_scan.ssid[g_wifi_scan.count][len] = '\0';
        g_wifi_scan.count++;
    }
}

/* 解析串口缓冲里的完整行。
   final_chunk=0：扫描还没结束，把最后那半行搬到缓冲头部继续收，
                 下次不会从半行中间开始解析（否则会拼出乱码 SSID）
   final_chunk=1：扫描结束，全部解析完并清空缓冲 */
static void wifi_scan_flush(u8 final_chunk)
{
    char *buf = (char *)usart2.recv_buff;
    char *nl = strrchr(buf, '\n');
    u16 tail;
    char save;

    if (nl != NULL)
    {
        u16 parsed_len = (u16)(nl + 1 - buf);

        tail = (u16)(usart2.recv_len - parsed_len);
        save = nl[1];
        nl[1] = '\0';
        wifi_parse_ssids(buf);
        nl[1] = save;

        if (!final_chunk && tail > 0)
        {
            memmove(buf, nl + 1, tail);
            usart2.recv_len = tail;
            buf[tail] = '\0';
            usart2.recv_flag = 0;
            return;
        }
    }
    else if (!final_chunk)
    {
        return; /* 一条完整行都还没有，继续等 */
    }
    else
    {
        wifi_parse_ssids(buf); /* 最后一包没换行也解析掉 */
    }

    memset(&usart2, 0, sizeof(usart2));
}

/* 等扫描结束：边等边解析收满的缓冲，避免 2048B 接收上限丢数据 */
static void wifi_scan_wait(void)
{
    u16 cnt = 800; /* 最多 8 秒 */
    u8 done = 0;

    while (cnt-- && !done)
    {
        if (usart2.recv_flag)
        {
            if (strstr((char *)usart2.recv_buff, "OK\r\n") != NULL)
                done = 1;
            else if (usart2.recv_len > 1800)
                wifi_scan_flush(0);
        }
        voice_handle();
        delay_ms(10);
    }
    wifi_scan_flush(1);
}

/* 扫描附近热点（阻塞几秒，只能在 WIFI 任务里调用） */
static u8 esp8266_scan_ap(void)
{
    g_wifi_scan.count = 0;
    g_wifi_scan.state = 1; /* 扫描中 */

    if (wifi_at_init() != 0)
    {
        DBG_LOG("[wifi] 扫描失败：模块无应答\r\n");
        g_wifi_scan.state = 3;
        return 1;
    }

    esp8266_send_cmd((u8 *)"AT+CWMODE=1\r\n", (u8 *)"OK", 1000);
    /* 只输出 SSID，减少串口数据量（接收缓冲只有 2048B） */
    esp8266_send_cmd((u8 *)"AT+CWLAPOPT=1,2\r\n", (u8 *)"OK", 500);

    memset(&usart2, 0, sizeof(usart2));
    usart2_send_string((u8 *)"AT+CWLAP\r\n");

    wifi_scan_wait();

    g_wifi_scan.state = 2; /* 扫描完成（count 为 0 表示附近没有热点） */
    DBG_LOG("[wifi] 扫描完成，共 %u 个热点\r\n", (unsigned)g_wifi_scan.count);
    return 0;
}

/* 等 CWJAP 结果：成功回 OK，密码错/超时回 FAIL/ERROR，收到就立刻返回 */
static u8 wifi_wait_join(u16 timeout_ms)
{
    u16 cnt = timeout_ms / 10;

    while (cnt--)
    {
        if (usart2.recv_flag)
        {
            char *buf = (char *)usart2.recv_buff;
            if (strstr(buf, "OK\r\n") != NULL)
                return 0;
            if (strstr(buf, "FAIL") != NULL || strstr(buf, "ERROR") != NULL)
                return 1;
        }
        voice_handle();
        delay_ms(10);
    }
    return 1;
}

/* 连接指定热点（阻塞，成功后更新默认热点供 MQTT 重连使用） */
static u8 esp8266_join_ap(const char *ssid, const char *pwd)
{
    static char cmd[128]; /* static 降栈 */

    if (wifi_at_init() != 0)
    {
        DBG_LOG("[wifi] 连接失败：模块无应答\r\n");
        return 1;
    }
    esp8266_send_cmd((u8 *)"AT+CWMODE=1\r\n", (u8 *)"OK", 1000);

    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, (pwd != NULL) ? pwd : "");
    DBG_LOG("[wifi] 正在连接热点 %s ...\r\n", ssid);

    memset(&usart2, 0, sizeof(usart2));
    usart2_send_string((u8 *)cmd);
    if (wifi_wait_join(12000) != 0)
    {
        memset(&usart2, 0, sizeof(usart2));
        DBG_LOG("[wifi] 热点连接失败（密码错误或信号弱）\r\n");
        return 1;
    }
    memset(&usart2, 0, sizeof(usart2));

    strncpy(s_sta_ssid, ssid, sizeof(s_sta_ssid) - 1);
    s_sta_ssid[sizeof(s_sta_ssid) - 1] = '\0';
    strncpy(s_sta_pwd, (pwd != NULL) ? pwd : "", sizeof(s_sta_pwd) - 1);
    s_sta_pwd[sizeof(s_sta_pwd) - 1] = '\0';

    DBG_LOG("[wifi] 热点连接成功\r\n");
    s_net_time_synced = 0; /* 换热点了，强制重同步时间 */
    esp8266_sync_net_time();
    return 0;
}

/* WIFI 任务里周期调用：处理界面下发的扫描/连接请求 */
u8 esp8266_wifi_service(void)
{
    if (s_scan_req)
    {
        s_scan_req = 0;
        esp8266_scan_ap();
    }

    if (s_join_req)
    {
        s_join_req = 0;
        if (esp8266_join_ap(s_join_ssid, s_join_pwd) == 0)
        {
            g_wifi_join_state = WIFI_JOIN_OK;
            return 1; /* 让调用方重连 MQTT */
        }
        g_wifi_join_state = WIFI_JOIN_FAIL;
    }
    return 0;
}

/* ========================= 网络时间同步 =========================
   用 ESP8266 TCP 直连 api.k780.com:80 做 HTTP GET，响应里有：
   {"result":{"datetime_1":"2026-11-17 15:52:43","week":"星期二",...}}
   直接 strstr 找 datetime_1 字段，不用 cJSON。
   拿到后 time_set_from_str() 更新 g_datetime，再 time_set() 写硬件 RTC。

   重要：本函数开头绝不能调 esp8266_connect_wifi() ——
   否则 connect_wifi 里又会调本函数，无限递归瞬间炸栈。
   这里用"只查不连"的 AT+CIFSR 验证 WiFi 即可。 */
#define K780_HOST     "api.k780.com"
#define NET_TIME_RETRIES  3  /* 单次调用内最多重连 3 次（每次独立 TCP 连接） */

u8 esp8266_sync_net_time(void)
{
    static char cmd[96];           /* static 降栈 */
    static char datetime_str[32];  /* static 降栈 */
    const char *http_get = "GET /?app=life.time&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json HTTP/1.0\r\nHost: " K780_HOST "\r\n\r\n";
    u16 get_len = (u16)strlen(http_get);
    u8 attempt;

    if (s_net_time_synced)
        return 0; /* 已同步过就不再试（wifi_task 后台重试也不会进来） */

    /* 只查 WiFi 是否有 IP，不重连（递归安全） */
    memset(&usart2, 0, sizeof(usart2));
    usart2_send_string((u8 *)"AT+CIFSR\r\n");
    {
        u16 cnt = 200;
        u8 has_ip = 0;
        while (cnt--)
        {
            if (usart2.recv_flag)
            {
                if (strstr((char *)usart2.recv_buff, "0.0.0.0") == NULL &&
                    strlen((char *)usart2.recv_buff) > 6)
                    has_ip = 1;
                break;
            }
            delay_ms(10);
        }
        memset(&usart2, 0, sizeof(usart2));
        if (!has_ip)
        {
            DBG_LOG("[wifi] 未联网，跳过时间同步\r\n");
            return 1;
        }
    }

    DBG_LOG("[wifi] 正在同步网络时间 ...\r\n");
    esp8266_send_cmd((u8 *)"AT+CIPMUX=0\r\n", (u8 *)"OK", 1000);

    for (attempt = 0; attempt < NET_TIME_RETRIES; attempt++)
    {
        char *p, *end;
        u8 tcp_ok = 0;

        /* 关一下上次可能没关的 TCP，给模块一点时间回收 */
        esp8266_send_cmd((u8 *)"AT+CIPCLOSE\r\n", NULL, 500);
        delay_ms(150);

        /* ============ 1) 异步建立 TCP：发 CIPSTART，等 CONNECT ============
              ESP8266 AT 手册：AT+CIPSTART 先回 OK（命令接收），
              DNS 解析 + 三次握手完才回 CONNECT，期间可能 1~3 秒。
              一定要等 CONNECT，不能看 OK！ */
        memset(cmd, 0, sizeof(cmd));
        sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",80\r\n", K780_HOST);
        memset(&usart2, 0, sizeof(usart2));
        usart2_send_string((u8 *)cmd);
        {
            u16 cnt = 500; /* 最多 5 秒等 CONNECT */
            while (cnt--)
            {
                if (usart2.recv_flag)
                {
                    if (strstr((char *)usart2.recv_buff, "CONNECT") != NULL)
                    {
                        tcp_ok = 1;
                    }
                    /* 如果模块直接回了 ERROR + CLOSED（DNS 失败/目标不可达）
                       不再死等，立刻进下一次重试 */
                    else if (strstr((char *)usart2.recv_buff, "CLOSED") != NULL ||
                             strstr((char *)usart2.recv_buff, "ERROR") != NULL)
                    {
                        break;
                    }
                    memset(&usart2, 0, sizeof(usart2));
                }
                delay_ms(10);
            }
        }
        if (!tcp_ok)
        {
            DBG_LOG("[wifi] CIPSTART 未收到 CONNECT (attempt %u)\r\n", (unsigned)(attempt + 1));
            continue;
        }
        DBG_LOG("[wifi] TCP 已连上 %s\r\n", K780_HOST);

        /* ============ 2) 发 CIPSEND=len，等 '>' 提示符 ============
              AT 手册要求：先发 CIPSEND=len → 模块回 > → 再发 len 字节。
              不这么做 HTTP 请求被整个丢掉 */
        memset(cmd, 0, sizeof(cmd));
        sprintf(cmd, "AT+CIPSEND=%u\r\n", (unsigned)get_len);
        memset(&usart2, 0, sizeof(usart2));
        usart2_send_string((u8 *)cmd);
        {
            u16 cnt = 200;
            u8 got_prompt = 0;
            while (cnt--)
            {
                if (usart2.recv_flag)
                {
                    if (strchr((char *)usart2.recv_buff, '>') != NULL)
                    {
                        got_prompt = 1;
                    }
                    else if (strstr((char *)usart2.recv_buff, "ERROR") != NULL ||
                             strstr((char *)usart2.recv_buff, "CLOSED") != NULL)
                    {
                        break;
                    }
                    memset(&usart2, 0, sizeof(usart2));
                }
                delay_ms(10);
            }
            if (!got_prompt)
            {
                DBG_LOG("[wifi] CIPSEND 未收到 > 提示符 (attempt %u)\r\n", (unsigned)(attempt + 1));
                continue;
            }
        }

        /* ============ 3) 发 HTTP GET ============ */
        usart2_send_string((u8 *)http_get);

        /* ============ 4) 等响应（最多 6 秒） ============
              ESP8266 HTTP 响应可能拆成多包，等 +CIPCLOSE（服务端关了）或 +IPD / 字段 */
        {
            u16 cnt = 600;
            u8 got_data = 0;
            while (cnt--)
            {
                if (usart2.recv_flag)
                {
                    if (usart2.recv_len > 0)
                        got_data = 1; /* 只要收到过字节就算有数据 */
                    if (strstr((char *)usart2.recv_buff, "+CIPCLOSE") != NULL ||
                        strstr((char *)usart2.recv_buff, "+IPD") != NULL ||
                        strstr((char *)usart2.recv_buff, "datetime_1") != NULL)
                    {
                        break;
                    }
                    memset(&usart2, 0, sizeof(usart2));
                }
                delay_ms(10);
            }
            if (!got_data)
                DBG_LOG("[wifi] HTTP 响应超时 (attempt %u)\r\n", (unsigned)(attempt + 1));
        }

        /* ============ 5) 关 TCP ============ */
        esp8266_send_cmd((u8 *)"AT+CIPCLOSE\r\n", NULL, 500);

        /* ============ 6) 解析 ============ */
        if ((p = strstr((char *)usart2.recv_buff, "\"datetime_1\":\"")) == NULL)
        {
            DBG_LOG("[wifi] 网络时间解析失败 (attempt %u), recv=\"%.160s\"\r\n",
                   (unsigned)(attempt + 1), (char *)usart2.recv_buff);
            memset(&usart2, 0, sizeof(usart2));
            continue;
        }
        p += 14;
        end = strchr(p, '"');
        if (end == NULL || (int)(end - p) >= (int)sizeof(datetime_str))
        {
            DBG_LOG("[wifi] datetime 字段截断 (attempt %u)\r\n", (unsigned)(attempt + 1));
            memset(&usart2, 0, sizeof(usart2));
            continue;
        }
        memcpy(datetime_str, p, end - p);
        datetime_str[end - p] = '\0';
        memset(&usart2, 0, sizeof(usart2));

        DBG_LOG("[wifi] 网络时间: %s (第 %u 次)\r\n", datetime_str, (unsigned)(attempt + 1));

        time_set_from_str(datetime_str, "1");
        {
            int year, month, day, hour, min, sec;
            if (sscanf(datetime_str, "%d-%d-%d %d:%d:%d",
                       &year, &month, &day, &hour, &min, &sec) == 6)
                time_set(1, (u16)year, (u8)month, (u8)day, (u8)hour, (u8)min, (u8)sec);
        }
        s_net_time_synced = 1;
        return 0;
    }

    DBG_LOG("[wifi] 网络时间同步全部失败（%u 次尝试），稍后再试\r\n", (unsigned)NET_TIME_RETRIES);
    return 1;
}

u8 esp8266_connect_mqtt(void)
{
    u8 ret;
    static u8 cmd[128]; /* static 降栈 */
    sprintf((char *)cmd, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n", MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PWD);
    ret = esp8266_connect_wifi();
    if (ret == 0)
    {
        ret = esp8266_send_cmd("AT+MQTTCLEAN=0\r\n", NULL, 1000); // 断开mqtt连接
        ret = esp8266_send_cmd(cmd, "OK", 1000);
        if (ret != 0)
        {
            DBG_LOG("设置mqtt失败\r\n");
            return 1;
        }
        else
        {
            DBG_LOG("设置mqtt成功\r\n");
        }
        memset((char *)cmd, 0, sizeof(cmd));
        sprintf((char *)cmd, "AT+MQTTCONN=0,\"%s\",%d,1\r\n", MQTT_SERVER, MQTT_PORT);
        ret = esp8266_send_cmd(cmd, "OK", 1000);
        if (ret != 0)
        {
            DBG_LOG("连接服务器失败\r\n");
            return 1;
        }
        else
        {
            DBG_LOG("连接服务器成功\r\n");
        }
        delay_ms(500);
        ret = esp8266_send_cmd("AT+MQTTSUB=0,\"attributes/push\",0\r\n", "OK", 3000);
        if (ret != 0)
        {
            DBG_LOG("订阅失败\r\n");
            return 1;
        }
        else
        {
            DBG_LOG("订阅成功\r\n");
        }

        /* 顺便订阅属性上报响应：平台拒收时会返回 errcode/原因，便于排查。
           该订阅失败不影响主流程，所以不检查返回值 */
        esp8266_send_cmd("AT+MQTTSUB=0,\"attributes/response\",0\r\n", "OK", 1000);
    }
    return 0;
}

/*
+MQTTSUBRECV:0,"attributes/push",13,{"deskLed":1}

+MQTTSUBRECV:0,"attributes/push",15,{"livingLed":1}
*/
int esp8266_mqtt_extract_json(const char *buf, char *out, int out_size)
{
    char *p = strstr(buf, "+MQTTSUBRECV");
    if (p == NULL)
    {
        return 0;
    }
    char *str = strchr(buf, '{');
    if (str == NULL)
    {
        return 0;
    }
    char *end = strrchr(str, '}');
    if (end == NULL)
    {
        return 0;
    }
    int len = end - str + 1;
    if (len >= out_size - 1)
    {
        len = out_size - 1;
    }
    memcpy(out, str, len);
    out[len] = '\0';
    return len;
}
u8 esp8266_mqtt_parse_json(const char *json)
{
    cJSON *root = cJSON_Parse(json);
    if (root == NULL)
    {
        DBG_LOG("这不是json格式数据\r\n");
        cJSON_Delete(root);
        return 1;
    }
    cJSON *relay = cJSON_GetObjectItem(root, "relay");
    cJSON *livingLed = cJSON_GetObjectItem(root, "livingLed");
    cJSON *bedroomLed = cJSON_GetObjectItem(root, "bedroomLed");
    cJSON *deskLed = cJSON_GetObjectItem(root, "deskLed");
    cJSON *fan = cJSON_GetObjectItem(root, "fan");
    cJSON *buzzer = cJSON_GetObjectItem(root, "buzzer_state");
    if (relay != NULL)
    {
        if (relay->valueint == 1)
        {
            RELAY_ON();
        }
        if (relay->valueint == 0)
        {
            RELAY_OFF();
        }
    }
    if (livingLed != NULL)
    {
        if (livingLed->valueint == 1)
        {
            LED1_ON();
        }
        if (livingLed->valueint == 0)
        {
            LED1_OFF();
        }
    }
    if (bedroomLed != NULL)
    {
        if (bedroomLed->valueint == 1)
        {
            LED2_ON();
        }
        if (bedroomLed->valueint == 0)
        {
            LED2_OFF();
        }
    }
    if (deskLed != NULL)
    {
        if (deskLed->valueint == 1)
        {
            LED3_ON();
        }
        if (deskLed->valueint == 0)
        {
            LED3_OFF();
        }
    }
    if (fan != NULL)
    {
        /* 风扇已改为 PC8 的 PWM 调速，写 GPIO 无效，必须改 PWM 档位 */
        if (fan->valueint > 0)
        {
            fan_set_gear(1);
        }
        else
        {
            fan_set_gear(0);
        }
    }
    if (buzzer != NULL)
    {
        if (buzzer->valueint == 1)
        {
            BEEP_ON();
        }
        if (buzzer->valueint == 0)
        {
            BEEP_OFF();
        }
    }
    cJSON_Delete(root);
    return 0;
}
void esp8266_mqtt_handle(void)
{
    static char buf[192]; /* static 降栈；平台错误响应约 110 字节，留足余量 */
    if (usart2.recv_flag)
    {
        buf[0] = '\0';
        if (esp8266_mqtt_extract_json((char *)usart2.recv_buff, buf, sizeof(buf)) != 0)
        {
            DBG_LOG("[mqtt] 收到: %s\r\n", buf);
            esp8266_mqtt_parse_json(buf);
        }
        memset(&usart2, 0, sizeof(usart2));
    }
}

/* 用 MQTTPUBRAW 发布：先发长度，模块回 '>'，再发原始 JSON。
   相比 AT+MQTTPUB 把整段 JSON 塞进命令行，这种方式不受命令行长度和
   引号转义限制，发布较长属性报文更可靠。返回 0 成功，1 失败。 */
static u8 mqtt_pub_raw(const char *json)
{
    static u8 cmd[64]; /* static 降栈 */
    u16 len = (u16)strlen(json);
    u16 cnt;
    u8 got_prompt = 0;

    sprintf((char *)cmd, "AT+MQTTPUBRAW=0,\"attributes\",%u,0,0\r\n", (unsigned)len);
    memset(&usart2, 0, sizeof(usart2));
    usart2_send_string(cmd);

    cnt = 300; /* 最多等 3 秒拿 '>' 提示符 */
    while (cnt--)
    {
        if (usart2.recv_flag)
        {
            if (strchr((char *)usart2.recv_buff, '>') != NULL)
            {
                got_prompt = 1;
                break;
            }
            if (strstr((char *)usart2.recv_buff, "ERROR") != NULL)
                break;
            memset(&usart2, 0, sizeof(usart2));
        }
        voice_handle();
        delay_ms(10);
    }
    if (!got_prompt)
    {
        DBG_LOG("[mqtt] 发布无提示符, 模块应答=%.80s\r\n", (char *)usart2.recv_buff);
        memset(&usart2, 0, sizeof(usart2));
        return 1;
    }

    memset(&usart2, 0, sizeof(usart2));
    usart2_send_string((u8 *)json); /* 原始 JSON，无需转义引号 */

    cnt = 300; /* 最多等 3 秒发布结果 */
    while (cnt--)
    {
        if (usart2.recv_flag)
        {
            if (strstr((char *)usart2.recv_buff, "OK") != NULL)
            {
                memset(&usart2, 0, sizeof(usart2));
                return 0;
            }
            if (strstr((char *)usart2.recv_buff, "ERROR") != NULL ||
                strstr((char *)usart2.recv_buff, "FAIL") != NULL)
            {
                DBG_LOG("[mqtt] 发布失败, 模块应答=%.80s\r\n", (char *)usart2.recv_buff);
                memset(&usart2, 0, sizeof(usart2));
                return 1;
            }
            memset(&usart2, 0, sizeof(usart2));
        }
        voice_handle();
        delay_ms(10);
    }
    DBG_LOG("[mqtt] 发布无应答\r\n");
    return 1;
}

/* 上报全部数据：环境参数 + 姿态角 + 设备开关状态
   （由 WiFi 任务每 30 秒调用，传感器数据来自测量采集任务）
   设备状态：继电器高电平有效、三路灯低电平点亮、风扇读 PWM 档位
   返回 0 发布成功 / 1 发布失败（调用方据此判定 MQTT 掉线并触发重连） */
u8 esp826_mqtt_pub(float temperature, float humidity,
                     u16 light_mv, u8 light_pct,
                     float pitch, float roll, float yaw)
{
    /* json 里不能再出现转义反斜杠（MQTTPUBRAW 发的是原始数据） */
    static char json[320]; /* static 降栈 */
    u8 relay = (RELAY != 0);
    u8 livingLed = (LED1 == 0);
    u8 bedroomLed = (LED2 == 0);
    u8 deskLed = (LED3 == 0);
    u8 fan = (fan_get_gear() > 0); /* PC8 已改为 PWM，不能读 GPIO */
    u8 buzzer = (BEEP != 0);

    /* 平台限制消息频率（连续多发会返回 403），所以所有属性合并为一条上报 */
    sprintf(json,
            "{\"temperature\":%.2f,\"humidity\":%.2f,"
            "\"light\":%u,\"light_mv\":%u,"
            "\"pitch\":%.2f,\"roll\":%.2f,\"yaw\":%.2f,"
            "\"livingLed\":%u,\"bedroomLed\":%u,\"deskLed\":%u,"
            "\"relay\":%u,\"fan\":%u,\"buzzer_state\":%u}",
            (double)temperature, (double)humidity,
            (unsigned)light_pct, (unsigned)light_mv,
            (double)pitch, (double)roll, (double)yaw,
            (unsigned)livingLed, (unsigned)bedroomLed, (unsigned)deskLed,
            (unsigned)relay, (unsigned)fan, (unsigned)buzzer);
    {
        u8 ret = mqtt_pub_raw(json);
        DBG_LOG("[mqtt] 属性上报%s\r\n", ret ? "失败" : "成功");
        return ret;
    }
}
