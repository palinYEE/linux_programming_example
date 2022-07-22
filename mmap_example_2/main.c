#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/mman.h>

#define MAX_BUF_LEN 4096
#define SLEEP_UTIME 1000
#define FORK_NUM 5

typedef struct anon_info{
    int pid;
    char Message[MAX_BUF_LEN];
} ANON_INFO;

typedef struct data_info{
    int size;
    char Message[MAX_BUF_LEN];
} DATA_INFO;

static void print_help(const char *progname)
{
    printf("Usage: %s [-fa] (monitor|send Message)\n", progname);
    printf("    options:\n");
    printf("        -f  (monitor|send Message) File_path \n");
    printf("        -a\n");   
}

static int do_monitoring(const char *file_path)
{
    int fd; 
    DATA_INFO *buf;
    DATA_INFO *tmp_buf;
    
    fd = open(file_path, O_CREAT | O_RDWR, 0644);
    if(fd == -1){
        perror("open()");
        return -1;
    }
    
    if(ftruncate(fd, sizeof(DATA_INFO)) == -1){ /* 파일을 넣을 데이터 만큼 늘려놓는다. */
        perror("ftruncate()");
        close(fd);
        return -1;
    }

    buf = (DATA_INFO *)mmap(NULL, sizeof(DATA_INFO), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(buf == MAP_FAILED){
        perror("mmap()");
        close(fd);
        return -1;
    }

    buf->size = 0;
    memset(buf->Message, 0, sizeof(char) * MAX_BUF_LEN);
    
    close(fd);

    tmp_buf = (DATA_INFO *)malloc(sizeof(DATA_INFO));
    memset((void *)tmp_buf, 0, sizeof(DATA_INFO));
    memcpy((void *)tmp_buf, (const void *)buf, sizeof(DATA_INFO));
    printf("[monitor] client said [%s], Message size: %d\n", tmp_buf->Message, tmp_buf->size);

    while(1){
        if(sizeof(tmp_buf) < sizeof(buf)){
            free(tmp_buf);
            tmp_buf = (DATA_INFO *)malloc(sizeof(DATA_INFO));
            memset((void *)tmp_buf, 0, sizeof(DATA_INFO));
            memcpy((void *)tmp_buf, (const void *)buf, sizeof(DATA_INFO));
            printf("[monitor] client said [%s], Message size: %d\n", tmp_buf->Message, tmp_buf->size);
        } else {
            if(memcmp((const void *)tmp_buf, (const void *)buf, sizeof(DATA_INFO))){
                memcpy((void *)tmp_buf, (const void *)buf, sizeof(DATA_INFO));
                printf("[monitor] client said [%s], Message size: %d\n", tmp_buf->Message, tmp_buf->size);

            }
        }
        usleep(SLEEP_UTIME);
    }
    
    free(tmp_buf);
    munmap(buf, sizeof(DATA_INFO));
    return 0;
}

static int do_send_data(const char *file_path, const char *message)
{
    int fd;
    fd = open(file_path, O_RDWR);
    DATA_INFO *buf;
    if(fd == -1){
        perror("open()");
        return -1;
    }
    buf = (DATA_INFO *)mmap(NULL, sizeof(DATA_INFO), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(buf == MAP_FAILED){
        close(fd);
        perror("mmap()");
        return -1;
    }
    buf->size = sizeof(message);
    strncpy(buf->Message, message, sizeof(buf->Message));
    munmap(buf, sizeof(DATA_INFO));
    close(fd);
    return 0;
}

ANON_INFO *mmap_init(void)
{
    ANON_INFO *info;
    info = mmap(NULL, sizeof(ANON_INFO), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(info == MAP_FAILED){
        perror("mmap()");
        return NULL;
    }
    memset((void *)info, 0, sizeof(ANON_INFO));
    return info;
}

static int anon_send_data(int index, ANON_INFO *info)
{
    sleep(index + 2);
    info->pid = getpid();
    snprintf(info->Message, sizeof(info->Message), "anonymous test %d", index);
    return 0;
}

static int anon_monitor(ANON_INFO *info)
{
    int n = 0;
    ANON_INFO local;

    memset((void *)&local, 0, sizeof(local));
    while(1){
        if(memcmp((const void *)&local, (const void *)info, sizeof(local))){
            printf("    * pid = %d, Message = %s\n", info->pid, info->Message);
            memcpy((void *)&local, (const void *)info, sizeof(local));
            n ++;
            if(n == FORK_NUM) break;
        }
        usleep(SLEEP_UTIME);
    }
    munmap((void *)info, sizeof(ANON_INFO));
    return 0;
}

static int do_anon(void)
{
    int i;
    pid_t pid;
    ANON_INFO *info;
    printf("* START ANONYMOUS MODE \n");
    info = mmap_init();
    for(i=0; i<FORK_NUM; i++){
        pid = fork();
        if(pid > 0){
            /* parent */
        } else if (pid == 0){
            /* child */
            anon_send_data(i, info);
            munmap((void *)info, sizeof(ANON_INFO));
            return 0;
        } else {
            perror("fork()");
            return -1;
        }
    }
    anon_monitor(info);
    printf("* END ANONYMOUS MODE\n");
    for(i=0; i<FORK_NUM; i++){
        pid = wait(NULL);
        if(pid == -1){
            perror("wait()");
            return -1;
        }
        printf("    [PID: %d] Terminate\n", pid);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if(argc < 2){
        goto main_err;
    }
    switch (argv[1][1])
    {
    case 'f':
        if(argc < 4){
            goto main_err;
        }
        if(!strcmp((const char *)argv[2], "monitor")){
            do_monitoring((const char *)argv[3]);
        } else if(!strcmp((const char *)argv[2], "send")){
            if(argc < 5){
                goto main_err;
            }
            do_send_data((const char *)argv[4], (const char *)argv[3]);
        } else {
            goto main_err;
        }
        break;
    case 'a':
        /* anonymous mode */
        do_anon();
        break;
    default:
        goto main_err;
    }
    return 0;
main_err:
    print_help(argv[0]);
    return -1;
}