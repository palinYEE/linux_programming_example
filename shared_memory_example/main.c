#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/mman.h>
#include<sys/stat.h>

#define KEY_PATH    "/tmp"
#define PROJ_ID     123
#define PAGE_SIZE 4096
#define MONITORING_COUNT 5
#define ROUNDUP(x)  (((x) + (PAGE_SIZE -1)) & ~(PAGE_SIZE - 1))
#define ERROR_CHECK(x, y, z) if((x) == (-1)) { perror(y); return z; }

#define SHM_FILE_PATH "/posix_test"

typedef struct data_info {
    int size;
    char message[PAGE_SIZE];
} DATA_INFO;

static void print_usage(const char *progname)
{
    printf("Usage: %s (posix|sysv) (recv|send Message)\n", progname);
}

static int posix_init(void)
{
    int fd;
    fd = shm_open(SHM_FILE_PATH, O_CREAT | O_RDWR, 0644);
    ERROR_CHECK(fd, "shm_open()", -1);

    ERROR_CHECK(ftruncate(fd, sizeof(DATA_INFO)), "ftruncate()", -1);
    
    return fd;
}

DATA_INFO *posix_gen_info(int fd)
{
    DATA_INFO *info;
    info = (DATA_INFO *)mmap(NULL, sizeof(DATA_INFO), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ERROR_CHECK(info, "mmap()", MAP_FAILED);
    return info;
}

static int posix_finish(int fd, DATA_INFO *info)
{
    munmap(info, sizeof(DATA_INFO));
    close(fd); 
    return 0; 
}

static int posix_recv_message(void)
{
    int fd;
    int n;
    DATA_INFO *info;
    DATA_INFO local;

    fd = posix_init();

    info = posix_gen_info(fd);
    
    memset(&local, 0, sizeof(local));
    memset(info, 0, sizeof(DATA_INFO));
    n = 0;
    while(1){
        if(memcmp((const void *)&local, (const void *)info, sizeof(local))){
            /* diff */
            printf("[Monitor - POSIX] size: %d, send Message: %s\n", info->size, info->message);
            memcpy(&local, info, sizeof(DATA_INFO));
            n ++;
            if( n == MONITORING_COUNT) break;
        }
        sleep(1);
    }
    
    return posix_finish(fd, info);
}

static int posix_send_message(const char * message)
{
    int fd;
    DATA_INFO *info;
    fd = shm_open(SHM_FILE_PATH, O_CREAT | O_RDWR, 0644);
    ERROR_CHECK(fd, "shm_open()", -1);
    info = (DATA_INFO *)mmap(NULL, sizeof(DATA_INFO), PROT_WRITE, MAP_SHARED, fd, 0);
    info->size = sizeof(message);
    strcpy(info->message, message);
    munmap(info, sizeof(DATA_INFO));
    return 0;
}

static int sysv_init(void)
{
    key_t key;
    int shm_id;

    key = ftok(KEY_PATH, PROJ_ID);
    ERROR_CHECK(key, "ftok()", -1);

    shm_id = shmget(key, ROUNDUP(sizeof(DATA_INFO)), IPC_CREAT | 0644);
    ERROR_CHECK(shm_id, "shmget()", -1);
    return shm_id;
}

static DATA_INFO *sysv_gen_info(int shm_id)
{
    DATA_INFO *info;
    info = (DATA_INFO *)shmat(shm_id, NULL, 0);
    ERROR_CHECK((void *)info, "shmat()", NULL);
    memset((void *)info, 0, sizeof(DATA_INFO));
    return info;
}

static int sysv_finish(const void *info)
{
    ERROR_CHECK((const void *)info, "shmdt()", -1);
    return 0;
}

static int sysv_recv_message(void)
{
    int n;
    int shm_id;
    DATA_INFO *info;
    DATA_INFO local;

    shm_id = sysv_init();
    ERROR_CHECK(shm_id, "sysv_init()", -1);

    info = sysv_gen_info(shm_id);
    if(info == NULL){
        perror("sysv_gen_info()");
        return -1;
    }    

    memset(&local, 0, sizeof(local));
    n = 0;
    while(1){
        if(memcmp((const void *)&local, (const void *)info, sizeof(local))){
            /* diff */
            printf("[Monitor - SYSV] size: %d, send Message: %s\n", info->size, info->message);
            memcpy(&local, info, sizeof(DATA_INFO));
            n ++;
            if( n == MONITORING_COUNT) break;
        }
        sleep(1);
    }

    return sysv_finish((const void *)info);
}

static int sys_send_message(const char *message)
{
    int shm_id;
    DATA_INFO *info;
    shm_id = sysv_init();
    ERROR_CHECK(shm_id, "sysv_init()", -1);
    info = sysv_gen_info(shm_id);
    if(info == NULL){
        perror("sysv_gen_info()");
        return -1;
    }
    info->size = sizeof(message);
    strcpy(info->message, message);

    return sysv_finish(info);
}

int main(int argc, char **argv)
{
    if(argc < 3){
        print_usage(argv[0]);
        return -1;
    }
    if(!strcmp((const char *)argv[1], "posix")) {
        /* POSIX */
        if(!strcmp((const char *)argv[2], "recv")){
            /* recv Message */
            posix_recv_message();
        } else if(!strcmp((const char *)argv[2], "send")){
            /* send Message */
            if(argc < 4) { goto main_err; }
            posix_send_message((const char *)argv[3]);
        } else { goto main_err;}

    } else if(!strcmp((const char *)argv[1], "sysv")){
        /* SYSV */
        if(!strcmp((const char *)argv[2], "recv")){
            /* recv Message */
            sysv_recv_message();
        } else if(!strcmp((const char *)argv[2], "send")){
            /* send Message */
            if(argc < 4) { goto main_err; }
            sys_send_message((const char *)argv[3]);
        } else { goto main_err;}
    } else { goto main_err; }

    return 0;
main_err:
    print_usage(argv[0]);
    return -1;
}