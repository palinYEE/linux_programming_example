#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>

void print_sockaddr_in(struct sockaddr_in* in)
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

int main(int argc, char *argv[])
{
    int                 sd;
    struct sockaddr_in  s_addr, c_addr;
    char                buf[BUFSIZ];
    int                 n, n_recv;
    int                 addr_len;

    sd = socket(PF_INET, SOCK_DGRAM, 0); 

    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = htons(9200);

    print_sockaddr_in(&s_addr);

    if(bind(sd,(const struct sockaddr *)&s_addr, (socklen_t)sizeof(s_addr)) < 0)
    {
        fprintf(stderr, "bind() error \n");
        return -1;
    }

    while(1)
    {
        fprintf(stderr, "waiting\n");

        addr_len = sizeof(c_addr);
        n_recv = recvfrom(sd, (void *)buf, (size_t)sizeof(buf), 0, (struct sockaddr *)&c_addr, (socklen_t *)&addr_len);
        if(n_recv < 0)
        {
            fprintf(stderr, "recvfrom() error\n");
            return -1;
        }
        n = sendto(sd, (const void *)buf, (size_t)sizeof(buf), 0, (const struct sockaddr *)&c_addr, (socklen_t)sizeof(c_addr));
        if(n < 0)
        {   
            fprintf(stderr, "sendto() error\n");
            return -1;
        }
    }
    close(sd);
    return 0;
}