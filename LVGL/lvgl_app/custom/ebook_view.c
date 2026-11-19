#include "ebook_view.h"
#include "ff.h"
#include <stdio.h>
#include <string.h>

/* ---------- 排版参数（与 setup_scr_screen_ebook.c 的正文框一致） ---------- */
#define EBK_COLS        14      /* 每行汉字数：14 * 16px = 224px */
#define EBK_ROWS        11      /* 每页行数：11 行 + 10 个 2px 行距，避免字形上下相压 */
#define EBK_LINE_CELLS  (EBK_COLS * 2)  /* 每行半格数（汉字=2 格，ASCII=1 格） */

#define EBK_TEXT_MAX    640     /* 单页文本缓冲：11 行 * 14 汉字 * 3 字节 + 换行 + NUL */
#define EBK_MAX_PAGES   2048    /* 单本最多分页数（约 37 万汉字，超出的部分不再细分） */
#define EBK_READ_CHUNK  1024    /* 文件读取分块大小（一页约 360 字节，减少重复读） */

/* ---------- 书本列表 ---------- */
static char s_book_path[EBK_MAX_BOOKS][EBK_PATH_MAX];
static uint16_t s_book_count = 0;
static uint16_t s_book_index = 0;

/* ---------- 当前打开的书 ---------- */
static FIL s_file;
static uint8_t s_file_open = 0;
static uint8_t s_utf8 = 1;        /* 1=文件是 UTF-8，0=GBK（ANSI） */
static uint32_t s_file_size = 0;
static uint32_t s_body_start = 0; /* 正文起点（跳过 UTF-8 BOM） */
static uint32_t s_page_off[EBK_MAX_PAGES]; /* 每页在文件中的起始偏移 */
static uint16_t s_page_count = 0;
static uint16_t s_page_index = 0;
static uint8_t s_page_truncated = 0;

/* ---------- 分块读取缓冲 ---------- */
static uint8_t s_chunk[EBK_READ_CHUNK];
static uint32_t s_chunk_base = 0; /* s_chunk[0] 对应的文件偏移 */
static uint32_t s_chunk_len = 0;

/* ---------- 当前页文本 ---------- */
static char s_page_text[EBK_TEXT_MAX];

/* ---------- 打开结果 ---------- */
#define EBK_OK          0       /* 打开并解析成功 */
#define EBK_ERR_OPEN    2       /* f_open 失败（文件不存在/卡读取失败） */
#define EBK_ERR_DECODE  3       /* 文件打开了，但两种编码都解不出正文 */
static uint8_t s_last_error = 0; /* 最近一次 ebk_open_book 的结果 */
static uint8_t s_last_fresult = 0; /* f_open 失败时 FatFs 返回的 FRESULT 原始码 */

/******************************************************************************
      函数说明：把读取缓冲重置（打开新文件后必须调用）
******************************************************************************/
static void ebk_reader_reset(void)
{
    s_chunk_base = 0;
    s_chunk_len = 0;
}

/******************************************************************************
      函数说明：读取文件任意偏移处的一个字节（自动按块补充缓冲）
      返 回 值：1 成功，0 越界或读失败
******************************************************************************/
static uint8_t ebk_read_byte(uint32_t offset, uint8_t *out)
{
    if (offset >= s_file_size)
        return 0;

    if (offset < s_chunk_base || offset >= s_chunk_base + s_chunk_len)
    {
        UINT br = 0;

        if (f_lseek(&s_file, (FSIZE_t)offset) != FR_OK)
            return 0;
        if (f_read(&s_file, s_chunk, EBK_READ_CHUNK, &br) != FR_OK || br == 0)
            return 0;

        s_chunk_base = offset;
        s_chunk_len = br;
    }

    *out = s_chunk[offset - s_chunk_base];
    return 1;
}

