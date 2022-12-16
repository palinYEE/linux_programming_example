#include<stdio.h>
#include<unistd.h>

#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    int fds[2];      /* 파이프의 입구, 출구의 파일디스크립터를 저장할 정수형 배열. */
    char str[] = "Who are you?";
    char buf[BUF_SIZE];
    pid_t pid;      /* 자식 프로세스의 pid 저장 */

    pipe(fds); /* pipe 함수의 인자로 정수형 배열의 주소값을 전달한다. fds[0]: 출구, fds[1]: 입구에 해당하는 파일디스크립터가 할당된다. */
    
    pid = fork();
    if(pid == 0)
    {
        /* Child */
        write(fds[1], str, sizeof(str));
    }
    else
    {
        /* Parent */
        read(fds[0], buf, BUF_SIZE);
        puts(buf);
    }

    return 0;
    
}