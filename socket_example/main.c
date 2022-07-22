#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>

#define QUEUE_DEFAULT_NUM 5
#define BUF_MAX_LEN 4096

static void print_help(const char *progname)
{
    printf("Usage: %s (s|c Message) (stream|datagram) (filepath)\n", progname);
}

static int setting_sock_type(const char *TYPE, int *out)
{   
    if(!strcmp(TYPE, "stream")){
        /* stream type */
        *out = SOCK_STREAM;
    } else if(!strcmp(TYPE, "datagram")){
        /* datagram type */
        *out = SOCK_DGRAM;
    } else {
        printf("[ERROR] setting_sock_type - input wrong socket type\n ");
        return -1;
    }
    return 0;
}

static void setting_sock_filepath(const char *PATH, char *out)
{
    memset((void *)out, 0, sizeof(out));
    strcpy(out, PATH);
}

static int partial_message_recv(int fd, void *buf, size_t size, int flags)
{
    int written = 0;
    int ret = 1;

    while(ret){
        ret = recv(fd, (char *)buf + written , size - written, flags);
        if(ret == -1){
            perror("recv()");
            return ret;
        }
        written += ret;
    }
    return 0;
}

static int partial_message_send(int fd, void *buf, size_t size, int flags)
{
    int written = 0;
    int ret;

    while(written < size){
        ret = send(fd, (char *)buf + written , size - written, flags);
        if(ret == -1){
            return ret;
        }
        written += ret;
    }
    printf("send data: %s\n", (char *)buf);
    return 0;
}

static int do_recv_data(const int socket_type, const char *filepath)
{
    int server_fd;
    int client_fd;
    char buf[BUF_MAX_LEN];

    memset((void *)buf, 0, sizeof(buf));

    struct sockaddr_un addr;

    server_fd = socket(AF_UNIX, (int)socket_type, 0);
    if(server_fd == -1){
        perror("socket()");
        return -1;
    }
    memset((void *)&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, filepath, sizeof(addr.sun_path)-1);
    if(bind(server_fd, (const struct sockaddr *)&addr, sizeof(addr)) == -1){
        perror("bind()");
        close(server_fd);
        return -1;
    }
    if(socket_type == SOCK_STREAM){
        listen(server_fd, QUEUE_DEFAULT_NUM);
        client_fd = accept(server_fd, NULL, NULL);
        if(partial_message_recv(client_fd, (void *)buf, sizeof(buf), 0) == -1){
            close(server_fd);
            close(client_fd);
            return -1;
        }
        printf("Server : Client said %s\n", buf);
        close(server_fd);
        close(client_fd);
    }
    else if(socket_type == SOCK_DGRAM){
        if(recvfrom(server_fd, (void *)buf, sizeof(buf), 0, NULL, NULL) == -1){
            perror("recvfrom()");
            close(server_fd);
            return -1;
        }
        printf("Server : Client said %s\n", buf);
        close(server_fd);
    } else {
        printf("[ERROR] do_recv_data - wrong socket_type\n");
        return -1;
    }
    
    return 0;
}

static int do_send_data(const int socket_type, const char *filepath, const char *message)
{
    int client_fd;

    struct sockaddr_un addr;

    client_fd = socket(AF_UNIX, (int)socket_type, 0);
    if(client_fd == -1){
        perror("socket()");
        return -1;
    }
    memset((void *)&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, filepath, sizeof(addr.sun_path)-1);
    if(socket_type == SOCK_STREAM){
        if(connect(client_fd, (const struct sockaddr *)&addr, sizeof(addr)) == -1){
            perror("connect()");
            close(client_fd);
            return -1;
        }
        if(partial_message_send(client_fd, (void *)message, sizeof(message), 0) == -1){
            close(client_fd);
            return -1;
        }
        close(client_fd);
    }
    else if(socket_type == SOCK_DGRAM){
        if(sendto(client_fd, (void *)message, sizeof(message), 0, (struct sockaddr *)&addr, sizeof(addr)) == -1){
            perror("sendto()");
            close(client_fd);
            return -1;
        }
        close(client_fd);
    } else {
        printf("[ERROR] do_recv_data - wrong socket_type\n");
        return -1;
    }
    
    return 0;
}

int main(int argc, char **argv)
{
    int SOCK_TYPE;
    char filepath[1024];
    
    if(argc < 4){
        print_help(argv[0]);
        return -1;
    }

    if(!strcmp(argv[1], "s")){
        /* Server */
        if(setting_sock_type(argv[2], &SOCK_TYPE) == -1){
            goto main_err;
        }
        setting_sock_filepath(argv[3], filepath);
        if(do_recv_data((const int)SOCK_TYPE, (const char *)filepath) == -1){
            goto main_err;
        }

    } else if(!strcmp(argv[1], "c")){
        /* Client */
        if(argc < 5){
            print_help(argv[0]);
            return -1;
        }
        // char message[1024];
        // memset((void *)message, 0, sizeof(message));
        // strncpy(message, (const char *)argv[2], sizeof(argv[2]));
        if(setting_sock_type(argv[3], &SOCK_TYPE) == -1){
            goto main_err;
        }
        setting_sock_filepath(argv[4], filepath);
        if(do_send_data((const int)SOCK_TYPE, (const char *)filepath, (const char *)argv[2]) == -1){
            goto main_err;
        }
    } else {
        goto main_err;
    }
    return 0;
main_err:
    print_help(argv[0]);
    return -1;
}