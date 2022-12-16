#include<stdio.h>
#include<string.h>
#include<netdb.h>
#include<sys/socket.h>
#include<arpa/inet.h>

int main()
{
    struct in_addr addr;
    struct hostent *host;
    const char *hostName = "www.google.com";
    int i;

    /* 문자열로 된 호스트 주소의 주소값을 gethostbyname에 넣어 이를 hostent 구조체로 반환받는다. */
    if((host = gethostbyname(hostName)) == NULL){
        printf("gethostbyname() error - check network\n");
        return -1;
    }

    printf("official name = %s \n", host->h_name);
    i = 0;
    while(host->h_aliases[i] != NULL){
        printf("aliases = %s\n", host->h_aliases[i++]);
    }
    printf("address type = %d\n", host->h_addrtype);
    printf("address length = %d\n", host->h_length);

    i = 0;
    while(host->h_addr_list[i] != NULL){
        memcpy(&addr.s_addr, host->h_addr_list[i], 4);
        printf("address = %s(0x%x)\n", inet_ntoa(addr), ntohl(*(long *)host->h_addr_list[i]));
        i++;
    }

    return 0;


}