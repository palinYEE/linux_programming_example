#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h> 
#include<netinet/in.h>
#include<sys/socket.h>

#define PORT 9000

char rBuffer[BUFSIZ];
char buffer[BUFSIZ] = "hello world";

void print_sockaddr_in(struct sockaddr_in *in){
    printf("======================\n");
    printf("print sockaddr_in\n");
    printf("[*] sockaddr_in.sin_family: %d \n", in->sin_family);
    printf("[*] sockaddr_in.addr.s_addr: %x\n", in->sin_addr.s_addr);
    printf("[*] sockaddr_in.sin_port: %d\n", in->sin_port);
}

int main(){
    int c_socket, s_socket; /* define socket file discription */
    struct sockaddr_in s_addr, c_addr;
    char *temp;
    int len;
    int length;
    int n;

    s_socket = socket(PF_INET, SOCK_STREAM, 0); /* create socket of IPv4 using TCP protocol */
    
    memset(&s_addr, 0, sizeof(s_addr));         /* zeroing sockaddr structure */
    s_addr.sin_family      = AF_INET;           /* setting Address Family value */
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* setting Address. At this time Address is network bytes order using htonl() */
    s_addr.sin_port        = htons(PORT);       /* setting port. At this time port is network bytes order using htons() */

#if 0L
    print_sockaddr_in(&s_addr);
#endif
    /* connet socket at port */
    if(bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1){
        printf("Cannot Bind\n");
        return -1;
    }   
    /* request to open kernel */
    if(listen(s_socket, 5) == -1){
        printf("listen Fail\n");
        return -1;
    }
    while(1){
        /* client connecting request */
        len = sizeof(c_addr);
        c_socket = accept(s_socket, (struct sockaddr *)&c_addr, (socklen_t *)&len);
        /* user program */
        length = 0;
        temp = rBuffer;
        while((n = read(c_socket, temp, 1)) > 0){
            if(*temp == '\r') continue;
            if(*temp == '\n') break;
            if(*temp == '\0') break;

            if(length == BUFSIZ) break;
            temp++;
            length++;
        }
#if 1L
        printf("rBuffer : %s\n", rBuffer);
        sleep(1);
#endif
        if(!strcmp(rBuffer, "print")){
            n = strlen(buffer);
            write(c_socket, buffer, n);
        }
        close(c_socket); /* close client socket */
    }
    close(s_socket);
    return 0;
}



