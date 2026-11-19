
/**
 * @file    music_player.c
 * @brief   LVGL 音乐界面的非阻塞 WAV 播放器
 *
 *  裸机里的 Wav_PlaySong() 是"死循环播完一首"的阻塞实现（内部还画 LCD、扫按键），
 *  放到 LVGL 里会把界面卡死。这里复用同一套底层（WM8978 + I2S + DMA 双缓冲 +
 *  Lib 歌词解析），但改成：
 *    - mus_play()  只负责打开文件、填满两个 DMA 缓冲区、启动 DMA
 *    - mus_tick()  由 main 主循环不断调用，哪个缓冲区播完就补哪个
 *      （不放在 LVGL 定时器里，避免界面重绘把补数拖慢导致 DMA 断流）
 *    - 暂停时 DMA 照常跑，mus_tick() 把播完的缓冲区清零，最多两块后完全静音
 *  这样界面线程不会被阻塞，播放/暂停/切歌/歌词都能实时响应。
 */

#include "music_player.h"
#include "ff.h"
#include "malloc.h"
#include "wavplay.h"
#include "audioplay.h"
#include "wm8978.h"
#include "i2s.h"
#include "lyric.h"

#include <stdio.h>
#include <string.h>

/* 多任务支持：mus_play/toggle/next/stop 由 LVGL 事件(界面任务)调用，
   mus_tick 由音频任务调用，用互斥锁串行化所有共享状态(s_file/缓冲区/状态机) */
#include "FreeRTOS.h"
#include "semphr.h"

#define MUS_DIR        "0:/music"
#define MUS_MAX_SONGS  32
#define MUS_PATH_MAX   128
#define MUS_TITLE_MAX  64

/* 音频块大小：16384 字节 ≈ 93ms(44.1kHz/16bit/stereo)，两块共 186ms。
   原来一块只有 8KB(46ms)，界面重绘（下拉框展开、歌词刷新）稍微久一点
   DMA 就会断流并循环播放旧数据，听起来就是明显杂音。加大后给 UI 留足余量。 */
#define MUS_BUF_SIZE   16384

/* 音量：界面 0~100%，WM8978 寄存器 0~63（0.5dB/step），每 10ms 走 1 级做平滑 */
#define MUS_VOL_MAX      100
#define MUS_VOL_REG_MAX  63

/* 播报提示音时音量临时提升的点数(0~100)：提示音是 TTS 录音，电平普遍比歌曲低，
   播放期间自动加 40 点(约 +12dB)，播完自动回到用户设定的音量 */
#define MUS_PROMPT_BOOST 30

static char s_paths[MUS_MAX_SONGS][MUS_PATH_MAX];
static char s_titles[MUS_MAX_SONGS][MUS_TITLE_MAX];

static int  s_count = 0;
static int  s_cur = -1;
static u8   s_state = MUS_STOPPED;
static u8   s_audio_ready = 0;

static FIL *s_file = NULL;
static u8  *s_buf1 = NULL;
static u8  *s_buf2 = NULL;
static u32  s_buf_size = MUS_BUF_SIZE; /* 每块实际大小（内存不足时回退 8KB） */
static u8   s_eof_wait = 0;      /* 已读到文件尾后，还需要等几个 DMA 完成中断 */
static int  s_lyric_idx = -1;    /* 当前歌词行号 */

static volatile u8 s_dma_pending = 0; /* bit0=buf1 已播完, bit1=buf2 已播完 */

/* 语音回复音（提示音）：先打断歌曲，播完再回到原位置继续 */
static u8  s_is_prompt = 0;     /* 当前播放的是提示音 */
static int s_resume_idx = -1;   /* 提示音打断的歌曲序号，-1 表示无需恢复 */
static u32 s_resume_pos = 0;    /* 提示音打断时的数据偏移（字节） */
static u8  s_last_play_resumed = 0; /* 最近一次起播是否为"续播"（仅日志区分用） */

static int s_vol_target = 50;    /* 目标音量（界面 0~100） */
static int s_vol_applied = -1;   /* 已写入 WM8978 的寄存器值，-1 表示尚未应用 */

static SemaphoreHandle_t s_lock = NULL; /* 串行化 mus_* 的并发调用 */

