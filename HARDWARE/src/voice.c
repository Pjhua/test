/**
 * @file    voice.c
 * @brief   HLK-V20 语音识别 → 控制硬件 + 播放 SD 卡 0:/prompt 回复音
 *
 * WiFi 任务里循环调用 voice_handle()：
 *   按 "4F 00 <ID> F4" 解析命令 ID（见《固件命令词与播报词格式.xlsx》），
 *   直接调用对应驱动函数控制硬件，并把播报语投递给 MUSIC 任务播放：
 *     0:/prompt/好的已为您打开风扇.wav
 *   文件名就是 SD 卡里的实际名字，文件不存在则只执行动作、不出声。
 *
 * 唤醒：
 *   唤醒词（小信小信/小盈同学）由模块自己播报，串口无输出；
 *   免唤醒命令词（小盈小盈/你好小盈）输出 4F 00 AA F4 → 轮流播 唤醒回复语；
 *   主动退出（再见/退下）输出 4F 00 AB F4 → 播 有需要再叫我.wav。
 *
 * 注意：本工程 FatFs 配置 FF_LFN_UNICODE=2（TCHAR 即 UTF-8），
 *       源码里的中文文件名可直接用于 f_open，不需要转码。
 */

#include "voice.h"
#include "dbg_log.h"
#include "usart3.h"
#include "music_player.h"

#include "FreeRTOS.h"
#include "task.h"

#include "led.h"
#include "beep.h"
#include "relay.h"
#include "fan.h"
#include "pwm.h"
#include "w2812.h"

#include <stdio.h>
#include <string.h>

#define VOICE_PROMPT_DIR "0:/prompt"

/* 唤醒回复语（0:/prompt 下的文件名，不含路径和 .wav），每次唤醒轮流播一个 */
static const char *s_wake_reply[] =
{
    "嗯我来了",
    "嗨我在呢",
    "您终于想起我了",
    "有什么可以帮助您",
};
static u8 s_wake_idx = 0;

/* ======================= RGB 氛围灯（PB15 WS2812，软件时序） ======================= */
/* 命令表里空闲的 ID 复用：
   0x23 打开浴室的灯→RGB开  0x24 关闭浴室的灯→RGB灭
   0x33 温暖模式→暖黄光     0x34 音乐模式→七彩流动    0x35 睡眠模式→微弱蓝光 */
volatile unsigned int  g_rgb_color = 0; /* 0xRRGGBB，0=全灭 */
volatile unsigned char g_rgb_effect = 0;
static u32 s_rgb_last = RGB_ORANGE;     /* 最近一次点亮的颜色，"打开"时恢复 */
static u16 s_rgb_hue = 0;               /* 七彩流动相位 */

static void rgb_show(u32 color)
{
    u8 i;

    __disable_irq(); /* 防止时序被打断（与界面发送方式一致） */
    w2812_reset();
    for (i = 0; i < Pixel_NUM; i++)
        w2812_send_color(color);
    __enable_irq();

    g_rgb_color = color;
    if (color != RGB_BLACK)
        s_rgb_last = color;
}

/* hue 0~255 → 七彩（红→绿→蓝循环） */
static u32 rgb_wheel(u8 pos)
{
    u8 r, g, b;

    if (pos < 85)
    {
        r = pos * 3;
        g = 255 - pos * 3;
        b = 0;
    }
    else if (pos < 170)
    {
        r = 255 - (pos - 85) * 3;
        g = 0;
        b = (pos - 85) * 3;
    }
    else
    {
        r = 0;
        g = (pos - 170) * 3;
        b = 255 - (pos - 170) * 3;
    }
    return ((u32)r << 16) | ((u32)g << 8) | b;
}

/* 七彩流动：voice_handle 每 10ms 调一次，每 150ms 走一格 */
static void rgb_effect_service(void)
{
    static TickType_t last = 0;

    if (g_rgb_effect == 0)
        return;
    if ((int32_t)(xTaskGetTickCount() - last) < (int32_t)pdMS_TO_TICKS(150))
        return;

    last = xTaskGetTickCount();
    s_rgb_hue += 8;
    rgb_show(rgb_wheel((u8)s_rgb_hue));
}

