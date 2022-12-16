#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main(int argc, char* argv[])
{
    int sd;
    struct sockaddr_in s_addr;
    char sndBuffer[BUFSIZ];
    int n, n_send, status;

    int on = 1;

    if(argc != 3)
    {
        fprintf(stderr, "usage: UDP broadcasst_ip_addr port\n");
        return -1;
    }

    sd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr(argv[1]);
    s_addr.sin_port = htons(argv[2]);

    status = setsockopt(sd, SOL_SOCKET, SO_BROADCAST, (const void *)&on, (socklen_t)sizeof(on));
    if(status != 0){
        fprintf(stderr, "setsockopt error\n");
        return -1;
    }

    while(1)
    {
        fprintf(stderr, "Type broadcasting data: ");
        n = read(0, sndBuffer, BUFSIZ);
        if(n > 0)
        {
            sndBuffer[n-1] = "\0";
            n_send = sendto(sd, (const void *)sndBuffer, strlen(sndBuffer), 0, (struct sockaddr *)&s_addr, (socklen_t)sizeof(s_addr));
            if(n_send < 0)
            {
                fprintf(stderr, "sendto() error\n");
                return -1;
            }
            if(!strncmp(sndBuffer, "quit", 4)) break;
        }
    }
}