static void mus_lock(void)
{
    if (s_lock != NULL)
        xSemaphoreTake(s_lock, portMAX_DELAY);
}

static void mus_unlock(void)
{
    if (s_lock != NULL)
        xSemaphoreGive(s_lock);
}

/* 无锁实现（调用者必须已持有 s_lock） */
static int  mus_scan_nolock(void);
static int  mus_play_nolock(int idx);
static int  mus_play_at_nolock(const char *path, int idx, u32 start_pos, u8 is_prompt);
static void mus_toggle_nolock(void);
static void mus_next_nolock(void);
static void mus_stop_nolock(void);
static void mus_tick_nolock(void);

/* 停止后如果还有一次残留的 DMA 中断，走这里，避免访问已经释放的缓冲区 */
static void mus_dma_idle(void)
{
}

/* DMA 传输完成回调：只记标志，补数据/静音都放到主循环做，
   中断里不做耗时操作，避免影响其它中断与界面响应。
   用位图记录，即使主循环被界面重绘阻塞、一次跨过两次完成，也不会漏补 */
static void mus_dma_callback(void)
{
    if (DMA1_Stream4->CR & 0x80000) /* CT=1 表示当前用 M1AR，刚播完的是 buf1 */
        s_dma_pending |= 0x01;
    else
        s_dma_pending |= 0x02;
}

/* ======================= 工具函数 ======================= */

static u8 mus_is_wav(const char *name)
{
    size_t n = strlen(name);
    const char *e;

    if (n < 4)
        return 0;
    e = name + n - 4;
    return (e[0] == '.' &&
            (e[1] == 'w' || e[1] == 'W') &&
            (e[2] == 'a' || e[2] == 'A') &&
            (e[3] == 'v' || e[3] == 'V'));
}

static void mus_trim_ext(const char *name, char *out, size_t cap)
{
    size_t n = strlen(name);
    size_t keep = n;

    if (mus_is_wav(name))
        keep = n - 4;
    if (keep >= cap)
        keep = cap - 1;
    memcpy(out, name, keep);
    out[keep] = '\0';
}

static u16 rd_u16(const u8 *p)
{
    return (u16)(p[0] | ((u16)p[1] << 8));
}

static u32 rd_u32(const u8 *p)
{
    return (u32)p[0] | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24);
}

/* 解析 WAV 头：顺序扫描 chunk，找到 fmt 与 data，填 wavctrl */
static int mus_wav_parse(FIL *fp, __wavctrl *w)
{
    u8 hdr[512];
    UINT br = 0;
    u32 pos = 12;
    u8 have_fmt = 0, have_data = 0;

    f_lseek(fp, 0);
    if (f_read(fp, hdr, 12, &br) != FR_OK || br < 12)
        return -1;
    if (rd_u32(hdr) != 0x46464952 || rd_u32(hdr + 8) != 0x45564157)
        return -2; /* 不是 "RIFF" + "WAVE" */

    while (1)
    {
        u32 id, sz;

        f_lseek(fp, pos);
        if (f_read(fp, hdr, 8, &br) != FR_OK || br < 8)
            return -3;
        id = rd_u32(hdr);
        sz = rd_u32(hdr + 4);

        if (id == 0x20746D66) /* "fmt " */
        {
            if (f_read(fp, hdr, 16, &br) != FR_OK || br < 16)
                return -3;
            w->audioformat = rd_u16(hdr);
            w->nchannels = rd_u16(hdr + 2);
            w->samplerate = rd_u32(hdr + 4);
            w->bitrate = rd_u32(hdr + 8) * 8; /* ByteRate → 位速(bit/s)，与 Wav_GetCurtime 的算法一致 */
            w->blockalign = rd_u16(hdr + 12);
            w->bps = rd_u16(hdr + 14);
            have_fmt = 1;
        }
        else if (id == 0x61746164) /* "data" */
        {
            w->datastart = pos + 8;
            w->datasize = sz;
            have_data = 1;
            break;
        }

        pos += 8 + ((sz + 1) & ~1u); /* chunk 按偶数字节对齐 */
        if (pos > f_size(fp))
            return -4;
    }

    if (!have_fmt || !have_data)
        return -4;
    if (w->datasize == 0)
        w->datasize = f_size(fp) - w->datastart;

    w->totsec = (w->bitrate != 0) ? (w->datasize / (w->bitrate / 8)) : 0;
    w->cursec = 0;
    return 0;
}