/* ======================= 音乐暂停 / 播报 / 恢复 ======================= */
/* 说话下命令时：先暂停音乐（模块更容易听清，播报也不会被盖住），
   回复语一播完就立刻回到原位置继续放歌，不做多余等待。
   只有唤醒/退出命令例外：音乐不能马上响，否则用户说下一条指令时
   模块就听不见了，所以留几秒安静时间 */
#define VOICE_HOLD_CMD_MS   2000  /* 普通命令兜底时长（正常情况下回复语播完就恢复） */
#define VOICE_HOLD_WAKE_MS  8000  /* 唤醒/退出后，留给用户说下一条指令的时间 */
#define VOICE_PROMPT_MAX_MS 15000 /* 回复音最长播放时间，超时强制停止（防卡死） */
#define VOICE_SESSION_IDLE_MS 30000 /* 最后一条指令后多久强制退出本次语音会话 */

static u8   s_hold_active = 0;        /* 音乐是被语音暂停的，需要恢复 */
static u8   s_hold_wake = 0;          /* 本次暂停由唤醒命令引起：回复语播完也要保持安静 */
static u8   s_hold_prompt_played = 0; /* 回复语确实播出来了（歌曲已被释放） */
static int  s_hold_idx = -1;          /* 被打断的歌曲序号 */
static u32  s_hold_pos = 0;           /* 被打断时的字节位置 */
static TickType_t s_hold_tick = 0;    /* 恢复时间点 */
static TickType_t s_prompt_tick = 0;  /* 回复音开始播放的时间点 */

static u8         s_session_active = 0; /* 本次语音会话是否还在进行 */
static TickType_t s_session_last = 0;   /* 最近一次收到指令的时刻 */

static int voice_say(const char *name); /* 把播报语投递给 MUSIC 任务 */

/* ==================== 语音 → 音乐任务 请求队列 ====================
   WIFI 任务只解析命令、控制硬件，把“放播报 / 暂停音乐 / 音乐命令”
   投递到这里，真正的音乐操作由 MUSIC 任务在 voice_music_service() 里执行：
   播放提示音要跑 FatFs + wav 解析，放在 MUSIC 任务的栈里更安全，也不占 WIFI 栈。 */
#define VOICE_MUS_Q_LEN       4
#define VOICE_PROMPT_NAME_MAX 40

#define VOICE_MUS_REQ_PROMPT 1 /* 播放播报语 */
#define VOICE_MUS_REQ_HOLD   2 /* 正在放歌：先暂停并记住恢复点 */
#define VOICE_MUS_REQ_CMD    3 /* 音乐命令 0x55~0x59 */

typedef struct
{
    u8   type;
    u8   id;
    char name[VOICE_PROMPT_NAME_MAX]; /* 播报语文件名（不含路径和 .wav） */
} voice_mus_req_t;

static voice_mus_req_t s_mus_q[VOICE_MUS_Q_LEN];
static volatile u8 s_mus_q_head = 0; /* 生产：WIFI 任务 */
static volatile u8 s_mus_q_tail = 0; /* 消费：MUSIC 任务 */

static void voice_play_prompt(const char *name);
static void voice_music_cmd(u8 id);

/* 投递一条请求；队列满返回 -1（MUSIC 任务每 5ms 取一次，正常不会满） */
static int voice_mus_post(u8 type, u8 id, const char *name)
{
    u8 next = (u8)((s_mus_q_head + 1) % VOICE_MUS_Q_LEN);
    voice_mus_req_t *r;
    int ret = 0;

    taskENTER_CRITICAL();
    if (next == s_mus_q_tail)
    {
        ret = -1; /* 队列满：丢弃 */
    }
    else
    {
        r = &s_mus_q[s_mus_q_head];
        r->type = type;
        r->id = id;
        r->name[0] = '\0';
        if (name != NULL)
            strncpy(r->name, name, sizeof(r->name) - 1);
        s_mus_q_head = next;
    }
    taskEXIT_CRITICAL();

    return ret;
}

