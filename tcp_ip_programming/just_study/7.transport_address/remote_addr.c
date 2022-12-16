#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>

int main()
{
    int tcpSd;
    struct sockaddr_in s_addr;

    if((tcpSd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket()");
        return -1;
    }

    memset(&s_addr, 0, sizeof(s_addr));

    s_addr.sin_family = AF_INET;
    inet_aton("192.168.123.123", &s_addr.sin_addr);
    s_addr.sin_port = htons(7);

    /* inet_ntoa: network order to address character */
    printf("ip(dotted decial) = %s\n", inet_ntoa(s_addr.sin_addr)); 
    /* ntohl: network order to host long */
    printf("ip(binary) = 0x%x\n", ntohl(s_addr.sin_addr.s_addr));
    /* ntohs: network order to host short */
    printf("port no = %d\n", ntohs(s_addr.sin_port));

    close(tcpSd);

    return 0;
}