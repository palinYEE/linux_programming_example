/*
 * author: YYJ
 * date: 2022.03.05
 * discription: one to one chatting program server
 */
#include<stdio.h>
#include<string.h>
#include<stdlib.h> 
#include<time.h>
#include<errno.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>

#define SERVER_PORT 9000
#define ID_LEN 8
#define CHATTING_ROOM_NUM 100

/* 디버그 레벨 */
#define DEBUG_LEVEL 2

/* 상태값 전역변수 */
#define SUCCESS 1
#define FAIL -1

/* DB SETTING */
#define DB_NAME "yj_chatting_DB.txt"


typedef struct ID
{
    char id_value;        /* 고유 ID 값 */
    pid_t pid;            /* 해당 ID 의 pid 값 */  
    int fds[2];           /* file desciption. fds[0]: exit, fds[1]: enter */
    struct ID *next;      /* 링크드 리스트 다음 구조체 포인터 */
    struct ID *before;    /* 링크드 리스트 이전 구조체 포인터 */
} _ID;

_ID Chatting_room[CHATTING_ROOM_NUM];

/* 고유 ID 값을 만드는 함수 */
int createID(char *buf, int num)
{
    int rndVal = 0;
    int i=0;
    srand((unsigned)time(NULL));
    for(i=0; i<num; i++)
    {   
        rndVal = (int)(rand()%26); 
        if(rndVal < 10) 
        {
            sprintf(buf + i, "%d", rndVal);
        }else if(rndVal > 35) 
        { 
            sprintf(buf + i, "%c", rndVal + 61);
        }else { 
            sprintf(buf + i, "%c", rndVal + 55); 
        } 
    }
    if(i < num)
    {
        return FAIL;
    }
#if DEBUG_LEVEL >= 2
    printf("[SERVER] - [createID] : randomID - %s\n", buf);
#endif
    return SUCCESS;
}

/* ID 값 저장 함수 */
void storeID(_ID *id, char *buf, int count)
{
    memcpy(&(id->id_value), buf, sizeof(buf)); /* 링크드 리스트로 관리 */

    /* 파일로 관리 */
    /*
        파일로 관리할 경우 다음과 같은 형식으로 저장한다. 
        <chatting number> <ID> <pid>
    */
    FILE *fp;
    int i;
    fp = fopen( DB_NAME, "a+" );
    if( fp == NULL )
    {
        printf("[SERVER] : file fopen() error\n");
    }
    else
    {   
        fprintf(fp, "%d %s %d\n", count, buf, getpid());
    }
    fclose(fp);
}

/* ID 값 삭제 함수 */
void clearID(_ID *id[CHATTING_ROOM_NUM])
{
    FILE *fp;
    int i;

    fp = fopen(DB_NAME, "w+");
    if( fp == NULL)
    {
        printf("[SERVER] : file fopen() error\n");
    }
    else{
        system("rm yj_chatting_DB.txt");
    }

    for(i=0; i<CHATTING_ROOM_NUM; i++)
    {
        id[i]->id_value = 0;
        id[i]->next = NULL;
        id[i]->before = NULL;
    }
    
}

/* file load 함수 */
int loadID(_ID *id[CHATTING_ROOM_NUM], int *num)
{
    FILE *fp;
    int count = 0;
    fp = fopen(CHATTING_ROOM_NUM, "r");

    if(fp == NULL)
    {
        return FAIL;
    }
    else
    {
        while((n=fscanf(fp, "%d %s %d", count, id[count]->id_value, id[count]->pid)) != EOF) 
        {
#if DEBUG_LEVEL >= 2
            printf("[SERVER] %d: id=%s, pid=%d\n", count, id[count]->id_value, id[count]->pid);
#endif
        }
    }
    *num = count;
    return SUCCESS;
}

/* 디버깅 프린트 함수 */
void debug_print(int status, char *msg_fail, char *msg_success)
{
#if DEBUG_LEVEL >= 2
        if(status == FAIL)
        {
            printf("[SERVER] - [FAIL] %s\n", msg_fail);
        }
        else if(status == SUCCESS)
        {
            printf("[SERVER] - [SUCCESS] %s\n", msg_success);
        }
        else{
            printf("[SERVER] - status value error...\n");
        }      
#endif
}

void menu_print()
{
    printf("============================\n");
    printf("1. Create chatting room\n");
    printf("2. show chatting room list\n");
    printf("3. connection chatting room\n");
    printf(" = Select: ");
}
/* 현재 열려있는 채팅방 목록을 확인하는 함수 */
// void print_chatting_room_list(_ID *id[CHATTING_ROOM_NUM])
// {
//     int i;
// }

int main()
{
    int c_socket, s_socket; /* socket descryption value */
    struct sockaddr_in s_addr, c_addr;
    int len;
    int n;
    int chatting_room_count = 0;

    int menu = 0; /* menu select */

    int status = 0; /* status value (success or fail) */

    char randomID[ID_LEN]; /* random ID */

    pid_t pid;
    _ID id[CHATTING_ROOM_NUM];
    FILE *fp;

    fp = fopen(CHATTING_ROOM_NUM, "r");
    if(fp == NULL)
    {
        printf("[SERVER] : file fopen() error\n");
    }
    else
    {
        if(loadID(&id[CHATTING_ROOM_NUM], &chatting_room_count) == FAIL)
        {
            printf("[SERVER] : loadID() error\n");
        }
    }

    s_socket = socket(PF_INET, SOCK_STREAM, 0); /* create socket */
    memset(&s_addr, 0, sizeof(s_addr));
    /* setting structure sockaddr_in */
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* all ip allow */
    s_addr.sin_port = htons(SERVER_PORT); 

    /* 소켓을 포트에 연결 */
    if(bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1)
    {
        printf("[SERVER] - [ERROR] : Fail bind\n");
        return -1;
    }

    /* 커널에 개방 요청 */
    if(listen(s_socket, 5) == -1)
    {
        printf("[SERVER] - [ERROR] : Fail listen\n");
    }

    /* 메인 로직 */
    while(1)
    {
        /* TODO: 메인 메뉴 화면을 넣는 건 어떨까? */
        /*
            1. Create chatting room
            2. show chatting room list
            3. connection chatting room (<-- input chatting ID )
        */
        menu_print();
        scanf("%d", &menu);
        switch (menu)
        {
        case 1:
            en = sizeof(c_addr);
            c_socket = accept(s_socket, (struct sockaddr *)&c_addr, (socklen_t *)&len); /* <-- 이거 소켓 있는데 굳이 필요할까 */
        
            pipe(id[chatting_room_count].fds);
            pid = fork()

            if( pid < 0)
            {
                printf("[SERVER] - [ERROR] : echo server cannot fork()\n");
                return FAIL;
            }
            else if(pid > 0)
            {
                status = createID(randomID, ID_LEN);
                debug_print(status, "fail create random ID", "success create random ID");
                if( status == SUCCESS)
                {
                    write(c_socket, randomID, strlen(randomID));
                    debug_print(status, "fail send random ID", "success send random ID");
                    chatting_room_count+= 1;
                    storeID(Chatting_room, randomID, chatting_room_count);
                }
                close(c_socket);
            }
            else if(pid == 0)
            {
                whil(1)
                {
                    /* child process routine add. */
                    read()
                }
            }
            break;
        
        default:
            break;
        }

    }
}
