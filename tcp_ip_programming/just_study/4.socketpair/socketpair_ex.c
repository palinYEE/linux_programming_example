#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>

int main()
{
    int sd[2], result;
    int n1, n2;
    char buf[BUFSIZ];
    char data[] = "This is from sd[0]";

    /* sockpair 선언 */
    result = socketpair(PF_LOCAL, SOCK_STREAM, 0, sd);

    n1 = write(sd[0], data, (size_t)strlen(data));
    printf("[send]: %s\n", data);

    n2 = read(sd[1], buf, (size_t)BUFSIZ);
    buf[n2] = '\0';
    printf("[receive]: %s\n", buf);

    close(sd[0]);
    close(sd[1]);
}