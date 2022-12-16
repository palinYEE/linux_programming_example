#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<signal.h>
#include<errno.h>

#define USE_ALARM 0
#define CHECK_TARGET_VALIDATION 0
#define CHECK_ASTNC_ERROR 1

void print_sockaddr_in(struct sockaddr_in *in)
{
    char *temp;
    if(in->sin_family == AF_INET){
        temp = "AF_INET";
    }
    else if(in->sin_family == AF_INET6){
        temp = "AF_INET6";
    }
    else{
        temp = "IDONTKNOW";
    }
    printf("====================\n");
    printf(" - sockaddr_in structure info print \n");
    printf("[*] sockaddr_in.sin_family: %s\n", temp);
    printf("[*] sockaddr_in.sin_addr.s_addr : %s\n", inet_ntoa(in->sin_addr));
    printf("[*] sockaddr_in.sin_port : %d\n", ntohs(in->sin_port));
    printf("====================\n");
}


void sigAlarm(int signo)
{
    fprintf(stderr, "alarm\n");
    return;
}

int main(int argc, char *argv[])
{
    int     sd;
    struct  sockaddr_in s_addr, c_addr;
    char    sndBuff[BUFSIZ];
    int     n, n_send;
    int     addr_len;

#if USE_ALARM /* 알람 설정 셋팅 */ 
/*
 UDP 프로토콜은 신뢰성을 보장하지 않기 때문에, 기존코드에서는 데이터를 보내고 상대방이 제대로 받지 않았을 때 무한으로 대기하는 현상이 발생한다. 따라서 이를 방지하기 위해 알람 기능을 추가하여 recvfrom에서 깨어난후 타임아웃 종료를 할 수 있게 수정.
*/
    int status;
    struct sigaction act;    

    act.__sigaction_u.__sa_handler = sigAlarm;
    sigemptyset(&act.sa_mask);

    status = sigaction(SIGALRM, &act, 0);
#endif

    sd = socket(PF_INET, SOCK_DGRAM, 0);

    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = htons(9200);

#if CHECK_ASTNC_ERROR
/*
 해당 주석은 UDP 통신을 할 떄 비동기 오류 발생에 대한 캐치를 하기 위함이다.
 또한 connect 후 자료 송수신시 상대방을 지정할 필요가 없으므로 read, write 함수를 사용할 수 있다. 
 하지만 이러한 기능은 일부 OS에서는 동작하지 않을 수 있으니 주의하자.
*/
    connect(sd, (const struct sockaddr*)&s_addr, (socklen_t)sizeof(s_addr));
#endif

    while(1)
    {
        fprintf(stderr, "waiting\n");
        n = read(0, (void *)sndBuff, (size_t)sizeof(sndBuff));
        if(n < 0)
        {
            fprintf(stderr, "read() error\n");
            return -1;
        }

        fprintf(stderr, "Original Data: %s\n", sndBuff);
#if CHECK_ASTNC_ERROR
        n_send = write(sd, (const void*)sndBuff, strlen(sndBuff));
#else
        n_send = sendto(sd, (const void*)sndBuff, strlen(sndBuff), 0, (const struct sockaddr *)&s_addr, (socklen_t)sizeof(s_addr));
#endif
        if(n_send < 0)
        {
            fprintf(stderr, "sendto() error\n");
            return -1;
        }

#if USE_ALARM
        alarm(2);
#endif
        addr_len = sizeof(c_addr);

#if CHECK_ASTNC_ERROR
        n = read(sd, (void *)sndBuff, strlen(sndBuff));
#else
        n = recvfrom(sd, (void *)sndBuff, strlen(sndBuff), 0, (struct sockaddr *)&c_addr, (socklen_t *)&addr_len);
#endif
        if(n < 0)
        {
#if USE_ALARM
            if(errno == EINTR){
                fprintf(stderr, "socket timeout\n");
            }
            else{
                fprintf(stderr, "recvfrom() error\n");
            }
#else
            fprintf(stderr, "recvfrom() error\n");
#endif
            return -1;
        }
        else{
#if USE_ALARM
            alarm(0);
#endif
#if CHECK_TARGET_VALIDATION 
/* 
 해당 부분은 UDP 프로토콜이 신뢰성을 보장하지 못하여 들어온 패킷이 우리가 원하는 사용자의 패킷인지 확인하는 부분이다. 
*/
            if(memcmp(&c_addr, &s_addr, addr_len))
            {
                fprintf(stderr, "reply from %s/%d (ignored)\n", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
                continue;
            }
#endif
            fprintf(stderr,"echoed Data: %s\n", sndBuff);   
        }
    }
    close(sd);
    return 0;

}