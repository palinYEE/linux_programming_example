#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>

#define PORT 9000
#define ERRORNO -1

void do_echo(int connSock){
    int n;
    char rcvBuffer[BUFSIZ];

    while((n = read(connSock, rcvBuffer, sizeof(rcvBuffer))) != 0){
        printf("child\n");
        write(connSock, rcvBuffer, n);
    }
}

int main()
{
    int c_socket, s_socket;
    struct sockaddr_in s_addr, c_addr;
    int len;
    pid_t pid;

    s_socket = socket(PF_INET, SOCK_STREAM, 0);

    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr =  htonl(INADDR_ANY);
    s_addr.sin_port = htons(PORT);

    if(bind(s_socket, (struct sockaddr*)&s_addr, sizeof(s_addr)) == ERRORNO){
        printf("Cannot bind\n");
        return -1;
    }

    if(listen(s_socket, 5) == ERRORNO){
        printf("listen Fail \n");
        return -1;
    }

    while(1){
        len = sizeof(c_addr);
        c_socket = accept(s_socket, (struct sockaddr*)&c_addr, (socklen_t*)&len);

        if((pid = fork()) < 0){
            printf("echo sercer can not fork()\n");
            return -1;
        }else if(pid > 0){
            close(c_socket);
            continue;
        }else if(pid == 0){
            close(s_socket);
            do_echo(c_socket);
        }
    }
    return 0;
}