/* 填充一块 DMA 缓冲区；读不满说明到文件尾了 */
static void mus_fill(u8 *buf)
{
    UINT br = 0;

    f_read(s_file, buf, s_buf_size, &br);
    if (br < s_buf_size)
    {
        memset(buf + br, 0, s_buf_size - br);
        s_eof_wait = 2; /* 这块播完 + 另一块播完 → 才算一首结束 */
    }
}

/* 停止并释放占用（DMA、文件、缓冲区、歌词） */
static void mus_release(void)
{
    /* 只有在真正播放过（DMA 已初始化）时才去操作 DMA 寄存器 */
    if (s_file != NULL || s_buf1 != NULL || s_buf2 != NULL)
    {
        Audio_Stop();
        DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
        i2s_tx_callback = mus_dma_idle; /* 后续残留中断不会访问已释放内存 */
    }

    if (s_file != NULL)
    {
        f_close(s_file);
        myfree(s_file);
        s_file = NULL;
    }
    if (s_buf1 != NULL)
    {
        myfree(s_buf1);
        s_buf1 = NULL;
    }
    if (s_buf2 != NULL)
    {
        myfree(s_buf2);
        s_buf2 = NULL;
    }

    audiodev.file = NULL;
    audiodev.i2sbuf1 = NULL;
    audiodev.i2sbuf2 = NULL;

    Lyric_Unload();
    s_eof_wait = 0;
    s_lyric_idx = -1;
    s_dma_pending = 0;
    wavtransferend = 0;
}

/* ======================= 对外接口 ======================= */

/* 设置目标音量（0~100%）。实际音量由 mus_vol_service() 逐级逼近，避免跳变。 */
void mus_set_volume(int pct)
{
    if (pct < 0)
        pct = 0;
    if (pct > MUS_VOL_MAX)
        pct = MUS_VOL_MAX;
    s_vol_target = pct;
}

int mus_volume(void)
{
    return s_vol_target;
}

/* 每 10ms 调一次：把硬件音量朝目标值走一级（0~63 共 64 级，全程约 0.6s） */
void mus_vol_service(void)
{
    int tgt;
    int pct;

    if (!s_audio_ready)
        return;

    /* 播报提示音时临时提升音量（提示音录音电平比歌曲低） */
    pct = s_vol_target + (s_is_prompt ? MUS_PROMPT_BOOST : 0);
    if (pct > MUS_VOL_MAX)
        pct = MUS_VOL_MAX;

    tgt = (pct * MUS_VOL_REG_MAX + (MUS_VOL_MAX / 2)) / MUS_VOL_MAX;
    if (s_vol_applied < 0)
    {
        s_vol_applied = tgt;
        WM8978_SPKvol_Set((u8)s_vol_applied);
        return;
    }
    if (s_vol_applied == tgt)
        return;

    s_vol_applied += (s_vol_applied < tgt) ? 1 : -1;
    WM8978_SPKvol_Set((u8)s_vol_applied);
}

void mus_init(void)
{
    if (s_lock == NULL)
        s_lock = xSemaphoreCreateMutex(); /* 首次进音乐界面时创建，此后两个任务并发访问都有锁保护 */

    if (s_audio_ready)
        return;

    if (Wm8978_Init() == 0)
    {
        s_audio_ready = 1;
        s_vol_applied = -1;          /* 让音量服务把当前目标值写入硬件 */
        mus_vol_service();
        printf("[music] WM8978 + I2S 初始化成功\r\n");
    }
    else
    {
        printf("[music] WM8978 初始化失败\r\n");
    }
}

int mus_scan(void)
{
    int n;

    mus_lock();
    n = mus_scan_nolock();
    mus_unlock();
    return n;
}