/* 收到一条命令时调用：正在放歌就先暂停，并记住恢复点 */
static void voice_hold_begin(u8 id)
{
    u32 hold_ms = VOICE_HOLD_CMD_MS;

    /* 音乐类命令自己会改变播放状态，不参与暂停/恢复编排 */
    if (id >= 0x55 && id <= 0x59)
    {
        s_hold_active = 0;
        return;
    }

    /* 音量类命令：音乐保持播放（不暂停），这样调完能立刻听出大小变化；
       回复音由 mus_play_prompt 打断歌曲、播完自动回原位置，不需要额外编排 */
    if (id >= 0x63 && id <= 0x66)
        return;

    /* 唤醒/退出：回复语播完也不能马上放歌，要给用户留出说下一条指令的时间 */
    s_hold_wake = (id == 0xAA || id == 0xAB) ? 1 : 0;
    if (s_hold_wake)
        hold_ms = VOICE_HOLD_WAKE_MS;

    if (!s_hold_active)
    {
        if (mus_state() != MUS_PLAYING || mus_is_prompt() || mus_current() < 0)
            return; /* 没在放歌：不需要恢复 */

        s_hold_idx = mus_current();
        s_hold_pos = mus_byte_pos();
        s_hold_prompt_played = 0;
        mus_pause();
        s_hold_active = 1;
        if (s_hold_wake)
            DBG_LOG("[voice] 音乐已暂停，等您说指令\r\n");
        else
            DBG_LOG("[voice] 音乐已暂停\r\n");
    }
    else if (s_hold_wake)
    {
        DBG_LOG("[voice] 音乐保持暂停，等您说指令\r\n");
    }

    /* 连续下命令时把恢复时间往后推，避免播报中途音乐插进来 */
    s_hold_tick = xTaskGetTickCount() + pdMS_TO_TICKS(hold_ms);
}

/* 真正恢复音乐：回复语播过就从原位置接着放，否则只是解除暂停 */
static void voice_hold_resume(void)
{
    s_hold_active = 0;
    s_hold_wake = 0;
    if (s_hold_prompt_played && s_hold_idx >= 0 && mus_state() == MUS_STOPPED)
        mus_play_at(s_hold_idx, s_hold_pos);
    else if (mus_state() == MUS_PAUSED)
        mus_resume();

    s_hold_prompt_played = 0;
    s_hold_idx = -1;
    s_hold_pos = 0;
    DBG_LOG("[voice] 音乐继续播放\r\n");
}

/* 回复语播完（普通命令）或安静时间到（唤醒命令）就恢复音乐 */
static void voice_hold_service(void)
{
    if (!s_hold_active)
        return;

    if (mus_is_prompt())
    {
        /* 防卡死：回复音异常（DMA 没跑起来）时不会永远等下去 */
        if ((int32_t)(xTaskGetTickCount() - s_prompt_tick) >
            (int32_t)pdMS_TO_TICKS(VOICE_PROMPT_MAX_MS))
        {
            DBG_LOG("[voice] 回复音播放超时，强制停止\r\n");
            mus_stop();
        }
        return;
    }

    /* 普通命令：回复语一播完马上续播，不用再干等到定时时间 */
    if (!s_hold_wake && s_hold_prompt_played)
    {
        voice_hold_resume();
        return;
    }

    if ((int32_t)(xTaskGetTickCount() - s_hold_tick) < 0)
        return;

    voice_hold_resume();
}

/* 强制退出：最后一条指令之后超过 VOICE_SESSION_IDLE_MS 没有新指令，
   就结束本次语音会话，并播报退出语（正在放歌会先打断，播完自动回到原位置继续）。
   注意：本板 USART3 只接模块的 TXD（MCU 发不出去），所以这里只能复位
   MCU 侧状态；要让模块本身退出/重启，得给模块 VCC 加 GPIO 控制的开关断电 */
static void voice_session_idle_service(void)
{
    if (!s_session_active)
        return;
    if ((int32_t)(xTaskGetTickCount() - s_session_last) <
        (int32_t)pdMS_TO_TICKS(VOICE_SESSION_IDLE_MS))
        return;

    s_session_active = 0;
    usart3.recv_flag = 0;
    usart3.recv_len = 0;
    memset(usart3.recv_buff, 0, sizeof(usart3.recv_buff));
    DBG_LOG("[voice] 最后一条指令已 %u 秒，强制退出语音会话\r\n",
           (unsigned)(VOICE_SESSION_IDLE_MS / 1000));

    /* 退出播报：交 MUSIC 任务播放，正在放歌会先打断、播完自动回到原位置 */
    voice_say("有需要再叫我");
}

/* ======================= 播报语播放 ======================= */

/* WIFI 任务：把播报语投递给 MUSIC 任务（name 为 0:/prompt 下的文件名，不含 .wav） */
static int voice_say(const char *name)
{
    if (name == NULL || name[0] == '\0')
        return -1;
    return voice_mus_post(VOICE_MUS_REQ_PROMPT, 0, name);
}

