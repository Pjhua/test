#include "wavplay.h"
#include "malloc.h"
#include "ff.h"
#include "audioplay.h"
#include "stdlib.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"
#include "i2s.h"
#include "key.h"
#include "lcd.h"
#include "lcd_init.h"
#include "CST816S.h"
#include "wm8978.h"
#include "lyric.h"

__wavctrl wavctrl;                         // WAV控制结构体，解析WAV格式的头部信息
volatile unsigned char wavtransferend = 0; // i2s传输完成标志
volatile unsigned char wavwitchbuf = 0;    // i2sbufx指示标志
char Volume = 40;

/*-----------------------------------------------------------------------
 * 音量条 UI 常量
 *-----------------------------------------------------------------------*/
#define VOL_X   50     /* 音量条左 x（居中：70+100/2=120，LCD_W=240） */
#define VOL_W   100    /* 音量条宽 */
#define VOL_Y   200    /* 音量条上 y（时间区 TIME_Y=230 的上方） */
#define VOL_H   30     /* 音量条高（加大，方便触摸） */
#define VOL_MAX 63     /* WM8978 音量范围 0~63 */

/* 重绘音量条 */
static void draw_volume_bar(void)
{
    u8 filled_w;
    char buf[8];
    LCD_Fill(VOL_X - 5, VOL_Y - 2, VOL_X + VOL_W + 2, VOL_Y + VOL_H + 4, WHITE);
    filled_w = (u16)Volume * VOL_W / VOL_MAX;
    LCD_Fill(VOL_X, VOL_Y, VOL_X + filled_w, VOL_Y + VOL_H, BLUE);
    LCD_Fill(VOL_X + filled_w, VOL_Y, VOL_X + VOL_W, VOL_Y + VOL_H, LGRAY);
    sprintf(buf, "V%02d", Volume);
    LCD_ShowString(VOL_X + VOL_W + 4, VOL_Y, (u8 *)buf, BLACK, WHITE, 12, 0);
}
/*********************************************************************************************************
 * 函 数 名 : WavDecode_Init
 * 功能说明 : 解析指定wav文件的头部信息
 * 形    参 : fname：文件路径+文件名，wavx：wav头部信息存放结构体指针
 * 返 回 值 : 0：成功，1：打开文件失败，2：非wav文件，3：data区域未找到
 * 备    注 : wav专用，不支持其他类型
 *********************************************************************************************************/
unsigned char Wav_DecodeInit(unsigned char *fname, __wavctrl *wavx)
{
    unsigned char res;
    unsigned int br = 0;
    unsigned char *buf;
    FIL *ftemp;
    ChunkRIFF *riff;
    ChunkFMT *fmt;
    ChunkFACT *fact;
    ChunkDATA *data;

    ftemp = (FIL *)mymalloc(sizeof(FIL)); // 为文件描述符分配内存空间
    buf = mymalloc(512);                  // 为data数据段分配内存空间

    if (ftemp && buf) // 申请失败则结束
    {
        res = f_open(ftemp, (TCHAR *)fname, FA_READ); // 只读的形式打开文件，避免误操作
        if (res == FR_OK)
        {
            f_read(ftemp, buf, 512, &br);   // 读取512字节在数据
            riff = (ChunkRIFF *)buf;        // 获取RIFF块，其数据格式是固定的，所以直接按照结构体内存分布强转即可
            if (riff->Format == 0X45564157) // 如果是WAV文件
            {
                fmt = (ChunkFMT *)(buf + 12);                                   // 获取FMT块
                fact = (ChunkFACT *)(buf + 12 + 8 + fmt->ChunkSize);            // 读取FACT块
                if (fact->ChunkID == 0X74636166 || fact->ChunkID == 0X5453494C) // 具有fact/LIST块的时候(未测试)
                    wavx->datastart = 12 + 8 + fmt->ChunkSize + 8 + fact->ChunkSize;
                else
                    wavx->datastart = 12 + 8 + fmt->ChunkSize;
                data = (ChunkDATA *)(buf + wavx->datastart); // 读取DATA块
                if (data->ChunkID == 0X61746164)             // 解析成功!
                {
                    wavx->audioformat = fmt->AudioFormat;  // 音频格式
                    wavx->nchannels = fmt->NumOfChannels;  // 通道数
                    wavx->samplerate = fmt->SampleRate;    // 采样率
                    wavx->bitrate = fmt->ByteRate * 8;     // 得到位速
                    wavx->blockalign = fmt->BlockAlign;    // 块对齐
                    wavx->bps = fmt->BitsPerSample;        // 位数,16/24/32位
                    wavx->datasize = data->ChunkSize;      // 数据块大小
                    wavx->datastart = wavx->datastart + 8; // 数据流开始的地方.
                }
                else
                    res = 3; // data区域未找到.
            }
            else
                res = 2; // 非wav文件
        }
        else
            res = 1; // 打开文件错误
    }

    f_close(ftemp); // 关闭文件
    myfree(ftemp);  // 释放内存
    myfree(buf);

    return 0;
}
/*********************************************************************************************************
 * 函 数 名 : Wav_BuffFill
 * 功能说明 : 读取文件数据，填充缓冲区
 * 形    参 : buf：缓冲区，size：填充数量
 * 返 回 值 : 读取到的数据个数
 * 备    注 : 读不够一帧缓冲区时，末尾补0
 *********************************************************************************************************/
