#include<stdio.h>
#include<netinet/in.h>

int main()
{
    short hostData = 0x1234;
    short netData;
    char *ch;

    printf("original data: 0x1234\n");

    ch = (char *)&hostData;

    if(ch[0] == 0x12 && ch[1] == 0x34){
        printf("host(big-endian): ");
    }
    else if(ch[1] == 0x12 && ch[0] == 0x34){
        printf("host(little-endian): ");
    }

    printf("0x%x%x\n", ch[0], ch[1]);

    netData = htons(hostData);

    ch = (char *)&netData;
    printf("network data(big-endian): 0x%x%x\n", ch[0], ch[1]);
    return 0;
}