/* MUSIC 任务：真正打开 0:/prompt/<name>.wav 并播放。
   本工程 FatFs 配置 FF_LFN_UNICODE=2（TCHAR 即 UTF-8），中文文件名可直接 f_open */
static void voice_play_prompt(const char *name)
{
    char path[96];

    snprintf(path, sizeof(path), "%s/%s.wav", VOICE_PROMPT_DIR, name);
    if (mus_play_prompt(path) == 0)
    {
        s_prompt_tick = xTaskGetTickCount();
        if (s_hold_active)
            s_hold_prompt_played = 1;
        return;
    }
    DBG_LOG("[voice] 提示音不存在: %s\r\n", name);
}

/* MUSIC 任务：执行语音下发的音乐命令（0x55~0x59） */
static void voice_music_cmd(u8 id)
{
    switch (id)
    {
    case 0x55: /* 播放音乐 */
        if (mus_state() == MUS_PAUSED)
            mus_toggle();
        else if (mus_state() == MUS_STOPPED)
        {
            if (mus_scan() > 0)
                mus_play(0);
        }
        break;
    case 0x56: /* 暂停播放 */
        if (mus_state() == MUS_PLAYING)
            mus_toggle();
        break;
    case 0x57: /* 停止播放 */
        mus_stop();
        break;
    case 0x58: /* 上一曲 */
        mus_prev();
        break;
    case 0x59: /* 下一曲 */
        mus_next();
        break;
    default:
        break;
    }
}

/* MUSIC 任务主循环调用：执行 WIFI 任务投递的请求，再处理音乐恢复定时。
   语音播报、音乐命令、暂停/续播全部在本任务里完成（音乐任务不再空转） */
void voice_music_service(void)
{
    for (;;)
    {
        voice_mus_req_t req;
        u8 empty;

        taskENTER_CRITICAL();
        empty = (s_mus_q_tail == s_mus_q_head) ? 1 : 0;
        if (!empty)
        {
            req = s_mus_q[s_mus_q_tail];
            s_mus_q_tail = (u8)((s_mus_q_tail + 1) % VOICE_MUS_Q_LEN);
        }
        taskEXIT_CRITICAL();

        if (empty)
            break;

        switch (req.type)
        {
        case VOICE_MUS_REQ_PROMPT:
            voice_play_prompt(req.name);
            break;
        case VOICE_MUS_REQ_HOLD:
            voice_hold_begin(req.id);
            break;
        case VOICE_MUS_REQ_CMD:
            voice_music_cmd(req.id);
            break;
        default:
            break;
        }
    }

    voice_hold_service(); /* 暂停时间到 → 回到原位置继续放歌 */
}

/* ======================= 命令处理（直接调原有函数） ======================= */

