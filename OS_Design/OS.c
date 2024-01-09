/*
22. ��ҳʽ�洢����ϵͳ����һ��������ҳ�洢����ϵͳģ�͡���1 �ˣ��Ѷȣ�4��
    ���ȷ���һƬ�ϴ���ڴ�ռ䣬��Ϊ�������еĿ��ô洢�ռ䣻
    ����Ӧ�ó����ģ�ͣ������ֶνṹ���ڣ�
    �������̵Ļ������ݽṹ����Ӧ�㷨
    ��������洢�ռ�Ļ����洢�ṹ��
    ���������ҳ�Ļ������ݽṹ���㷨��
    ��ƴ洢�ռ�ķ���������㷨��
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>

typedef struct Page // ҳ�ṹ
{
    int ID;      // ҳ��
    char in_out; // �Ƿ����ڴ�
    int pageOffset;
    int block_num;  // ��Ӧ���
    int time;       // ���ڴ��ʱ��
    int Process_ID; // ��������ID
    char name[10];  // ����������
    int Segment_ID; // �������̶κ�
    struct Page *next;
} Pa, *PaNode;

typedef struct Segment // �νṹ
{
    int ID;      // �κ�
    Pa *page;  // ���е�ҳ�ṹ
    int size;    // �δ�С
    char in_out; // �Ƿ����ڴ���
    struct Segment *next;
} Seg, *SegNode;

typedef struct PCB // ������Ϣ
{
    Seg *segm;    // ���̶���Ϣ
    int total_segmet; // ������
    int ID;           // ����ID
    char name[10];    // ������
    struct PCB *next;
} PCB, *pcb;

typedef struct Memr // �ڴ����Ϣ
{
    int ID;             // ҳ���
    char allocated;     // ����Ƿ��ѷ���
    int block_size;     // ҳ���С
    int Process_ID;     // ռ�ý���ID
    char name[10];      // ռ�ý�����
    int Segment_ID;     // ռ�ý��̶κ�
    int Page_ID;        // ռ�ý��̵�ҳ��
    int time_in_memory; // ҳ���ڴ��е�ʱ��
    int recently;       // ������ʵ�ʱ��
    struct Memr *next;
} *memory, Memr;

// ��������
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

int Flash;             // �ڴ��С
int BLOCK;             // ҳ���С
int remained;          // ʣ����ڴ�
pcb process;           // ����һ����������
int total_process = 0; // ��������
memory memory_info;    // ����һ����¼�ڴ���Ϣ������

// ��������
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

    printf("�������������:");
    scanf("%d", &now_process);
    total_process += now_process;
    for (i = 0; i < now_process; i++)
    {
        s = (PCB *)malloc(sizeof(PCB));
        printf("\n�������%d��������:", i + 1);
        scanf("%s", s->name);
        s->ID = total_process - now_process + i;
        printf("\n������ý��̵Ķ�����:");
        scanf("%d", &s->total_segmet);
        InitSegment(&s->segm);
        n = s->segm;
        for (j = 0; j < s->total_segmet; j++)
        {
            m = (Seg *)malloc(sizeof(Seg));
            printf("\n�������%d���εĶδ�С: ", j + 1);
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
// ��ʼ��
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

// ��ʼ���ڴ�
void CreateMemory()
{
    InitMemory(&memory_info);
    Memr *m, *s;
    s = memory_info;
    int i;
    printf("�������ڴ��С:");
    scanf("%d", &Flash);
    remained = Flash;
    printf("\n������ҳ���С:");
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

// Ϊ���̷����ڴ�
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
    int sum = 0; // ��¼���̵Ĵ�С
    int flag = 1;
    int ID;      // ����ID
    while (flag) // �жϽ���ID�Ƿ�Ϸ�
    {
        printf("���������ID:");
        scanf("%d", &ID);
        if (ID >= total_process)
        {
            printf("\n���벻�Ϸ�!����������!\n");
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
        printf("����ռ����,����������!\n");
    }

    else // ���ڴ���ѡ����п���ý���
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
                    // ����memory�ռ䣬�ҵ�δ����Ŀռ�
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
                    // ����memory�ռ䣬�ҵ�δ����Ŀռ�
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

// �ڴ���ս���
void Recycle()
{
    Memr *mer;
    PCB *p;
    Seg *s;
    Pa *q;
    int ID; // Ҫ���յĽ��̺�
    printf("��������̺�:");
    scanf("%d", &ID);
    // �޸��ڴ���Ϣ
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

    // �޸Ľ�����Ϣ
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

// ��ӡ������Ϣ
void PrintProcessSegment()
{
    int i, j;
    PCB *pro;
    Seg *p;
    Pa *s;
    pro = process->next;
    printf("������Ҫ��ʾ�Ľ��̺���Ϣ:");
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
        printf("\n������Ϣ:\n");
        printf("������:%s", pro->name);
        printf("\n���̺�:%d\n\n", pro->ID);
        while (p != NULL)
        {
            s = p->page->next;
            printf("�κ�:%d\t�δ�С:%d\t�Ƿ����ڴ�:%c\n", p->ID, p->size, p->in_out);
            printf("����ҳ��:\n");
            if (p->size % BLOCK == 0)
            {
                for (i = 0; i < (p->size / BLOCK); i++)
                {
                    printf("ҳ��:%d\t���:%d\t�Ƿ�����ڴ�:%c\n", s->ID, s->block_num, s->in_out);
                    s = s->next;
                }
            }
            else
            {
                for (i = 0; i < (p->size / BLOCK) + 1; i++)
                {
                    printf("ҳ��:%d\t���:%d\t�Ƿ�����ڴ�:%c\n", s->ID, s->block_num, s->in_out);
                    s = s->next;
                }
            }

            printf("\n");
            p = p->next;
        }
    }
    return;
}

// ��ӡ�ڴ������Ϣ
void PrintMemory()
{
    Memr *m;
    m = memory_info->next;

    printf("�ڴ��СΪ:%d", Flash);
    printf("\t�ܹ��ֳ���%d��,ÿ��Ϊ:%d", Flash / BLOCK, BLOCK);
    printf("\n�ѷ�����ڴ�����Ϊ:");
    while (m != NULL)
    {
        if (m->allocated == 'Y')
        {
            printf("\n������\t���̺�\t���̶κ�\tҳ��\t���\n");
            printf("%s\t%d\t%d\t\t%d\t%d\n", m->name, m->Process_ID, m->Segment_ID, m->Page_ID, m->ID);
            m = m->next;
        }
        else
            m = m->next;
    }

    printf("\n��ռ��%d���ڴ��,ʣ��%d��.\n", (Flash - remained) / BLOCK, remained / BLOCK);
}
void AddressExchange()
{
    int ID;       // ����ID
    int i;        // �κ�
    int j;        // ҳ��
    int address;  // �����ַ
    int flag = 1; // ������������Ƿ�Ϸ�
    PCB *p;
    Seg *s;
    Pa *q;
    Memr *mer;
    p = process->next;
    mer = memory_info->next;
    while (flag) // �жϽ���ID�Ƿ�Ϸ�
    {
        printf("���������ID:");
        scanf("%d", &ID);
        if (ID >= total_process)
        {
            printf("\n���벻�Ϸ�!����������!\n");
            flag = 1;
        }
        else
            flag = 0;
    }

    flag = 1;
    while (flag) // �ж϶κ��Ƿ�Ϸ�
    {
        printf("\n������κ�:");
        scanf("%d", &i);
        // �ҵ����̺�ΪID�Ľ���
        p = process->next;
        while (p->ID != ID)
            p = p->next;
        if (p->total_segmet <= i)
        {
            printf("\n���벻�Ϸ�!����������!\n");
            flag = 1;
        }
        else
            flag = 0;
    }
    flag = 1;
    while (flag) // �ж�ҳ���Ƿ�Ϸ�
    {
        printf("\n������ҳ��:");
        scanf("%d", &j);
        // �ҵ���
        s = p->segm->next;
        while (s->ID != i)
            s = s->next;
        if (j >= s->size / BLOCK)
        {
            printf("\n���벻�Ϸ�!����������!\n");
            flag = 1;
        }
        else
            flag = 0;
    }
    // �ж��Ƿ�ȱ��
    // ���ҽ��̵Ķα�
    while (p->ID != ID)
        p = p->next;
    s = p->segm->next;
    // �ҵ��κ�Ϊi�Ķ�
    while (s->ID != i)
        s = s->next;
    if (s->in_out == 'N')
    {

        printf("\n�ý��̲����ڴ�!");
        Sleep(2000);
    }
    // �ж��Ƿ�ȱҳ
    else
    {
        // �ҵ�ҳ��Ϊj��ҳ
        q = s->page->next;
        while (q->ID != j)
            q = q->next;
        if (q->in_out == 'N')
        {

            printf("\n�ý��̲����ڴ�!");
            Sleep(2000);
        }
        else
        {
            q->time += 1;
            // �ı�����ڴ������ʱ��
            mer = memory_info->next;
            while (mer->Page_ID != j)
            {
                mer = mer->next;
            }
            mer->recently += 1;
            address = q->block_num * BLOCK;
            printf("\n�����ַΪ:%d", address);
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
        printf("1.��ʼ���ڴ�\n");
        printf("2.��������\n");
        printf("3.���������ڴ�\n");
        printf("4.���ս����ڴ�\n");
        printf("5.��ʾ�ڴ���Ϣ\n");
        printf("6.��ʾ������Ϣ\n");
        printf("7.����ҳ���ַ\n");
        printf("8.�˳�\n");
        printf("��ѡ��:");
        scanf("%d", &slect);
        switch (slect)
        {
        case 1:
            system("cls");
            CreateMemory();
            printf("\n�������������������\n");
            getchar();
            getchar();
            break;
        case 2:
            system("cls");
            myCreateProcess();
            printf("\n�������������������\n");
            getchar();
            getchar();
            break;
        case 3:
            system("cls");
            Apply();
            printf("\n�������������������\n");
            getchar();
            getchar();
            break;
        case 4:
            system("cls");
            Recycle();
            printf("\n�������������������\n");
            getchar();
            getchar();
            break;
        case 5:
            system("cls");
            PrintMemory();
            printf("\n�������������������\n");
            getchar();
            getchar();
            break;
        case 6:
            system("cls");
            PrintProcessSegment();
            printf("\n�������������������\n");
            getchar();
            getchar();
            break;
        case 7:
            system("cls");
            AddressExchange();
            printf("\n�������������������\n");
            getchar();
            getchar();
            break;
        case 8:
            flag = 0;
            free(memory_info);
            free(process);
            printf("\n�������������������\n");
            getchar();
            getchar();
            break;
        default:
            printf("�������,������ѡ��!\n");
            printf("\n�������������������\n");
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