/******************************************************************************
      函数说明：探测文件编码（UTF-8 / GBK）
      返 回 值：1 = UTF-8（含 BOM），0 = GBK（ANSI，中文 txt 的常见编码）
      备    注：GBK 双字节的尾字节常落在 0x40~0x7F，不可能连续通过
               UTF-8 续字节(0x80~0xBF)检查，因此连续校验 4KB 即可可靠区分
******************************************************************************/
static uint8_t ebk_detect_utf8(void)
{
    uint8_t c, b;
    uint32_t start = 0;
    uint32_t i;
    uint32_t checked = 0;
    uint16_t need, k;

    /* 有 UTF-8 BOM 直接判定 */
    if (ebk_read_byte(0, &c) && c == 0xEF &&
        ebk_read_byte(1, &b) && b == 0xBB &&
        ebk_read_byte(2, &b) && b == 0xBF)
        return 1;

    /* 先跳过开头的纯 ASCII 部分（英文书名、空行、目录页），最多看开头 64KB。
       只看固定 1KB 的话，开头 ASCII 超过 1KB 的 GBK 文件会被误判成 UTF-8 */
    while (start < s_file_size && start < 65536u)
    {
        if (!ebk_read_byte(start, &c))
            break;
        if (c >= 0x80)
            break;
        start++;
    }
    if (start >= s_file_size || start >= 65536u)
        return 1; /* 通篇 ASCII，两种编码没区别 */

    /* 从第一个非 ASCII 字节开始连续校验 UTF-8 合法性（最多 4096 字节） */
    i = start;
    while (i < s_file_size && checked < 4096u)
    {
        if (!ebk_read_byte(i, &c))
            break;
        if (c < 0x80)
        {
            i++;
            checked++;
            continue;
        }
        if ((c & 0xE0) == 0xC0)
            need = 2;
        else if ((c & 0xF0) == 0xE0)
            need = 3;
        else if ((c & 0xF8) == 0xF0)
            need = 4;
        else
            return 0; /* 非法首字节 → GBK */

        for (k = 1; k < need; k++)
        {
            if (!ebk_read_byte(i + k, &b))
                return 1; /* 采样被截断，按 UTF-8 */
            if ((b & 0xC0) != 0x80)
                return 0; /* 非法续字节 → GBK */
        }
        i += need;
        checked += need;
    }
    return 1;
}

/******************************************************************************
      函数说明：把中文标点映射成 ASCII
      备    注：工程里的 myFont_simkai16 只有 GB2312 的 6763 个汉字 + ASCII，
               没有任何中文标点（。，、！？：；（）《》“”… 都没有字模），
               不映射的话这些字符会整片显示成方块
******************************************************************************/
static uint32_t ebk_map_punct(uint32_t uni)
{
    if (uni < 0x20 || uni == 0x7F) /* 制表符等控制字符（\r\n 已提前处理） */
        return ' ';
    if (uni >= 0xFF01 && uni <= 0xFF5E) /* ，！？：；（）等全角 ASCII */
        return uni - 0xFF01 + 0x21;
    if (uni == 0x3000) /* 全角空格 */
        return ' ';

    switch (uni)
    {
    case 0x3002: return '.';           /* 。 */
    case 0x3001: return ',';           /* 、 */
    case 0x300A: return '<';           /* 《 */
    case 0x300B: return '>';           /* 》 */
    case 0x3008: return '<';           /* 〈 */
    case 0x3009: return '>';           /* 〉 */
    case 0x300C: case 0x300D:          /* 「」 */
    case 0x201C: case 0x201D: return '"';  /* “” */
    case 0x2018: case 0x2019: return '\''; /* ‘’ */
    case 0x3010: return '[';           /* 【 */
    case 0x3011: return ']';           /* 】 */
    case 0x2013: case 0x2014: case 0x2015:
    case 0x2500: return '-';           /* –—─ */
    case 0x2026: return '~';           /* … */
    case 0x00B7: case 0x2022: return '.'; /* ·• */
    default: return uni;
    }
}

/******************************************************************************
      函数说明：把 Unicode 字符写成 UTF-8 追加到页文本缓冲，返回新的写入位置
******************************************************************************/
static uint16_t ebk_put_utf8(uint16_t n, uint32_t uni)
{
    if (uni < 0x80)
    {
        s_page_text[n++] = (char)uni;
    }
    else if (uni < 0x800)
    {
        s_page_text[n++] = (char)(0xC0 | (uni >> 6));
        s_page_text[n++] = (char)(0x80 | (uni & 0x3F));
    }
    else if (uni < 0x10000)
    {
        s_page_text[n++] = (char)(0xE0 | (uni >> 12));
        s_page_text[n++] = (char)(0x80 | ((uni >> 6) & 0x3F));
        s_page_text[n++] = (char)(0x80 | (uni & 0x3F));
    }
    else
    {
        s_page_text[n++] = (char)(0xF0 | (uni >> 18));
        s_page_text[n++] = (char)(0x80 | ((uni >> 12) & 0x3F));
        s_page_text[n++] = (char)(0x80 | ((uni >> 6) & 0x3F));
        s_page_text[n++] = (char)(0x80 | (uni & 0x3F));
    }
    return n;
}

