#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>

#include<malloc.h>

#include<fcntl.h>
#include<sys/stat.h>
#include<mqueue.h>

#define FILE_PATH "/posix_example"

long max_len;

static void print_help(char *program_name)
{
    printf("Usage: %s (s|r)\n", program_name);
}

static void send_data(void)
{
    mqd_t mq_fd;
    struct mq_attr atr;
    char *buf;
    mq_fd = mq_open(FILE_PATH, O_RDWR);
    if(mq_fd == -1){
        perror("mq_open()");
        return ;
    }
    memset((void *)&atr, 0, sizeof(struct mq_attr));
    mq_getattr(mq_fd, &atr);
    printf("* mq_msgsize: %lu\n", atr.mq_msgsize);
    max_len = atr.mq_msgsize;
    buf = (char *)malloc(sizeof(char) * max_len);
    memset((void *)buf, 0, max_len);
    snprintf(buf, max_len, "hello world");
    if(mq_send(mq_fd, buf, max_len, 1) == -1){
        perror("mq_send()");
        free(buf);
        close(mq_fd);
        return ;
    }
    free(buf);
    mq_close(mq_fd);
}

static void recv_data(void)
{
    mqd_t mq_fd;
    struct mq_attr atr;
    char *buf;
    unsigned int priority;
    mq_fd = mq_open(FILE_PATH, O_CREAT | O_RDWR, 0644, NULL);
    if(mq_fd == -1){
        perror("mq_open()");
        return ;
    }
    memset((void *)&atr, 0, sizeof(struct mq_attr));
    mq_getattr(mq_fd, &atr);
    max_len = atr.mq_msgsize;
    buf = (char *)malloc(sizeof(char) * max_len);
    memset((void *)buf, 0, sizeof(buf));
    if(mq_receive(mq_fd, buf, max_len, &priority) == -1){
        perror("mq_receive()");
        free(buf);
        close(mq_fd);
        return ;
    }
    printf("[RECV] data: %s\n", buf);
    free(buf);
    mq_close(mq_fd);
}

int main(int argc, char **argv)
{
    if(argc < 2){
        print_help(argv[0]);
        return -1;
    }

    if(!strcmp((const char*)argv[1], "s"))
    {
        /* Sender */
        send_data();
    }
    else if(!strcmp((const char *)argv[1], "r"))
    {
        /* recv */
        recv_data();
    }
    else {
        print_help(argv[0]);
        return -1;
    }
    return 0;
}