static int mus_scan_nolock(void)
{
    DIR dir;
    FILINFO fno;
    FRESULT res;

    s_count = 0;
    res = f_opendir(&dir, MUS_DIR);
    if (res != FR_OK)
    {
        printf("[music] 打开 %s 失败 (%d)\r\n", MUS_DIR, res);
        s_cur = -1;
        return 0;
    }

    while (1)
    {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == '\0')
            break;
        if (fno.fattrib & AM_DIR)
            continue;
        if (!mus_is_wav(fno.fname))
            continue;
        if (s_count >= MUS_MAX_SONGS)
            break;

        snprintf(s_paths[s_count], MUS_PATH_MAX, "%s/%s", MUS_DIR, fno.fname);
        mus_trim_ext(fno.fname, s_titles[s_count], MUS_TITLE_MAX);
        s_count++;
    }
    f_closedir(&dir);

    if (s_cur >= s_count)
        s_cur = (s_count > 0) ? 0 : -1;

    /* 只在歌曲数量变化时打印，避免每次 0x55 都刷一条 */
    {
        static int s_scan_printed = -1;

        if (s_count != s_scan_printed)
        {
            s_scan_printed = s_count;
            printf("[music] %s 下找到 %d 首可播放歌曲\r\n", MUS_DIR, s_count);
        }
    }
    return s_count;
}

int mus_current(void)
{
    return s_cur;
}

int mus_state(void)
{
    return (int)s_state;
}

const char *mus_title(int idx)
{
    if (idx < 0 || idx >= s_count)
        return "";
    return s_titles[idx];
}

int mus_play(int idx)
{
    int r;

    mus_lock();
    r = mus_play_nolock(idx);
    mus_unlock();
    return r;
}

static int mus_play_nolock(int idx)
{
    if (idx < 0 || idx >= s_count)
        return -1;
    return mus_play_at_nolock(s_paths[idx], idx, 0, 0);
}

/* 通用播放：path=WAV 文件路径；idx=歌曲序号（提示音传 -1，表示不改 s_cur）；
   start_pos=从数据区哪个偏移开始（0 或越界则从头）；is_prompt=1 表示提示音，
   播完后自动回到被打断歌曲的原位置继续 */
static int mus_play_at_nolock(const char *path, int idx, u32 start_pos, u8 is_prompt)
{
    if (!s_audio_ready)
        mus_init();
    if (!s_audio_ready)
        return -2;

    mus_release();

    s_file = (FIL *)mymalloc(sizeof(FIL));
    s_buf_size = MUS_BUF_SIZE;
    s_buf1 = (u8 *)mymalloc(s_buf_size);
    s_buf2 = (u8 *)mymalloc(s_buf_size);
    if (s_buf1 == NULL || s_buf2 == NULL)
    {
        /* 内存不够时回退到 8KB 一块，保证还能播放（抗 UI 阻塞能力略降） */
        if (s_buf1) { myfree(s_buf1); s_buf1 = NULL; }
        if (s_buf2) { myfree(s_buf2); s_buf2 = NULL; }
        s_buf_size = MUS_BUF_SIZE / 2;
        s_buf1 = (u8 *)mymalloc(s_buf_size);
        s_buf2 = (u8 *)mymalloc(s_buf_size);
    }
    if (s_file == NULL || s_buf1 == NULL || s_buf2 == NULL)
    {
        printf("[music] 内存分配失败\r\n");
        mus_release();
        return -3;
    }

    audiodev.file = s_file;
    audiodev.i2sbuf1 = s_buf1;
    audiodev.i2sbuf2 = s_buf2;

    if (f_open(s_file, path, FA_READ) != FR_OK)
    {
        printf("[music] 打不开 %s\r\n", path);
        mus_release();
        return -4;
    }
    if (mus_wav_parse(s_file, &wavctrl) != 0)
    {
        printf("[music] %s 不是可播放的 WAV\r\n", path);
        mus_release();
        return -5;
    }

    I2S2_SampleRate_Set(wavctrl.samplerate);

    /* 定位到数据区（歌曲从头，提示音结束后的恢复则回到打断点） */
    /* 归一化前先记录这次是不是"从打断点续播"，供日志区分 */
    {
        u8 resumed = (start_pos > wavctrl.datastart &&
                      start_pos <= wavctrl.datastart + wavctrl.datasize);
        s_last_play_resumed = resumed;
    }
    if (start_pos < wavctrl.datastart || start_pos > wavctrl.datastart + wavctrl.datasize)
        start_pos = wavctrl.datastart;
    f_lseek(s_file, start_pos);

    if (!is_prompt)
    {
        /* 同名 .lrc 歌词，找不到也没关系 */
        Lyric_Load(path);
    }
    else
    {
        Lyric_Unload(); /* 提示音没有歌词，避免界面还显示上一首的歌词 */
    }
    s_lyric_idx = -1;

    /* 重新绑定双缓冲并强制 CT=0：每次都从 buf1 开始，
       否则上次停止时 CT 可能停在 M1AR，会从 buf2 起播导致音频顺序错乱 */
    DMA1_Stream4->M0AR = (u32)s_buf1;
    DMA_DoubleBufferModeConfig(DMA1_Stream4, (u32)s_buf2, DMA_Memory_0);
    DMA1_Stream4->M1AR = (u32)s_buf2;
    DMA_SetCurrDataCounter(DMA1_Stream4, s_buf_size / 2); /* 16bit 数据: 个数按半字算 */
    i2s_tx_callback = mus_dma_callback;

    wavwitchbuf = 0;
    wavtransferend = 0;
    s_dma_pending = 0;
    s_eof_wait = 0;
    mus_fill(s_buf1);
    mus_fill(s_buf2);

    Audio_Start();

    if (is_prompt)
    {
        s_is_prompt = 1;
    }
    else
    {
        s_is_prompt = 0;
        s_resume_idx = -1;
        s_resume_pos = 0;
        if (idx >= 0)
            s_cur = idx;
    }
    s_state = MUS_PLAYING;
    if (is_prompt)
        printf("[music] 播报 %s\r\n", path);   /* 提示音只有这一行日志 */
    else if (s_last_play_resumed)
        printf("[music] 恢复播放 %s（回到播报打断前的位置）\r\n", path);
    else
        printf("[music] 开始播放 %s\r\n", path);
    return 0;
}

