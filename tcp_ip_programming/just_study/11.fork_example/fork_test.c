#include<stdio.h>
#include<unistd.h>

int main()
{
    int pid;
    int a = 10;
    int b = 20;

    a = a + 1;

    pid = fork();
    if(pid > 0) /* 부모 프로세스 */
    {
        a = a+1;
        printf("PARENT");
    }else if(pid == 0) /* 자식 프로세스. 자식 프로세스는 pid값이 0이다. */
    {
        b = b + 1;
        printf("CHILD");
    }
    printf("[%d] a = %d / b = %d\n", getpid(), a, b);
}