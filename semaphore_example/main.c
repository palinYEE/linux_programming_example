#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<semaphore.h>
#include<time.h>

#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/mman.h>
#include<sys/stat.h>

#define ERROR(x, y, z, r) if((x) == (y)){ perror(z); return (r); }

/* ftok argument */
#define FTOK_PATHNAME   "/tmp"
#define FTOK_PROJ       123

#define SEM_NUM     5   /* 세마포어 개수 */
#define SEM_COUNT   5   /* Counting 세마포어 값 */
#define SEM_BINARY  1   /* binary 세마포어 값 */

#define SEM_SYSV_RELEASE    1   /* sysv 세마포어 해제 값 */
#define SEM_SYSV_WAIT       0   /* sysv 세마포어 값이 0이 될때까지 waiting 하는 값 */
#define SEM_SYSV_GET        -1  /* sysv 세마포어 획득 값 */

#define POSIX_NAMED_FILENAME    "/named_posix"  /* POSIX NAMED 세마포어 key를 도출하기 위한 path */
#define POSIX_NAMED_TIME_DELAY  5       /* timedwait 에서 delay 시간 값 */ 

#define POSIX_UNNAMED_FILENAME "/unnamed_posix" /* POSIX UNNAMED 세마포어 예제를 위한 공유 메모리 path */
#define POSIX_UNNAMED_THREAD_SHARED    0        /* thread shared mode */
#define POSIX_UNNAMED_PROCESS_SHARED    1       /* process shared mode */

typedef struct data_info {
    sem_t sem;
    int pid;
    int count;
} DATA_INFO;

static void print_help(const char *progname)
{
    printf("Usage: %s [options]\n", progname);
    printf("    - options list: \n");
    printf("        * sysv [init|show|get SEM_IDX|release SEM_IDX|wait0 SEM_IDX]\n");
    printf("        * unnamed_posix [monitor|get|put]\n");
    printf("        * named_posix [show|wait|trywait|timewait|release]\n");
}

int sysv_sem_setting(void)
{
    int sem_id;
    key_t key;
    key = ftok(FTOK_PATHNAME, FTOK_PROJ);
    ERROR(key, -1, "ftok()", -1)
    sem_id = semget(key, SEM_NUM, IPC_CREAT | 0644);
    ERROR(sem_id, -1, "semget()", -1)
    return sem_id;
}

int sysv_sem_init(int sem_id)
{
    /*
        odd: binary semaphore
        even: count semaphore
    */
    for(int i=1; i<=SEM_NUM; i++){
        if(i % 2 == 0){            
            ERROR(semctl(sem_id, i-1, SETVAL, SEM_COUNT), -1, "semctl()", -1)
        } else {
            ERROR(semctl(sem_id, i-1, SETVAL, SEM_BINARY), -1, "semctl()", -1)
        }
    }
    return 0;    
}

int sysv_sem_show(int sem_id)
{
    int ret = 0;
    printf("=====================================\n");
    printf("SHOW SYSV SEMPAHORE VALUES \n");
    for(int i=0; i<SEM_NUM; i++){
        ret = semctl(sem_id, i, GETVAL);
        ERROR(ret, -1, "semctl()", -1)
        printf("    - Semaphore[%d] = %d\n ", i, ret);
    }
    printf("=====================================\n");
    return 0;
}

int sysv_sem_get(int sem_id, int sem_index)
{
    struct sembuf buf;
    buf.sem_num = sem_index;
    buf.sem_op = SEM_SYSV_GET;
    ERROR(semop(sem_id, &buf, 1), -1, "semop()", -1)
    return 0;
}

int sysv_sem_release(int sem_id, int sem_index)
{
    struct sembuf buf;
    buf.sem_num = sem_index;
    buf.sem_op = SEM_SYSV_RELEASE;
    ERROR(semop(sem_id, &buf, 1), -1, "semop()", -1)
    return 0;
}

int sysv_sem_wait(int sem_id, int sem_index)
{
    struct sembuf buf;
    buf.sem_num = sem_index;
    buf.sem_op = SEM_SYSV_WAIT;
    ERROR(semop(sem_id, &buf, 1), -1, "semop()", -1)
    return 0;
}

void sysv_main(int argc, char **argv)
{
    int sem_id;
    int sem_index;
    sem_id = sysv_sem_setting();
    if(!strcmp(argv[2], "init")){
        sysv_sem_init(sem_id);
    } else if(!strcmp(argv[2], "show")){
        sysv_sem_show(sem_id);
    } else if(!strcmp(argv[2], "get")){
        if(argc < 4){
            print_help(argv[0]);
            return ;
        }
        sem_index = atoi(argv[3]);
        sysv_sem_get(sem_id, sem_index);
    } else if(!strcmp(argv[2], "release")){
        if(argc < 4){
            print_help(argv[0]);
            return ;
        }
        sem_index = atoi(argv[3]);
        sysv_sem_release(sem_id, sem_index);
    } else if(!strcmp(argv[2], "wait0")){
        if(argc < 4){
            print_help(argv[0]);
            return ;
        }
        sem_index = atoi(argv[3]);
        sysv_sem_wait(sem_id, sem_index);
    } else {
        print_help(argv[0]);
    }
}

