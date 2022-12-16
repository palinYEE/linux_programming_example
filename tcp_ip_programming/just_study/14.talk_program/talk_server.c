#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdlib.h>

void do_keyboard(int listenSock)
{
    int n;
    char sbuf[BUFSIZ];

    while((n = read(0, sbuf, BUFSIZ)) > 0)
    {
        if(write(listenSock, sbuf, n) != n)
        {
            printf("Talk server fail in sending\n");
        }
        if(strncmp(sbuf, quit, 4) == 0)
        {
            kill(pid, SIGQUIT);
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    struct sockaddr_in c_addr, s_addr;
    int connSock, listenSock;
    char rbuf[BUFSIZ];

    int len;
    int pid;


    connSock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = htons(atoi(argv[1]));

    if(bind(connSock, (struct sockaddr*)&s_addr, sizeof(s_addr)) == -1){
        printf("Cannot bind\n");
        return -1;
    }

    if(listen(connSock, 5) == -1){
        printf("listen Fail\n");
        return -1;
    }

    while(1)
    {
        len = sizeof(c_addr);
        listenSock = accept(connSock, (struct sockaddr*)&c_addr, (socket_t*)&len);

        if(listenSock < 0){
            printf("accept fail!\n");
            return -1;
        }

        if((pid = fork()) < 0)
        {
            printf("fork error\n");
            return -1;
        } else if( pid > 0)
        {
            
        } else if (pid == 0)
        {

        }
    }

    close(listenSock);
    close(connSock);
}