unsigned int Wav_BuffFill(unsigned char *buf, unsigned short size)
{
    unsigned int bread = 0;
    unsigned short i;

    f_read(audiodev.file, buf, size, (UINT *)&bread);
    if (bread < size) // 不够数据了,补充0
        for (i = bread; i < size - bread; i++)
            buf[i] = 0;

    return bread;
}
/*********************************************************************************************************
 * 函 数 名 : Wav_I2sDmaTx_Callback
 * 功能说明 : dma传输回调，可以理解为dma的中断服务函数
 * 形    参 : 无
 * 返 回 值 : 无
 * 备    注 : dma轮流搬运两个缓冲区的数据，每传完一个缓冲区会触发一次
 *********************************************************************************************************/
void Wav_I2sDmaTx_Callback(void)
{
    if (DMA1_Stream4->CR & 0x80000) // 当前使用的是存储器一
    {
        wavwitchbuf = 0;
        if ((audiodev.status & 0X01) == 0) // 当暂停播放时，给缓冲区填充0
            memset(audiodev.i2sbuf1, 0, WAV_I2S_TX_DMA_BUFSIZE);
    }
    else // 当前使用的是存储器二
    {
        wavwitchbuf = 1;
        if ((audiodev.status & 0X01) == 0)
            memset(audiodev.i2sbuf2, 0, WAV_I2S_TX_DMA_BUFSIZE);
    }
    wavtransferend = 1; // 标记传输完成
}
/*********************************************************************************************************
 * 函 数 名 : Wav_GetCurtime
 * 功能说明 : 获取当前播放时间
 * 形    参 : fx：文件指针，wavx：wav播放控制器
 * 返 回 值 : 无
 * 备    注 : 无
 *********************************************************************************************************/
void Wav_GetCurtime(FIL *fx, __wavctrl *wavx)
{
    long long fpos = 0;

    wavx->totsec = wavx->datasize / (wavx->bitrate / 8); // 歌曲总长度(单位:秒)
    fpos = fx->fptr - wavx->datastart;                   // 得到当前文件播放到的地方
    wavx->cursec = fpos * wavx->totsec / wavx->datasize; // 当前播放到第多少秒了?
}
/*********************************************************************************************************
 * 函 数 名 : Wav_PlaySong
 * 功能说明 : 播放wav格式的音频文件
 * 形    参 : fname：文件路径+文件名
 * 返 回 值 :
 * 备    注 : 无
 *********************************************************************************************************/

