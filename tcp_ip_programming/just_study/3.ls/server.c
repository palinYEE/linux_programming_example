#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
#include<netinet/in.h>
#include<sys/socket.h>

#define PORT 9000
#define ERRORNUM -1

char rBuffer[BUFSIZ];
char err_1[]="Directory error";

int main(){
    int c_socket, s_socket;
    struct sockaddr_in c_addr, s_addr;
    char *temp;
    int len;
    int length;
    int n;
    /* file flie value */
    DIR *dp;
    struct dirent *dir;

    s_socket = socket(PF_INET, SOCK_STREAM, 0);

    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = htons(PORT);

    len = sizeof(s_addr);
    if(bind(s_socket, (struct sockaddr *)&s_addr, (socklen_t)len) == ERRORNUM){
        printf("Cannot Bind\n");
        return -1;
    }

    if(listen(s_socket, 5) == ERRORNUM){
        printf("Cannot Listen\n");
        return -1;
    }
    while(1){
        len = sizeof(c_addr);
        c_socket = accept(s_socket, (struct sockaddr *)&c_socket, (socklen_t *)&len);
        /* user programing */
        length = 0;
        temp = rBuffer;

        if(strcmp(rBuffer, "ls")){
            if((dp = opendir(".")) == NULL){
                write(c_socket, err_1, strlen(err_1));
            }
            else{
                while((dir = readdir(dp)) != NULL){
                    if(dir->d_ino == 0) continue;

                    write(c_socket, dir->d_name, strlen(dir->d_name));
                    write(c_socket, " ", 1);
                }
                closedir(dp);
            }
        }
        close(c_socket);
    }   
    close(c_socket);
}