static void voice_dispatch(u8 id)
{
    u8 g;

    switch (id)
    {
    /* ---- 唤醒 / 退出 ---- */
    case 0xAA: /* 免唤醒命令词：小盈小盈/你好小盈/小盈你好 */
        voice_say(s_wake_reply[s_wake_idx]);
        s_wake_idx++;
        if (s_wake_idx >= (u8)(sizeof(s_wake_reply) / sizeof(s_wake_reply[0])))
            s_wake_idx = 0;
        return;
    case 0xAB: /* 主动退出：再见/退下/拜拜 */
        voice_say("有需要再叫我");
        return;

    /* ---- 风扇 ---- */
    case 0x01: /* 关闭风扇 */
        FAN_OFF();
        fan_set_gear(0);
        voice_say("好的已为您关闭风扇");
        return;
    case 0x02: /* 打开风扇 */
        if (fan_get_gear() == 0)
            fan_set_gear(1);
        FAN_ON();
        voice_say("好的已为您打开风扇");
        return;
    case 0x05: /* 一档 */
        fan_set_gear(1);
        FAN_ON();
        voice_say("好的风扇已为您调到一档");
        return;
    case 0x06: /* 二档 */
        fan_set_gear(2);
        FAN_ON();
        voice_say("好的风扇已为您调到二档");
        return;
    case 0x07: /* 三档 */
        fan_set_gear(3);
        FAN_ON();
        voice_say("好的风扇已为您调到三档");
        return;
    case 0x08: /* 四档（本板最高 3 档） */
        fan_set_gear(3);
        FAN_ON();
        voice_say("好的风扇已为您调到四档");
        return;
    case 0x09: /* 增大风速 */
        g = fan_get_gear();
        if (g < 3)
        {
            g++;
            fan_set_gear(g);
            FAN_ON();
        }
        voice_say("风速已为您增加百分之十");
        return;
    case 0x10: /* 减小风速 */
        g = fan_get_gear();
        if (g > 0)
        {
            g--;
            fan_set_gear(g);
            if (g == 0)
                FAN_OFF();
        }
        voice_say("风速已为您减小百分之十");
        return;

    /* ---- LED（低电平点亮） ---- */
    case 0x13: /* 打开所有的灯（LED + RGB 氛围灯全亮） */
        LED_ON_ALL();
        g_rgb_effect = 0;
        rgb_show(RGB_WHITE);
        voice_say("好的已为您打开所有的灯");
        return;
    case 0x14: /* 关闭所有的灯（LED + RGB 全灭） */
        LED_OFF_ALL();
        g_rgb_effect = 0;
        rgb_show(RGB_BLACK);
        voice_say("好的已为您关闭所有的灯");
        return;
    case 0x17: /* 打开卧室的灯 → LED1 */
        LED1_ON();
        voice_say("好的已为您打开");
        return;
    case 0x18: /* 关闭卧室的灯 */
        LED1_OFF();
        voice_say("好的已为您关闭");
        return;
    case 0x19: /* 打开客厅的灯 → LED2 */
        LED2_ON();
        voice_say("好的已为您打开");
        return;
    case 0x20: /* 关闭客厅的灯 */
        LED2_OFF();
        voice_say("好的已为您关闭");
        return;
    case 0x21: /* 打开厨房的灯 → LED3 */
        LED3_ON();
        voice_say("好的已为您打开");
        return;
    case 0x22: /* 关闭厨房的灯 */
        LED3_OFF();
        voice_say("好的已为您关闭");
        return;

    /* ---- RGB 氛围灯（空闲 ID 复用） ---- */
    case 0x23: /* 打开浴室的灯 → RGB 氛围灯开 */
        g_rgb_effect = 0;
        rgb_show(s_rgb_last);
        voice_say("好的已为您打开");
        return;
    case 0x24: /* 关闭浴室的灯 → RGB 氛围灯灭 */
        g_rgb_effect = 0;
        rgb_show(RGB_BLACK);
        voice_say("好的已为您关闭");
        return;
    case 0x33: /* 温暖模式 → 暖黄光 */
        g_rgb_effect = 0;
        rgb_show(0xFFA040);
        voice_say("好的已为你设置"); /* SD 卡上的文件名是"为你"不是"为您" */
        return;
    case 0x34: /* 音乐模式 → 七彩流动 */
        g_rgb_effect = 1;
        voice_say("好的已为你设置");
        return;
    case 0x35: /* 睡眠模式 → 微弱蓝光 */
        g_rgb_effect = 0;
        rgb_show(0x000080);
        voice_say("好的已为你设置");
        return;

    /* ---- 继电器氖泡灯（台灯/吸顶灯共用一路，高电平吸合） ---- */
    case 0x25: /* 打开台灯 */
    case 0x31: /* 打开吸顶灯 */
        RELAY_ON();
        voice_say("好的已为您打开");
        return;
    case 0x26: /* 关闭台灯 */
    case 0x32: /* 关闭吸顶灯 */
        RELAY_OFF();
        voice_say("好的已为您关闭");
        return;

    /* ---- 舵机（窗户/门共用一路） ---- */
    case 0x50: /* 打开窗户 */
        sg90_set_angle(90);
        voice_say("好的已为您打开");
        return;
    case 0x51: /* 关闭窗户 */
        sg90_set_angle(0);
        voice_say("好的已为您关闭");
        return;
    case 0x52: /* 半开窗户 */
        sg90_set_angle(45);
        voice_say("好的已为您调整");
        return;
    case 0x53: /* 开门 */
        sg90_set_angle(90);
        voice_say("好的已为您打开");
        return;
    case 0x54: /* 关门 */
        sg90_set_angle(0);
        voice_say("好的已为您关闭");
        return;

    /* ---- 音乐（动作由 MUSIC 任务执行，这里只投递请求 + 播报） ---- */
    case 0x55: /* 播放音乐 */
        voice_mus_post(VOICE_MUS_REQ_CMD, id, NULL);
        voice_say("好的已为您播放");
        return;
    case 0x56: /* 暂停播放 */
        voice_mus_post(VOICE_MUS_REQ_CMD, id, NULL);
        voice_say("好的已为您暂停");
        return;
    case 0x57: /* 停止播放 */
        voice_mus_post(VOICE_MUS_REQ_CMD, id, NULL);
        voice_say("好的已为您关闭");
        return;
    case 0x58: /* 上一曲 */
        voice_mus_post(VOICE_MUS_REQ_CMD, id, NULL);
        voice_say("好的，已为您切换");
        return;
    case 0x59: /* 下一曲 */
        voice_mus_post(VOICE_MUS_REQ_CMD, id, NULL);
        voice_say("好的，已为您切换");
        return;

    /* ---- 蜂鸣器（警报） ---- */
    case 0x60: /* 打开警报 */
        BEEP_ON();
        voice_say("好的已为您打开");
        return;
    case 0x61: /* 关闭警报 */
        BEEP_OFF();
        voice_say("好的已为您关闭");
        return;

    /* ---- 音量 ---- */
    case 0x63: /* 减小音量 */
        g = mus_volume();
        mus_set_volume((g > 10) ? (g - 10) : 0);
        DBG_LOG("[voice] 音量 -> %d%%\r\n", mus_volume());
        voice_say("好的已为你设置");
        return;
    case 0x64: /* 增大音量 */
        g = mus_volume();
        mus_set_volume((g < 90) ? (g + 10) : 100);
        DBG_LOG("[voice] 音量 -> %d%%\r\n", mus_volume());
        voice_say("好的已为你设置");
        return;
    case 0x65: /* 最小音量 */
        mus_set_volume(0);
        DBG_LOG("[voice] 音量 -> %d%%\r\n", mus_volume());
        voice_say("好的已为你设置");
        return;
    case 0x66: /* 最大音量 */
        mus_set_volume(100);
        DBG_LOG("[voice] 音量 -> %d%%\r\n", mus_volume());
        voice_say("好的已为你设置");
        return;

    default: /* 表里没有的 ID（风扇定时/空调/浴室灯等本板无硬件）：不反应 */
        DBG_LOG("[voice] 命令 0x%02X 无对应动作，忽略\r\n", (unsigned)id);
        return;
    }
}

