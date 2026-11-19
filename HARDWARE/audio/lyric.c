#include "lyric.h"
#include "ff.h"
#include "malloc.h"
#include "string.h"
#include "stdio.h"

/* 全局歌词数据 —— 同一时刻只播一首歌，全局足够 */
static LyricLine *g_lines = NULL;
static u16 g_line_count = 0; /* 已解析的行数 */
static u16 g_line_cap = 0;	 /* 容量（= LYRIC_LINE_MAX） */

/* 当前显示的行号（用于 LCD 避免重复刷屏） */
static u16 g_current_idx = 0xFFFF;

/* 已加载歌词对应的 wav 路径：同一首歌打断续播时不必重读 SD 卡 */
static char g_loaded_wav[64] = "";
/* 上次打印过“加载成功”的路径：同一首歌反复打断/续播只打印一次 */
static char g_printed_wav[64] = "";

/*================================================================
 * 把 "0:/music/xxx.wav" 改成 "0:/music/xxx.lrc"
 *================================================================*/
static void wav_path_to_lrc_path(const char *wav, char *out)
{
	strncpy(out, wav, 255);
	out[255] = '\0';
	/* 找最后一个 '.'，把扩展名改成 .lrc */
	{
		char *dot = strrchr(out, '.');
		if (dot)
			strcpy(dot, ".lrc");
		else
			strcat(out, ".lrc"); /* 没扩展名也加 */
	}
}

/*================================================================
 * 把一行文本中的时间戳 [mm:ss.xx] 提出来
 * 返回：成功提取的时间（秒）；返回 0xFFFF 表示这行没有合法时间戳
 *
 * 支持格式：
 *   [00:31.87]
 *   [00:31]
 *   [0:31.8]
 * 支持一行里有多个时间戳：[00:31.87][00:35.00] text → 全部都返回一个
 *================================================================*/
static u32 parse_timestamp(const u8 *s, u8 **text_out)
{
	/* 格式：[mm:ss.xx] */
	u32 mm = 0, ss = 0;
	const u8 *p = s;

	if (*p != '[')
		return 0xFFFF;
	p++;
	/* 读分钟 */
	while (*p >= '0' && *p <= '9')
	{
		mm = mm * 10 + (*p - '0');
		p++;
	}
	if (*p != ':')
		return 0xFFFF;
	p++;
	/* 读秒 */
	while (*p >= '0' && *p <= '9')
	{
		ss = ss * 10 + (*p - '0');
		p++;
	}
	/* 小数点及后面（可选，跳过） */
	if (*p == '.')
	{
		p++;
		while (*p >= '0' && *p <= '9')
			p++;
	}
	if (*p != ']')
		return 0xFFFF;
	p++; /* 跳过 ] */

	/* 跳过紧接着的下一个时间戳（一行里多个时间戳的情况）
	 * 简单处理：如果又是 '['，只取第一个时间戳即可 */

	*text_out = (u8 *)p; /* 指向文本开头 */
	return mm * 60 + ss;
}

/*================================================================
 * 加载歌词：文件全部读进来，然后逐行解析
 *================================================================*/
