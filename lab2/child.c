#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    int a, b;
    printf("It is child processing function. PID=%d\n", getpid());
    FILE *fp = fopen("./test.txt", "r");
    fscanf(fp, "%d %d", &a, &b);
    fclose(fp);
    printf("Add signals: %d + %d = %d. PID=%d\n", a, b, (a+b), getpid());
}