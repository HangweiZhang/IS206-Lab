/* 
请求进程: client
    作为发送方从文件send.txt中按行读取数据并发送
    使用UNIX消息通信机制
*/
#include "msgcom.h"
#include "stdio.h"
#include "unistd.h"
#include "string.h"
#define MAX_SIZE 512

int main(){
    struct msgtype buf;
    char strbuf[MAX_SIZE];
    int qid, pid, len;
    int cnt = 0;
    FILE *fp = fopen("send.txt", "r");
    FILE *fpp = fopen("receive.txt", "w");

    qid = msgget(MSGKEY, IPC_CREAT|0666);  //生成消息队列
    buf.mtype = 1;

    while (fgets(strbuf, MAX_SIZE, fp))
    {
        len = strlen(strbuf);
        strbuf[len-1] = 0;
        strcpy(buf.text, strbuf);
        buf.pid = pid = getpid();
        msgsnd(qid, &buf, sizeof(buf.text), 0);  //发送消息
        cnt += 1;
    }
    fclose(fp);

    
    while (cnt-- > 0)  //从消息队列接受消息
    {
        msgrcv(qid, &buf, 512, pid, MSG_NOERROR);
        printf("Request received a message from server, type is:%ld\n", buf.mtype);
        printf("Message is '%s'\n", buf.text);
        fputs(buf.text, fpp);
        fputs("\n", fpp);
    }
    fclose(fpp);

    return 0;
}
