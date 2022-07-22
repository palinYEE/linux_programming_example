#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>

#define PATH_NAME "/home/alwns28/example_folder/namePipe_example/fifo"
#define MAX_BUF_LEN 1024

static void print_help(const char *program_name){
    printf("Usage: %s (s|r) \n", program_name);
}

static void send_data(void)
{
    int fd;
    char buf[MAX_BUF_LEN];
    if(unlink(PATH_NAME) == -1){
        printf("파일 생성이 필요합니다.\n");
    }
    if(mkfifo(PATH_NAME, 0644) == -1){
        perror("mkfifo()");
        return ;
    }

    fd = open(PATH_NAME, O_CREAT | O_WRONLY, 0644);
    memset((void *)buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "fifo TEST hello world");
    printf("[SEND](%u) fifo TEST hello world\n", getpid());
    if(write(fd, (const void *)buf, sizeof(buf)) == -1){
        perror("write()");
    }
    close(fd);
}

static void recv_data(void)
{
    int fd;
    char buf[MAX_BUF_LEN];
    fd = open(PATH_NAME, O_RDONLY);
    memset((void *)buf, 0, sizeof(buf));
    if(read(fd, (void *)buf, sizeof(buf)) == -1){
        perror("read()");
        close(fd);
        return ;
    }
    printf("[RECV](%u) %s\n", getpid(), buf);
    close(fd);
}

int main(int argv, char **argc)
{
    if(argv < 2) {
        print_help((const char *)argc[0]);
        return -1;
    }

    if(!strcmp((const char *)argc[1], "r")){
        recv_data();
    } else if(!strcmp((const char *)argc[1], "s")){
        send_data();
    } else {
        print_help((const char *)argc[0]);
    }

    return 0;
}