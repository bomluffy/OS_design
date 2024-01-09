#include "define.h"
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