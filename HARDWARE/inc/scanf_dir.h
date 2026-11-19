
#ifndef __LIST__H__
#define __LIST__H__
#include "ff.h"

#include "stm32f4xx.h"
#include "diskio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 双向链表节点：存储文件/目录的完整路径和基本信息
typedef struct FileNode
{
    char full_path[256];   // 完整路径（如"0:/docs/note.txt"）
    uint32_t size;         // 文件大小（目录为0）
    uint8_t is_dir;        // 是否为目录（1:目录，0:文件）
    struct FileNode *prev; // 前驱节点
    struct FileNode *next; // 后继节点
} FileNode;

// 链表头节点（方便管理）
extern FileNode *list_head;

// 链表操作接口
FileNode *create_node(const char *path, uint32_t size, uint8_t is_dir); // 创建新节点
void append_node(FileNode **head, FileNode *new_node);                  // 向链表尾部插入节点
void free_list(FileNode **head);                                        // 释放链表所有节点
FRESULT recursive_read_dir(const TCHAR *dir_path, FileNode **head);     // 递归扫描目录并存入链表
void print_list(FileNode *head);                                        // 打印链表内容

int scanf_dir_test(void);


extern FileNode *list_tail; // 链表尾节点(向前翻页到头时用它绕回)
extern u16 list_node_num ;      // 链表节点总数(目录+文件)
extern u16 list_file_num ;      // 链表中的文件节点个数

 void list_count(FileNode *head);
 FileNode *list_first_file(FileNode *head);
 FileNode *list_next_file(FileNode *node, int dir);
 void list_remove_node(FileNode *node);
#endif
