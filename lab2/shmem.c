/*
共享内存机制
父进程和子进程相互通信
*/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define SHMKEY 18001    //共享内存关键字
#define SIZE 1024       //共享内存长度
#define SEMKEY1 19001   //信号量组1关键字
#define SEMKEY2 19002   //信号量组2关键字

int creatsem(key_t key){
    int sid;
    union semun
    {
        int val;
        struct semid_ds *buf;
        ushort *array;
    } arg;
    if((sid=semget(key, 1, 0666|IPC_CREAT))==-1)
        perror("semget");

    arg.val = 1;
    if(semctl(sid, 0, SETVAL, arg)==-1)
        perror("semctl");
        
    return sid;
}

static void semcall(int sid, int op){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = op;
    sb.sem_flg = 0;
    if(semop(sid, &sb, 1)==-1)
        perror("semop");
}

void semWait(int sid){
    semcall(sid, -1);
}

void semSignal(int sid){
    semcall(sid, 1);
}

int main(){  
    char *segaddr;
    int segid, sid1, sid2, pid;

    //创建共享内存段
    if((segid=shmget(SHMKEY, SIZE, IPC_CREAT|0666))==-1)
        perror("shmget");
    segaddr = shmat(segid, 0, 0);    //将共享内存映射到进程数据空间
    //创建两个信号量，初值为1
    sid1 = creatsem(SEMKEY1);
    sid2 = creatsem(SEMKEY2);
    semWait(sid2);                   //置信号量2值为0,表示缓冲区空

    if (!(pid=fork())){
        //子进程，接受和输出   
        while(1){
            semWait(sid2);
            printf("[Child] Received from Parent: %s\n", segaddr);
            semSignal(sid1);
        }
    }else{
        //父进程，输入和存储
        FILE *fp = fopen("send.txt", "r");
        char buf[SIZE];
        while (fgets(buf, SIZE, fp))
        {
            int len = strlen(buf);
            buf[len-1] = 0;
            semWait(sid1);
            strcpy(segaddr, buf);
            printf("[Parent] Read from file: %s\n", segaddr);
            //scanf("%s", segaddr);
            semSignal(sid2); 
        }
        fclose(fp);        
    }

    return 0;
}
