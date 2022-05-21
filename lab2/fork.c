/*
Part1: 子进程实验
合作功能: 1.父进程在test.txt中写入a、b，并发送信号给子进程;
        2.信号处理程序输出test.txt中写入的a、b;
        3.子进程读取a、b，计算a+b并输出
基本实现：1.创建子进程; 2.进程睡眠; 3.进程图像改换; 
        4.父进程等待子进程终止; 5.子进程终止;
        6.信号的设置与传送
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

/*
信号处理程序
*/
void func(){
    int a, b;
    printf("It is signal processing function. PID=%d\n", getpid());
    FILE *fp = fopen("./test.txt", "r");
    fscanf(fp, "%d %d", &a, &b);
    fclose(fp);
    printf("Signals received: %d,%d. PID=%d\n", a, b, getpid());
}

int main(){
    int i, pid, status=1;
    void func();
    signal(SIGUSR1, func);  //预置信号处理程序

    printf("[Parent] Before fork call.\n");
    while((i=fork()) == -1);  //创建子进程

    if (i)  //父进程
    {
        printf("[Parent] It is the parent process: PID=%d\n", getpid());
        FILE *fp = fopen("./test.txt", "w");
        fprintf(fp, "1 2");
        fclose(fp);

        kill(i, SIGUSR1);
        pid = wait(&status);  //父进程等待子进程终止
        printf("[Parent] Child process: PID=%d, status=%d\n", pid, status);
    } else {  //子进程
        sleep(10);  //进程睡眠，等待接受信号
        printf("[Child] It is the child process: PID=%d\n", getpid());
        execl("./child", "", (char*)0);  //进程图像改换
        printf("[Child] execl error.\n");
        exit(2);  //子进程终止
    }
    printf("[Parent] Parent process finish.\n");

    return 0;
}