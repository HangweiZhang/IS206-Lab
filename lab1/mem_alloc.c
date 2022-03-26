#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 1000

struct map
{
    unsigned m_size;                   // 空闲区大小
    char *m_addr;                      // 空闲区起始地址
    struct map *next, *prior;          // 指向前后空闲区的指针
};
struct map *coremap;                   // 空闲存储区表
struct map *pointer;                   // 查找指针

void print_map()
{
    struct map *p;
    p = coremap;
    
    printf("------------------------------------------------\n");
    printf("%16s\t %5s\n", "address", "size");
    do
    {
        printf("%16p\t %5d\n", p->m_addr, p->m_size);
        p = p->next;
    } while (p != coremap);            // 遍历链表
    printf("------------------------------------------------\n");
    
    return;
}

void* lmalloc(unsigned size)
{
    register char *a;
    register struct map *bp;
    bp = pointer;
    do
    {
        if (bp->m_size >= size)
        {
            a = bp->m_addr;
            bp->m_addr += size;
            pointer = bp->next;
            if ((bp->m_size -= size) == 0)
            {
                bp->next->prior = bp->prior;
                bp->prior->next = bp->next;
                if (bp != coremap)  // 考虑coremap可能被删的情况
                {
                    free(bp);
                } else {
                    if (bp->next != coremap){
                        coremap = bp->next;
                        free(bp);
                    }
                }
            }   
            printf("Success: allocate address %p, size %d\n", a, size);
            return a;
        }
        bp = bp->next;
    } while (bp != pointer);
    printf("Can't allocate enough memory.\n");
    return 0;
}

void lfree(unsigned size, char *addr)
{
    struct map *bp;
    
    for(bp = coremap; bp->m_addr <= addr; bp = bp->next);
    if ((bp != coremap)&&(bp->prior->m_addr + bp->prior->m_size == addr))   //情况1,2
    {
        bp->prior->m_size += size;  //情况1

        if (addr + size == bp->m_addr)  //情况2
        {
            bp->prior->m_size += bp->m_size;
            bp->prior->next = bp->next;
            bp->next->prior = bp->prior;

            if (bp == pointer)  // 考虑查找指针是否仍指向空闲表
                pointer = bp->next;

            free(bp);   
        }
        
    } 
    else    
    {
        if (addr + size == bp->m_addr)  //情况3
        {
            bp->m_addr -= size;
            bp->m_size += size;
        }
        else
        {
            struct map *new_map;        //情况4
            new_map = (struct map*)malloc(sizeof(struct map));
            new_map->m_addr = addr;
            new_map->m_size = size;
            new_map->prior = bp->prior;
            new_map->next = bp;
            bp->prior->next = new_map;  //注意此处指针赋值的顺序
            bp->prior = new_map;

            if (coremap->m_addr >= new_map->m_addr){
                coremap = new_map;  //coremap始终指向空闲表地址最低项
                printf("%p\t %p\n", coremap->m_addr, coremap->next->m_addr);
            }
        }   
    }
    printf("Success: free address %p, size %d\n", addr, size);
    return;
}

void initialize(unsigned size, char *addr)
{
    printf("initialize: address start with %p, memory size is %d\n", addr, size);
    coremap = (struct map*)malloc(sizeof(struct map));
    coremap->m_size = size;                                 // 本实验 size = 1000
    coremap->m_addr = addr;                                 // 分配到的内存空间的初始地址
    coremap->prior = coremap;
    coremap->next = coremap;
    pointer = coremap;                                      // 查找指针指向空闲存储区表
}

int main()
{
    char *mem_addr;
    char c;
    unsigned size, addr;

    if ((mem_addr = (char*)malloc(MAX_SIZE)) == NULL)
    {
        printf("Can't initializing!\n");
        exit(1);
    }
    
    initialize(MAX_SIZE, mem_addr);

    printf("Command: 1.allcaote: m [size]; 2.free: f [size] [address]; 3.quit: q\n");

    while (c != 'q')                       // 按'q'退出
    {
        do                                 // 过滤所有输入的空格
        {
            c = getchar();
        } while ((c == '\n')||(c == '\t')||(c == ' '));
        
        switch (c)
        {
        case 'm':                          // 按'm'分配内存
            scanf("%u", &size);
            lmalloc(size);
            print_map();
            break;
        
        case 'f':                          // 按'f'释放内存 
            scanf("%u%u", &size, &addr);
            lfree(size, mem_addr + addr);  // 采用相对地址进行管理
            print_map();
            break;

        default:
            break;
        }
    }

    free(coremap);
    free(mem_addr);
    return 0;
}