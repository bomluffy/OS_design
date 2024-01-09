/*
22. 段页式存储管理系统：建一个基本段页存储管理系统模型。（1 人，难度：4）
    首先分配一片较大的内存空间，作为程序运行的可用存储空间；
    建立应用程序的模型，包括分段结构在内；
    建立进程的基本数据结构及相应算法-09
    建立管理存储空间的基本存储结构。
    建立管理段页的基本数据结构与算法。
    设计存储空间的分配与回收算法；
*/
#ifndef DFINE
#define DFINE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <windows.h>

typedef struct Page // 页结构
{
    int ID;         // 页号
    char in_out;    // 是否在内存
    int pageOffset; 
    int block_num;  // 对应块号
    int time;       // 在内存的时间
    int Process_ID; // 所属进程ID
    char name[10];  // 所属进程名
    int Segment_ID; // 所属进程段号
    struct Page *next;
} Pa, *PaNode;

typedef struct Segment // 段结构
{
    int ID;      // 段号
    Pa *page;  // 段中的页结构
    int size;    // 段大小
    char in_out; // 是否在内存中
    struct Segment *next;
} Seg, *SegNode;

typedef struct PCB // 进程信息
{
    Seg *segm;    // 进程段信息
    int total_segmet; // 段总数
    int ID;           // 进程ID
    char name[10];    // 进程名
    struct PCB *next;
} PCB, *pcb;

typedef struct Memory // 内存块信息
{
    int ID;             // 页框号
    char allocated;     // 标记是否已分配
    int block_size;     // 页框大小
    int Process_ID;     // 占用进程ID
    char name[10];      // 占用进程名
    int Segment_ID;     // 占用进程段号
    int Page_ID;        // 占用进程的页号
    int time_in_memory; // 页在内存中的时间
    int recently;       // 最近访问的时间
    struct Memory *next;
} Memr, *memory;

// 函数申明
void CreateSegment();
void CreateMemory();
void myCreateProcess();
void InitPage(PaNode *p);
void InitSegment(SegNode *s);
void InitProcess(pcb *pro);
void ApplyMemory();
void AddressExchange();
void PrintMemory();
void PrintProcessSegment();
void Apply();
void Recycle();


int Flash;              // 内存大小
int BLOCK;              // 页框大小
int remained;           // 剩余的内存
pcb process;            // 申明一个进程链表
int total_process = 0;  // 进程总数
memory memory_info;     // 申明一个记录内存信息的链表

#endif