/* 播报语音提示音（如 0:/prompt/fanSwitchO.wav）：
   正在放歌时先记住位置打断，提示音播完 mus_tick 会自动回到原位置继续 */
int mus_play_prompt(const char *path)
{
    int r;
    int resume_idx = -1;
    u32 resume_pos = 0;

    if (path == NULL || path[0] == '\0')
        return -1;

    mus_lock();
    if (s_is_prompt)
    {
        /* 提示音还没播完又来了新的：沿用第一次打断时的恢复点 */
        resume_idx = s_resume_idx;
        resume_pos = s_resume_pos;
    }
    else if (s_state == MUS_PLAYING && s_cur >= 0)
    {
        resume_idx = s_cur;
        resume_pos = wavctrl.datastart +
                     (u32)((double)wavctrl.cursec * (double)wavctrl.bitrate / 8.0);
    }
    r = mus_play_at_nolock(path, -1, 0, 1);
    if (r == 0)
    {
        s_resume_idx = resume_idx;
        s_resume_pos = resume_pos;
    }
    mus_unlock();
    return r;
}

void mus_toggle(void)
{
    mus_lock();
    mus_toggle_nolock();
    mus_unlock();
}

static void mus_toggle_nolock(void)
{
    if (s_state == MUS_PLAYING)
    {
        Audio_Pause();
        s_state = MUS_PAUSED;
    }
    else if (s_state == MUS_PAUSED)
    {
        Audio_Resume();
        s_state = MUS_PLAYING;
    }
    else
    {
        mus_play_nolock((s_cur >= 0 && s_cur < s_count) ? s_cur : 0);
    }
}

void mus_prev(void)
{
    int base;

    mus_lock();
    if (s_count > 0)
    {
        base = (s_cur >= 0) ? s_cur : 0;
        mus_play_nolock((base + s_count - 1) % s_count);
    }
    mus_unlock();
}

void mus_next(void)
{
    mus_lock();
    mus_next_nolock();
    mus_unlock();
}

static void mus_next_nolock(void)
{
    int base;

    if (s_count <= 0)
        return;
    base = (s_cur >= 0) ? s_cur : -1;
    mus_play_nolock((base + 1) % s_count);
}

void mus_stop(void)
{
    mus_lock();
    mus_stop_nolock();
    mus_unlock();
}

static void mus_stop_nolock(void)
{
    mus_release();
    s_state = MUS_STOPPED;
}