unsigned char Wav_PlaySong(unsigned char *fname)
{
    unsigned char res;
    unsigned int fillnum;
    u8 key;
    signed char dir;

    audiodev.file = (FIL *)mymalloc(sizeof(FIL));        // 申请用于存储文件描述符的内存
    audiodev.i2sbuf1 = mymalloc(WAV_I2S_TX_DMA_BUFSIZE); // 申请缓存区1的内存，8kb大小
    audiodev.i2sbuf2 = mymalloc(WAV_I2S_TX_DMA_BUFSIZE); // 申请缓存区2的内存，8kb大小

    if (audiodev.file && audiodev.i2sbuf1 && audiodev.i2sbuf2) // 判断有没有申请成功，失败一个都会直接结束
    {
        res = Wav_DecodeInit(fname, &wavctrl); // 得到文件的信息
        if (res == 0)                          // 解析文件成功
        {
            Audio_Stop();
            DMA1_Stream4->M0AR = (unsigned int)audiodev.i2sbuf1; // 绑定缓冲区一
            DMA1_Stream4->M1AR = (unsigned int)audiodev.i2sbuf2; // 绑定缓冲区二
            i2s_tx_callback = Wav_I2sDmaTx_Callback;             // 绑定回调函数

            res = f_open(audiodev.file, (TCHAR *)fname, FA_READ); // 打开文件
            if (res == 0)
            {
                f_lseek(audiodev.file, wavctrl.datastart); // 跳过文件头
                fillnum = Wav_BuffFill(audiodev.i2sbuf1, WAV_I2S_TX_DMA_BUFSIZE);
                fillnum = Wav_BuffFill(audiodev.i2sbuf2, WAV_I2S_TX_DMA_BUFSIZE);

                char showname[100];
                const char *pname = strrchr((char *)fname, '/');
                if (!pname)
                    pname = (char *)fname;
                else
                    pname++;
                strncpy(showname, pname, sizeof(showname) - 1);
                showname[sizeof(showname) - 1] = '\0';
                /* 截断扩展名：最后一个 '.' 改成 '\0' */
                char *dot = strrchr(showname, '.');
                if (dot)
                    *dot = '\0';
                LCD_Fill(0, 0, LCD_W, 200, WHITE);
                LCD_ShowMixed_Flash(25, 50, (const u8 *)showname, BLUE, WHITE, 16, 0);
				draw_volume_bar();

                /* 加载同名 .lrc 歌词文件 */
                Lyric_Load((const char *)fname);

                /* 初始化歌词显示区域（留空，等第一句歌词到达再刷） */
                LCD_Fill(0, 100, LCD_W, 140, WHITE);

                Audio_Start();
                while (res == 0)
                {
                    while (1) // 等待wav传输完成;
                    {
                        dir=cst816s_get_swipe();
                        if (dir==SLI_UP)
                        {
                            res=5;
							break;
                        }
                        if (dir==SLI_RIGHT)
                        {
                            if (Volume <= 58)
                                Volume+=5;
							draw_volume_bar();
							WM8978_SPKvol_Set(Volume);
                        }
                        if (dir==SLI_LEFT)
                        {
                            if (Volume >=5)
                                Volume-=5;
							draw_volume_bar();
							WM8978_SPKvol_Set(Volume);
                        }
                        key = key_scan();
                        if (key == KEY2_PRE)
                        {
                            if (audiodev.status & 0x01)
                                audiodev.status &= ~(1 << 0);
                            else
                                audiodev.status |= 0x01;
                        }
                        else if (key == KEY1_PRE || key == KEY3_PRE)
                        {
                            res = key;
                            break;
                        }
                        if ((audiodev.status & 0x01) == 0)
                            delay_ms(10);
                        else if (wavtransferend)
                            break;
                    }
                    wavtransferend = 0;
                    static u32 last_cursec = 0xFFFFFFFF; // 跨 Wav_PlaySong 调用也能工作
                    u32 cur_min, cur_sec, total_min, total_sec;
                    char buf[24];
                    Wav_GetCurtime(audiodev.file, &wavctrl);
                    if (wavctrl.cursec != last_cursec)
                    {
                        last_cursec = wavctrl.cursec;
                        cur_min = wavctrl.cursec / 60;
                        cur_sec = wavctrl.cursec % 60;
                        total_min = wavctrl.totsec / 60;
                        total_sec = wavctrl.totsec % 60;
                        sprintf(buf, "%u:%02u/%u:%02u", cur_min, cur_sec, total_min, total_sec);
                        LCD_Fill(0, 150, LCD_W, 180, WHITE);
                        LCD_ShowString(25, 155, (const u8 *)buf, BLACK, WHITE, 16, 0);
                        u16 idx = Lyric_GetCurrentIdx(wavctrl.cursec);
                        if (idx != 0xFFFF)
                        {
                            const u8 *txt = Lyric_GetCurrent(wavctrl.cursec);
                            if (txt)
                            {
                                LCD_Fill(0, 100, LCD_W, 140, WHITE);
                                LCD_ShowMixed_Flash(5, 105, txt, RED, WHITE, 16, 0);
                            }
                        }
                    }
                    if (key == KEY1_PRE || key == KEY3_PRE||res==5)
                    {
                        break;
                    }
                    if (fillnum != WAV_I2S_TX_DMA_BUFSIZE) // 播放结束
                    {
                        res = 0;
                        break;
                    }
                    if (wavwitchbuf)
                        fillnum = Wav_BuffFill(audiodev.i2sbuf2, WAV_I2S_TX_DMA_BUFSIZE); // 填充buf2
                    else
                        fillnum = Wav_BuffFill(audiodev.i2sbuf1, WAV_I2S_TX_DMA_BUFSIZE); // 填充buf1
                }
                Audio_Stop();
            }
            else
                res = 0XFF;
        }
        else
            res = 0XFF;
    }
    else
        res = 0XFF;

    f_close(audiodev.file);
    myfree(audiodev.file);    // 释放内存
    myfree(audiodev.i2sbuf1); // 释放内存
    myfree(audiodev.i2sbuf2); // 释放内存

    return res;
}
