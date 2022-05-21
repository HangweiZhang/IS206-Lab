/*
服务器进程： server
    作为接受方将接受的信息写入receive.txt文件
    使用UNIX消息通信机制
*/
#include "msgcom.h"
#include "stdio.h"

int main(){
    struct msgtype buf;
    int qid;

    if ((qid=msgget(MSGKEY, IPC_CREAT|0666))==-1)
        return -1;
    
    while (1)
    {
        msgrcv(qid, &buf, 512, 1, MSG_NOERROR);
        printf("Server received a request from process %d\n", buf.pid);
        buf.mtype = buf.pid;
        msgsnd(qid, &buf, sizeof(buf.text), 0);
    }
    
    return 0;
}
