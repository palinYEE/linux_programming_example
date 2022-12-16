#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<netinet/in.h>

#define PORT 9000
#define IPADDR "127.0.0.1"

int main()
{
    int c_socket;
    struct sockaddr_in c_addr;
    int len;
    int n;
    int n_left;
    int n_recv;

    char rcvBuffer[BUFSIZ];         /* 받을 데이터 저장 변수 */
    char sndBuffer[BUFSIZ];         /* 보낼 데이터 저장 변수 */

    c_socket = socket(PF_INET, SOCK_STREAM, 0);     /* 소켓 생성 */
    memset(&c_addr, 0, sizeof(c_addr));             /* sockaddr_in 초기화 */
    c_addr.sin_family = AF_INET;                    /* sockaddr_in 구조체 변수 셋팅 */
    inet_aton(IPADDR, &c_addr.sin_addr);
    c_addr.sin_port = htons(PORT);

    len = sizeof(c_addr);
    /* c_socket 파일 디스크립션 생성 (이는 소켓 연결 과정이다. ) */
    if(connect(c_socket, (const struct sockaddr *)&c_addr, (socklen_t)len) == -1){
        printf("Cannet connect\n");
        close(c_socket);
        return -1;
    }
    else{
        printf("Success Connection\n");
    }

    while(1){   /* 무한 루프를 돌게하여 내가 설정한 특정 문자열을 보내야 끝나도록 설정 */
        if((n = read(0, sndBuffer, (size_t)BUFSIZ)) > 0){
            sndBuffer[n] = '\0';
            if(!strcmp(sndBuffer, "quit\n")) break;

            printf("original data: %s", sndBuffer);
            /* c_socket 을 이용하여 연결된 소켓에 문자열을 전달한다. */
            if((n = write(c_socket, sndBuffer, strlen(sndBuffer))) < 0){
                printf("fin write\n");
                return -1;
            }
            
            n_left = n;
            n_recv = 0;
            while(n_left){
                /* 상대편이 작성한 문자열을 받아온다. */
                if((n = read(c_socket, &rcvBuffer[n_recv], n_left)) < 0){
                    printf("fin write 2\n");
                    return -1;
                }

                n_left = n_left - n;
                n_recv = n_recv + n;
            }

            rcvBuffer[n_recv] = '\0';
            printf("echoed Data: %s", rcvBuffer);
        }
        else{
            printf("^_^ please retry\n");
        }
    }

    close(c_socket);
}