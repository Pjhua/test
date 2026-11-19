/*用递归思想扫描指定目录中的特定文件，并计数求出总文件大小。对扫描得到的结果，进行存储到链表中。*/
#include "scanf_dir.h"
#include "dbg_log.h"
#include "bmp.h"
#include "malloc.h"
FileNode *list_head = NULL;


 FileNode *list_tail = NULL; // 链表尾节点(向前翻页到头时用它绕回)
 u16 list_node_num = 0;      // 链表节点总数(目录+文件)
 u16 list_file_num = 0;      // 链表中的文件节点个数
 
// 创建新节点
FileNode *create_node(const char *path, uint32_t size, uint8_t is_dir)
{
    FileNode *node = (FileNode *)mymalloc(sizeof(FileNode));
    if (node == NULL)
    {
        DBG_LOG("内存分配失败\n");
        return NULL;
    }
    // 复制路径（确保不超过缓冲区大小）
    strncpy(node->full_path, path, sizeof(node->full_path) - 1);
    node->full_path[sizeof(node->full_path) - 1] = '\0';
    node->size = size;
    node->is_dir = is_dir;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

// 向链表尾部插入节点
void append_node(FileNode **head, FileNode *new_node)
{
    if (*head == NULL)
    {
        *head = new_node; // 链表为空时，新节点作为头节点
        return;
    }
    // 遍历到链表尾部
    FileNode *cur = *head;
    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    // 插入新节点
    cur->next = new_node;
    new_node->prev = cur;
}

// 释放链表所有节点
void free_list(FileNode **head)
{
    FileNode *cur = *head;
    while (cur != NULL)
    {
        FileNode *temp = cur;
        cur = cur->next;
        myfree(temp);
    }
    *head = NULL; // 清空头指针
}

// 递归读取目录并将文件/目录信息存入双向链表
FRESULT recursive_read_dir(const TCHAR *dir_path, FileNode **list_head)
{
    DIR dir;
    FILINFO fno;
    FRESULT res;
    char sub_path[256]; // 用于拼接子目录/文件的完整路径

    // 打开当前目录
    res = f_opendir(&dir, dir_path);
    if (res != FR_OK)
    {
        DBG_LOG("打开目录失败: %s, 错误码: %d\n", dir_path, res);
        return res;
    }
    // 遍历目录条目
    while (1)
    {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0)
        {
            break; // 错误或读取完毕
        }
        // 跳过 "." 和 ".."
        if (fno.fname[0] == '.' && (fno.fname[1] == 0 || (fno.fname[1] == '.' && fno.fname[2] == 0)))
        {
            continue;
        }

        // 拼接完整路径（目录路径 + 文件名）
        if (dir_path[strlen(dir_path) - 1] == '/')
        {
            // 路径以"/"结尾（如"0:/docs/"），直接拼接文件名
            snprintf(sub_path, sizeof(sub_path), "%s%s", dir_path, fno.fname);
        }
        else
        {
            // 路径不以"/"结尾（如"0:/docs"），加"/"后拼接
            snprintf(sub_path, sizeof(sub_path), "%s/%s", dir_path, fno.fname);
        }

        // 判断是否为目录
        if (fno.fattrib & AM_DIR)
        {
            // 是目录：创建节点存入链表，并递归读取该目录
            FileNode *dir_node = create_node(sub_path, 0, 1); // 目录大小为0
            if (dir_node != NULL)
            {
                append_node(list_head, dir_node);
            }
            // 递归处理子目录
            recursive_read_dir(sub_path, list_head);
        }
        else
        {
            // 是文件：创建节点存入链表（记录文件大小）
            FileNode *file_node = create_node(sub_path, fno.fsize, 0);
            if (file_node != NULL)
            {
                append_node(list_head, file_node);
            }
        }
    }

    // 关闭目录
    f_closedir(&dir);
    return FR_OK;
}

// 打印链表内容
void print_list(FileNode *head)
{
    FileNode *cur = head;
    if (cur == NULL)
    {
        DBG_LOG("链表为空\n");
        return;
    }
    DBG_LOG("\n文件/目录列表（完整路径）：\n");
    while (cur != NULL)
    {
        if (cur->is_dir)
        {
            DBG_LOG("[目录] %s\n", cur->full_path);
        }
        else
        {
            DBG_LOG("[文件] %s （大小：%lu字节）\n", cur->full_path, cur->size);
        }
        cur = cur->next;
    }
}

int scanf_dir_test(void)
{
    FATFS fs;
    FRESULT res;

    // 挂载文件系统（"0:"为驱动器号，根据硬件修改）
    res = f_mount(&fs, "0:", 1);
    if (res != FR_OK)
    {
        DBG_LOG("挂载文件系统失败，错误码: %d\n", res);
        return 1;
    }

    // 递归读取根目录（可替换为其他目录，如"0:/docs"）
    DBG_LOG("开始递归读取目录: 0:/ICOS\n");
    res = recursive_read_dir("0:/ICOS", &list_head);
    if (res != FR_OK)
    {
        DBG_LOG("递归读取失败，错误码: %d\n", res);
        f_mount(NULL, "0:", 1);
        return 1;
    }
    // 打印链表内容
    print_list(list_head);
    // 释放资源
    free_list(&list_head);
    f_mount(NULL, "0:", 1); // 卸载文件系统
    return 0;
}





// 统计链表：节点总数、文件个数、尾节点
 void list_count(FileNode *head)
{
    FileNode *p;
    list_node_num = 0;
    list_file_num = 0;
    list_tail = NULL;
    for (p = head; p != NULL; p = p->next)
    {
        list_node_num++;
        if (!p->is_dir)
            list_file_num++;
        list_tail = p;
    }
}

// 从head开始找第一个文件节点(跳过目录节点)
 FileNode *list_first_file(FileNode *head)
{
    FileNode *p;
    for (p = head; p != NULL; p = p->next)
    {
        if (!p->is_dir)
            return p;
    }
    return NULL;
}

// 向后(dir=1)/向前(dir=-1)找下一个文件节点(跳过目录节点)，到链表两端时绕回另一端
 FileNode *list_next_file(FileNode *node, int dir)
{
    FileNode *p = node;
    u16 i;
    for (i = 0; i < list_node_num; i++)
    {
        p = (dir > 0) ? p->next : p->prev;
        if (p == NULL) // 已到链表尾/头，绕回另一端继续找
            p = (dir > 0) ? list_head : list_tail;
        if (p == NULL)
            break;
        if (!p->is_dir)
            return p; // 找到文件节点
    }
    return node; // 没有其它文件节点时保持当前图片
}

// 从链表中摘除并释放一个节点(文件删除后调用)，同时维护头尾指针和计数
 void list_remove_node(FileNode *node)
{
    if (node == NULL)
        return;
    if (node->prev != NULL) // 不是头节点：前驱直接指向后继
        node->prev->next = node->next;
    else
        list_head = node->next;
    if (node->next != NULL) // 不是尾节点：后继直接指向前驱
        node->next->prev = node->prev;
    else
        list_tail = node->prev;

    free(node);
    if (list_node_num > 0)
        list_node_num--;
    if (list_file_num > 0)
        list_file_num--;
}