int posix_unnamed_monitor(void)
{
    int fd;
    int sval;
    int n = 0;
    DATA_INFO *info;
    DATA_INFO local;
    fd = shm_open(POSIX_UNNAMED_FILENAME, O_CREAT | O_RDWR, 0644);
    ERROR(fd, -1, "shm_open()", -1)
    ERROR(ftruncate(fd, sizeof(DATA_INFO)), -1, "ftruncate()", -1)
    info = mmap(NULL, sizeof(DATA_INFO), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(info == MAP_FAILED){
        perror("mmap()");
        close(fd);
        return -1;
    }
    memset((void *)&local, 0, sizeof(DATA_INFO));
    ERROR(sem_init(&info->sem, POSIX_UNNAMED_PROCESS_SHARED, SEM_NUM), -1, "sem_init()", -1)
    close(fd);

    printf("========================================\n");
    printf(" NAMED SEMAPHORE MONITORING START\n");
    while(1){
        usleep(10 * 1000);  /* CPU를 100% 사용하게 되면 memcmp 함수가 제대로 동작 안함. 따라서 sleep 값을 주어 cpu 부하를 줄인다. */
        if(memcmp((const void *)&local, (const void *)info, sizeof(DATA_INFO)) != 0){
            printf("    * pid = %d, counter = %d, sem: %d\n", info->pid, info->count, sval);
            sem_getvalue(&info->sem, &sval);
            memcpy((void *)&local, (const void *)info, sizeof(DATA_INFO));
            n ++;
            if( n == 5) break;
        }
    }
    sem_destroy(&info->sem);
    munmap((void *)info, sizeof(DATA_INFO));
    return 0;
}

int posix_unnamed_get(void)
{
    int fd;
    DATA_INFO *info;
    fd = shm_open(POSIX_UNNAMED_FILENAME, O_RDWR, 0644);
    ERROR(fd, -1, "shm_open()", -1)
    info = (DATA_INFO *)mmap(NULL, sizeof(DATA_INFO), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(info == MAP_FAILED){
        perror("mmap()");
        close(fd);
        return -1;
    }

    close(fd);
    printf("PUT DATA IN MMAP \n");
    ERROR(sem_wait(&info->sem), -1, "sem_wait()", -1)
    info->pid = getpid();
    info->count += 1;
    printf("INPUT DATA pid: %d, count : %d\n", info->pid, info->count);
    munmap((void *)info, sizeof(DATA_INFO));
    return 0;
}


int posix_unnamed_put(void)
{
    int fd;
    DATA_INFO *info;
    fd = shm_open(POSIX_UNNAMED_FILENAME, O_RDWR, 0644);
    ERROR(fd, -1, "shm_open()", -1)
    info = (DATA_INFO *)mmap(NULL, sizeof(DATA_INFO), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(info == MAP_FAILED){
        perror("mmap()");
        close(fd);
        return -1;
    }

    close(fd);
    ERROR(sem_post(&info->sem), -1, "sem_post()", -1)
    munmap((void *)info, sizeof(DATA_INFO));
    return 0;
}

// monitor|get|put
void posix_unnamed_main(char **argv)
{
    if(!strcmp(argv[2], "monitor")){
        posix_unnamed_monitor();
    } else if(!strcmp(argv[2], "get")){
        posix_unnamed_get();
    } else if(!strcmp(argv[2], "put")){
        posix_unnamed_put();
    } else {
        print_help(argv[0]);
    }
}

sem_t *posix_named_init(void)
{
    sem_t *sem;
    sem = sem_open(POSIX_NAMED_FILENAME, O_CREAT, 0644, SEM_NUM);
    ERROR(sem, SEM_FAILED, "sem_open()", NULL)
    return sem;
}

int posix_named_show(sem_t *sem)
{
    int sval;
    ERROR(sem_getvalue(sem, &sval), -1, "sem_getvalue()", -1)
    printf("============================================\n");
    printf(" POSIX NAMED SEMAPHORE VALUE CHECK \n");
    printf("    * semaphore : %d\n", sval);
    return 0;
}

int posix_named_wait(sem_t *sem)
{
    ERROR(sem_wait(sem), -1, "sem_wait()", -1)
    return 0;
}

int posix_named_trywait(sem_t *sem)
{
    ERROR(sem_trywait(sem), -1, "sem_trywait()", -1)
    return 0;
}

int posix_named_timedwait(sem_t *sem)
{
    struct timespec tpc;
    clock_gettime(CLOCK_REALTIME, &tpc);
    tpc.tv_sec += POSIX_NAMED_TIME_DELAY;
    ERROR(sem_timedwait(sem, &tpc), -1, "sem_timedwait()", -1)
    return 0;
}

int posix_named_release(sem_t *sem)
{
    ERROR(sem_post(sem), -1, "sem_post()", -1)
    return 0;
}

void posix_named_fin(sem_t *sem)
{
    sem_close(sem);
    // sem_unlink(POSIX_NAMED_FILENAME);
}

// show|wait|trywait|timewait|release
void posix_named_main(char **argv)
{
    sem_t *sem;
    sem = posix_named_init();
    if(sem == NULL){
        return ;
    }
    if(!strcmp(argv[2], "show")){
        posix_named_show(sem);
    }else if(!strcmp(argv[2], "wait")){
        posix_named_wait(sem);
    }else if(!strcmp(argv[2], "trywait")){
        posix_named_trywait(sem);
    }else if(!strcmp(argv[2], "timewait")){
        posix_named_timedwait(sem);
    }else if(!strcmp(argv[2], "release")){
        posix_named_release(sem);
    }else{
        print_help(argv[0]);
        posix_named_fin(sem);
        return ;
    }
    posix_named_fin(sem);
}

int main(int argc, char **argv)
{
    if(argc < 3){
        print_help(argv[0]);
        return -1;
    }
    if(!strcmp(argv[1], "sysv")){
        /* semaphore SYSV API */
        sysv_main(argc, argv);
    } else if(!strcmp(argv[1], "unnamed_posix")){
        /* unnamed semaphore POSIX API */
        posix_unnamed_main(argv);
    } else if(!strcmp(argv[1], "named_posix")){
        /* named semaphore POSIX API */
        posix_named_main(argv);
    } else {
        print_help(argv[0]);
        return -1;
    }

    return 0;
}