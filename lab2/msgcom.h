/* msgcom.h */
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MSGKEY 5678
#define BUF_SIZE 512

/*
msgtype: 进程通信消息结构
    mtype: 消息类型
    pid: 发送消息进程PID
    text: 消息正文
*/
struct msgtype
{
    long mtype;
    int pid;
    char text[BUF_SIZE];
};