u16 Lyric_Load(const char *wav_path)
{
	FIL lrc_file;
	FRESULT res;
	u32 filesize, br;
	u8 *filebuf = NULL;
	u8 *line_start, *p, *eol;  
	char lrc_path[256];

	/* 同一首歌的歌词已经在内存里（播报打断后续播）：直接复用，不再读卡、不重复打印 */
	if (g_lines != NULL && strcmp(g_loaded_wav, wav_path) == 0)
		return g_line_count;

	/* 先释放上一次的 */
	Lyric_Unload();

	wav_path_to_lrc_path(wav_path, lrc_path);
	res = f_open(&lrc_file, lrc_path, FA_READ);
	if (res != FR_OK)
	{
		printf("[lyric] 打开失败: %s (res=%d)\r\n", lrc_path, res);
		return 0;
	}

	filesize = f_size(&lrc_file);
	if (filesize < 10 || filesize > 65535) /* 太小或太大都不合理 */
	{
		f_close(&lrc_file);
		printf("[lyric] 文件大小异常: %lu\r\n", filesize);
		return 0;
	}

	filebuf = (u8 *)mymalloc(filesize + 1);
	if (!filebuf)
	{
		f_close(&lrc_file);
		printf("[lyric] mymalloc 失败\r\n");
		return 0;
	}

	res = f_read(&lrc_file, filebuf, filesize, &br);
	f_close(&lrc_file);
	filebuf[br] = '\0';

	/* 分配歌词行数组 */
	g_line_cap = LYRIC_LINE_MAX;
	g_lines = (LyricLine *)mymalloc(sizeof(LyricLine) * g_line_cap);
	if (!g_lines)
	{
		myfree(filebuf);
		printf("[lyric] 行数组 malloc 失败\r\n");
		return 0;
	}
	g_line_count = 0;

	/* 去掉 UTF-8 BOM */
	p = filebuf;
	if (br >= 3 && p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF)
		p += 3;

	/* 逐行解析 */
	line_start = p;
	while (p <= filebuf + br)
	{
		/* 找到行尾 (\r\n 或 \n) */
		eol = p;
		while (eol < filebuf + br && *eol != '\n')
			eol++;
		/* 去掉尾部 \r */
		if (eol > line_start && *(eol - 1) == '\r')
			*(eol - 1) = '\0';
		else
			*eol = '\0';

		if (*line_start == '[')
		{
			u8 *text_ptr;
			u32 ts = parse_timestamp(line_start, &text_ptr);
			if (ts != 0xFFFF && text_ptr && *text_ptr != '\0')
			{
				if (g_line_count < g_line_cap)
				{
					g_lines[g_line_count].sec = (u16)ts;
					strncpy((char *)g_lines[g_line_count].text,
							(char *)text_ptr,
							LYRIC_TEXT_MAX - 1);
					g_lines[g_line_count].text[LYRIC_TEXT_MAX - 1] = '\0';
					g_line_count++;
				}
			}
		}

		p = eol + 1;
		line_start = p;
	}

	myfree(filebuf);

	/* 简单按时间戳从小到大排序（插入排序，行数小，够用） */
	{
		u16 i, j;
		for (i = 1; i < g_line_count; i++)
		{
			LyricLine tmp = g_lines[i];
			j = i;
			while (j > 0 && g_lines[j - 1].sec > tmp.sec)
			{
				g_lines[j] = g_lines[j - 1];
				j--;
			}
			g_lines[j] = tmp;
		}
	}

	g_current_idx = 0xFFFF; /* 重置显示索引 */
	strncpy(g_loaded_wav, wav_path, sizeof(g_loaded_wav) - 1);
	g_loaded_wav[sizeof(g_loaded_wav) - 1] = '\0';
	if (strcmp(g_printed_wav, wav_path) != 0)
	{
		strncpy(g_printed_wav, wav_path, sizeof(g_printed_wav) - 1);
		g_printed_wav[sizeof(g_printed_wav) - 1] = '\0';
		printf("[lyric] 加载 %s 成功，共 %u 行\r\n", lrc_path, g_line_count);
	}
	return g_line_count;
}

/*================================================================
 * 释放
 *================================================================*/
void Lyric_Unload(void)
{
	if (g_lines)
	{
		myfree(g_lines);
		g_lines = NULL;
	}
	g_line_count = 0;
	g_line_cap = 0;
	g_current_idx = 0xFFFF;
	g_loaded_wav[0] = '\0';
}

/*================================================================
 * 查询当前秒数应该显示的歌词
 * 策略：找最后一个 sec <= cursec 的行
 *================================================================*/
const u8 *Lyric_GetCurrent(u32 cursec)
{
	u16 i;
	if (!g_lines || g_line_count == 0)
		return NULL;

	/* 从头扫，找到最后一个 sec <= cursec 的位置 */
	for (i = 0; i < g_line_count; i++)
	{
		if (g_lines[i].sec > cursec)
			break;
	}
	if (i == 0)
		return NULL; /* 所有行的时间都比 cursec 大（还没到第一句） */
	return g_lines[i - 1].text;
}

/*================================================================
 * 行号版查询 —— 同时返回行号，LCD 可判断要不要刷新
 * 返回：0xFFFF 表示未改变；否则是新的行号
 *================================================================*/
u16 Lyric_GetCurrentIdx(u32 cursec)
{
	u16 i, new_idx;
	if (!g_lines || g_line_count == 0)
		return 0xFFFF;

	for (i = 0; i < g_line_count; i++)
	{
		if (g_lines[i].sec > cursec)
			break;
	}
	if (i == 0)
		return 0xFFFF;
	new_idx = i - 1;

	if (new_idx != g_current_idx)
	{
		g_current_idx = new_idx;
		return new_idx; /* 变了，返回新行号 */
	}
	return 0xFFFF; /* 没变 */
}

u16 Lyric_GetLineCount(void) { return g_line_count; }

/*================================================================
 * 按行号取歌词文本（LVGL 一次显示多行时用）
 *================================================================*/
const u8 *Lyric_GetLine(u16 idx)
{
	if (!g_lines || idx >= g_line_count)
		return NULL;
	return g_lines[idx].text;
}