/* ======================= 帧解析 ======================= */

void voice_init(void)
{
    memset(&usart3, 0, sizeof(usart3));
    s_session_active = 0;
    s_session_last = 0;
    s_mus_q_head = 0;
    s_mus_q_tail = 0;
}

void voice_handle(void)
{
    u8 i, len;
    const u8 *b;
    int hit = 0;

    /* 会话超时 + RGB 七彩流动：即使没有新命令也要跑（音乐暂停/恢复在 MUSIC 任务里） */
    voice_session_idle_service();
    rgb_effect_service();

    if (!usart3.recv_flag)
        return;

    usart3.recv_flag = 0;
    len = usart3.recv_len;
    b = (const u8 *)usart3.recv_buff;

    /* 帧：4F 00 <ID> F4，一次空闲可能收到多帧 */
    for (i = 0; (u16)i + 3 < len; i++)
    {
        if (b[i] == 0x4F && b[i + 1] == 0x00 && b[i + 3] == 0xF4)
        {
            hit = 1;
            s_session_active = 1;
            s_session_last = xTaskGetTickCount();
            DBG_LOG("[voice] 收到命令 0x%02X\r\n", (unsigned)b[i + 2]);
            voice_mus_post(VOICE_MUS_REQ_HOLD, b[i + 2], NULL); /* 正在放歌 → 先暂停，播报后再续 */
            voice_dispatch(b[i + 2]);
            i += 3;
        }
    }

    if (!hit && len > 0)
    {
        /* 没解析出有效帧：打印原始数据，方便排查模块实际发的协议 */
        DBG_LOG("[voice] 未知数据(%u 字节):", (unsigned)len);
        for (i = 0; i < len && i < 16; i++)
            DBG_LOG(" %02X", (unsigned)b[i]);
        DBG_LOG("\r\n");
    }

    usart3.recv_len = 0;
    memset(usart3.recv_buff, 0, sizeof(usart3.recv_buff));
}
