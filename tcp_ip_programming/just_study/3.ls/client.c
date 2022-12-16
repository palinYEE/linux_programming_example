#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 9000
#define IPADDR "127.0.0.1"

int main()
{
    int c_socket;
    struct sockaddr_in c_addr;
    int len;
    int n;

    char rcvBuffer[BUFSIZ];
    char buffer[BUFSIZ];

    c_socket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&c_addr, 0, sizeof(c_addr));
    c_addr.sin_family = AF_INET;
    c_addr.sin_addr.s_addr = inet_addr(IPADDR);
    c_addr.sin_port = htons(PORT);

    len = sizeof(c_addr);
    if(connect(c_socket, (struct sockaddr *)&c_addr, (socklen_t)len) == -1){
        printf("Cannot Connect\n");
        close(c_socket);
        return -1;
    }

    scanf("%s", buffer);

    printf("send Data: %s\n", buffer);
    write(c_socket, buffer, sizeof(buffer));

    if((n = read(c_socket, rcvBuffer, sizeof(rcvBuffer) - 1)) < 0){
        return -1;
    }

    printf("receive Data: %s\n", rcvBuffer);
    close(c_socket);
    return 0;
}