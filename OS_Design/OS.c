/*
22. 段页式存储管理系统：建一个基本段页存储管理系统模型。（1 人，难度：4）
    首先分配一片较大的内存空间，作为程序运行的可用存储空间；
    建立应用程序的模型，包括分段结构在内；
    建立进程的基本数据结构及相应算法
    建立管理存储空间的基本存储结构。
    建立管理段页的基本数据结构与算法。
    设计存储空间的分配与回收算法；
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>

typedef struct Page // 页结构
{
    int ID;      // 页号
    char in_out; // 是否在内存
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

typedef struct Memr // 内存块信息
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
    struct Memr *next;
} *memory, Memr;

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

int Flash;             // 内存大小
int BLOCK;             // 页框大小
int remained;          // 剩余的内存
pcb process;           // 申明一个进程链表
int total_process = 0; // 进程总数
memory memory_info;    // 申明一个记录内存信息的链表

// 创建进程
void myCreateProcess()
{
    PCB *p, *s;
    Seg *m, *n;
    Pa *r, *t;
    p = process;
    while (p->next != NULL)
    {
        p = p->next;
    }
    int i, j, k;
    int now_process;

    printf("请输入进程总数:");
    scanf("%d", &now_process);
    total_process += now_process;
    for (i = 0; i < now_process; i++)
    {
        s = (PCB *)malloc(sizeof(PCB));
        printf("\n请输入第%d个进程名:", i + 1);
        scanf("%s", s->name);
        s->ID = total_process - now_process + i;
        printf("\n请输入该进程的段总数:");
        scanf("%d", &s->total_segmet);
        InitSegment(&s->segm);
        n = s->segm;
        for (j = 0; j < s->total_segmet; j++)
        {
            m = (Seg *)malloc(sizeof(Seg));
            printf("\n请输入第%d个段的段大小: ", j + 1);
            scanf("%d", &m->size);
            m->ID = j;
            m->in_out = 'N';
            InitPage(&m->page);
            t = m->page;
            if (m->size % BLOCK == 0)
            {
                // printf("%d",1);
                for (k = 0; k < m->size / BLOCK; k++)
                {
                    r = (Pa *)malloc(sizeof(Pa));
                    r->block_num = -1;
                    r->ID = k;
                    r->in_out = 'N';
                    r->time = 0;
                    r->pageOffset = 0;
                    r->Process_ID = i;
                    r->Segment_ID = j;
                    strcpy(r->name, s->name);
                    t->next = r;
                    t = t->next;
                    t->next = NULL;
                }
            }
            else
            {
                // printf("%d",0);
                for (k = 0; k < (m->size / BLOCK) + 1; k++)
                {
                    r = (Pa *)malloc(sizeof(Pa));
                    r->block_num = -1;
                    r->ID = k;
                    r->in_out = 'N';
                    r->time = 0;
                    r->pageOffset = 0;
                    r->Process_ID = i;
                    r->Segment_ID = j;
                    strcpy(r->name, s->name);
                    t->next = r;
                    t = t->next;
                    t->next = NULL;
                }
            }

            n->next = m;
            n = n->next;
            n->next = NULL;
        }
        p->next = s;
        p = p->next;
        p->next = NULL;
    }
}
// 初始化
void InitPage(PaNode *p)
{
    *p = (PaNode)malloc(sizeof(Pa));
    (*p)->next = NULL;
}

void InitSegment(SegNode *s)
{
    *s = (SegNode)malloc(sizeof(Seg));
    (*s)->next = NULL;
}

void InitProcess(pcb *pro)
{
    *pro = (pcb)malloc(sizeof(PCB));
    (*pro)->next = NULL;
}

void InitMemory(memory *m)
{
    *m = (memory)malloc(sizeof(Memr));
    (*m)->allocated = 'W';
    (*m)->block_size = 0;
    (*m)->ID = -1;
    (*m)->next = NULL;
    (*m)->Process_ID = -1;
}

// 初始化内存
void CreateMemory()
{
    InitMemory(&memory_info);
    Memr *m, *s;
    s = memory_info;
    int i;
    printf("请输入内存大小:");
    scanf("%d", &Flash);
    remained = Flash;
    printf("\n请输入页框大小:");
    scanf("%d", &BLOCK);
    for (i = 0; i < Flash / BLOCK; i++)
    {
        m = (Memr *)malloc(sizeof(Memr));
        m->ID = i;
        m->allocated = 'N';
        m->Process_ID = -1;
        m->block_size = BLOCK;
        m->time_in_memory = 0;
        m->recently = 0;
        s->next = m;
        s = s->next;
        s->next = NULL;
    }
}

// 为进程分配内存
void Apply()
{
    PCB *p;
    Seg *m = NULL, *n;
    Pa *t;
    Memr *mer;
    mer = memory_info->next;
    int max;
    int j;
    int k;
    int sum = 0; // 记录进程的大小
    int flag = 1;
    int ID;      // 进程ID
    while (flag) // 判断进程ID是否合法
    {
        printf("请输入进程ID:");
        scanf("%d", &ID);
        if (ID >= total_process)
        {
            printf("\n输入不合法!请重新输入!\n");
            flag = 1;
        }
        else
            flag = 0;
    }
    p = process->next;
    while (p->ID != ID)
        p = p->next;
    m = p->segm;
    // printf("%d\n",0);
    while ((m = m->next) != NULL)
    {
        if (m->size % BLOCK == 0)
        {
            sum += m->size;
        }
        else
        {
            sum += ((m->size / BLOCK) + 1) * BLOCK;
        }
    }
    if (sum > remained)
    {
        printf("申请空间过大,请重新申请!\n");
    }

    else // 从内存中选择空闲块给该进程
    {
        n = p->segm->next;
        max = mer->recently;
        while (mer != NULL)
        {
            if (mer->recently > max)
            {
                max = mer->recently;
                mer = mer->next;
            }
            else
                mer = mer->next;
        }
        mer = memory_info->next;
        for (j = 0; j < p->total_segmet; j++)
        {
            n->in_out = 'Y';
            t = n->page->next;

            if (n->size % BLOCK == 0)
            {
                for (k = 0; k < n->size / BLOCK; k++)
                {
                    t->in_out = 'Y';
                    t->time += 1;
                    // 查找memory空间，找到未分配的空间
                    while (mer->allocated != 'N')
                        mer = mer->next;
                    t->block_num = mer->ID;
                    mer->allocated = 'Y';
                    mer->recently = max + 1;
                    mer->Process_ID = p->ID;
                    mer->Segment_ID = n->ID;
                    mer->Page_ID = t->ID;
                    mer->time_in_memory = t->time;
                    strcpy(mer->name, p->name);
                    t->pageOffset = t->ID + t->block_num;
                    if (t->pageOffset >= Flash)
                        t->pageOffset -= Flash;
                    mer = memory_info->next;
                    t = t->next;
                }
            }
            else
            {
                for (k = 0; k < (n->size / BLOCK) + 1; k++)
                {
                    t->in_out = 'Y';
                    t->time += 1;
                    // 查找memory空间，找到未分配的空间
                    while (mer->allocated != 'N')
                        mer = mer->next;
                    t->block_num = mer->ID;
                    mer->allocated = 'Y';
                    mer->recently = max + 1;
                    mer->Process_ID = p->ID;
                    mer->Segment_ID = n->ID;
                    mer->Page_ID = t->ID;
                    mer->time_in_memory = t->time;
                    strcpy(mer->name, p->name);
                    t->pageOffset = t->ID + t->block_num;
                    if (t->pageOffset >= Flash)
                        t->pageOffset -= Flash;
                    mer = memory_info->next;
                    t = t->next;
                }
            }
            n = n->next;
        }
        remained -= sum;
    }
}

// 内存回收进程
void Recycle()
{
    Memr *mer;
    PCB *p;
    Seg *s;
    Pa *q;
    int ID; // 要回收的进程号
    printf("请输入进程号:");
    scanf("%d", &ID);
    // 修改内存信息
    mer = memory_info->next;
    while (mer != NULL)
    {
        if (mer->Process_ID == ID)
        {
            mer->allocated = 'N';
            mer->Page_ID = -1;
            mer->Process_ID = -1;
            mer->Segment_ID = -1;
            mer->recently = 0;
            mer->time_in_memory = 0;
            remained += BLOCK;

            mer = mer->next;
        }
        else
            mer = mer->next;
    }

    // 修改进程信息
    p = process->next;
    while (p != NULL)
    {
        if (p->ID == ID)
        {
            s = p->segm->next;
            while (s != NULL)
            {
                q = s->page->next;
                while (q != NULL)
                {
                    s->in_out = 'N';
                    q->in_out = 'N';
                    q->pageOffset = 0;
                    q->block_num = -1;
                    q->time = 0;
                    q->Process_ID = -1;
                    q->Segment_ID = -1;
                    q = q->next;
                }
                s = s->next;
            }
            break;
        }
        else
            p = p->next;
    }
}

// 打印进程信息
void PrintProcessSegment()
{
    int i, j;
    PCB *pro;
    Seg *p;
    Pa *s;
    pro = process->next;
    printf("请输入要显示的进程号信息:");
    scanf("%d", &j);
    while (pro != NULL)
    {
        if (pro->ID == j)
            break;
        else
            pro = pro->next;
    }
    if (pro == NULL)
        printf("\nERROR\n");
    else
    {
        p = pro->segm->next;
        printf("\n进程信息:\n");
        printf("进程名:%s", pro->name);
        printf("\n进程号:%d\n\n", pro->ID);
        while (p != NULL)
        {
            s = p->page->next;
            printf("段号:%d\t段大小:%d\t是否在内存:%c\n", p->ID, p->size, p->in_out);
            printf("段内页表:\n");
            if (p->size % BLOCK == 0)
            {
                for (i = 0; i < (p->size / BLOCK); i++)
                {
                    printf("页号:%d\t块号:%d\t是否存在内存:%c\n", s->ID, s->block_num, s->in_out);
                    s = s->next;
                }
            }
            else
            {
                for (i = 0; i < (p->size / BLOCK) + 1; i++)
                {
                    printf("页号:%d\t块号:%d\t是否存在内存:%c\n", s->ID, s->block_num, s->in_out);
                    s = s->next;
                }
            }

            printf("\n");
            p = p->next;
        }
    }
    return;
}

// 打印内存分配信息
void PrintMemory()
{
    Memr *m;
    m = memory_info->next;

    printf("内存大小为:%d", Flash);
    printf("\t总共分成了%d块,每块为:%d", Flash / BLOCK, BLOCK);
    printf("\n已分配的内存块情况为:");
    while (m != NULL)
    {
        if (m->allocated == 'Y')
        {
            printf("\n进程名\t进程号\t进程段号\t页号\t块号\n");
            printf("%s\t%d\t%d\t\t%d\t%d\n", m->name, m->Process_ID, m->Segment_ID, m->Page_ID, m->ID);
            m = m->next;
        }
        else
            m = m->next;
    }

    printf("\n共占用%d块内存块,剩余%d块.\n", (Flash - remained) / BLOCK, remained / BLOCK);
}
void AddressExchange()
{
    int ID;       // 进程ID
    int i;        // 段号
    int j;        // 页号
    int address;  // 物理地址
    int flag = 1; // 用来标记输入是否合法
    PCB *p;
    Seg *s;
    Pa *q;
    Memr *mer;
    p = process->next;
    mer = memory_info->next;
    while (flag) // 判断进程ID是否合法
    {
        printf("请输入进程ID:");
        scanf("%d", &ID);
        if (ID >= total_process)
        {
            printf("\n输入不合法!请重新输入!\n");
            flag = 1;
        }
        else
            flag = 0;
    }

    flag = 1;
    while (flag) // 判断段号是否合法
    {
        printf("\n请输入段号:");
        scanf("%d", &i);
        // 找到进程号为ID的进程
        p = process->next;
        while (p->ID != ID)
            p = p->next;
        if (p->total_segmet <= i)
        {
            printf("\n输入不合法!请重新输入!\n");
            flag = 1;
        }
        else
            flag = 0;
    }
    flag = 1;
    while (flag) // 判断页号是否合法
    {
        printf("\n请输入页号:");
        scanf("%d", &j);
        // 找到段
        s = p->segm->next;
        while (s->ID != i)
            s = s->next;
        if (j >= s->size / BLOCK)
        {
            printf("\n输入不合法!请重新输入!\n");
            flag = 1;
        }
        else
            flag = 0;
    }
    // 判断是否缺段
    // 查找进程的段表
    while (p->ID != ID)
        p = p->next;
    s = p->segm->next;
    // 找到段号为i的段
    while (s->ID != i)
        s = s->next;
    if (s->in_out == 'N')
    {

        printf("\n该进程不在内存!");
        Sleep(2000);
    }
    // 判断是否缺页
    else
    {
        // 找到页号为j的页
        q = s->page->next;
        while (q->ID != j)
            q = q->next;
        if (q->in_out == 'N')
        {

            printf("\n该进程不在内存!");
            Sleep(2000);
        }
        else
        {
            q->time += 1;
            // 改变访问内存块的最近时间
            mer = memory_info->next;
            while (mer->Page_ID != j)
            {
                mer = mer->next;
            }
            mer->recently += 1;
            address = q->block_num * BLOCK;
            printf("\n物理地址为:%d", address);
            printf("\n");
        }
    }
}
void test()
{
    int flag = 1;
    int slect;
    InitProcess(&process);
    system("cls");
    while (flag)
    {
        system("cls");
        printf("1.初始化内存\n");
        printf("2.创建进程\n");
        printf("3.进程申请内存\n");
        printf("4.回收进程内存\n");
        printf("5.显示内存信息\n");
        printf("6.显示进程信息\n");
        printf("7.查找页面地址\n");
        printf("8.退出\n");
        printf("请选择:");
        scanf("%d", &slect);
        switch (slect)
        {
        case 1:
            system("cls");
            CreateMemory();
            printf("\n按任意键继续…………\n");
            getchar();
            getchar();
            break;
        case 2:
            system("cls");
            myCreateProcess();
            printf("\n按任意键继续…………\n");
            getchar();
            getchar();
            break;
        case 3:
            system("cls");
            Apply();
            printf("\n按任意键继续…………\n");
            getchar();
            getchar();
            break;
        case 4:
            system("cls");
            Recycle();
            printf("\n按任意键继续…………\n");
            getchar();
            getchar();
            break;
        case 5:
            system("cls");
            PrintMemory();
            printf("\n按任意键继续…………\n");
            getchar();
            getchar();
            break;
        case 6:
            system("cls");
            PrintProcessSegment();
            printf("\n按任意键继续…………\n");
            getchar();
            getchar();
            break;
        case 7:
            system("cls");
            AddressExchange();
            printf("\n按任意键继续…………\n");
            getchar();
            getchar();
            break;
        case 8:
            flag = 0;
            free(memory_info);
            free(process);
            printf("\n按任意键继续…………\n");
            getchar();
            getchar();
            break;
        default:
            printf("输入错误,请重新选择!\n");
            printf("\n按任意键继续…………\n");
            getchar();
            getchar();
            break;
        }
    }
}

int main()
{
    test();
    return 0;
}