/******************************************************************************
      函数说明：从文件中解码一个字符
      入口数据：off 为字符在文件中的偏移
      出口数据：uni 为 Unicode 码点
      返 回 值：字符占用的字节数，0 表示该位置不是合法字符
      备    注：按 s_utf8 选择 UTF-8 解码或 GBK 解码（GBK → Unicode 用 FatFs
               的 936 码页表，不需要自己维护转换表）
******************************************************************************/
static uint16_t ebk_decode(uint32_t off, uint32_t *uni)
{
    uint8_t c, c2, c3, c4;
    uint32_t cp;

    if (!ebk_read_byte(off, &c))
        return 0;

    if (c < 0x80) /* ASCII 两种编码一致 */
    {
        *uni = c;
        return 1;
    }

    if (s_utf8)
    {
        if ((c & 0xE0) == 0xC0)
        {
            if (!ebk_read_byte(off + 1, &c2) || (c2 & 0xC0) != 0x80)
                return 0;
            *uni = ((uint32_t)(c & 0x1F) << 6) | (c2 & 0x3F);
            return 2;
        }
        if ((c & 0xF0) == 0xE0)
        {
            if (!ebk_read_byte(off + 1, &c2) || !ebk_read_byte(off + 2, &c3))
                return 0;
            if ((c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80)
                return 0;
            *uni = ((uint32_t)(c & 0x0F) << 12) | ((uint32_t)(c2 & 0x3F) << 6) | (c3 & 0x3F);
            return 3;
        }
        if ((c & 0xF8) == 0xF0) /* 4 字节字符（emoji 等）：字库里没有，显示 '?' */
        {
            if (!ebk_read_byte(off + 1, &c2) || !ebk_read_byte(off + 2, &c3) ||
                !ebk_read_byte(off + 3, &c4))
                return 0;
            if ((c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80 || (c4 & 0xC0) != 0x80)
                return 0;
            *uni = '?';
            return 4;
        }
        return 0;
    }

    /* GBK：双字节 */
    if (c < 0x81 || c > 0xFE)
        return 0;
    if (!ebk_read_byte(off + 1, &c2))
        return 0;
    if (c2 < 0x40 || c2 == 0x7F || c2 > 0xFE)
        return 0;

    cp = (uint32_t)ff_oem2uni((WCHAR)(((uint16_t)c << 8) | c2), 936);
    *uni = (cp != 0) ? cp : '?';
    return 2;
}


/******************************************************************************
      函数说明：判断文件名是否为 .txt（大小写不敏感）
******************************************************************************/
static uint8_t ebk_name_is_txt(const char *name)
{
    size_t len = strlen(name);
    const char *ext;

    if (len < 4)
        return 0;
    ext = name + len - 4;
    return (ext[0] == '.') &&
           (ext[1] == 't' || ext[1] == 'T') &&
           (ext[2] == 'x' || ext[2] == 'X') &&
           (ext[3] == 't' || ext[3] == 'T');
}

/******************************************************************************
      函数说明：排版一页（同时生成该页可直接显示的文本）
      入口数据：start 为这一页在文件中的起始偏移
      出口数据：next_off 为下一页的起始偏移
      备    注：\r 忽略，\n 强制换行；行满或行数满时换行/换页；
               页首连续空行跳过，避免整页空白
******************************************************************************/
static void ebk_layout_page(uint32_t start, uint32_t *next_off)
{
    uint32_t off = start;
    uint16_t line = 0;  /* 已用行号（0 起） */
    uint16_t cells = 0; /* 当前行已占半格数 */
    uint16_t n = 0;     /* s_page_text 已写字节数 */
    uint8_t c;
    uint16_t nbytes, need;
    uint32_t uni;

    s_page_text[0] = '\0';

    /* 页首空行跳过（章节分隔处常见连续换行） */
    while (off < s_file_size && ebk_read_byte(off, &c))
    {
        if (c == '\r' || c == '\n')
        {
            off++;
            continue;
        }
        break;
    }

    while (off < s_file_size && n + 8 < EBK_TEXT_MAX)
    {
        if (!ebk_read_byte(off, &c))
            break;

        if (c == '\r') /* 忽略回车 */
        {
            off++;
            continue;
        }

        if (c == '\n') /* 强制换行 */
        {
            off++;
            if (line + 1 >= EBK_ROWS)
                break; /* 本页已满 */
            line++;
            cells = 0;
            s_page_text[n++] = '\n';
            continue;
        }

        /* 解码字符（UTF-8 / GBK 自动区分），字库缺失的标点映射成 ASCII */
        nbytes = ebk_decode(off, &uni);
        if (nbytes == 0) /* 非法字节：跳过 */
        {
            off++;
            continue;
        }
        uni = ebk_map_punct(uni);
        need = (uni < 0x80) ? 1 : 2; /* ASCII 占 1 格，汉字占 2 格 */

        if (cells + need > EBK_LINE_CELLS) /* 行满，自动换行 */
        {
            if (line + 1 >= EBK_ROWS)
                break; /* 本页已满 */
            line++;
            cells = 0;
            s_page_text[n++] = '\n';
        }

        n = ebk_put_utf8(n, uni);

        cells += need;
        off += nbytes;
    }

    s_page_text[n] = '\0';
    *next_off = off;
}

/******************************************************************************
      函数说明：全书扫描一次，建立页起始偏移表（用于总页数与进度）
******************************************************************************/
static void ebk_build_pages(void)
{
    uint32_t off = s_body_start;

    s_page_count = 0;
    s_page_index = 0;
    s_page_truncated = 0;
    s_page_text[0] = '\0';

    if (off >= s_file_size) /* 空文件：给一页空页，避免除零 */
    {
        s_page_off[0] = off;
        s_page_count = 1;
        return;
    }

    s_page_off[0] = off;
    s_page_count = 1;

    while (off < s_file_size)
    {
        uint32_t next = off;

        ebk_layout_page(off, &next);
        if (next == off) /* 没有前进，防止死循环 */
            break;

        off = next;
        if (off >= s_file_size)
            break; /* 正好读完 */

        if (s_page_count >= EBK_MAX_PAGES)
        {
            s_page_truncated = 1; /* 页数达到上限，后面不再细分 */
            break;
        }
        s_page_off[s_page_count++] = off;
    }
}

/******************************************************************************
      函数说明：定位到某一页并生成该页文本
******************************************************************************/
static void ebk_show_page(uint16_t index)
{
    uint32_t next;

    if (s_page_count == 0 || index >= s_page_count)
    {
        s_page_index = 0;
        s_page_text[0] = '\0';
        return;
    }

    s_page_index = index;
    ebk_layout_page(s_page_off[index], &next);
}

/******************************************************************************
      函数说明：按当前 s_utf8 分页并取出第 0 页
      返 回 值：1 表示第 0 页解出了可见字符，0 表示整页空白（编码可能判错）
      备    注：BOM、空行、错误编码排出来的页面通常全是空白字符，
               只要有一个 >' ' 的字节（ASCII 可见字符或 UTF-8 汉字首字节）就算有内容
******************************************************************************/
static uint8_t ebk_parse(void)
{
    uint8_t c0, c1, c2;
    uint16_t n;

    /* 只有 UTF-8 才有 BOM，GBK 文件开头的 0xEF 0xBB 0xBF 是普通汉字 */
    s_body_start = 0;
    if (s_utf8 && ebk_read_byte(0, &c0) && ebk_read_byte(1, &c1) &&
        ebk_read_byte(2, &c2) && c0 == 0xEF && c1 == 0xBB && c2 == 0xBF)
        s_body_start = 3;

    ebk_build_pages();
    ebk_show_page(0);

    for (n = 0; s_page_text[n] != '\0'; n++)
    {
        if ((uint8_t)s_page_text[n] > ' ')
            return 1;
    }
    return 0;
}

/******************************************************************************
      函数说明：关闭当前文件
******************************************************************************/
static void ebk_close_file(void)
{
    if (s_file_open)
    {
        f_close(&s_file);
        s_file_open = 0;
    }
}

/******************************** 对外接口 ********************************/

uint16_t ebk_scan_dir(void)
{
    DIR dir;
    FILINFO fno;
    FRESULT res;

    s_book_count = 0;
    s_book_index = 0;

    res = f_opendir(&dir, EBK_DIR);
    if (res != FR_OK)
        return 0;

    while (s_book_count < EBK_MAX_BOOKS)
    {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == '\0')
            break;

        if (fno.fattrib & AM_DIR) /* 目录跳过 */
            continue;

        if (!ebk_name_is_txt(fno.fname))
            continue;

        if (snprintf(s_book_path[s_book_count], EBK_PATH_MAX, "%s/%s",
                     EBK_DIR, fno.fname) >= EBK_PATH_MAX)
            continue; /* 路径太长放不下，跳过 */

        s_book_count++;
    }

    f_closedir(&dir);
    return s_book_count;
}

uint16_t ebk_book_count(void)
{
    return s_book_count;
}

uint16_t ebk_book_index(void)
{
    return s_book_index;
}

const char *ebk_book_title(uint16_t index)
{
    const char *slash;

    if (index >= s_book_count)
        return "";

    slash = strrchr(s_book_path[index], '/');
    return slash ? slash + 1 : s_book_path[index];
}

const char *ebk_book_name(void)
{
    if (s_book_count == 0)
        return "";

    return ebk_book_title(s_book_index);
}

/******************************************************************************
      函数说明：清空当前书的解析状态（打开新书前调用）
******************************************************************************/
static void ebk_reset_pages(void)
{
    s_file_size = 0;
    s_body_start = 0;
    s_page_count = 0;
    s_page_index = 0;
    s_page_truncated = 0;
    s_page_text[0] = '\0';
    ebk_reader_reset();
}

uint8_t ebk_open_book(uint16_t index)
{
    uint8_t r = 1;
    uint8_t attempt;
    FRESULT fres;

    if (index >= s_book_count)
        return 1;

    ebk_close_file();

    /* SD 卡偶发读失败（大文件更容易碰到）时重试一次 */
    for (attempt = 0; attempt < 2; attempt++)
    {
        s_last_fresult = 0;
        ebk_reset_pages();

        fres = f_open(&s_file, s_book_path[index], FA_READ);
        if (fres != FR_OK)
        {
            r = EBK_ERR_OPEN;
            s_last_fresult = (uint8_t)fres; /* 记录原始错误码，便于界面排查 */
            continue;
        }

        s_file_open = 1;
        s_file_size = (uint32_t)f_size(&s_file);

        /* 自动识别编码：中文 txt 多半是 GBK(ANSI)，而 LVGL 只认 UTF-8 */
        s_utf8 = ebk_detect_utf8();

        if (ebk_parse() != 0 || s_file_size == 0)
        {
            r = EBK_OK;
        }
        else
        {
            /* 第 0 页一个字都没解出来 → 编码判错，换另一种编码再解析一次 */
            s_utf8 = (uint8_t)(s_utf8 ^ 1u);
            r = (ebk_parse() != 0 || s_file_size == 0) ? EBK_OK : EBK_ERR_DECODE;
        }

        if (r == EBK_OK)
            break;

        ebk_close_file(); /* 解析不出来：关掉重开一次，可能是读卡偶发失败 */
    }

    /* 成功与否都切到这本：下滑手势始终前进一本，书名/页码不会停在旧书，
       打不开的那本由界面提示原因，不会从书籍循环里“消失” */
    s_book_index = index;
    s_last_error = r;
    return r;
}

uint8_t ebk_next_book(void)
{
    uint16_t next;

    if (s_book_count < 2)
        return 0; /* 只有一本，不用重开也不会原地打转 */

    next = (uint16_t)((s_book_index + 1) % s_book_count); /* 最后一本之后回绕 */
    ebk_open_book(next);
    return 1; /* 无论如何都刷新界面：打不开的书会显示原因，手势绝不卡住 */
}

void ebk_close_view(void)
{
    ebk_close_file();

    s_book_count = 0;
    s_book_index = 0;
    s_last_error = 0;
    s_last_fresult = 0;
    s_file_size = 0;
    s_body_start = 0;
    s_utf8 = 1;
    s_page_count = 0;
    s_page_index = 0;
    s_page_truncated = 0;
    s_page_text[0] = '\0';
    ebk_reader_reset();
}

uint8_t ebk_next_page(void)
{
    if (s_page_count == 0 || s_page_index + 1 >= s_page_count)
        return 0;
    ebk_show_page((uint16_t)(s_page_index + 1));
    return 1;
}

uint8_t ebk_prev_page(void)
{
    if (s_page_count == 0 || s_page_index == 0)
        return 0;
    ebk_show_page((uint16_t)(s_page_index - 1));
    return 1;
}

uint16_t ebk_page_index(void)
{
    return s_page_index;
}

uint16_t ebk_page_count(void)
{
    return (s_page_count == 0) ? 1 : s_page_count;
}

uint8_t ebk_progress(void)
{
    if (s_page_count == 0)
        return 0;
    return (uint8_t)(((uint32_t)(s_page_index + 1) * 100u) / s_page_count);
}

uint8_t ebk_truncated(void)
{
    return s_page_truncated;
}

uint8_t ebk_error(void)
{
    return s_last_error;
}

/* f_open 失败时 FatFs 的原始 FRESULT（0 表示没有发生过打开失败） */
uint8_t ebk_fresult(void)
{
    return s_last_fresult;
}

const char *ebk_page_text(void)
{
    return s_page_text;
}
