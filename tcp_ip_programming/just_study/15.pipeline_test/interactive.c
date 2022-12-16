#include<stdio.h>
#include<unistd.h>

#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    int fds1[2], fds2[2];       /* 파이트 생성을 위한 정수형 배열 선언 */
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";
    char buf[BUF_SIZE];
    pid_t pid;

    pipe(fds1), pipe(fds2);
    pid = fork();

    if(pid == 0)
    {
        /* Child */
        write(fds1[1], str1, sizeof(str1));
        read(fds2[0], buf, BUF_SIZE);
        printf("[Child] output: %s\n", buf);
    }
    else
    {
        /* Parent */
        read(fds1[0], buf, BUF_SIZE);
        printf("[Parent] output: %s\n", buf);
        write(fds2[1], str2, sizeof(str2));

        sleep(3);
    }
    return 0;
}