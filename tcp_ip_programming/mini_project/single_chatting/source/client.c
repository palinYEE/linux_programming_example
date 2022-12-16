/*
 * author: YYJ
 * date: 2022.02.27
 * discription: one to one chatting program client
 */
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>

/* TODO: 헤더 파일을 만들어서 하나로 관리하기 */
#define SERVER_PORT 9000
#define SERVER_IP "192.168.64.4"
#define ID_LEN 8

int main()
{
    int c_socket; 
    struct sockaddr_in c_addr;
    int len;
    int n;

    char rcvBuffer[BUFSIZ];
    char my_chatting_room_id[ID_LEN];

    /* 소켓 생성 */
    c_socket = socket(PF_INET, SOCK_STREAM, 0);
    /* 구조체 셋팅 */
    memset(&c_addr, 0, sizeof(c_addr));
    c_addr.sin_family = AF_INET;
    c_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    c_addr.sin_port = htons(SERVER_PORT);
    /* 소켓을 서버에 연결 */
    len = sizeof(c_addr);
    if(connect(c_socket, (struct sockaddr*)&c_addr, (socklen_t)len) == -1)
    {
        printf("[CLIENT] - [ERROR] : Fail Connect\n"); /* TODO: 에러 출력 함수 하나로 만들기 */
        close(c_socket);
        return -1;
    }
    /* 서비스 요청과 처리 */
    n = read(c_socket, my_chatting_room_id, sizeof(my_chatting_room_id));
    if(n < 0)
    {
        printf("[CLIETN] - [ERROR] : Empty rcvBuffer\n");
        return -1;
    } 

    printf("[CLIENT] My Chatting Room ID: %s\n", my_chatting_room_id);
    

    close(c_socket);
}