/* 只暂停 / 只续播（语音交互里"先暂停音乐"用，不影响当前曲目） */
void mus_pause(void)
{
    mus_lock();
    if (s_state == MUS_PLAYING)
    {
        Audio_Pause();
        s_state = MUS_PAUSED;
    }
    mus_unlock();
}

void mus_resume(void)
{
    mus_lock();
    if (s_state == MUS_PAUSED)
    {
        Audio_Resume();
        s_state = MUS_PLAYING;
    }
    mus_unlock();
}

u8 mus_is_prompt(void)
{
    return s_is_prompt;
}

/* 当前播放位置（数据区字节偏移），用于暂停后回到原位置继续 */
u32 mus_byte_pos(void)
{
    if (s_state == MUS_STOPPED || s_file == NULL || wavctrl.bitrate == 0)
        return 0;
    return wavctrl.datastart +
           (u32)((double)wavctrl.cursec * (double)wavctrl.bitrate / 8.0);
}

int mus_play_at(int idx, u32 byte_pos)
{
    int r;

    if (idx < 0 || idx >= s_count)
        return -1;

    mus_lock();
    r = mus_play_at_nolock(s_paths[idx], idx, byte_pos, 0);
    mus_unlock();
    return r;
}

void mus_tick(void)
{
    if (s_dma_pending == 0)
        return;
    mus_lock();
    mus_tick_nolock();
    mus_unlock();
}

static void mus_tick_nolock(void)
{
    u8 pm;
    u16 idx;

    if (s_dma_pending == 0)
        return;
    pm = s_dma_pending;
    s_dma_pending &= (u8)(~pm); /* 只清本次处理的位，新到的完成事件保留 */

    if (s_file == NULL || s_buf1 == NULL || s_buf2 == NULL)
        return;

    /* 暂停：DMA 还在循环播放，把播完的块清零，最多两块(≈186ms)后完全静音 */
    if (s_state == MUS_PAUSED)
    {
        if (pm & 0x01)
            memset(s_buf1, 0, s_buf_size);
        if (pm & 0x02)
            memset(s_buf2, 0, s_buf_size);
        return;
    }

    if (s_state != MUS_PLAYING)
        return;

    /* 文件已经读完：等缓冲区都播完，再自动切下一首 */
    if (s_eof_wait > 0)
    {
        if (pm & 0x01)
        {
            memset(s_buf1, 0, s_buf_size);
            if (s_eof_wait)
                s_eof_wait--;
        }
        if (pm & 0x02)
        {
            memset(s_buf2, 0, s_buf_size);
            if (s_eof_wait)
                s_eof_wait--;
        }

        if (s_eof_wait == 0)
        {
            if (s_is_prompt)
            {
                /* 提示音播完：回到被打断歌曲的原位置继续，没有则停止 */
                int ridx = s_resume_idx;
                u32 rpos = s_resume_pos;

                s_is_prompt = 0;
                s_resume_idx = -1;
                s_resume_pos = 0;
                mus_release();
                s_state = MUS_STOPPED;
                if (ridx >= 0 && ridx < s_count)
                    mus_play_at_nolock(s_paths[ridx], ridx, rpos, 0);
            }
            else if (s_count > 0)
                mus_next_nolock(); /* 一首结束自动切下一首（最后一首绕回第一首） */
            else
                mus_stop_nolock();
        }
        return;
    }

    if (pm & 0x01)
        mus_fill(s_buf1);
    if (pm & 0x02)
        mus_fill(s_buf2);

    Wav_GetCurtime(s_file, &wavctrl);

    idx = Lyric_GetCurrentIdx(wavctrl.cursec);
    if (idx != 0xFFFF)
        s_lyric_idx = (int)idx;
}

unsigned int mus_cursec(void)
{
    if (s_state == MUS_STOPPED)
        return 0;
    return (unsigned int)wavctrl.cursec;
}

unsigned int mus_totsec(void)
{
    if (s_state == MUS_STOPPED)
        return 0;
    return (unsigned int)wavctrl.totsec;
}

int mus_lyric_index(void)
{
    return s_lyric_idx;
}

const char *mus_lyric_line(int idx)
{
    const u8 *txt;

    if (idx < 0)
        return NULL;
    txt = Lyric_GetLine((u16)idx);
    if (txt == NULL)
        return NULL;
    return (const char *)txt;
}
