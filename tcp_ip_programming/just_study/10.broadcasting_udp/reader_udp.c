#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<errno.h>

int main(int argc, char* argv[])
{
    int sd;
    struct sockaddr_in s_addr, c_addr;
    char rcvBuffer[BUFSIZ];
    int n, n_send;
    int addr_len;

    int status;
    int on = 1;

    if(argc != 2)
    {
        fprintf(stderr, "usage: UDPreader port\n");
        return -1;
    }

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = htons(atoi(argv[1]));

    if(bind(sd, (struct sockaddr*)&s_addr, sizeof(s_addr)) != 0)
    {
        fprintf(stderr, "bind error\n");
        return -1;
    }

    while(1)
    {
        addr_len = sizeof(s_addr);
        n = recvfrom(sd, (void *)rcvBuffer, sizeof(rcvBuffer), 0, (struct sockaddr *)&c_addr, (socklen_t *)&addr_len);
        if(n < 0)
        {
            fprintf(stderr, "recvfrom error\n");
            return -1;
        }
        rcvBuffer[n] = "\0";
        if(!strncmp(rcvBuffer, "quit", 4)) break;

        fprintf(stderr, "Broadcast from %s [%s] \n", inet_ntoa(c_addr.sin_addr), rcvBuffer);
    }
    close(sd);

    return 0;
}